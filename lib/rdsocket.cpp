//   rdsocket.cpp
//
//   A QSocket object with connection-ID.
//
//   (C) Copyright 2002 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdsocket.cpp,v 1.5 2010/07/29 19:32:34 cvs Exp $
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
//

#include <stdlib.h>
#include <stdio.h>
#include <qwidget.h>
#include <qsocket.h>

#include <rdsocket.h>


RDSocket::RDSocket(int id,QObject *parent,const char *name)
  : QSocket(parent,name)
{
  id_num=id;
  connect(this,SIGNAL(hostFound()),this,SLOT(hostFoundData()));
  connect(this,SIGNAL(connected()),this,SLOT(connectedData()));
  connect(this,SIGNAL(connectionClosed()),this,SLOT(connectionClosedData()));
  connect(this,SIGNAL(delayedCloseFinished()),
	  this,SLOT(delayedCloseFinishedData()));
  connect(this,SIGNAL(readyRead()),this,SLOT(readyReadData()));
  connect(this,SIGNAL(bytesWritten(int)),this,SLOT(bytesWrittenData(int)));
  connect(this,SIGNAL(error(int)),this,SLOT(errorData(int)));
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


void RDSocket::delayedCloseFinishedData()
{
  emit delayedCloseFinishedID(id_num);
}


void RDSocket::readyReadData()
{
  emit readyReadID(id_num);
}


void RDSocket::bytesWrittenData(int nbytes)
{
  emit bytesWrittenID(nbytes,id_num);
}


void RDSocket::errorData(int error)
{
  emit errorID(error,id_num);
}
