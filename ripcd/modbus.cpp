// modbus.cpp
//
// A Rivendell switcher driver for Modbus TCP
//
//   (C) Copyright 2017-2020 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdapplication.h>

#include "modbus.h"

Modbus::Modbus(RDMatrix *matrix,QObject *parent)
  : Switcher(matrix,parent)
{
  modbus_istate=0;
  modbus_watchdog_active=false;
  modbus_busy=false;

  modbus_gpis=matrix->gpis();
  modbus_input_bytes=modbus_gpis/8;
  if((modbus_gpis%8)!=0) {
    modbus_input_bytes++;
  }
  for(int i=0;i<modbus_input_bytes;i++) {
    modbus_input_states.push_back(0);
  }
  modbus_gpos=matrix->gpos();
  modbus_ip_address=matrix->ipAddress(RDMatrix::Primary);
  modbus_ip_port=matrix->ipPort(RDMatrix::Primary);

  modbus_socket=new QTcpSocket(this);
  connect(modbus_socket,SIGNAL(connected()),this,SLOT(connectedData()));
  connect(modbus_socket,SIGNAL(readyRead()),this,SLOT(readyReadData()));
  connect(modbus_socket,SIGNAL(error(QAbstractSocket::SocketError)),
	  this,SLOT(errorData(QAbstractSocket::SocketError)));
  modbus_socket->connectToHost(modbus_ip_address.toString(),modbus_ip_port);

  modbus_poll_timer=new QTimer(this);
  connect(modbus_poll_timer,SIGNAL(timeout()),this,SLOT(pollInputs()));

  modbus_reset_mapper=new QSignalMapper(this);
  connect(modbus_reset_mapper,SIGNAL(mapped(int)),
	  this,SLOT(resetStateData(int)));
  for(int i=0;i<modbus_gpos;i++) {
    modbus_reset_timers.push_back(new QTimer(this));
    connect(modbus_reset_timers.back(),SIGNAL(timeout()),
	    modbus_reset_mapper,SLOT(map()));
    modbus_reset_mapper->setMapping(modbus_reset_timers.back(),i);
    modbus_reset_states.push_back(false);
  }

  modbus_watchdog_timer=new QTimer(this);
  connect(modbus_watchdog_timer,SIGNAL(timeout()),this,SLOT(watchdogData()));
}


Modbus::~Modbus()
{
  delete modbus_watchdog_timer;
  delete modbus_poll_timer;
  for(unsigned i=0;i<modbus_reset_timers.size();i++) {
    delete modbus_reset_timers[i];
  }
  delete modbus_reset_mapper;
  delete modbus_socket;
}


RDMatrix::Type Modbus::type()
{
  return RDMatrix::Modbus;
}


unsigned Modbus::gpiQuantity()
{
  return modbus_gpis;
}


unsigned Modbus::gpoQuantity()
{
  return modbus_gpos;
}


bool Modbus::primaryTtyActive()
{
  return false;
}


bool Modbus::secondaryTtyActive()
{
  return false;
}


void Modbus::processCommand(RDMacro *cmd)
{
  switch(cmd->command()) {
  case RDMacro::GO:
    if((cmd->argQuantity()!=5)||
       ((cmd->arg(1).lower()!="i")&&
	(cmd->arg(1).lower()!="o"))||
       (cmd->arg(2).toInt()<1)||(cmd->arg(3).toInt()>modbus_gpos)||
       (cmd->arg(2).toInt()>modbus_gpos)||
       ((cmd->arg(3).toInt()!=1)&&(cmd->arg(3).toInt()!=0)&&
	(cmd->arg(1).lower()!="i"))||
       ((cmd->arg(3).toInt()!=1)&&(cmd->arg(3).toInt()!=0)&&
	(cmd->arg(3).toInt()!=-1)&&(cmd->arg(1).lower()=="i"))||
       (cmd->arg(4).toInt()<0)) {
      cmd->acknowledge(false);
      emit rmlEcho(cmd);
      return;
    }
    if(cmd->arg(3).toInt()==0) {  // Turn OFF
      if(cmd->arg(4).toInt()==0) {
	if(cmd->arg(1).lower()=="o") {
	  SetCoil(cmd->arg(2).toInt()-1,false);
	  emit gpoChanged(matrixNumber(),cmd->arg(2).toInt()-1,false);
	}
      }
      else {
	if(cmd->echoRequested()) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	}
	return;
      }
    }
    else {
      if(cmd->arg(4).toInt()==0) {  // Turn ON
	if(cmd->arg(1).lower()=="o") {
	  SetCoil(cmd->arg(2).toInt()-1,true);
	  emit gpoChanged(matrixNumber(),cmd->arg(2).toInt()-1,true);
	}
      }
      else {  // Pulse
	if(cmd->arg(1).lower()=="o") {
	  SetCoil(cmd->arg(2).toInt()-1,cmd->arg(3).toInt()!=0);
	  modbus_reset_states[cmd->arg(2).toInt()-1]=cmd->arg(3).toInt()==0;
	  modbus_reset_timers[cmd->arg(2).toInt()-1]->
	    start(cmd->arg(4).toInt(),true);
	  emit gpoChanged(matrixNumber(),cmd->arg(2).toInt()-1,
			  cmd->arg(3).toInt()!=0);
	}
      }
    }
    
    if(cmd->echoRequested()) {
      cmd->acknowledge(true);
      emit rmlEcho(cmd);
    }
    break;

  default:
    break;
  }
}


void Modbus::connectedData()
{
  rda->syslog(LOG_INFO,
	      "connection to Modbus device at %s:%u established",
	      (const char *)modbus_ip_address.toString(),0xffff&modbus_ip_port);
  modbus_watchdog_active=false;
  modbus_busy=false;
  pollInputs();
}


void Modbus::readyReadData()
{
  char data[1501];
  int n;
  char byte;
  uint16_t len;
  char count=0;
  int base=0;

  modbus_watchdog_timer->stop();
  modbus_watchdog_timer->start(MODBUS_WATCHDOG_INTERVAL,true);

  while((n=modbus_socket->readBlock(data,1500))>0) {
    //    logBytes((uint8_t *)data,n);
    for(int i=0;i<n;i++) {
      printf("istate: %d\n",modbus_istate);
      byte=0xff&data[i];
      switch(modbus_istate) {
      case 0:   // Transaction Identifier
	modbus_istate=1;
	break;

      case 1:
	modbus_istate=2;
	break;

      case 2:   // Protocol Identifier
	if(byte==0) {
	  modbus_istate=3;
	}
	else {
	  modbus_istate=0;
	}
	break;

      case 3:   // Protocol Identifier
	if(byte==0) {
	  modbus_istate=4;
	}
	else {
	  modbus_istate=0;
	}
	break;

      case 4:   // Message Length
	len=byte<<8;
	modbus_istate=5;
	break;

      case 5:   // Message Length
	len+=byte;
	modbus_istate=6;
	break;

      case 6:   // Modbus ID
	modbus_istate=7;
	break;

      case 7:   // Function Code
	if(byte==0x02) {  // Read Inputs
	  modbus_istate=8;
	}
	else {
	  if(byte==0x05) {  // Set Coil
	    modbus_istate=100;
	  }
	  else {
	    modbus_istate=0;
	  }
	}
	break;

      case 8:   // Byte Count
	count=byte;
	modbus_istate=9;
	break;

      case 9:   // Input Status
	base=modbus_input_bytes-count;
	ProcessInputByte(byte,base);
	if(--count==0) {
	  modbus_poll_timer->start(MODBUS_POLL_INTERVAL,true);
	  modbus_istate=0;
	  modbus_busy=false;
	  if(modbus_coil_lines.size()>0) {
	    SetCoil(modbus_coil_lines.front(),modbus_coil_states.front());
	    modbus_coil_lines.pop();
	    modbus_coil_states.pop();
	  }
	}
	break;

      case 100:  // Output Address
	modbus_istate=101;
	break;

      case 101:
	modbus_istate=102;
	break;

      case 102:  // Output Value
	modbus_istate=103;
	break;
 
      case 103:
	modbus_istate=0;
	modbus_busy=false;
	if(modbus_coil_lines.size()>0) {
	  SetCoil(modbus_coil_lines.front(),modbus_coil_states.front());
	  modbus_coil_lines.pop();
	  modbus_coil_states.pop();
	}
	break;	
      }
    }
  }
}


void Modbus::errorData(QAbstractSocket::SocketError err)
{
  watchdogData();
}


void Modbus::pollInputs()
{
  if(modbus_busy) {
    modbus_poll_timer->start(MODBUS_POLL_INTERVAL,true);
    return;
  }

  char msg[12];
  msg[0]=0x88;  // Transaction Identifier
  msg[1]=0x88;

  msg[2]=0x00;  // Protocol Identifier
  msg[3]=0x00;

  msg[4]=0x00;  // Message Length
  msg[5]=0x06;

  msg[6]=0x01;  // Modbus ID

  msg[7]=0x02;  // Function Code (Read Discrete Input)

  msg[8]=0x00;  // Starting Address
  msg[9]=0x00;

  msg[10]=0xff&(modbus_gpis>>8);  // Quantity of Inputs
  msg[11]=0xff&modbus_gpis;

  modbus_socket->writeBlock(msg,12);
  modbus_busy=true;
}


void Modbus::resetStateData(int line)
{
  SetCoil(line,modbus_reset_states[line]);
  emit gpoChanged(matrixNumber(),line,modbus_reset_states[line]);
}


void Modbus::watchdogData()
{
  if(!modbus_watchdog_active) {
    rda->syslog(LOG_WARNING,
	      "connection to Modbus device at %s:%u lost, attempting reconnect",
	      (const char *)modbus_ip_address.toString(),0xffff&modbus_ip_port);
    modbus_watchdog_active=true;
  }
  modbus_socket->close();
  modbus_socket->connectToHost(modbus_ip_address.toString(),modbus_ip_port);
}


void Modbus::ProcessInputByte(char byte,int base)
{
  char mask=byte^modbus_input_states[base];
  for(int i=0;i<8;i++) {
    if(((1<<i)&mask)!=0) {
      int line=8*base+i;
      if(line<modbus_gpis) {
	emit gpiChanged(matrixNumber(),line,((1<<i)&byte)!=0);
      }
    }
  }
  modbus_input_states[base]=byte;
}


void Modbus::SetCoil(int line,bool state)
{
  //  printf("SetCoil(%d,%d)\n",line,state);
  if(modbus_busy) {
    modbus_coil_lines.push(line);
    modbus_coil_states.push(state);
    return;
  }
  modbus_busy=true;

  char msg[12];

  msg[0]=0x44;  // Transaction Identifier
  msg[1]=0x44;

  msg[2]=0x00;  // Protocol Identifier
  msg[3]=0x00;

  msg[4]=0x00;  // Message Length
  msg[5]=0x06;

  msg[6]=0x01;  // Modbus ID

  msg[7]=0x05;  // Function Code (Write Single Coil)

  msg[8]=0xff&(line>>8);  // Coil number
  msg[9]=0xff&line;

  msg[10]=0x00;  // Coil state
  msg[11]=0x00;
  if(state) {
    msg[10]=0xff;
  }

  modbus_socket->writeBlock(msg,12);
}


bool Modbus::InputState(int line) const
{
  return (1<<(line%8)&modbus_input_states.at(line/8))!=0;
}
