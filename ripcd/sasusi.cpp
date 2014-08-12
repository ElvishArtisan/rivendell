// sasusi.cpp
//
// A Rivendell switcher driver for the SAS User Serial Interface Protocol
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: sasusi.cpp,v 1.24 2011/12/28 18:59:19 cvs Exp $
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
#include <rddb.h>
#include <globals.h>
#include <sasusi.h>

SasUsi::SasUsi(RDMatrix *matrix,QObject *parent,const char *name)
  : Switcher(matrix,parent,name)
{
  QString sql;
  RDSqlQuery *q;

  RDTty *tty;
  sas_matrix=matrix->matrix();
  sas_ptr=0;

  //
  // Get Matrix Parameters
  //
  sas_porttype=matrix->portType(RDMatrix::Primary);
  sas_ipaddress=matrix->ipAddress(RDMatrix::Primary);
  sas_ipport=matrix->ipPort(RDMatrix::Primary);
  sas_inputs=matrix->inputs();
  sas_outputs=matrix->outputs();
  sas_gpis=matrix->gpis();
  sas_gpos=matrix->gpos();
  sas_start_cart=matrix->startCart(RDMatrix::Primary);
  sas_stop_cart=matrix->stopCart(RDMatrix::Primary);

  //
  // Load Switch Table
  //
  sql=QString().
    sprintf("select ENGINE_NUM,DEVICE_NUM,RELAY_NUM \
             from VGUEST_RESOURCES where (STATION_NAME=\"%s\")&&\
             (MATRIX_NUM=%d) order by NUMBER",
	    (const char *)ripcd_config->stationName(),
	    matrix->matrix());
  q=new RDSqlQuery(sql);
  while(q->next()) {
    sas_console_numbers.push_back(q->value(0).toInt());
    sas_source_numbers.push_back(q->value(1).toInt());
    sas_relay_numbers.push_back(q->value(2).toInt());
  }
  delete q;

  //
  // Reconnection Timer
  //
  sas_reconnect_timer=new QTimer(this,"sas_reconnect_timer");
  connect(sas_reconnect_timer,SIGNAL(timeout()),this,SLOT(ipConnect()));

  //
  // Initialize the connection
  //
  switch(sas_porttype) {
  case RDMatrix::TtyPort:
    tty=new RDTty(rdstation->name(),matrix->port(RDMatrix::Primary));
    sas_device=new RDTTYDevice();
    if(tty->active()) {
      sas_device->setName(tty->port());
      sas_device->setSpeed(tty->baudRate());
      sas_device->setWordLength(tty->dataBits());
      sas_device->setParity(tty->parity());
      sas_device->open(IO_Raw|IO_ReadWrite);
    }
    delete tty;

  case RDMatrix::TcpPort:
    sas_socket=new QSocket(this,"sas_socket");
    connect(sas_socket,SIGNAL(connected()),this,SLOT(connectedData()));
    connect(sas_socket,SIGNAL(connectionClosed()),
	    this,SLOT(connectionClosedData()));
    connect(sas_socket,SIGNAL(readyRead()),
	    this,SLOT(readyReadData()));
    connect(sas_socket,SIGNAL(error(int)),this,SLOT(errorData(int)));
    ipConnect();
    break;

  case RDMatrix::NoPort:
    break;
  }
}


RDMatrix::Type SasUsi::type()
{
  return RDMatrix::SasUsi;
}


unsigned SasUsi::gpiQuantity()
{
  return sas_gpis;
}


unsigned SasUsi::gpoQuantity()
{
  return sas_gpos;
}


bool SasUsi::primaryTtyActive()
{
  return sas_porttype==RDMatrix::TtyPort;
}


bool SasUsi::secondaryTtyActive()
{
  return false;
}


void SasUsi::processCommand(RDMacro *cmd)
{
  char str[256];
  char cmd_byte;
  QString label;

  switch(cmd->command()) {
      case RDMacro::CL:
	if((cmd->arg(1).toInt()<1)||
	   ((cmd->arg(1).toInt()>256)&&(cmd->arg(1).toInt()!=999))||
	   (cmd->arg(2).toInt()<1)||(cmd->arg(2).toInt()>sas_inputs)) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}
	for(int i=3;i<(cmd->argQuantity()-1);i++) {
	  label+=(cmd->arg(i).toString()+" ");
	}
	label+=cmd->arg(cmd->argQuantity()-1).toString();
	if(label.length()>8) {
	  label=label.left(8);
	}
	for(int i=label.length();i<8;i++) {
	  label+=" ";
	}
	snprintf(str,256,"%c21%03d%04d%s\x0D\x0A",26,
		cmd->arg(1).toInt(),cmd->arg(2).toInt(),(const char *)label);
	SendCommand(str);
	cmd->acknowledge(true);
	emit rmlEcho(cmd);
	break;

      case RDMacro::FS:
	if((cmd->arg(1).toInt()<1)||(cmd->arg(1).toInt()>256)||
	   (sas_porttype!=RDMatrix::TcpPort)) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}
	snprintf(str,256,"%c1%03d\x0D\x0A",0x13,cmd->arg(1).toInt());
	SendCommand(str);
	cmd->acknowledge(true);
	emit rmlEcho(cmd);
	break;

      case RDMacro::SG:
	if((cmd->arg(1).toInt()<1)||(cmd->arg(1).toInt()>sas_inputs)||
	   (cmd->arg(2).toInt()<1)||(cmd->arg(2).toInt()>sas_outputs)||
	   (cmd->arg(3).toInt()<-1024)||(cmd->arg(3).toInt()>1024)) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}
	snprintf(str,256,"%c00%04d%04d%04d00548\x0D\x0A",26,
		cmd->arg(1).toInt(),cmd->arg(2).toInt(),
		cmd->arg(3).toInt()+1024);
	SendCommand(str);
	cmd->acknowledge(true);
	emit rmlEcho(cmd);
	break;

      case RDMacro::SX:
	if((cmd->arg(1).toInt()<1)||(cmd->arg(1).toInt()>sas_inputs)||
	   (cmd->arg(2).toInt()<1)||(cmd->arg(2).toInt()>sas_outputs)||
	   (cmd->arg(3).toInt()<-1024)||(cmd->arg(3).toInt()>1024)) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}
	snprintf(str,256,"%c10%04d%04d%04d0010\x0D\x0A",26,
		cmd->arg(1).toInt(),cmd->arg(2).toInt(),
		cmd->arg(3).toInt()+1024);
	SendCommand(str);
	cmd->acknowledge(true);
	emit rmlEcho(cmd);
	break;

      case RDMacro::SL:
	if((cmd->arg(1).toInt()<1)||(cmd->arg(1).toInt()>sas_inputs)||
	   (cmd->arg(2).toInt()<-1024)||(cmd->arg(2).toInt()>1024)) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}
	snprintf(str,256,"%c10%04d0000%04d0001\x0D\x0A",26,
		cmd->arg(1).toInt(),cmd->arg(2).toInt()+1024);
	SendCommand(str);
	cmd->acknowledge(true);
	emit rmlEcho(cmd);
	break;

      case RDMacro::SA:
	if((cmd->arg(1).toInt()<1)||(cmd->arg(1).toInt()>sas_inputs)||
	   (cmd->arg(2).toInt()<1)||(cmd->arg(2).toInt()>sas_outputs)) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}
	snprintf(str,256,"%c00%04d%04d102400036\x0D\x0A",26,
		cmd->arg(1).toInt(),cmd->arg(2).toInt());
	SendCommand(str);
	cmd->acknowledge(true);
	emit rmlEcho(cmd);
	break;

      case RDMacro::SR:
	if((cmd->arg(1).toInt()<1)||(cmd->arg(1).toInt()>sas_inputs)||
	   (cmd->arg(2).toInt()<1)||(cmd->arg(2).toInt()>sas_outputs)) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}
	snprintf(str,256,"%c00%04d%04d102400032\x0D\x0A",26,
		cmd->arg(1).toInt(),cmd->arg(2).toInt());
	SendCommand(str);
	cmd->acknowledge(true);
	emit rmlEcho(cmd);
	break;

      case RDMacro::ST:
	if((cmd->arg(1).toInt()<0)||(cmd->arg(1).toInt()>sas_inputs)||
	   (cmd->arg(2).toInt()<1)||(cmd->arg(2).toInt()>sas_outputs)) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}
	snprintf(str,256,"%c%03d%03d\x0D\x0A",20,
		cmd->arg(1).toInt(),cmd->arg(2).toInt());
	SendCommand(str);
	cmd->acknowledge(true);
	emit rmlEcho(cmd);
	break;

      case RDMacro::GO:
	if((cmd->arg(1).toString().lower()!="o")||
	   (cmd->arg(2).toInt()<1)||(cmd->arg(2).toInt()>sas_gpos)) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}
	if(cmd->arg(4).toInt()==0) {   // Latch
	  if(cmd->arg(3).toInt()==0) {   // Off
	    cmd_byte=0x03;
	  }
	  else {
	    cmd_byte=0x02;
	  }
	}
	else {
	  if(cmd->arg(3).toInt()==0) {
	    cmd->acknowledge(false);
	    emit rmlEcho(cmd);
	    return;
	  }
	  cmd_byte=0x01;
	}
	if(cmd->arg(2).toUInt()<sas_relay_numbers.size()) {
	  if(sas_relay_numbers[cmd->arg(2).toUInt()-1]>=0) {
	    snprintf(str,256,"\x05R%d%04d\x0D\x0A",cmd_byte,
		    sas_relay_numbers[cmd->arg(2).toUInt()-1]);
	    SendCommand(str);
	    cmd->acknowledge(true);
	    emit rmlEcho(cmd);
	  }
	  else {
	    if((sas_console_numbers[cmd->arg(2).toUInt()-1]>=0)&&
	       (sas_source_numbers[cmd->arg(2).toUInt()-1]>=0)) {
	      if(cmd->arg(3).toInt()==0) {   // Off
		cmd_byte=0;
	      }
	      else {
		cmd_byte=1;
	      }
	      snprintf(str,256,"\x1A%s%d%03d%04d\x0D\x0A","20",cmd_byte,
		      sas_console_numbers[cmd->arg(2).toUInt()-1],
		      sas_source_numbers[cmd->arg(2).toUInt()-1]);
	      SendCommand(str);
	      cmd->acknowledge(true);
	      emit rmlEcho(cmd);
	    }
	  }
	}
	break;

      default:
	cmd->acknowledge(false);
	emit rmlEcho(cmd);
	break;
  }
}


void SasUsi::ipConnect()
{
  sas_socket->connectToHost(sas_ipaddress.toString(),sas_ipport);
}


void SasUsi::connectedData()
{
  LogLine(RDConfig::LogInfo,QString().
	  sprintf("Connection to SasUsi device at %s:%d established",
		  (const char *)sas_ipaddress.toString(),
		  sas_ipport));
  if(sas_start_cart>0) {
    ExecuteMacroCart(sas_start_cart);
  }
}


void SasUsi::connectionClosedData()
{
  LogLine(RDConfig::LogNotice,QString().
	  sprintf("Connection to SasUsi device at %s:%d closed unexpectedly, attempting reconnect",
		  (const char *)sas_ipaddress.toString(),
		  sas_ipport));
  if(sas_stop_cart>0) {
    ExecuteMacroCart(sas_stop_cart);
  }
  sas_reconnect_timer->start(SASUSI_RECONNECT_INTERVAL,true);
}


void SasUsi::readyReadData()
{
  char buffer[256];
  unsigned n;

  while((n=sas_socket->readBlock(buffer,255))>0) {
    buffer[n]=0;
    for(unsigned i=0;i<n;i++) {
      if(buffer[i]==10) {  // End of line
	sas_buffer[--sas_ptr]=0;
	DispatchCommand();
	sas_ptr=0;
      }
      else {
	if(sas_ptr==SASUSI_MAX_LENGTH) {  // Buffer overflow
	  sas_ptr=0;
	}
	sas_buffer[sas_ptr++]=buffer[i];
      }
    }
  }
}


void SasUsi::errorData(int err)
{
  switch((QSocket::Error)err) {
      case QSocket::ErrConnectionRefused:
	LogLine(RDConfig::LogNotice,QString().sprintf(
	  "Connection to SasUsi device at %s:%d refused, attempting reconnect",
		  (const char *)sas_ipaddress.toString(),
		  sas_ipport));
	sas_reconnect_timer->start(SASUSI_RECONNECT_INTERVAL,true);
	break;

      case QSocket::ErrHostNotFound:
	LogLine(RDConfig::LogWarning,QString().sprintf(
	  "Error on connection to SasUsi device at %s:%d: Host Not Found",
		  (const char *)sas_ipaddress.toString(),
		  sas_ipport));
	break;

      case QSocket::ErrSocketRead:
	LogLine(RDConfig::LogWarning,QString().sprintf(
	  "Error on connection to SasUsi device at %s:%d: Socket Read Error",
				  (const char *)sas_ipaddress.toString(),
				  sas_ipport));
	break;
  }
}


void SasUsi::SendCommand(char *str)
{
  LogLine(RDConfig::LogDebug,QString().sprintf("sending USI cmd: %s",(const char *)PrettifyCommand(str)));
  switch(sas_porttype) {
  case RDMatrix::TtyPort:
    sas_device->writeBlock(str,strlen(str));
    break;

  case RDMatrix::TcpPort:
    sas_socket->writeBlock(str,strlen(str));
    break;

  case RDMatrix::NoPort:
    break;
  }
}


void SasUsi::DispatchCommand()
{
  char buffer[SASUSI_MAX_LENGTH];
  unsigned input;
  unsigned output;
  int line;
  unsigned action;
  int console;
  int source;
  bool state;
  bool ok=false;
  QString cmd;
  QString label;
  QString sql;
  RDSqlQuery *q;

  //LogLine(RDConfig::LogNotice,QString().sprintf("DISPATCHED: %s",(const char *)sas_buffer));

  //
  // Startup Sequence.  Get the input and output lists.  The response
  // to the ^EI command lets us know when the lists are done.
  //
  if(QString("login sucessful")==(QString(sas_buffer).lower())) {
    sprintf(buffer,"%cX9999\x0D\x0A",5);  // Request Input List
    SendCommand(buffer);
    sprintf(buffer,"%cY9999\x0D\x0A",5);  // Request Output List
    SendCommand(buffer);
    sprintf(buffer,"%cI0001\x0D\x0A",5);  // Start Finished
    SendCommand(buffer);
    return;
  }

  //
  // Work around the idiotic 'SAS READY' prompt
  //
  if(sas_buffer[0]==27) {
    for(unsigned i=17;i<strlen(sas_buffer)+1;i++) {
      sas_buffer[i-17]=sas_buffer[i];
    }
  }

  LogLine(RDConfig::LogDebug,QString().sprintf("received USI cmd: %s",(const char *)PrettifyCommand(sas_buffer)));


  //
  // Process Commands
  //
  switch(sas_buffer[0]) {
  case 21:   // Input Name [^U]
    if(strlen(sas_buffer)<13) {
      return;
    }
    label=sas_buffer+5;
    sas_buffer[5]=0;
    if(sscanf(sas_buffer+1,"%u",&input)!=1) {
      return;
    }
    sql=QString().sprintf("select NUMBER from INPUTS where \
                           (STATION_NAME=\"%s\")&&	   \
                           (MATRIX=%d)&&(NUMBER=%d)",
			  (const char *)rdstation->name(),
			  sas_matrix,input);
    q=new RDSqlQuery(sql);
    if(q->first()) {
      sql=QString().sprintf("update INPUTS set NAME=\"%s\" where \
                            (STATION_NAME=\"%s\")&&\
                            (MATRIX=%d)&&(NUMBER=%d)",
			    (const char *)label,
			    (const char *)rdstation->name(),
			    sas_matrix,input);
    }
    else {
      sql=QString().sprintf("insert into INPUTS set NAME=\"%s\",\
                            STATION_NAME=\"%s\",MATRIX=%d,NUMBER=%d",
			    (const char *)label,
			    (const char *)rdstation->name(),
				sas_matrix,input);
    }
    delete q;
    q=new RDSqlQuery(sql);
    delete q;
    break;

  case 22:   // Output Name [^V]
    if(strlen(sas_buffer)<13) {
      return;
    }
    label=sas_buffer+5;
    sas_buffer[5]=0;
    if(sscanf(sas_buffer+1,"%u",&output)!=1) {
      return;
    }
    sql=QString().sprintf("select NUMBER from OUTPUTS where \
                           (STATION_NAME=\"%s\")&&\
                           (MATRIX=%d)&&(NUMBER=%d)",
			  (const char *)rdstation->name(),
			  sas_matrix,output);
    q=new RDSqlQuery(sql);
    if(q->first()) {
      sql=QString().sprintf("update OUTPUTS set NAME=\"%s\" where \
                             (STATION_NAME=\"%s\")&&\
                             (MATRIX=%d)&&(NUMBER=%d)",
			    (const char *)label,
			    (const char *)rdstation->name(),
			    sas_matrix,output);
    }
    else {
      sql=QString().sprintf("insert into OUTPUTS set NAME=\"%s\",\
                             STATION_NAME=\"%s\",MATRIX=%d,NUMBER=%d",
			    (const char *)label,
			    (const char *)rdstation->name(),
			    sas_matrix,output);
    }
    delete q;
    q=new RDSqlQuery(sql);
    delete q;
    break;

  case 'M':    // Console Module Action
    if(strlen(sas_buffer)<9) {
      return;
    }
    cmd=QString(sas_buffer);
    console=cmd.mid(2,3).toInt(&ok);
    if(!ok) {
      return;
    }
    source=cmd.mid(5,4).toInt(&ok);
    if(!ok) {
      return;
    }
    for(unsigned i=0;i<sas_console_numbers.size();i++) {
      if((console==sas_console_numbers[i])&&(source==sas_source_numbers[i])) {
	action=cmd.mid(1,1).toUInt(&ok);
	LogLine(RDConfig::LogNotice,QString().sprintf("action: %u",action));
	if(!ok) {
	  return;
	}
	switch(action) {
	case 0:    // Module OFF
	  emit gpiChanged(sas_matrix,i,false);	  
	  break;

	case 1:    // Module ON
	  emit gpiChanged(sas_matrix,i,true);	  
	  break;

	case 2:    // Cue OFF
	  break;

	case 3:    // Cue ON
	  break;
	}
      }
    }
    break;

  case 'Z':    // Opto/Relay Tally Update
    if(strlen(sas_buffer)<9) {
      return;
    }
    cmd=QString(sas_buffer);
    line=cmd.mid(4,4).toInt(&ok);
    if(!ok) {
      return;
    }
    state=cmd.right(1).toInt(&ok);
    if(!ok) {
      return;
    }
    for(unsigned i=0;i<sas_relay_numbers.size();i++) {
      if(line==sas_relay_numbers[i]) {
	switch(cmd.mid(1,2).toInt()) {
	case 0:   // Opto
	  emit gpiChanged(sas_matrix,i,state);
	  break;
      
	case 1:   // Relay
	  emit gpoChanged(sas_matrix,i,state);
	  break;
	}
      }
    }
    break;
  }
}


void SasUsi::ExecuteMacroCart(unsigned cartnum)
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


QString SasUsi::PrettifyCommand(const char *cmd) const
{
  QString ret;
  if(cmd[0]<26) {
    ret=QString().sprintf("^%c%s",'@'+cmd[0],cmd+1);
  }
  else {
    ret=cmd;
  }
  return ret;
}
