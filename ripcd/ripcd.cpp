// ripcd.cpp
//
// Rivendell Interprocess Communication Daemon
//
//   (C) Copyright 2002-2007,2010,2016-2018 Fred Gleason <fredg@paravelsystems.com>
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
#include <rdcheck_daemons.h>
#include <rdescape_string.h>
#include <rdnotification.h>

#include "globals.h"
#include "ripcd_socket.h"
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

  rda=new RDApplication("ripcd","ripcd",RIPCD_USAGE,this);
  if(!rda->open(&err_msg)) {
    fprintf(stderr,"ripcd: %s\n",(const char *)err_msg.utf8());
    exit(1);
  }

  //
  // Make sure we're the only instance running
  //
  if(RDCheckDaemon(RD_RIPCD_PID)) {
    LogLine(RDConfig::LogErr,
	    "ERROR ripcd aborting - multiple instances not allowed");
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
  server=new RipcdSocket(RIPCD_TCP_PORT,0,this);
  if(!server->ok()) {
    exit(1);
  }
  connect(server,SIGNAL(connection(int)),this,SLOT(newConnection(int)));

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

  if(qApp->argc()==1) {
    RDDetach(rda->config()->logCoreDumpDirectory());
  }
  else {
    debug=true;
  }
  ::signal(SIGCHLD,SigHandler);
  ::signal(SIGTERM,SigHandler);
  ::signal(SIGINT,SigHandler);
  if(!RDWritePid(RD_PID_DIR,"ripcd.pid",rda->config()->uid())) {
    printf("ripcd: can't write pid file\n");
    exit(1);
  }

  //
  // The RML Sockets
  //
  ripcd_rml_send=new QSocketDevice(QSocketDevice::Datagram);

  ripcd_rml_echo=new QSocketDevice(QSocketDevice::Datagram);
  ripcd_rml_echo->bind(QHostAddress(),RD_RML_ECHO_PORT);
  ripcd_rml_echo->setBlocking(false);

  ripcd_rml_noecho=new QSocketDevice(QSocketDevice::Datagram);
  ripcd_rml_noecho->bind(QHostAddress(),RD_RML_NOECHO_PORT);
  ripcd_rml_noecho->setBlocking(false);

  ripcd_rml_reply=new QSocketDevice(QSocketDevice::Datagram);
  ripcd_rml_reply->bind(QHostAddress(),RD_RML_REPLY_PORT);
  ripcd_rml_reply->setBlocking(false);

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
  // Start RML Polling
  //
  QTimer *timer=new QTimer(this);
  timer->changeInterval(RIPCD_RML_READ_INTERVAL);
  connect(timer,SIGNAL(timeout()),this,SLOT(readRml()));
  timer->start(true);

  //
  // Database Backup Timer
  //
  databaseBackup();
  ripcd_backup_timer=new QTimer(this);
  connect(ripcd_backup_timer,SIGNAL(timeout()),this,SLOT(databaseBackup()));
  ripcd_backup_timer->start(86400000);

  //
  // Maintenance Routine Timer
  //
  srandom(QTime::currentTime().msec());
  ripcd_maint_timer=new QTimer(this);
  connect(ripcd_maint_timer,SIGNAL(timeout()),this,SLOT(checkMaintData()));
  int interval=GetMaintInterval();
  if(!rda->config()->disableMaintChecks()) {
    ripcd_maint_timer->start(interval);
  }
  else {
    log(RDConfig::LogInfo,"maintenance checks disabled on this host!");
  }

  //
  // Exit Timer
  //
  timer=new QTimer(this);
  connect(timer,SIGNAL(timeout()),this,SLOT(exitTimerData()));
  timer->start(200);

  //
  // Garbage Timer
  //
  ripcd_garbage_timer=new QTimer(this);
  connect(ripcd_garbage_timer,SIGNAL(timeout()),this,SLOT(garbageData()));

  LogLine(RDConfig::LogInfo,"started");
}


MainObject::~MainObject()
{
  delete server;
  delete ripcd_db;
}

void MainObject::log(RDConfig::LogPriority prio,const QString &msg)
{
  LogLine(prio,msg);
}


void MainObject::newConnection(int fd)
{
  unsigned i=0;

  while((i<ripcd_conns.size())&&(ripcd_conns[i]!=NULL)) {
    i++;
  }
  if(i==ripcd_conns.size()) {      // Table full, create a new slot
    ripcd_conns.push_back(new RipcdConnection(i,fd));
  }
  else {
    ripcd_conns[i]=new RipcdConnection(i,fd);
  }
  ripcd_ready_mapper->setMapping(ripcd_conns[i]->socket(),i);
  connect(ripcd_conns[i]->socket(),SIGNAL(readyRead()),
	  ripcd_ready_mapper,SLOT(map()));
  ripcd_kill_mapper->setMapping(ripcd_conns[i]->socket(),i);
  connect(ripcd_conns[i]->socket(),SIGNAL(connectionClosed()),
	  ripcd_kill_mapper,SLOT(map()));
}


void MainObject::notificationReceivedData(const QString &msg,
					  const QHostAddress &addr)
{
  RDNotification *notify=new RDNotification();
  if(!notify->read(msg)) {
    LogLine(RDConfig::LogWarning,
	    "Invalid notification received from "+addr.toString());
    delete notify;
    return;
  }
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
    ripcd_rml_send->writeBlock(str.utf8(),str.utf8().length(),
			       rml->address(),rml->port());
    break;

  case RDMacro::Reply:
    if(!(ripcd_host_addr==rml->address())) {
      ripcd_rml_send->writeBlock(str.utf8(),str.utf8().length(),
				 rml->address(),RD_RML_REPLY_PORT);
    }
    break;

  default:
    break;
  }
}


void MainObject::readRml()
{
  ReadRmlSocket(ripcd_rml_echo,RDMacro::Cmd,true);
  ReadRmlSocket(ripcd_rml_noecho,RDMacro::Cmd,false);
  ReadRmlSocket(ripcd_rml_reply,RDMacro::Reply,false);
}


void MainObject::databaseBackup()
{
  QString cmd;
  QDateTime datetime=QDateTime::currentDateTime();
  int life;

  if((life=rda->station()->backupLife())<=0) {
    return;
  }
  if(fork()==0) {
    cmd=QString().sprintf("find %s -name *.sql -ctime +%d -exec rm \\{\\} \\;",
			  (const char *)rda->station()->backupPath(),
			  rda->station()->backupLife());
    system((const char *)cmd);
    cmd=QString().
	sprintf("mysqldump -c Rivendell -h %s -u %s -p%s > %s/%s.sql",
		(const char *)rda->config()->mysqlHostname(),
		(const char *)rda->config()->mysqlUsername(),
		(const char *)rda->config()->mysqlPassword(),
		(const char *)rda->station()->backupPath(),
		(const char *)datetime.date().toString("yyyyMMdd"));
    system((const char *)cmd);
    exit(0);
  }
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
    for(unsigned i=0;i<line.length();i++) {
      QChar c=line.ref(i);
      if(c=="!") {
	if(!DispatchCommand(conn)) {
	  return;
	}
	conn->accum="";
      }
      else {
	if((c!="\r")&&(c!="\n")) {
	  conn->accum+=c;
	}
      }
    }
  }
}


void MainObject::killData(int conn_id)
{
  ripcd_conns[conn_id]->socket()->close();
  ripcd_garbage_timer->start(1,true);
}


void MainObject::checkMaintData()
{
  QString sql;
  RDSqlQuery *q;
  QDateTime current_datetime=
    QDateTime(QDate::currentDate(),QTime::currentTime());
  bool run=false;

  RunLocalMaintRoutine();

  //
  // Should we try to run system maintenance?
  //
  if(!rda->station()->systemMaint()) {
    return;
  }

  //
  // Get the system-wide maintenance timestamp
  //
  sql="lock tables VERSION write";
  q=new RDSqlQuery(sql);
  delete q;
  sql="select LAST_MAINT_DATETIME from VERSION";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    run=1000*q->value(0).toDateTime().secsTo(current_datetime)>
      RD_MAINT_MAX_INTERVAL;
  }
  delete q;
  sql="unlock tables";
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Run the routines
  //
  if(run) {
    RunSystemMaintRoutine();
  }
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
    LogLine(RDConfig::LogInfo,"ripcd exiting normally");
    RDDeletePid(RD_PID_DIR,"ripcd.pid");
    exit(0);
  }
}


void MainObject::garbageData()
{
  for(unsigned i=0;i<ripcd_conns.size();i++) {
    if(ripcd_conns[i]!=NULL) {
      if(ripcd_conns[i]->socket()->state()==QSocket::Idle) {
	delete ripcd_conns[i];
	ripcd_conns[i]=NULL;
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

  QStringList cmds=cmds.split(" ",conn->accum);

  //
  // Common Commands
  // Authentication not required to execute these!
  //
  if(cmds[0]=="DC") {  // Drop Connection
    conn->socket()->close();
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
    for(unsigned i=4;i<cmds.size();i++) {
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
    for(unsigned i=4;i<cmds.size();i++) {
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
    for(unsigned i=1;i<cmds.size();i++) {
      msg+=QString(cmds[i])+" ";
    }
    msg=msg.left(msg.length()-1);
    RDNotification *notify=new RDNotification();
    if(!notify->read(msg)) {
      LogLine(RDConfig::LogWarning,"invalid notification processed");
      delete notify;
      return true;
    }
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
  if(ripcd_conns[ch]->socket()->state()==QSocket::Connected) {
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


void MainObject::ReadRmlSocket(QSocketDevice *dev,RDMacro::Role role,
			       bool echo)
{
  char buffer[1501];
  QString output;
  int n;
  QHostAddress peer_addr;
  RDMacro macro;

  while((n=dev->readBlock(buffer,1501))>0) {
    buffer[n]=0;
    macro=RDMacro::fromString(QString::fromUtf8(buffer));
    if(!macro.isNull()) {
      if(macro.command()==RDMacro::AG) {
	if(ripc_onair_flag) {
	  QStringList f0=f0.split(" ",QString::fromUtf8(buffer));
	  f0.pop_front();
	  QString rmlstr=f0.join(" ");
	  macro=RDMacro::fromString(rmlstr);
	  if(macro.isNull()) {
	    break;
	  }
	}
	else {
	  LogLine(RDConfig::LogDebug,
		  QString("rejected rml: \"")+buffer+
		  "\": on-air flag not active");
	  break;
	}
      }
      macro.setRole(role);
      macro.setAddress(dev->peerAddress());
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
      LogLine(RDConfig::LogWarning,
	      QString().sprintf("received malformed rml: \"%s\" from %s:%u",
				buffer,
				(const char *)dev->peerAddress().toString(),
				dev->peerPort()));
      if(echo) {
	macro.setRole(RDMacro::Reply);
	macro.setCommand(RDMacro::NN);
	macro.addArg("-");
	macro.setAddress(dev->peerAddress());
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


void LogLine(RDConfig::LogPriority prio,const QString &line)
{
  FILE *logfile;

  rda->config()->log("ripcd",prio,line);

  if((!rda->config()) || rda->config()->ripcdLogname().isEmpty()) {
    return;
  }

  QDateTime current=QDateTime::currentDateTime();
  logfile=fopen(rda->config()->ripcdLogname(),"a");
  if(logfile==NULL) {
    return;
  }
  chmod(rda->config()->ripcdLogname(),S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
  fprintf(logfile,"%02d/%02d/%4d - %02d:%02d:%02d.%03d : %s\n",
	  current.date().month(),
	  current.date().day(),
	  current.date().year(),
	  current.time().hour(),
	  current.time().minute(),
	  current.time().second(),
	  current.time().msec(),
	  (const char *)line.utf8());
  fclose(logfile);
}

/* This is an overloaded virtual function to tell a session manager not to restart this daemon. */
void QApplication::saveState(QSessionManager &sm) {
  sm.setRestartHint(QSessionManager::RestartNever);
  LogLine(RDConfig::LogDebug,"ripcd saveState(), set restart hint to Never");
  return;
};


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
