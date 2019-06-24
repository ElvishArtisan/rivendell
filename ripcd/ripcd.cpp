// ripcd.cpp
//
// Rivendell Interprocess Communication Daemon
//
//   (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public
//   License along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#include <syslog.h>

#include <qapplication.h>
#include <qobject.h>
#include <qtimer.h>
#include <qdir.h>
#include <qsessionmanager.h>
#include <qsignalmapper.h>
#include <qstringlist.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <math.h>
#include <netdb.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <rdapplication.h>
#include <rdconf.h>
#include <rdescape_string.h>
#include <rdnotification.h>

#include "globals.h"
#include "ripcd.h"

bool global_exiting=false;

void SigHandler(int signo)
{
  pid_t pLocalPid;

  switch(signo) {
  case SIGCHLD:
    pLocalPid=waitpid(-1,NULL,WNOHANG);
    while(pLocalPid>0) {
            pLocalPid=waitpid(-1,NULL,WNOHANG);
    }
    ::signal(SIGCHLD,SigHandler);
    ::signal(SIGTERM,SigHandler);
    ::signal(SIGINT,SigHandler);
    return;

  case SIGTERM:
  case SIGINT:
    global_exiting=true;
    break;
  }
}


MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  QString err_msg;
  RDApplication::ErrorType err_type=RDApplication::ErrorOk;

  rda=new RDApplication("ripcd","ripcd",RIPCD_USAGE,this);
  if(!rda->open(&err_msg,&err_type,false)) {
    fprintf(stderr,"ripcd: %s\n",(const char *)err_msg.utf8());
    exit(1);
  }

  //
  // Initialize Data Structures
  //
  debug=false;
  for(int i=0;i<MAX_MATRICES;i++) {
    ripcd_switcher[i]=NULL;
    for(int j=0;j<MAX_GPIO_PINS;j++) {
      ripcd_gpi_state[i][j]=false;
      ripcd_gpo_state[i][j]=false;
    }
  }
  ripc_onair_flag=false;

  //
  // Client Connections
  //
  ripcd_ready_mapper=new QSignalMapper(this);
  connect(ripcd_ready_mapper,SIGNAL(mapped(int)),this,SLOT(readyReadData(int)));

  ripcd_kill_mapper=new QSignalMapper(this);
  connect(ripcd_kill_mapper,SIGNAL(mapped(int)),this,SLOT(killData(int)));
  server=new QTcpServer(this);
  if(!server->listen(QHostAddress::Any,RIPCD_TCP_PORT)) {
    rda->syslog(LOG_ERR,"unable to bind ripc port");
    exit(1);
  }
  connect(server,SIGNAL(newConnection()),this,SLOT(newConnectionData()));

  //
  // Macro Timers
  //
  QSignalMapper *mapper=new QSignalMapper(this);
  connect(mapper,SIGNAL(mapped(int)),this,SLOT(macroTimerData(int)));
  for(int i=0;i<RD_MAX_MACRO_TIMERS;i++) {
    ripc_macro_cart[i]=0;
    ripc_macro_timer[i]=new QTimer(this);
    mapper->setMapping(ripc_macro_timer[i],i);
    connect(ripc_macro_timer[i],SIGNAL(timeout()),mapper,SLOT(map()));
  }

  ripcd_host_addr=rda->station()->address();

  //
  // CAE Connection
  //
  rda->cae()->connectHost();

  if(qApp->argc()!=1) {
    debug=true;
  }
  ::signal(SIGCHLD,SigHandler);
  ::signal(SIGTERM,SigHandler);
  ::signal(SIGINT,SigHandler);

  //
  // The RML Sockets
  //
  ripcd_rml_send=new QUdpSocket(this);

  ripcd_rml_echo=new QUdpSocket(this);
  ripcd_rml_echo->bind(QHostAddress::Any,RD_RML_ECHO_PORT);
  connect(ripcd_rml_echo,SIGNAL(readyRead()),this,SLOT(rmlEchoData()));

  ripcd_rml_noecho=new QUdpSocket(this);
  ripcd_rml_noecho->bind(QHostAddress::Any,RD_RML_NOECHO_PORT);
  connect(ripcd_rml_noecho,SIGNAL(readyRead()),this,SLOT(rmlNoechoData()));

  ripcd_rml_reply=new QUdpSocket(this);
  ripcd_rml_reply->bind(QHostAddress::Any,RD_RML_REPLY_PORT);
  connect(ripcd_rml_reply,SIGNAL(readyRead()),this,SLOT(rmlReplyData()));

  LoadGpiTable();

  //
  // Initialize local RMLs
  //
  LoadLocalMacros();

  //
  // Initialize Notifications
  //
  ripcd_notification_mcaster=new RDMulticaster(this);
  ripcd_notification_mcaster->enableLoopback(false);
  connect(ripcd_notification_mcaster,
	  SIGNAL(received(const QString &,const QHostAddress &)),
	  this,
	  SLOT(notificationReceivedData(const QString &,const QHostAddress &)));
  ripcd_notification_mcaster->bind(RD_NOTIFICATION_PORT);
  ripcd_notification_mcaster->subscribe(rda->system()->notificationAddress());

  //
  // Exit Timer
  //
  QTimer *timer=new QTimer(this);
  connect(timer,SIGNAL(timeout()),this,SLOT(exitTimerData()));
  timer->start(200);

  //
  // Garbage Timer
  //
  ripcd_garbage_timer=new QTimer(this);
  connect(ripcd_garbage_timer,SIGNAL(timeout()),this,SLOT(garbageData()));

  rda->syslog(LOG_INFO,"started");
}


MainObject::~MainObject()
{
  delete server;
  delete ripcd_db;
}


void MainObject::newConnectionData()
{
  unsigned i=0;

  QTcpSocket *sock=server->nextPendingConnection();
  while((i<ripcd_conns.size())&&(ripcd_conns[i]!=NULL)) {
    i++;
  }
  if(i==ripcd_conns.size()) {      // Table full, create a new slot
    ripcd_conns.push_back(new RipcdConnection(i,sock));
  }
  else {
    ripcd_conns[i]=new RipcdConnection(i,sock);
  }
  ripcd_ready_mapper->setMapping(ripcd_conns[i]->socket(),i);
  connect(ripcd_conns[i]->socket(),SIGNAL(readyRead()),
	  ripcd_ready_mapper,SLOT(map()));
  ripcd_kill_mapper->setMapping(ripcd_conns[i]->socket(),i);
  connect(ripcd_conns[i]->socket(),SIGNAL(connectionClosed()),
	  ripcd_kill_mapper,SLOT(map()));
  rda->syslog(LOG_DEBUG,"added new connection %d",i);
}


void MainObject::notificationReceivedData(const QString &msg,
					  const QHostAddress &addr)
{
  RDNotification *notify=new RDNotification();

  if(!notify->read(msg)) {
    rda->syslog(LOG_DEBUG,"invalid notification received from %s",
		(const char *)addr.toString().toUtf8());
    delete notify;
    return;
  }
  RunLocalNotifications(notify);
  BroadcastCommand("ON "+msg+"!");

  delete notify;
}


void MainObject::sendRml(RDMacro *rml)
{
  QString str;

  if(rml->isNull()) {
    return;
  }
  str=rml->toString();
  switch(rml->role()) {
  case RDMacro::Cmd:
    ripcd_rml_send->writeDatagram(str.utf8(),str.utf8().length(),
				  rml->address(),rml->port());
    break;

  case RDMacro::Reply:
    if(!(ripcd_host_addr==rml->address())) {
      ripcd_rml_send->writeDatagram(str.utf8(),str.utf8().length(),
				    rml->address(),RD_RML_REPLY_PORT);
    }
    break;

  default:
    break;
  }
}


void MainObject::rmlEchoData()
{
  ReadRmlSocket(ripcd_rml_echo,RDMacro::Cmd,true);
}


void MainObject::rmlNoechoData()
{
  ReadRmlSocket(ripcd_rml_noecho,RDMacro::Cmd,false);
}


void MainObject::rmlReplyData()
{
  ReadRmlSocket(ripcd_rml_reply,RDMacro::Reply,false);
}


void MainObject::readyReadData(int conn_id)
{
  char data[1501];
  int n;
  RipcdConnection *conn=ripcd_conns[conn_id];
  QChar c;

  while((n=conn->socket()->readBlock(data,1500))>0) {
    data[n]=0;
    QString line=QString::fromUtf8(data);
    for(int i=0;i<line.length();i++) {
      QChar c=line.at(i);
      if(c.toAscii()=='!') {
	if(!DispatchCommand(conn)) {
	  return;
	}
	conn->accum="";
      }
      else {
	if((c.toAscii()!='\r')&&(c.toAscii()!='\n')) {
	  conn->accum+=c;
	}
      }
    }
  }
}


void MainObject::killData(int conn_id)
{
  ripcd_conns[conn_id]->close();
  ripcd_garbage_timer->start(1,true);
  rda->syslog(LOG_DEBUG,"closed connection %d",conn_id);
}


void MainObject::macroTimerData(int num)
{
  ExecCart(ripc_macro_cart[num]);
  ripc_macro_cart[num]=0;
}


void MainObject::exitTimerData()
{
  if(global_exiting) {
    for(int i=0;i<MAX_MATRICES;i++) {
      if(ripcd_switcher[i]!=NULL) {
	delete ripcd_switcher[i];
      }
    }
    rda->syslog(LOG_INFO,"ripcd exiting normally");
    exit(0);
  }
}


void MainObject::garbageData()
{
  for(unsigned i=0;i<ripcd_conns.size();i++) {
    if(ripcd_conns[i]!=NULL) {
      if(ripcd_conns[i]->isClosing()) {
	delete ripcd_conns[i];
	ripcd_conns[i]=NULL;
	rda->syslog(LOG_DEBUG,"cleaned up connection %d",i);
      }
    }
  }
}


void MainObject::SetUser(QString username)
{
  rda->station()->setUserName(username);
  BroadcastCommand(QString("RU ")+username+"!");
}


bool MainObject::DispatchCommand(RipcdConnection *conn)
{
  QString str;
  RDMacro macro;
  int echo=0;
  QHostAddress addr;

  //printf("DispatchCommand(%s)\n",(const char *)conn->accum.toUtf8());
  QStringList cmds=conn->accum.split(" ",QString::SkipEmptyParts);

  //
  // Common Commands
  // Authentication not required to execute these!
  //
  if(cmds[0]=="DC") {  // Drop Connection
    killData(conn->id());
    return false;
  }

  if((cmds[0]=="PW")&&(cmds.size()==2)) {  // Password Authenticate
    if(cmds[1]==rda->config()->password()) {
      conn->setAuthenticated(true);
      EchoCommand(conn->id(),"PW +!");
      return true;
    }
    else {
      conn->setAuthenticated(false);
      EchoCommand(conn->id(),"PW -!");
      return true;
    }
  }

  //
  // Priviledged Commands
  // Authentication required to execute these!
  //
  if(!conn->isAuthenticated()) {
    EchoCommand(conn->id(),cmds.join(" ")+"-!");
    return true;
  }
  if(cmds[0]=="RU") {  // Request User
    EchoCommand(conn->id(),(const char *)QString("RU ")+rda->station()->userName()+"!");
    return true;
  }

  if((cmds[0]=="SU")&&(cmds.size()==2)) {  // Set User
    SetUser(cmds[1]);
  }

  if(cmds[0]=="MS") {  // Send RML Command
    if(cmds.size()<4) {
      return true;
    }
    str=cmds[3];
    for(int i=4;i<cmds.size();i++) {
      str+=" "+cmds[i];
    }
    str+="!";
  }
  macro=RDMacro::fromString(str);
  if(!macro.isNull()) {
    addr.setAddress(cmds[1]);
    macro.setAddress(addr);
    macro.setPort(cmds[2].toInt());
    macro.setRole(RDMacro::Cmd);

    if(!macro.address().isNull()) {
      if(macro.address()==rda->station()->address()&&
	 ((macro.port()==RD_RML_ECHO_PORT)||
	  (macro.port()==RD_RML_NOECHO_PORT))) {  // Local Loopback
	if(macro.echoRequested()) {
	  echo=1;
	}
	RunLocalMacros(&macro);
	BroadcastCommand(QString("MS ")+macro.address().toString()+
			 QString().sprintf(" %d ",echo)+macro.toString());
      }
      else {
	sendRml(&macro);
      }
    }
  }

  if(cmds[0]=="ME") {  // Send RML Reply
    if(cmds.size()<4) {
      return true;
    }
    str=cmds[3];
    for(int i=4;i<cmds.size();i++) {
      str+=" "+cmds[i];
    }
    str+="!";
  }
  macro=RDMacro::fromString(str,RDMacro::Reply);
  if(!macro.isNull()) {
    QHostAddress addr;
    addr.setAddress(cmds[1]);
    macro.setAddress(addr);
    if(macro.address()==rda->station()->address()) {  // Local Loopback
      BroadcastCommand(QString("ME ")+macro.address().toString()+" 0 "+
		       macro.toString());
    }
    else {
      sendRml(&macro);
    }
  }

  if(cmds[0]=="RG") {  // Reload the GPI Table
    LoadGpiTable();
  }

  if((cmds[0]=="GI")&&(cmds.size()==2)) {  // Send Complete GPI Status
    SendGpi(conn->id(),cmds[1].toInt());
  }

  if((cmds[0]=="GO")&&(cmds.size()==2)) {  // Send Complete GPO Status
    SendGpo(conn->id(),cmds[1].toInt());
  }

  if((cmds[0]=="GM")&&(cmds.size()==2)) {  // Send Complete GPI Mask States
    SendGpiMask(conn->id(),cmds[1].toInt());
  }

  if((cmds[0]=="GN")&&(cmds.size()==2)) {  // Send Complete GPO Mask States
    SendGpoMask(conn->id(),cmds[1].toInt());
  }

  if((cmds[0]=="GC")&&(cmds.size()==2)) {  // Send Complete GPI Cart Assignments
    SendGpiCart(conn->id(),cmds[1].toInt());
  }

  if((cmds[0]=="GD")&&(cmds.size()==2)) {  // Send Complete GPO Cart Assignments
    SendGpoCart(conn->id(),cmds[1].toInt());
  }

  if(cmds[0]=="ON") {  // Send Notification
    QString msg;
    for(int i=1;i<cmds.size();i++) {
      msg+=QString(cmds[i])+" ";
    }
    msg=msg.left(msg.length()-1);
    RDNotification *notify=new RDNotification();
    if(!notify->read(msg)) {
      rda->syslog(LOG_DEBUG,"invalid notification processed");
      delete notify;
      return true;
    }
    RunLocalNotifications(notify);
    BroadcastCommand("ON "+msg+"!",conn->id());
    ripcd_notification_mcaster->
      send(msg,rda->system()->notificationAddress(),RD_NOTIFICATION_PORT);
    delete notify;
  }

  if(cmds[0]=="TA") {  // Send Onair Flag State
    EchoCommand(conn->id(),QString().sprintf("TA %d!",ripc_onair_flag));
  }

  return true;
}


void MainObject::EchoCommand(int ch,const QString &cmd)
{
  //  printf("EchoCommand(%d,%s)\n",ch,(const char *)cmd.utf8());
  if(ripcd_conns[ch]->socket()->state()==QAbstractSocket::ConnectedState) {
    ripcd_conns[ch]->socket()->writeBlock(cmd.utf8(),cmd.utf8().length());
  }
}


void MainObject::BroadcastCommand(const QString &cmd,int except_ch)
{
  for(unsigned i=0;i<ripcd_conns.size();i++) {
    if((int)i!=except_ch) {
      if(ripcd_conns[i]!=NULL) {
	EchoCommand(i,cmd);
      }
    }
  }
}


void MainObject::ReadRmlSocket(QUdpSocket *sock,RDMacro::Role role,
			       bool echo)
{
  char buffer[1501];
  QString output;
  int n;
  QHostAddress peer_addr;
  RDMacro macro;

  while((n=sock->readDatagram(buffer,1501,&peer_addr))>0) {
    buffer[n]=0;
    macro=RDMacro::fromString(QString::fromUtf8(buffer));
    if(!macro.isNull()) {
      if(macro.command()==RDMacro::AG) {
	if(ripc_onair_flag) {
	  QStringList f0=
	    QString::fromUtf8(buffer).split(" ",QString::SkipEmptyParts);
	  f0.pop_front();
	  QString rmlstr=f0.join(" ");
	  macro=RDMacro::fromString(rmlstr);
	  if(macro.isNull()) {
	    break;
	  }
	}
	else {
	  rda->syslog(LOG_DEBUG,
		      "rejected rml: \"%s\": on-air flag not active",buffer);
	  break;
	}
      }
      macro.setRole(role);
      macro.setAddress(peer_addr);
      macro.setEchoRequested(echo);
      switch(role) {
      case RDMacro::Cmd:
	RunLocalMacros(&macro);
	BroadcastCommand(QString("MS ")+macro.address().toString()+
			 QString().sprintf(" %d ",echo)+
			 macro.toString());
	break;

      default:
	break;
      }
    }
    else {
      rda->syslog(LOG_DEBUG,"received malformed rml: \"%s\" from %s:%u",
		  buffer,
		  (const char *)sock->peerAddress().toString().toUtf8(),
		  sock->peerPort());
      if(echo) {
	macro.setRole(RDMacro::Reply);
	macro.setCommand(RDMacro::NN);
	macro.addArg("-");
	macro.setAddress(peer_addr);
	sendRml(&macro);
      }
    }
  }
}


void MainObject::LoadGpiTable()
{
  for(int i=0;i<MAX_MATRICES;i++) {
    for(int j=0;j<MAX_GPIO_PINS;j++) {
      ripcd_gpi_mask[i][j]=true;
      ripcd_gpo_mask[i][j]=true;
      for(int k=0;k<2;k++) {
	ripcd_gpi_macro[i][j][k]=0;
	ripcd_gpo_macro[i][j][k]=0;
      }
    }
  }
  QString sql=QString("select ")+
    "MATRIX,"+          // 00
    "NUMBER,"+          // 01
    "OFF_MACRO_CART,"+  // 02
    "MACRO_CART "+      // 03
    "from GPIS where "+
    "STATION_NAME=\""+RDEscapeString(rda->config()->stationName())+"\"";
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    ripcd_gpi_macro[q->value(0).toInt()][q->value(1).toInt()-1][0]=
      q->value(2).toInt();
    ripcd_gpi_macro[q->value(0).toInt()][q->value(1).toInt()-1][1]=
      q->value(3).toInt();
  }
  delete q;

  sql=QString("select ")+
    "MATRIX,"+          // 00
    "NUMBER,"+          // 01
    "OFF_MACRO_CART,"+  // 02
    "MACRO_CART "+      // 03
    "from GPOS where "+
    "STATION_NAME=\""+RDEscapeString(rda->config()->stationName())+"\"";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    ripcd_gpo_macro[q->value(0).toInt()][q->value(1).toInt()-1][0]=
      q->value(2).toInt();
    ripcd_gpo_macro[q->value(0).toInt()][q->value(1).toInt()-1][1]=
      q->value(3).toInt();
  }
  delete q;
}


void MainObject::SendGpi(int ch,int matrix)
{
  if(ripcd_switcher[matrix]==NULL) {
    return;
  }
  for(unsigned i=0;i<ripcd_switcher[matrix]->gpiQuantity();i++) {
    EchoCommand(ch,QString().sprintf("GI %d %d %d %d!",
				     matrix,i,ripcd_gpi_state[matrix][i],
				     ripcd_gpi_mask[matrix][i]));
  }
}


void MainObject::SendGpo(int ch,int matrix)
{
  if(ripcd_switcher[matrix]==NULL) {
    return;
  }
  for(unsigned i=0;i<ripcd_switcher[matrix]->gpoQuantity();i++) {
    EchoCommand(ch,QString().sprintf("GO %d %d %d %d!",
				     matrix,i,ripcd_gpo_state[matrix][i],
				     ripcd_gpo_mask[matrix][i]));
  }
}


void MainObject::SendGpiMask(int ch,int matrix)
{
  if(ripcd_switcher[matrix]==NULL) {
    return;
  }
  for(unsigned i=0;i<ripcd_switcher[matrix]->gpiQuantity();i++) {
    EchoCommand(ch,QString().sprintf("GM %d %d %d!",
				     matrix,i,ripcd_gpi_mask[matrix][i]));
  }
}


void MainObject::SendGpoMask(int ch,int matrix)
{
  if(ripcd_switcher[matrix]==NULL) {
    return;
  }
  for(unsigned i=0;i<ripcd_switcher[matrix]->gpoQuantity();i++) {
    EchoCommand(ch,QString().sprintf("GN %d %d %d!",
				     matrix,i,ripcd_gpo_mask[matrix][i]));
  }
}


void MainObject::SendGpiCart(int ch,int matrix)
{
  if(ripcd_switcher[matrix]==NULL) {
    return;
  }
  for(unsigned i=0;i<ripcd_switcher[matrix]->gpiQuantity();i++) {
    EchoCommand(ch,QString().sprintf("GC %d %d %d %d!",
				     matrix,i,ripcd_gpi_macro[matrix][i][0],
				     ripcd_gpi_macro[matrix][i][1]));
  }
}


void MainObject::SendGpoCart(int ch,int matrix)
{
  if(ripcd_switcher[matrix]==NULL) {
    return;
  }
  for(unsigned i=0;i<ripcd_switcher[matrix]->gpoQuantity();i++) {
    EchoCommand(ch,QString().sprintf("GD %d %d %d %d!",
				     matrix,i,ripcd_gpo_macro[matrix][i][0],
				     ripcd_gpo_macro[matrix][i][1]));
  }
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
