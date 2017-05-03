// modbus.cpp
//
// A Rivendell switcher driver for Modbus TCP
//
//   (C) Copyright 2017 Fred Gleason <fredg@paravelsystems.com>
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

#include "modbus.h"

Modbus::Modbus(RDMatrix *matrix,QObject *parent)
  : Switcher(matrix,parent)
{
  modbus_istate=0;
  modbus_watchdog_active=false;

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

  modbus_socket=new QSocket(this);
  connect(modbus_socket,SIGNAL(connected()),this,SLOT(connectedData()));
  connect(modbus_socket,SIGNAL(readyRead()),this,SLOT(readyReadData()));
  connect(modbus_socket,SIGNAL(error(int)),this,SLOT(errorData(int)));
  modbus_socket->connectToHost(modbus_ip_address.toString(),modbus_ip_port);

  modbus_watchdog_timer=new QTimer(this);
  connect(modbus_watchdog_timer,SIGNAL(timeout()),this,SLOT(watchdogData()));
}


Modbus::~Modbus()
{
  delete modbus_watchdog_timer;
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
}


void Modbus::connectedData()
{
  syslog(LOG_INFO,
	 "connection to Modbus device at %s:%u established",
	 (const char *)modbus_ip_address.toString(),0xffff&modbus_ip_port);
  modbus_watchdog_active=false;
  PollInputs();
}


void Modbus::readyReadData()
{
  char data[1501];
  int n;
  char byte;
  uint16_t len;
  //  char id;
  char count=0;
  int base=0;

  while((n=modbus_socket->readBlock(data,1500))>0) {
    for(int i=0;i<n;i++) {
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
	//	id=byte;
	modbus_istate=7;
	break;

      case 7:   // Function Code
	if(byte==0x02) {
	  modbus_istate=8;
	}
	else {
	  modbus_istate=0;
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
	  PollInputs();
	  modbus_istate=0;
	}
	break;
      }
    }
  }
}


void Modbus::errorData(int err)
{
  watchdogData();
}


void Modbus::watchdogData()
{
  if(!modbus_watchdog_active) {
    syslog(LOG_WARNING,
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


void Modbus::PollInputs()
{
  printf("HERE1\n");
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
  modbus_watchdog_timer->stop();
  modbus_watchdog_timer->start(MODBUS_WATCHDOG_INTERVAL,true);
  printf("HERE2\n");
}
