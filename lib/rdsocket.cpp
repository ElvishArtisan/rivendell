//   rdsocket.cpp
//
//   A QTcpSocket object with connection-ID.
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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
#include <stdio.h>
#include <qwidget.h>

#include "rdsocket.h"

RDSocket::RDSocket(int id,QObject *parent)
  : QTcpSocket(parent)
{
  id_num=id;
  connect(this,SIGNAL(hostFound()),this,SLOT(hostFoundData()));
  connect(this,SIGNAL(connected()),this,SLOT(connectedData()));
  connect(this,SIGNAL(disconnected()),this,SLOT(connectionClosedData()));
  connect(this,SIGNAL(readyRead()),this,SLOT(readyReadData()));
  connect(this,SIGNAL(error(QAbstractSocket::SocketError)),
	  this,SLOT(errorData(QAbstractSocket::SocketError)));
}


void RDSocket::hostFoundData()
{
  emit hostFoundID(id_num);
}
 

void RDSocket::connectedData()
{
  emit connectedID(id_num);
}


void RDSocket::connectionClosedData()
{
  emit connectionClosedID(id_num);
}


void RDSocket::readyReadData()
{
  emit readyReadID(id_num);
}


void RDSocket::errorData(QAbstractSocket::SocketError error)
{
  emit errorID(error,id_num);
}
