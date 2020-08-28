// rdripc.cpp
//
// Connection to the Rivendell Interprocess Communication Daemon
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

#include <ctype.h>
#include <syslog.h>

#include <qobject.h>
#include <qapplication.h>
#include <qdatetime.h>

#include "rdapplication.h"
#include "rddatedecode.h"
#include "rddb.h"
#include "rdescape_string.h"
#include "rdripc.h"

RDRipc::RDRipc(RDStation *station,RDConfig *config,QObject *parent)
  : QObject(parent)
{
  ripc_station=station;
  ripc_config=config;
  ripc_onair_flag=false;
  ripc_ignore_mask=false;
  ripc_accum="";
  debug=false;

  ripc_connected=false;

  //
  // TCP Connection
  //
  ripc_socket=new QTcpSocket(this);
  connect(ripc_socket,SIGNAL(connected()),this,SLOT(connectedData()));
  connect(ripc_socket,SIGNAL(error(QAbstractSocket::SocketError)),
	  this,SLOT(errorData(QAbstractSocket::SocketError)));
  connect(ripc_socket,SIGNAL(readyRead()),this,SLOT(readyData()));
}


RDRipc::~RDRipc()
{
  delete ripc_socket;
}


QString RDRipc::user() const
{
  return ripc_user;
}


QString RDRipc::station() const
{
  return ripc_station->name();
}


bool RDRipc::onairFlag() const
{
  return ripc_onair_flag;
}


void RDRipc::setUser(QString user)
{
  SendCommand(QString("SU ")+user+"!");
}


void RDRipc::setIgnoreMask(bool state)
{
  ripc_ignore_mask=state;
}


void RDRipc::connectHost(QString hostname,Q_UINT16 hostport,QString password)
{
  ripc_password=password;
  ripc_socket->connectToHost(hostname,hostport);
}


void RDRipc::connectedData()
{
  SendCommand(QString("PW ")+ripc_password+"!");
}


void RDRipc::sendGpiStatus(int matrix)
{
  SendCommand(QString().sprintf("GI %d!",matrix));
}


void RDRipc::sendGpoStatus(int matrix)
{
  SendCommand(QString().sprintf("GO %d!",matrix));
}


void RDRipc::sendGpiMask(int matrix)
{
  SendCommand(QString().sprintf("GM %d!",matrix));
}


void RDRipc::sendGpoMask(int matrix)
{
  SendCommand(QString().sprintf("GN %d!",matrix));
}


void RDRipc::sendGpiCart(int matrix)
{
  SendCommand(QString().sprintf("GC %d!",matrix));
}


void RDRipc::sendGpoCart(int matrix)
{
  SendCommand(QString().sprintf("GD %d!",matrix));
}


void RDRipc::sendNotification(RDNotification::Type type,
			      RDNotification::Action action,const QVariant &id)
{
  RDNotification *notify=new RDNotification(type,action,id);
  sendNotification(*notify);
  delete notify;
}


void RDRipc::sendNotification(const RDNotification &notify)
{
  SendCommand("ON "+notify.write()+"!");
}


void RDRipc::sendOnairFlag()
{
  SendCommand(QString().sprintf("TA %d!",ripc_onair_flag));
}


void RDRipc::sendRml(RDMacro *macro)
{
  QString cmd;
  Q_UINT16 port=RD_RML_NOECHO_PORT;
  QDateTime now=QDateTime::currentDateTime();

  if(macro->echoRequested()) {
    port=RD_RML_ECHO_PORT;
  }
  if(macro->port()>0) {
    port=macro->port();
  }
  QString rmlline=macro->toString();
  QString sql=QString("select NAME,VARVALUE from HOSTVARS where ")+
    "STATION_NAME=\""+RDEscapeString(ripc_station->name())+"\"";
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    rmlline.replace(q->value(0).toString(),q->value(1).toString());
  }
  delete q;
  rmlline=RDDateTimeDecode(rmlline,now,ripc_station,ripc_config);
  switch(macro->role()) {
  case RDMacro::Cmd:
    cmd=QString("MS ")+macro->address().toString()+
      QString().sprintf(" %d ",port)+rmlline;
    break;
	
  case RDMacro::Reply:
    cmd=QString("ME ")+macro->address().toString()+
      QString().sprintf(" %d ",port)+rmlline;
    break;

  default:
    break;
  }
  SendCommand(cmd);
}


void RDRipc::reloadHeartbeat()
{
  SendCommand("RH!");
}


void RDRipc::errorData(QAbstractSocket::SocketError err)
{
  rda->syslog(LOG_DEBUG,"received socket error %d",err);
}


void RDRipc::readyData()
{
  char data[1501];
  int n;

  while((n=ripc_socket->read(data,1500))>0) {
    data[n]=0;
    QString line=QString::fromUtf8(data);
    for(int i=0;i<line.length();i++) {
      QChar c=line.at(i);
      if(c.ascii()=='!') {
	DispatchCommand();
	ripc_accum="";
      }
      else {
	if((c.ascii()!='\r')&&(c.ascii()!='\n')) {
	  ripc_accum+=c;
	}
      }
    }
  }
}


void RDRipc::SendCommand(const QString &cmd)
{
  //  printf("RDRipc::SendCommand(%s)\n",(const char *)cmd.toUtf8());
  ripc_socket->write(cmd.toUtf8());
}


void RDRipc::DispatchCommand()
{
  RDMacro macro;
  QString str;

  //  printf("RDRipc::DispatchCommand: %s\n",(const char *)ripc_accum.toUtf8());
  QStringList cmds=ripc_accum.split(" ",QString::SkipEmptyParts);

  if(cmds.size()==0) {
    return;
  }
  
  if(cmds[0]=="PW") {  // Password Response
    SendCommand("RU!");
  }

  if((cmds[0]=="RU")&&(cmds.size()==2)) {  // User Identity
    if(cmds[1]!=ripc_user) {
      ripc_user=cmds[1];
      if(!ripc_connected) {
	ripc_connected=true;
	emit connected(true);
      }
      emit userChanged();
    }
  }

  if(cmds[0]=="MS") {  // Macro Sent
    if(cmds.size()<4) {
      return;
    }
    str=cmds[3];
    for(int i=4;i<cmds.size();i++) {
      str+=" "+cmds[i];
    }
    str+="!";
    macro=RDMacro::fromString(str,RDMacro::Cmd);
    if(!macro.isNull()) {
      QHostAddress addr;
      addr.setAddress(cmds[1]);
      if(cmds[2].left(1)=="1") {
	macro.setEchoRequested(true);
      }
      macro.setAddress(addr);
      emit rmlReceived(&macro);
    }
    return;
  }

  if(cmds[0]=="ME") {  // Macro Echoed
    if(cmds.size()<4) {
      return;
    }
    str=cmds[3];
    for(int i=4;i<cmds.size();i++) {
      str+=" "+cmds[i];
    }
    str+="!";
    macro=RDMacro::fromString(str,RDMacro::Reply);
    if(!macro.isNull()) {
      macro.setAddress(QHostAddress(cmds[1]));
      macro.setRole(RDMacro::Reply);
      emit rmlReceived(&macro);
    }
    return;
  }

  if(cmds[0]=="GI") {   // GPI State Changed
    if(cmds.size()<4) {
      return;
    }
    int matrix=cmds[1].toInt();
    int line=cmds[2].toInt();
    int mask=cmds[4].toInt();
    if((mask>0)||ripc_ignore_mask) {
      if(cmds[3].left(1)=="0") {
	emit gpiStateChanged(matrix,line,false);
      }
      else {
	emit gpiStateChanged(matrix,line,true);
      }
    }
  }

  if(cmds[0]=="GO") {   // GPO State Changed
    if(cmds.size()<4) {
      return;
    }
    int matrix=cmds[1].toInt();
    int line=cmds[2].toInt();
    int mask=cmds[4].toInt();
    if((mask>0)||ripc_ignore_mask) {
      if(cmds[3].left(1)=="0") {
	emit gpoStateChanged(matrix,line,false);
      }
      else {
	emit gpoStateChanged(matrix,line,true);
      }
    }
  }

  if(cmds[0]=="GM") {   // GPI Mask Changed
    if(cmds.size()<4) {
      return;
    }
    int matrix=cmds[1].toInt();
    int line=cmds[2].toInt();
    if(cmds[3].left(1)=="0") {
      emit gpiMaskChanged(matrix,line,false);
    }
    else {
      emit gpiMaskChanged(matrix,line,true);
    }
  }

  if(cmds[0]=="GN") {   // GPO Mask Changed
    if(cmds.size()<4) {
      return;
    }
    int matrix=cmds[1].toInt();
    int line=cmds[2].toInt();
    if(cmds[3].left(1)=="0") {
      emit gpoMaskChanged(matrix,line,false);
    }
    else {
      emit gpoMaskChanged(matrix,line,true);
    }
  }

  if(cmds[0]=="GC") {   // GPI Cart Changed
    if(cmds.size()<5) {
      return;
    }
    int matrix=cmds[1].toInt();
    int line=cmds[2].toInt();
    unsigned off_cartnum=cmds[3].toUInt();
    unsigned on_cartnum=cmds[4].toUInt();
    emit gpiCartChanged(matrix,line,off_cartnum,on_cartnum);
  }

  if(cmds[0]=="GD") {   // GPO Cart Changed
    if(cmds.size()<5) {
      return;
    }
    int matrix=cmds[1].toInt();
    int line=cmds[2].toInt();
    unsigned off_cartnum=cmds[3].toUInt();
    unsigned on_cartnum=cmds[4].toUInt();
    emit gpoCartChanged(matrix,line,off_cartnum,on_cartnum);
  }

  if(cmds[0]=="TA") {   // On Air Flag Changed
    if(cmds.size()!=2) {
      return;
    }
    ripc_onair_flag=cmds[1].left(1)=="1";
    emit onairFlagChanged(ripc_onair_flag);
  }

  if(cmds[0]=="ON") {   // Notification Received
    if(cmds.size()<4) {
      return;
    }
    QString msg;
    for(int i=1;i<cmds.size();i++) {
      msg+=QString(cmds[i])+" ";
    }
    msg=msg.left(msg.length()-1);
    RDNotification *notify=new RDNotification();
    if(!notify->read(msg)) {
      delete notify;
      return;
    }
    emit notificationReceived(notify);
    delete notify;
  }
}
