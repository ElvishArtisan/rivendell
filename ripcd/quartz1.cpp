// quartz1.cpp
//
// A Rivendell switcher driver for the Quartz Type 1 Switcher Protocol
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <qsignalmapper.h>

#include <rdapplication.h>
#include <rddb.h>

#include "globals.h"
#include "quartz1.h"

Quartz1::Quartz1(RDMatrix *matrix,QObject *parent)
  : Switcher(matrix,parent)
{
  RDTty *tty;
  sas_matrix=matrix->matrix();
  sas_ptr[0]=0;
  sas_ptr[1]=0;

  //
  // Get Matrix Parameters
  //
  sas_porttype[0]=matrix->portType(RDMatrix::Primary);
  sas_ipaddress[0]=matrix->ipAddress(RDMatrix::Primary);
  sas_ipport[0]=matrix->ipPort(RDMatrix::Primary);
  sas_port[0]=matrix->port(RDMatrix::Primary);
  sas_porttype[1]=matrix->portType(RDMatrix::Backup);
  sas_ipaddress[1]=matrix->ipAddress(RDMatrix::Backup);
  sas_ipport[1]=matrix->ipPort(RDMatrix::Backup); 
  sas_port[1]=matrix->port(RDMatrix::Backup);
  sas_inputs=matrix->inputs();
  sas_outputs=matrix->outputs();
  sas_layer=matrix->layer();

  //
  // Reconnection Timers
  //
  QSignalMapper *mapper=new QSignalMapper(this);
  connect(mapper,SIGNAL(mapped(int)),this,SLOT(ipConnect(int)));
  for(int i=0;i<2;i++) {
    sas_reconnect_timer[i]=new QTimer(this);
    sas_reconnect_timer[i]->setSingleShot(true);
    mapper->setMapping(sas_reconnect_timer[i],i);
    connect(sas_reconnect_timer[i],SIGNAL(timeout()),mapper,SLOT(map()));
  }

  //
  // Initialize the connections
  //
  QSignalMapper *connected_mapper=new QSignalMapper(this);
  connect(connected_mapper,SIGNAL(mapped(int)),this,SLOT(connectedData(int)));
  QSignalMapper *closed_mapper=new QSignalMapper(this);
  connect(closed_mapper,SIGNAL(mapped(int)),
	  this,SLOT(connectionClosedData(int)));
  for(int i=0;i<2;i++) {
    switch(sas_porttype[i]) {
    case RDMatrix::TtyPort:
      tty=new RDTty(rda->station()->name(),sas_port[i]);
      sas_device[i]=new RDTTYDevice();
      if(tty->active()) {
	sas_device[i]->setName(tty->port());
	sas_device[i]->setSpeed(tty->baudRate());
	sas_device[i]->setWordLength(tty->dataBits());
	sas_device[i]->setParity(tty->parity());
	sas_device[i]->open(QIODevice::Unbuffered|
			    QIODevice::WriteOnly);
      }
      delete tty;
	      
    case RDMatrix::TcpPort:
      sas_socket[i]=new QTcpSocket(this);
      connected_mapper->setMapping(sas_socket[i],i);
      connect(sas_socket[i],SIGNAL(connected()),
	      connected_mapper,SLOT(map()));
      closed_mapper->setMapping(sas_socket[i],i);
      connect(sas_socket[i],SIGNAL(disconnected()),
	      closed_mapper,SLOT(map()));
      switch(i) {
      case 0:
	connect(sas_socket[i],SIGNAL(error(QAbstractSocket::SocketError)),
		this,SLOT(error0Data(QAbstractSocket::SocketError)));
	break;

      case 1:
	connect(sas_socket[i],SIGNAL(error(QAbstractSocket::SocketError)),
		this,SLOT(error1Data(QAbstractSocket::SocketError)));
	break;
      }
      ipConnect(i);
      break;
	      
    case RDMatrix::NoPort:
      break;
    }
  }
}


RDMatrix::Type Quartz1::type()
{
  return RDMatrix::Quartz1;
}


unsigned Quartz1::gpiQuantity()
{
  return 0;
}


unsigned Quartz1::gpoQuantity()
{
  return 0;
}


bool Quartz1::primaryTtyActive()
{
  return sas_porttype[0]==RDMatrix::TtyPort;
}


bool Quartz1::secondaryTtyActive()
{
  return sas_porttype[1]==RDMatrix::TtyPort;
}


void Quartz1::processCommand(RDMacro *cmd)
{
  char str[9];
  QString label;

  switch(cmd->command()) {
      case RDMacro::FS:
	if((cmd->arg(1).toInt()<1)||(cmd->arg(1).toInt()>32)) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}
	sprintf(str,".F%d\x0D",cmd->arg(1).toInt());
	SendCommand(str);
	cmd->acknowledge(true);
	emit rmlEcho(cmd);
	break;

      case RDMacro::ST:
	if((cmd->arg(1).toInt()<1)||(cmd->arg(1).toInt()>sas_inputs)||
	   (cmd->arg(2).toInt()<1)||(cmd->arg(2).toInt()>sas_outputs)) {
	  cmd->acknowledge(false);
	  emit rmlEcho(cmd);
	  return;
	}
	sprintf(str,".S%c%0d,%d\x0D",sas_layer,
		cmd->arg(2).toInt(),cmd->arg(1).toInt());
	SendCommand(str);
	cmd->acknowledge(true);
	emit rmlEcho(cmd);
	break;

      default:
	cmd->acknowledge(false);
	emit rmlEcho(cmd);
	break;
  }
}


void Quartz1::ipConnect(int conn)
{
  sas_socket[conn]->
      connectToHost(sas_ipaddress[conn].toString(),sas_ipport[conn]);
}


void Quartz1::connectedData(int conn)
{
  rda->syslog(LOG_INFO,"connection to Quartz1 device at %s:%d established",
	      (const char *)sas_ipaddress[conn].toString().toUtf8(),
	      sas_ipport[conn]);
}


void Quartz1::connectionClosedData(int conn)
{
  rda->syslog(LOG_WARNING,
	 "connection to Quartz1 device at %s:%d closed unexpectedly, attempting reconnect",
	      (const char *)sas_ipaddress[conn].toString().toUtf8(),
	      sas_ipport[conn]);
  sas_reconnect_timer[conn]->start(QUARTZ1_RECONNECT_INTERVAL);
}


void Quartz1::errorData(int conn,QAbstractSocket::SocketError err)
{
  switch(err) {
  case QAbstractSocket::ConnectionRefusedError:
    rda->syslog(LOG_WARNING,
	  "connection to Quartz1 device at %s:%d refused, attempting reconnect",
		(const char *)sas_ipaddress[conn].toString().toUtf8(),
		sas_ipport[conn]);
    sas_reconnect_timer[conn]->start(QUARTZ1_RECONNECT_INTERVAL);
    break;

  case QAbstractSocket::HostNotFoundError:
    rda->syslog(LOG_WARNING,
	       "error on connection to Quartz1 device at %s:%d: Host Not Found",
		(const char *)sas_ipaddress[conn].toString().toUtf8(),
		sas_ipport[conn]);
    break;

  default:
    rda->syslog(LOG_WARNING,
		"error %d on connection to Quartz1 device at %s:%d",
		err,
		(const char *)sas_ipaddress[conn].toString().toUtf8(),
		sas_ipport[conn]);
    break;
  }
}


void Quartz1::error0Data(QAbstractSocket::SocketError err)
{
    errorData(0,err);
}


void Quartz1::error1Data(QAbstractSocket::SocketError err)
{
    errorData(1,err);
}


void Quartz1::SendCommand(const char *str)
{
  // LogLine(QString::asprintf("SENDING: %s",str));
  for(int i=0;i<2;i++) {
    switch(sas_porttype[i]) {
    case RDMatrix::TtyPort:
      sas_device[i]->write(str,strlen(str));
      break;
      
    case RDMatrix::TcpPort:
      sas_socket[i]->write(str,strlen(str));
      break;

    case RDMatrix::NoPort:
      break;
    }
  }
}
