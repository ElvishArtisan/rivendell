// gvc7000.cpp
//
// A Rivendell switcher driver for Grass Valley Series 7000 Protocol
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

#include <stdlib.h>
#include <syslog.h>

#include <qsignalmapper.h>

#include <rdapplication.h>

#include "gvc7000.h"

Gvc7000::Gvc7000(RDMatrix *matrix,QObject *parent)
  : Switcher(matrix,parent)
{
  gvc_matrix=matrix->matrix();

  //
  // Get Matrix Parameters
  //
  gvc_ipaddress=matrix->ipAddress(RDMatrix::Primary);
  gvc_ipport=matrix->ipPort(RDMatrix::Primary);
  gvc_inputs=matrix->inputs();
  gvc_outputs=matrix->outputs();

  //
  // Keepalive Timer
  //
  gvc_keepalive_timer=new QTimer(this);
  connect(gvc_keepalive_timer,SIGNAL(timeout()),this,SLOT(keepaliveData()));
  gvc_keepalive_timer->start(GVC7000_KEEPALIVE_INTERVAL);

  //
  // Reconnection Timer
  //
  gvc_reconnect_timer=new QTimer(this);
  gvc_reconnect_timer->setSingleShot(true);
  connect(gvc_reconnect_timer,SIGNAL(timeout()),this,SLOT(ipConnect()));

  //
  // Data Pacer
  //
  gvc_pacer=new RDDataPacer(this);
  gvc_pacer->setPaceInterval(10);
  connect(gvc_pacer,SIGNAL(dataSent(const QByteArray &)),
	  this,SLOT(sendCommandData(const QByteArray &)));

  //
  // Initialize the connection
  //
  gvc_socket=new QTcpSocket(this);
  connect(gvc_socket,SIGNAL(connected()),this,SLOT(connectedData()));
  connect(gvc_socket,SIGNAL(disconnected()),this,SLOT(disconnectedData()));
  connect(gvc_socket,SIGNAL(error(QAbstractSocket::SocketError)),
	  this,SLOT(errorData(QAbstractSocket::SocketError)));
  ipConnect();
}


RDMatrix::Type Gvc7000::type()
{
  return RDMatrix::Gvc7000;
}


unsigned Gvc7000::gpiQuantity()
{
  return 0;
}


unsigned Gvc7000::gpoQuantity()
{
  return 0;
}


bool Gvc7000::primaryTtyActive()
{
  return false;
}


bool Gvc7000::secondaryTtyActive()
{
  return false;
}


void Gvc7000::processCommand(RDMacro *cmd)
{
  switch(cmd->command()) {
  case RDMacro::ST:
    if((cmd->arg(1).toInt()<1)||(cmd->arg(1).toInt()>gvc_inputs)||
       (cmd->arg(2).toInt()<1)||(cmd->arg(2).toInt()>gvc_outputs)) {
      cmd->acknowledge(false);
      emit rmlEcho(cmd);
      return;
    }
    gvc_pacer->send(ToSeries7000Native(QString().sprintf("TI,%04X,%04X",cmd->arg(2).toInt()-1,cmd->arg(1).toInt()-1)).toAscii());
    cmd->acknowledge(true);
    emit rmlEcho(cmd);
    break;

  default:
    cmd->acknowledge(false);
    emit rmlEcho(cmd);
    break;
  }
}


void Gvc7000::ipConnect()
{
  gvc_socket->connectToHost(gvc_ipaddress.toString(),gvc_ipport);
}


void Gvc7000::keepaliveData()
{
  gvc_pacer->send(ToSeries7000Native("QJ").toAscii());
}


void Gvc7000::connectedData()
{
  rda->syslog(LOG_INFO,"connection to Gvc7000 device at %s:%d established",
	      (const char *)gvc_ipaddress.toString().toUtf8(),gvc_ipport);
}


void Gvc7000::disconnectedData()
{
  rda->syslog(LOG_WARNING,
	 "connection to Gvc7000 device at %s:%d closed unexpectedly, attempting reconnect",
	      (const char *)gvc_ipaddress.toString().toUtf8(),gvc_ipport);
  gvc_reconnect_timer->start(GVC7000_RECONNECT_INTERVAL,true);
}


void Gvc7000::errorData(QAbstractSocket::SocketError err)
{
  switch(err) {
  case QAbstractSocket::ConnectionRefusedError:
    rda->syslog(LOG_WARNING,
	  "connection to Gvc7000 device at %s:%d refused, attempting reconnect",
		(const char *)gvc_ipaddress.toString().toUtf8(),gvc_ipport);
    gvc_reconnect_timer->start(GVC7000_RECONNECT_INTERVAL,true);
    break;

  case QAbstractSocket::HostNotFoundError:
    rda->syslog(LOG_WARNING,
	       "error on connection to Gvc7000 device at %s:%d: Host Not Found",
		(const char *)gvc_ipaddress.toString().toUtf8(),
		gvc_ipport);
    break;

  case QAbstractSocket::RemoteHostClosedError:
    break;

  default:
    rda->syslog(LOG_WARNING,
	    "socket error %d on connection to Gvc7000 device at %s:%d: Socket Read Error",
		err,
		(const char *)gvc_ipaddress.toString().toUtf8(),gvc_ipport);
    break;
  }
}


void Gvc7000::sendCommandData(const QByteArray &data)
{
  syslog(LOG_DEBUG,"gvc7000 sending \"%s\"",data.constData());
  gvc_socket->write(data);
}

/*
void Gvc7000::SendCommand(const QString &str)
{
  syslog(LOG_DEBUG,"gvc7000 sending \"%s\"",(const char *)str.toAscii());
  gvc_socket->write(str.toAscii());
}
*/

QString Gvc7000::ToSeries7000Native(const QString &str) const
{
  QString ret=str;

  ret.replace(",","\t");
  if(ret.right(1)!="\t") {
    ret+="\t";
  }

  QString msg=QString("N")+  // Protocol ID
    "0"+                     // Sequence Flag
    ret;                     // Data

  uint8_t sum=0;
  for(int i=0;i<msg.length();i++) {
    sum+=msg.at(i).toAscii();
  }
  sum=0x100-sum;

  return QChar(1)+                  // Header
    msg+                            // Message
    QString().sprintf("%02X",sum)+  // Checksum
    QChar(4);                       // Footer
}
