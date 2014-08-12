// rdripc.cpp
//
// Connection to the Rivendell Interprocess Communication Daemon
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdripc.cpp,v 1.36.6.2 2013/03/09 00:21:11 cvs Exp $
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

#include <qobject.h>
#include <qapplication.h>

#include <rddb.h>
#include <rdripc.h>


RDRipc::RDRipc(QString stationname,QObject *parent,const char *name)
  : QObject(parent,name)
{
  ripc_stationname=stationname;
  ripc_onair_flag=false;
  ripc_ignore_mask=false;
  debug=false;
  argnum=0;
  argptr=0;

  ripc_connected=false;

  //
  // TCP Connection
  //
  ripc_socket=new QSocket(this,"ripc_socket");
  connect(ripc_socket,SIGNAL(connected()),this,SLOT(connectedData()));
  connect(ripc_socket,SIGNAL(error(int)),this,SLOT(errorData(int)));
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
  return ripc_stationname;
}


bool RDRipc::onairFlag() const
{
  return ripc_onair_flag;
}


void RDRipc::setUser(QString user)
{
  SendCommand(QString().sprintf("SU %s!",(const char *)user));
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
  SendCommand(QString().sprintf("PW %s!",(const char *)ripc_password));
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


void RDRipc::sendOnairFlag()
{
  SendCommand("TA!");
}


void RDRipc::sendRml(RDMacro *macro)
{
  char buffer[RD_RML_MAX_LENGTH];
  char cmd[RD_RML_MAX_LENGTH+4];
  /*
  int echo=0;

  if(macro->echoRequested()) {
    echo=1;
  }
  */
  Q_UINT16 port=RD_RML_NOECHO_PORT;

  if(macro->echoRequested()) {
    port=RD_RML_ECHO_PORT;
  }
  if(macro->port()>0) {
    port=macro->port();
  }
  macro->generateString(buffer,RD_RML_MAX_LENGTH-1);
  QString rmlline(buffer);
  QString sql=QString().sprintf("select NAME,VARVALUE from HOSTVARS \
                                   where STATION_NAME=\"%s\"",
				(const char *)ripc_stationname);
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    rmlline.replace(q->value(0).toString(),q->value(1).toString());
  }
  delete q;
  switch(macro->role()) {
      case RDMacro::Cmd:
	sprintf(cmd,"MS %s %d %s",(const char *)macro->address().toString(),
		port,(const char *)rmlline);
	break;
	
      case RDMacro::Reply:
	sprintf(cmd,"ME %s %d %s",(const char *)macro->address().toString(),
		port,(const char *)rmlline);
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


void RDRipc::errorData(int errorcode)
{
}


void RDRipc::readyData()
{
  char buf[255];
  int c;

  while((c=ripc_socket->readBlock(buf,256))>0) {
    buf[c]=0;
    for(int i=0;i<c;i++) {
      if(buf[i]==' ') {
	if(argnum<RIPC_MAX_ARGS) {
	  args[argnum][argptr]=0;
	  argptr=0;
	  argnum++;
	}
	else {
	  if(debug) {
	    printf("Argument list truncated!\n");
	  }
	}
      }
      if(buf[i]=='!') {
	args[argnum++][argptr]=0;
	DispatchCommand();
	argnum=0;
	argptr=0;
	if(ripc_socket==NULL) {
	  return;
	}
      }
      if((isgraph(buf[i]))&&(buf[i]!='!')) {
	if(argptr<RIPC_MAX_LENGTH) {
	  args[argnum][argptr]=buf[i];
	  argptr++;
	}
	else {
	  if(debug) {
	    printf("WARNING: argument truncated!\n");
	  }
	}
      }
    }
  }
}


void RDRipc::SendCommand(QString cmd)
{
  // printf("RDRipc::SendCommand(%s)\n",(const char *)cmd);
  ripc_socket->writeBlock((const char *)cmd,cmd.length());
}


void RDRipc::DispatchCommand()
{
  RDMacro macro;
  char str[RD_RML_MAX_LENGTH];

  if(!strcmp(args[0],"PW")) {   // Password Response
    SendCommand(QString("RU!"));
  }

  if(!strcmp(args[0],"RU")) {   // User Identity
    if(QString(args[1])!=ripc_user) {
      ripc_user=QString(args[1]);
      if(!ripc_connected) {
	ripc_connected=true;
	emit connected(true);
      }
      emit userChanged();
    }
  }

  if(!strcmp(args[0],"MS")) {   // Macro Sent
    if(argnum<4) {
      return;
    }
    strcpy(str,args[3]);
    for(int i=4;i<argnum;i++) {
      strcat(str," ");
      strcat(str,args[i]);
    }
    strcat(str,"!");
    if(macro.parseString(str,strlen(str))) {
      QHostAddress addr;
      addr.setAddress(args[1]);
      if(args[2][0]=='1') {
	macro.setEchoRequested(true);
      }
      macro.setAddress(addr);
      macro.setRole(RDMacro::Cmd);
      emit rmlReceived(&macro);
    }
    return;
  }

  if(!strcmp(args[0],"ME")) {   // Macro Echoed
    if(argnum<4) {
      return;
    }
    strcpy(str,args[3]);
    for(int i=4;i<argnum;i++) {
      strcat(str," ");
      strcat(str,args[i]);
    }
    strcat(str,"!");
    if(macro.parseString(str,strlen(str))) {
      macro.setAddress(QHostAddress().setAddress(args[1]));
      macro.setRole(RDMacro::Reply);
      emit rmlReceived(&macro);
    }
    return;
  }

  if(!strcmp(args[0],"GI")) {   // GPI State Changed
    if(argnum<4) {
      return;
    }
    int matrix;
    int line;
    int mask;
    sscanf(args[1],"%d",&matrix);
    sscanf(args[2],"%d",&line);
    sscanf(args[4],"%d",&mask);
    if((mask>0)||ripc_ignore_mask) {
      if(args[3][0]=='0') {
	emit gpiStateChanged(matrix,line,false);
      }
      else {
	emit gpiStateChanged(matrix,line,true);
      }
    }
  }

  if(!strcmp(args[0],"GO")) {   // GPO State Changed
    if(argnum<4) {
      return;
    }
    int matrix;
    int line;
    int mask;
    sscanf(args[1],"%d",&matrix);
    sscanf(args[2],"%d",&line);
    sscanf(args[4],"%d",&mask);
    if((mask>0)||ripc_ignore_mask) {
      if(args[3][0]=='0') {
	emit gpoStateChanged(matrix,line,false);
      }
      else {
	emit gpoStateChanged(matrix,line,true);
      }
    }
  }

  if(!strcmp(args[0],"GM")) {   // GPI Mask Changed
    if(argnum<4) {
      return;
    }
    int matrix;
    int line;
    sscanf(args[1],"%d",&matrix);
    sscanf(args[2],"%d",&line);
    if(args[3][0]=='0') {
      emit gpiMaskChanged(matrix,line,false);
    }
    else {
      emit gpiMaskChanged(matrix,line,true);
    }
  }

  if(!strcmp(args[0],"GN")) {   // GPO Mask Changed
    if(argnum<4) {
      return;
    }
    int matrix;
    int line;
    sscanf(args[1],"%d",&matrix);
    sscanf(args[2],"%d",&line);
    if(args[3][0]=='0') {
      emit gpoMaskChanged(matrix,line,false);
    }
    else {
      emit gpoMaskChanged(matrix,line,true);
    }
  }

  if(!strcmp(args[0],"GC")) {   // GPI Cart Changed
    if(argnum<5) {
      return;
    }
    int matrix;
    int line;
    unsigned off_cartnum; 
    unsigned on_cartnum; 
    sscanf(args[1],"%d",&matrix);
    sscanf(args[2],"%d",&line);
    sscanf(args[3],"%d",&off_cartnum);
    sscanf(args[4],"%d",&on_cartnum);
    emit gpiCartChanged(matrix,line,off_cartnum,on_cartnum);
  }

  if(!strcmp(args[0],"GD")) {   // GPO Cart Changed
    if(argnum<5) {
      return;
    }
    int matrix;
    int line;
    unsigned off_cartnum; 
    unsigned on_cartnum; 
    sscanf(args[1],"%d",&matrix);
    sscanf(args[2],"%d",&line);
    sscanf(args[3],"%d",&off_cartnum);
    sscanf(args[4],"%d",&on_cartnum);
    emit gpoCartChanged(matrix,line,off_cartnum,on_cartnum);
  }

  if(!strcmp(args[0],"TA")) {   // On Air Flag Changed
    if(argnum!=2) {
      return;
    }
    emit onairFlagChanged(args[1][0]=='1');
  }

  if(!strcmp(args[0],"TA")) {   // On Air Flag Changed
    if(argnum!=2) {
      return;
    }
    ripc_onair_flag=args[1][0]=='1';
    emit onairFlagChanged(ripc_onair_flag);
  }
}
