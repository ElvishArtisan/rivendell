// vguest.cpp
//
// A Rivendell switcher driver for the Logitek vGuest Protocol
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: vguest.cpp,v 1.36.8.3 2013/11/07 23:00:00 cvs Exp $
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

#include <stdlib.h>

#include <qsignalmapper.h>

#include <rddb.h>
#include <globals.h>
#include <vguest.h>


VGuest::VGuest(RDMatrix *matrix,QObject *parent,const char *name)
  : Switcher(matrix,parent,name)
{
  RDTty *tty;
  QString sql;
  RDSqlQuery *q;
  int n;

  for(int i=0;i<2;i++) {
    vguest_device[i]=NULL;
    vguest_socket[i]=NULL;
    vguest_error_notified[i]=false;
  }

  //
  // Get Matrix Parameters
  //
  vguest_matrix=matrix->matrix();
  vguest_device[0]=NULL;
  vguest_device[1]=NULL;
  vguest_socket[0]=NULL;
  vguest_socket[1]=NULL;
  vguest_porttype[0]=matrix->portType(RDMatrix::Primary);
  vguest_porttype[1]=matrix->portType(RDMatrix::Backup);
  vguest_ipaddress[0]=matrix->ipAddress(RDMatrix::Primary);
  vguest_ipaddress[1]=matrix->ipAddress(RDMatrix::Backup);
  vguest_username[0]=PadString(matrix->username(RDMatrix::Primary),16);
  vguest_username[1]=PadString(matrix->username(RDMatrix::Backup),16);
  vguest_password[0]=PadString(matrix->password(RDMatrix::Primary),16);
  vguest_password[1]=PadString(matrix->password(RDMatrix::Backup),16);
  vguest_start_cart[0]=matrix->startCart(RDMatrix::Primary);
  vguest_start_cart[1]=matrix->startCart(RDMatrix::Backup);
  vguest_stop_cart[0]=matrix->stopCart(RDMatrix::Primary);
  vguest_stop_cart[1]=matrix->stopCart(RDMatrix::Backup);
  vguest_ipport[0]=matrix->ipPort(RDMatrix::Primary);
  vguest_ipport[1]=matrix->ipPort(RDMatrix::Backup);
  vguest_inputs=matrix->inputs();
  vguest_outputs=matrix->outputs();
  vguest_gpis=matrix->gpis();
  vguest_gpos=matrix->gpos();

  //
  // Load Engine Data - Inputs
  //
  sql=QString().sprintf("select NUMBER,ENGINE_NUM,DEVICE_NUM from INPUTS where\
                         (STATION_NAME=\"%s\")&&(MATRIX=%d) order by NUMBER",
			(const char *)matrix->station(),matrix->matrix());
  q=new RDSqlQuery(sql);
  n=1;
  while(q->next()) {
    while(q->value(0).toInt()>n) {
      vguest_input_engine_nums.push_back(-1);
      vguest_input_device_nums.push_back(-1);
      n++;
    }
    vguest_input_engine_nums.push_back(q->value(1).toInt());
    vguest_input_device_nums.push_back(q->value(2).toInt());
    n++;
  }
  delete q;

  //
  // Load Engine Data - Outputs
  //
  sql=
    QString().sprintf("select NUMBER,ENGINE_NUM,DEVICE_NUM from OUTPUTS where\
                       (STATION_NAME=\"%s\")&&(MATRIX=%d) order by NUMBER",
		      (const char *)matrix->station(),matrix->matrix());
  q=new RDSqlQuery(sql);
  n=1;
  while(q->next()) {
    while(q->value(0).toInt()>n) {
      vguest_output_engine_nums.push_back(-1);
      vguest_output_device_nums.push_back(-1);
      n++;
    }
    vguest_output_engine_nums.push_back(q->value(1).toInt());
    vguest_output_device_nums.push_back(q->value(2).toInt());
    n++;
  }
  delete q;

  //
  // Load Engine Data - Relays
  //
  sql=
    QString().sprintf("select NUMBER,ENGINE_NUM,DEVICE_NUM,SURFACE_NUM,\
                       RELAY_NUM from VGUEST_RESOURCES where\
                       (STATION_NAME=\"%s\")&&(MATRIX_NUM=%d)&&\
                       (VGUEST_TYPE=%d) order by NUMBER",
		      (const char *)matrix->station(),matrix->matrix(),
		      RDMatrix::VguestTypeRelay);
  q=new RDSqlQuery(sql);
  n=1;
  while(q->next()) {
    while(q->value(0).toInt()>n) {
      vguest_relays_engine_nums.push_back(-1);
      vguest_relays_device_nums.push_back(-1);
      vguest_relays_surface_nums.push_back(-1);
      vguest_relays_relay_nums.push_back(-1);
      n++;
    }
    vguest_relays_engine_nums.push_back(q->value(1).toInt());
    vguest_relays_device_nums.push_back(q->value(2).toInt());
    vguest_relays_surface_nums.push_back(q->value(3).toInt());
    vguest_relays_relay_nums.push_back(q->value(4).toInt());
    n++;
  }
  delete q;

  //
  // Load Engine Data - Displays
  //
  sql=
    QString().sprintf("select NUMBER,ENGINE_NUM,DEVICE_NUM,SURFACE_NUM\
                       from VGUEST_RESOURCES where\
                       (STATION_NAME=\"%s\")&&(MATRIX_NUM=%d)&&\
                       (VGUEST_TYPE=%d) order by NUMBER",
		      (const char *)matrix->station(),matrix->matrix(),
		      RDMatrix::VguestTypeDisplay);
  q=new RDSqlQuery(sql);
  n=1;
  while(q->next()) {
    while(q->value(0).toInt()>n) {
      vguest_displays_engine_nums.push_back(-1);
      vguest_displays_device_nums.push_back(-1);
      vguest_displays_surface_nums.push_back(-1);
      n++;
    }
    vguest_displays_engine_nums.push_back(q->value(1).toInt());
    vguest_displays_device_nums.push_back(q->value(2).toInt());
    vguest_displays_surface_nums.push_back(q->value(3).toInt());
    n++;
  }
  delete q;

  //
  // Ping Timers
  //
  vguest_ping_mapper=new QSignalMapper(this);
  connect(vguest_ping_mapper,SIGNAL(mapped(int)),this,SLOT(pingData(int)));
  vguest_ping_response_mapper=new QSignalMapper(this);
  connect(vguest_ping_response_mapper,SIGNAL(mapped(int)),
	  this,SLOT(pingResponseData(int)));
  for(int i=0;i<2;i++) {
    vguest_ping_timer[i]=new QTimer(this);
    vguest_ping_mapper->setMapping(vguest_ping_timer[i],i);
    connect(vguest_ping_timer[i],SIGNAL(timeout()),
	    vguest_ping_mapper,SLOT(map()));

    vguest_ping_response_timer[i]=new QTimer(this);
    vguest_ping_response_mapper->setMapping(vguest_ping_timer[i],i);
    connect(vguest_ping_response_timer[i],SIGNAL(timeout()),
	    vguest_ping_response_mapper,SLOT(map()));
  }

  //
  // Reconnection Timer
  //
  QSignalMapper *reconnect_mapper=new QSignalMapper(this);
  connect(reconnect_mapper,SIGNAL(mapped(int)),
	  this,SLOT(ipConnect(int)));
  for(int i=0;i<2;i++) {
    vguest_reconnect_timer[i]=new QTimer(this);
    reconnect_mapper->setMapping(vguest_reconnect_timer[i],i);
    connect(vguest_reconnect_timer[i],SIGNAL(timeout()),
	    reconnect_mapper,SLOT(map()));
  }

  //
  // Interval OneShots
  //
  vguest_gpio_oneshot=new RDOneShot(this);
  connect(vguest_gpio_oneshot,SIGNAL(timeout(void *)),
	  this,SLOT(gpioOneshotData(void*)));

  //
  // Initialize the connection
  //
  for(int i=0;i<2;i++) {
    if(vguest_porttype[i]==RDMatrix::TtyPort) {
      tty=new RDTty(rdstation->name(),matrix->port((RDMatrix::Role)i));
      vguest_device[i]=new RDTTYDevice();
      if(tty->active()) {
	vguest_device[i]->setName(tty->port());
	vguest_device[i]->setSpeed(tty->baudRate());
	vguest_device[i]->setWordLength(tty->dataBits());
	vguest_device[i]->setParity(tty->parity());
	vguest_device[i]->open(IO_Raw|IO_ReadWrite);
      }
      delete tty;
    }
    else {
      if(vguest_porttype[i]==RDMatrix::TcpPort) {
	vguest_socket[i]=new RDSocket(i,this);
	connect(vguest_socket[i],SIGNAL(connectedID(int)),
		this,SLOT(connectedData(int)));
	connect(vguest_socket[i],SIGNAL(connectionClosedID(int)),
		this,SLOT(connectionClosedData(int)));
	connect(vguest_socket[i],SIGNAL(readyReadID(int)),
		this,SLOT(readyReadData(int)));
	connect(vguest_socket[i],SIGNAL(errorID(int,int)),
		this,SLOT(errorData(int,int)));
	ipConnect(i);
      }
    }
  }
}


VGuest::~VGuest()
{
  delete vguest_gpio_oneshot;
  for(int i=0;i<2;i++) {
    delete vguest_reconnect_timer[i];
    delete vguest_ping_response_timer[i];
    delete vguest_ping_timer[i];
    if(vguest_device[i]!=NULL) {
      delete vguest_device[i];
    }
    if(vguest_socket[i]!=NULL) {
      delete vguest_socket[i];
    }
  }
}


RDMatrix::Type VGuest::type()
{
  return RDMatrix::LogitekVguest;
}


unsigned VGuest::gpiQuantity()
{
  return vguest_gpis;
}


unsigned VGuest::gpoQuantity()
{
  return vguest_gpos;
}


bool VGuest::primaryTtyActive()
{
  return vguest_porttype[0]==RDMatrix::TtyPort;
}


bool VGuest::secondaryTtyActive()
{
  return vguest_porttype[1]==RDMatrix::TtyPort;
}


void VGuest::processCommand(RDMacro *cmd)
{
  char buffer[VGUEST_MAX_COMMAND_LENGTH];
  char cmd_byte=0;
  QString label;

  switch(cmd->command()) {
      case RDMacro::SD:
	if((cmd->argQuantity()<5)||
	   (cmd->arg(1).toUInt()>vguest_displays_engine_nums.size())) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  LogLine(RDConfig::LogWarning,"*** not enough vGuest arguments ***");
	  return;
	}
	if((vguest_displays_engine_nums[cmd->arg(1).toInt()-1]<0)||
	   (vguest_displays_device_nums[cmd->arg(1).toInt()-1]<0)||
	   (vguest_displays_surface_nums[cmd->arg(1).toInt()-1]<0)) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  LogLine(RDConfig::LogWarning,"*** invalid vGuest hex parameters ***");
	  return;
	}
	label=cmd->rollupArgs(5).left(VGUEST_MAX_TEXT_LENGTH);
	sprintf(buffer,"\x02%c\x5C%c%c%c%c%c%c%c%s",8+label.length(),
		(char)vguest_displays_engine_nums[cmd->arg(1).toInt()-1],
		(char)(vguest_displays_device_nums[cmd->arg(1).toInt()-1]>>8),
		(char)(vguest_displays_device_nums[cmd->arg(1).toInt()-1]&0xFF),
		(char)vguest_displays_surface_nums[cmd->arg(1).toInt()-1],
		(char)(0xFF&cmd->arg(2).toInt()),
		(char)(0xFF&cmd->arg(3).toInt()),
		(char)(0xFF&cmd->arg(4).toInt()),
		(const char *)label);
	SendCommand(buffer,10+label.length());
	break;

      case RDMacro::ST:
	if((cmd->arg(1).toInt()<1)||(cmd->arg(1).toInt()>vguest_inputs)||
	   (cmd->arg(2).toInt()<1)||(cmd->arg(2).toInt()>vguest_outputs)) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}
	if((vguest_input_engine_nums[cmd->arg(1).toInt()-1]<0)||
	   (vguest_input_device_nums[cmd->arg(1).toInt()-1]<0)||
	   (vguest_output_engine_nums[cmd->arg(2).toInt()-1]<0)||
	   (vguest_output_device_nums[cmd->arg(2).toInt()-1]<0)) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}
	sprintf(buffer,"\x02\x08\x54%c%c%c%c%c%c%c",
		(char)vguest_output_engine_nums[cmd->arg(2).toInt()-1],
		(char)(vguest_output_device_nums[cmd->arg(2).toInt()-1]>>8),
		(char)(vguest_output_device_nums[cmd->arg(2).toInt()-1]&0xFF),
		VGUEST_DEFAULT_SURFACE_NUMBER,
		(char)vguest_input_engine_nums[cmd->arg(1).toInt()-1],
		(char)(vguest_input_device_nums[cmd->arg(1).toInt()-1]>>8),
		(char)(vguest_input_device_nums[cmd->arg(1).toInt()-1]&0xFF));
	SendCommand(buffer,10);
	cmd->acknowledge(true);
	emit rmlEcho(cmd);
	break;

      case RDMacro::GO:
	if(((cmd->arg(1).toString().lower()!="i")&&
	    (cmd->arg(1).toString().lower()!="o"))||
	   (cmd->arg(2).toInt()<1)||(cmd->arg(2).toInt()>vguest_gpos)||
	   (cmd->arg(3).toInt()<0)||(cmd->arg(3).toInt()>1)||
	   (cmd->arg(4).toInt()<0)) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}
	if(cmd->arg(4).toInt()>0) {
	  cmd_byte=0x51;
	}
	else {
	  switch(cmd->arg(3).toInt()) {
	      case 0:
		cmd_byte=0x53;
		break;

	      case 1:
		cmd_byte=0x52;
		break;
	  }
	}
	if((vguest_relays_engine_nums[cmd->arg(2).toInt()-1]<0)||
	   (vguest_relays_device_nums[cmd->arg(2).toInt()-1]<0)||
	   (vguest_relays_surface_nums[cmd->arg(2).toInt()-1]<0)||
	   (vguest_relays_relay_nums[cmd->arg(2).toInt()-1]<0)) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}
	switch(0xFF&cmd_byte) {
	    case 0x51:
	      sprintf(buffer,"\x02\x07\x51%c%c%c%c%c%c",
		      vguest_relays_engine_nums[cmd->arg(2).toInt()-1],
		      vguest_relays_device_nums[cmd->arg(2).toInt()-1]>>8,
		      vguest_relays_device_nums[cmd->arg(2).toInt()-1]&0xFF,
		      vguest_relays_surface_nums[cmd->arg(2).toInt()-1],
		      vguest_relays_relay_nums[cmd->arg(2).toInt()-1],
		      cmd->arg(4).toInt()/50);
	      SendCommand(buffer,9);
	      emit gpiChanged(vguest_matrix,cmd->arg(2).toInt()-1,true);
	      emit gpoChanged(vguest_matrix,cmd->arg(2).toInt()-1,true);
	      vguest_gpio_oneshot->start((void *)(cmd->arg(2).toInt()-1),2000);
	      break;

	    case 0x52:
	    case 0x53:
	      sprintf(buffer,"\x02\x06%c%c%c%c%c%c",
		      cmd_byte,
		      vguest_relays_engine_nums[cmd->arg(2).toInt()-1],
		      vguest_relays_device_nums[cmd->arg(2).toInt()-1]>>8,
		      vguest_relays_device_nums[cmd->arg(2).toInt()-1]&0xFF,
		      vguest_relays_surface_nums[cmd->arg(2).toInt()-1],
		      vguest_relays_relay_nums[cmd->arg(2).toInt()-1]);
	      SendCommand(buffer,8);
	      emit gpiChanged(vguest_matrix,cmd->arg(2).toInt()-1,
			      cmd->arg(3).toInt());
	      emit gpoChanged(vguest_matrix,cmd->arg(2).toInt()-1,
			      cmd->arg(3).toInt());
	}
	cmd->acknowledge(true);
	emit rmlEcho(cmd);
	break;

      default:
	cmd->acknowledge(false);
	emit rmlEcho(cmd);
	break;
  }
}


void VGuest::ipConnect(int id)
{
  if(!vguest_ipaddress[id].isNull()) {
    vguest_socket[id]->
      connectToHost(vguest_ipaddress[id].toString(),vguest_ipport[id]);
  }
}


void VGuest::connectedData(int id)
{
  vguest_istate[id]=0;
}


void VGuest::connectionClosedData(int id)
{
  int interval=GetHoldoff();
  if(!vguest_error_notified[id]) {
    LogLine(RDConfig::LogNotice,QString().
	    sprintf("Connection to vGuest device at %s:%d closed, attempting reconnect",
		    (const char *)vguest_ipaddress[id].toString(),
		    vguest_ipport[id]));
    vguest_error_notified[id]=true;
  }
  if(vguest_stop_cart[id]>0) {
    ExecuteMacroCart(vguest_stop_cart[id]);
  }
  vguest_ping_timer[id]->stop();
  vguest_ping_response_timer[id]->stop();
  vguest_reconnect_timer[id]->start(interval,true);
}


void VGuest::readyReadData(int id)
{
  char buffer[255];
  int n=0;

  while((n=vguest_socket[id]->readBlock(buffer,255))>0) {
    for(int i=0;i<n;i++) {
      switch(vguest_istate[id]) {
      case 0:   // STX Command Start
	switch(buffer[i]) {
	case 0x02:
	  vguest_istate[id]=1;
	  break;

	case 0x04:
	  vguest_istate[id]=11;
	  break;
	}
	break;

      case 1:   // LP2 Command Length
	vguest_cmd_length[id]=buffer[i];
	vguest_cmd_buffer[id][0]=2;
	vguest_cmd_buffer[id][1]=buffer[i];
	vguest_cmd_ptr[id]=2;
	vguest_istate[id]=2;
	break;

      case 2:   // LP2 Command Body
	vguest_cmd_buffer[id][vguest_cmd_ptr[id]++]=buffer[i];
	if(vguest_cmd_ptr[id]==(vguest_cmd_length[id]+2)) {
	  DispatchCommand(vguest_cmd_buffer[id],vguest_cmd_length[id]+2,id);
	  vguest_istate[id]=0;
	}
	break;

      case 11:   // Metadata Command Length
	vguest_cmd_length[id]=buffer[i];
	vguest_cmd_buffer[id][0]=2;
	vguest_cmd_buffer[id][1]=buffer[i];
	vguest_cmd_ptr[id]=2;
	vguest_istate[id]=12;
	break;

      case 12:   // LP2 Command Body
	vguest_cmd_buffer[id][vguest_cmd_ptr[id]++]=buffer[i];
	if(vguest_cmd_ptr[id]==(vguest_cmd_length[id]+2)) {
	  MetadataCommand(vguest_cmd_buffer[id],vguest_cmd_length[id]+2,id);
	  vguest_istate[id]=0;
	}
	break;
      }
    }
  }
}


void VGuest::errorData(int err,int id)
{
  int interval=VGUEST_RECONNECT_MIN_INTERVAL;

  switch((QSocket::Error)err) {
      case QSocket::ErrConnectionRefused:
	interval=GetHoldoff();
	if(!vguest_error_notified[id]) {
	  LogLine(RDConfig::LogNotice,QString().sprintf(
		 "Connection to vGuest device at %s:%d refused, attempting reconnect",
		 (const char *)vguest_ipaddress[id].toString(),
		 vguest_ipport[id]));
	  vguest_error_notified[id]=true;
	}
	vguest_reconnect_timer[id]->start(interval,true);
	break;

      case QSocket::ErrHostNotFound:
	if(!vguest_error_notified[id]) {
	  LogLine(RDConfig::LogWarning,QString().sprintf(
	    "Error on connection to vGuest device at %s:%d: Host Not Found",
	    (const char *)vguest_ipaddress[id].toString(),
	    vguest_ipport[id]));
	  vguest_error_notified[id]=true;
	}
	break;

      case QSocket::ErrSocketRead:
	if(!vguest_error_notified[id]) {
	  LogLine(RDConfig::LogWarning,QString().sprintf(
	    "Error on connection to vGuest device at %s:%d: Socket Read Error",
	    (const char *)vguest_ipaddress[id].toString(),
	    vguest_ipport[id]));
	  vguest_error_notified[id]=true;
	}
	break;
  }
}


void VGuest::gpioOneshotData(void *data)
{
  emit gpiChanged(vguest_matrix,(long)data,false);
  emit gpoChanged(vguest_matrix,(long)data,false);
}


void VGuest::pingData(int id)
{
  char buffer[VGUEST_MAX_COMMAND_LENGTH];

  buffer[0]=0x04;
  buffer[1]=0x01;
  buffer[2]=0x03;   // LPCore Connection Ping
  vguest_socket[id]->writeBlock(buffer,3);
  vguest_ping_response_timer[id]->start(VGUEST_PING_INTERVAL,true);
}


void VGuest::pingResponseData(int id)
{
  vguest_socket[id]->clearPendingData();
  vguest_socket[id]->close();
  LogLine(RDConfig::LogWarning,"vGuest connection to "+
	  vguest_ipaddress[id].toString()+" timed out, restarting connection");
}


void VGuest::SendCommand(char *str,int len)
{
  // LogLine(QString().sprintf("SENT: %s",(const char *)RenderCommand(str,len)));
  for(int i=0;i<2;i++) {
    switch(vguest_porttype[i]) {
      case RDMatrix::TtyPort:
	if(vguest_device[i]!=NULL) {
	  vguest_device[i]->writeBlock(str,len);
	}
	break;
	
      case RDMatrix::TcpPort:
	if(vguest_socket[i]!=NULL) {
	  vguest_socket[i]->writeBlock(str,len);
	}
	break;

      case RDMatrix::NoPort:
	break;
    }
  }
}


void VGuest::DispatchCommand(char *cmd,int len,int id)
{
  char buffer[VGUEST_MAX_COMMAND_LENGTH];
  QString str;
  int linenum;

  //  LogLine(RDConfig::LogNotice,
  //  QString().sprintf("RCVD: %s",(const char *)RenderCommand(cmd,len)));

  switch(0xFF&cmd[2]) {
  case 0xF9:   // Username/Password Query
    buffer[0]=0x02;
    buffer[1]=0x22;
    buffer[2]=0xF9;
    buffer[3]=VGUEST_ID_BYTE;
    sprintf(buffer+4,"%s%s",
	    (const char *)vguest_username[id],
	    (const char *)vguest_password[id]);
    SendCommand(buffer,36);
    break;

  case 0xF0:   // Connect Status
    switch(0xFF&cmd[3]) {
    case 0x0A:  // Valid connection
    case 0x14:
      LogLine(RDConfig::LogInfo,QString().sprintf(
	      "connection to vGuest device at %s:%d established",
	      (const char *)vguest_ipaddress[id].toString(),
	      vguest_ipport[id]));
      vguest_error_notified[id]=false;
      if(vguest_start_cart[id]>0) {
	ExecuteMacroCart(vguest_start_cart[id]);
      }
      if(vguest_socket[id]!=NULL) {
	buffer[0]=0x04;
	buffer[1]=0x01;
	buffer[2]=0x03;   // LPCore Connection Ping
	vguest_socket[id]->writeBlock(buffer,3);
      }
      break;

    case 0x0B:  // Invalid Username
    case 0x15:
      LogLine(RDConfig::LogWarning,QString().sprintf(
	      "connection to vGuest device at %s:%d refused: username invalid",
	      (const char *)vguest_ipaddress[id].toString(),
	      vguest_ipport[id]));
      vguest_socket[id]->close();
      connectionClosedData(id);
      break;
      
    case 0x0C:  // Invalid Password
    case 0x16:
      LogLine(RDConfig::LogWarning,QString().sprintf(
	      "connection to vGuest device at %s:%d refused: password invalid",
	      (const char *)vguest_ipaddress[id].toString(),
	      vguest_ipport[id]));
      vguest_socket[id]->close();
      connectionClosedData(id);
      break;

    case 0x0D:  // No vGuest Permission
    case 0x17:
      LogLine(RDConfig::LogWarning,QString().sprintf(
	   "connection to vGuest device at %s:%d refused: no vGuest permission",
	   (const char *)vguest_ipaddress[id].toString(),
	   vguest_ipport[id]));
      vguest_socket[id]->close();
      connectionClosedData(id);
      break;

    case 0x0E:  // No Profile
    case 0x18:
      LogLine(RDConfig::LogWarning,QString().sprintf(
	    "connection to vGuest device at %s:%d refused: no profile assigned",
	    (const char *)vguest_ipaddress[id].toString(),
	    vguest_ipport[id]));
      vguest_socket[id]->close();
      connectionClosedData(id);
      break;
    }
    break;

  case 0x52:   // Turn On
    if((linenum=GetRelay(0xFF&cmd[3],256*(0xFF&cmd[4])+(0xFF&cmd[5]),
			 0xFF&cmd[6],0xFF&cmd[7]))>=0) {
      emit gpiChanged(vguest_matrix,linenum,true);
      emit gpoChanged(vguest_matrix,linenum,true);
    }
    else {
      LogLine(RDConfig::LogDebug,QString().
	      sprintf("unhandled vGuest command received: %s",
		      (const char *)RenderCommand(cmd,len)));
    }
    break;

  case 0x53:   // Turn Off
    if((linenum=GetRelay(0xFF&cmd[3],256*(0xFF&cmd[4])+(0xFF&cmd[5]),
			 0xFF&cmd[6],0xFF&cmd[7]))>=0) {
      emit gpiChanged(vguest_matrix,linenum,false);
      emit gpoChanged(vguest_matrix,linenum,false);
    }
    else {
      LogLine(RDConfig::LogDebug,QString().
	      sprintf("unhandled vGuest command received: %s",
		      (const char *)RenderCommand(cmd,len)));
    }
    break;

  case 0x54:   // Input Assign
    break;
    
  case 0x55:   // Input Mode
    break;
    
  case 0x56:   // Fader Level
    break;
    
  default:
    LogLine(RDConfig::LogDebug,QString().
	    sprintf("unrecognized vGuest command received: %s",
		    (const char *)RenderCommand(cmd,len)));
    break;
  }
}


void VGuest::MetadataCommand(char *cmd,int len,int id)
{
  switch(0xFF&cmd[2]) {
  case 0x03:   // Connection Ping
    if(vguest_ping_response_timer[id]->isActive()) {
      vguest_ping_response_timer[id]->stop();
    }
    else {
      LogLine(RDConfig::LogDebug,"vGuest system at "+
	      vguest_ipaddress[id].toString()+
	      " understands ping, activating timeout monitoring");
    }
    vguest_ping_timer[id]->start(VGUEST_PING_INTERVAL,true);
    break;
  }
}


QString VGuest::PadString(QString str,unsigned len)
{
  QString out;
  out=str.left(len);
  while(out.length()<len) {
    out+=" ";
  }
  return out;
}


QString VGuest::RenderCommand(char *cmd,int len)
{
  QString str;

  for(int i=0;i<len;i++) {
    str+=QString().sprintf("%02X",0xFF&cmd[i]);
  }
  return str;
}


int VGuest::GetRelay(int enginenum,int devicenum,int surfacenum,int relaynum)
{
  for(unsigned i=0;i<vguest_relays_engine_nums.size();i++) {
/*
    LogLine(QString().sprintf("Checking Engine: %d | %d",vguest_relays_engine_nums[i],enginenum));
    LogLine(QString().sprintf("Checking Device: 0x%04X | 0x%04X",vguest_relays_device_nums[i],devicenum));
    LogLine(QString().sprintf("Checking Surface: 0x%04X | 0x%04X",vguest_relays_surface_nums[i],surfacenum));
    LogLine(QString().sprintf("Checking Relay: 0x%04X | 0x%04X",vguest_relays_relay_nums[i],relaynum));
*/
    if((vguest_relays_engine_nums[i]==enginenum)&&
       (vguest_relays_device_nums[i]==devicenum)&&
       (vguest_relays_surface_nums[i]==surfacenum)&&
       (vguest_relays_relay_nums[i]==relaynum)) {
//      LogLine("  MATCH!");
      return (int)i;
    }
  }
  return -1;
}


int VGuest::GetHoldoff()
{
  return (int)(VGUEST_RECONNECT_MIN_INTERVAL+
	       (VGUEST_RECONNECT_MAX_INTERVAL-VGUEST_RECONNECT_MIN_INTERVAL)*
	       (double)random()/(double)RAND_MAX);
}


void VGuest::ExecuteMacroCart(unsigned cartnum)
{
  RDMacro rml;
  rml.setRole(RDMacro::Cmd);
  rml.setCommand(RDMacro::EX);
  rml.setAddress(rdstation->address());
  rml.setEchoRequested(false);
  rml.setArgQuantity(1);
  rml.setArg(0,cartnum);
  emit rmlEcho(&rml);
}
