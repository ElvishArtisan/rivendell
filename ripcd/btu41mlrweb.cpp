// btu41mlrweb.cpp
//
// Rivendell switcher driver for the BroadcastTools Universal 4.1 MLR>>Web
//
//   (C) Copyright 2017-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include "btu41mlrweb.h"

BtU41MlrWeb::BtU41MlrWeb(RDMatrix *matrix,QObject *parent)
  : Switcher(matrix,parent)
{
  bt_watchdog_active=false;

  bt_ip_address=matrix->ipAddress(RDMatrix::Primary);
  bt_ip_port=matrix->ipPort(RDMatrix::Primary);

  for(int i=0;i<BTU41MLRWEB_GPI_QUAN;i++) {
    bt_gpi_states[i]=false;
  }

  bt_socket=new QTcpSocket(this);
  connect(bt_socket,SIGNAL(connected()),this,SLOT(connectedData()));
  connect(bt_socket,SIGNAL(readyRead()),this,SLOT(readyReadData()));
  connect(bt_socket,SIGNAL(error(QAbstractSocket::SocketError)),
	  this,SLOT(errorData(QAbstractSocket::SocketError)));
  bt_socket->connectToHost(bt_ip_address.toString(),bt_ip_port);

  bt_keepalive_timer=new QTimer(this);
  connect(bt_keepalive_timer,SIGNAL(timeout()),this,SLOT(keepaliveData()));

  bt_watchdog_timer=new QTimer(this);
  connect(bt_watchdog_timer,SIGNAL(timeout()),this,SLOT(watchdogData()));
}


BtU41MlrWeb::~BtU41MlrWeb()
{
  delete bt_watchdog_timer;
  delete bt_keepalive_timer;
  delete bt_socket;
}


RDMatrix::Type BtU41MlrWeb::type()
{
  return RDMatrix::BtU41MlrWeb;
}


unsigned BtU41MlrWeb::gpiQuantity()
{
  return BTU41MLRWEB_GPI_QUAN;
}


unsigned BtU41MlrWeb::gpoQuantity()
{
  return BTU41MLRWEB_GPO_QUAN;
}


bool BtU41MlrWeb::primaryTtyActive()
{
  return false;
}


bool BtU41MlrWeb::secondaryTtyActive()
{
  return false;
}


void BtU41MlrWeb::processCommand(RDMacro *cmd)
{
  switch(cmd->command()) {
  case RDMacro::ST:
    if((cmd->arg(1).toInt()<0)||(cmd->arg(1).toInt()>BTU41MLRWEB_INPUT_QUAN)||
       (cmd->arg(2).toInt()<1)||(cmd->arg(2).toInt()>2)) {
      cmd->acknowledge(false);
      emit rmlEcho(cmd);
      return;
    }
    if(cmd->arg(1).toInt()==0) {
      SendCommand("*0MA");
    }
    else {
      SendCommand(QString::asprintf("*0%02d",cmd->arg(1).toInt()));
    }
    break;

  default:
    break;
  }
}


void BtU41MlrWeb::connectedData()
{
  rda->syslog(LOG_INFO,
	      "connection to BroadcastTools device at %s:%u established",
	      bt_ip_address.toString().toUtf8().constData(),0xffff&bt_ip_port);
  bt_watchdog_active=false;
  SendCommand("*0SPA");
  bt_keepalive_timer->start(BTU41MLRWEB_KEEPALIVE_INTERVAL);
  bt_watchdog_timer->start(BTU41MLRWEB_WATCHDOG_INTERVAL);
}


void BtU41MlrWeb::readyReadData()
{
  char data[1501];
  int n=0;

  while((n=bt_socket->read(data,1500))>0) {
    data[n]=0;
    for(int i=0;i<n;i++) {
      switch(0xff&data[i]) {
      case 13:
	break;

      case 10:
	ProcessCommand(bt_accum);
	bt_accum="";
	break;

      default:
	bt_accum+=data[i];
	break;
      }
    }
  }
}


void BtU41MlrWeb::errorData(QAbstractSocket::SocketError err)
{
  watchdogData();
}

void BtU41MlrWeb::keepaliveData()
{
  SendCommand("*0SL");
}


void BtU41MlrWeb::watchdogData()
{
  if(!bt_watchdog_active) {
    rda->syslog(LOG_WARNING,
      "connection to BroadcastTools device at %s:%u lost, attempting reconnect",
		bt_ip_address.toString().toUtf8().constData(),
		0xffff&bt_ip_port);
    bt_keepalive_timer->stop();
    bt_watchdog_active=true;
  }
  bt_socket->close();
  bt_socket->connectToHost(bt_ip_address.toString(),bt_ip_port);
}


void BtU41MlrWeb::ProcessCommand(const QString &cmd)
{
  //  rda->syslog(LOG_DEBUG,"ProcessCommand(%s)\n",(const char *)cmd);

  QStringList cmds=cmd.split(",");

  if((cmds.size()==7)&&(cmds[0]=="S0P")) {
    for(int i=0;i<BTU41MLRWEB_GPI_QUAN;i++) {
      if(bt_gpi_states[i]!=(cmds[i+2]=="1")) {
	bt_gpi_states[i]=cmds[i+2]=="1";
	emit gpiChanged(matrixNumber(),i,bt_gpi_states[i]);
      }
    }
  }

  if((cmds.size()==5)&&(cmds[0]=="S0L")) {
    bt_watchdog_timer->stop();
    bt_watchdog_timer->start(BTU41MLRWEB_WATCHDOG_INTERVAL);
  }
}


void BtU41MlrWeb::SendCommand(const QString &cmd)
{
  bt_socket->write(cmd.toUtf8());
}
