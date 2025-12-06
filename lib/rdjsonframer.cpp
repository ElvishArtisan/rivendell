//   rdjsonframer.cpp
//
//   Frame an unsynchronized stream of JSON messages
//
//   (C) Copyright 2024 Fred Gleason <fredg@paravelsystems.com>
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

#include <syslog.h>

#include "rdjsonframer.h"

RDJsonFramer::RDJsonFramer(QTcpSocket *in_sock,QObject *parent)
  : QObject(parent)
{
  d_socket=in_sock;
  d_empty_read_counter=0;
  connect(d_socket,SIGNAL(readyRead()),this,SLOT(readyReadData()));
  connect(d_socket,SIGNAL(disconnected()),this,SLOT(disconnectedData()));
}


RDJsonFramer::RDJsonFramer(QObject *parent)
  : QObject(parent)
{
  d_socket=NULL;
}


RDJsonFramer::~RDJsonFramer()
{
  if(d_socket!=NULL) {
    delete d_socket;
  }
}


QByteArray RDJsonFramer::currentDocument() const
{
  return d_current_document;
}


void RDJsonFramer::write(const QByteArray &data)
{
  d_current_document=data;
  emit documentReceived(d_current_document);
}


void RDJsonFramer::reset()
{
  emit documentReset();  
}


void RDJsonFramer::readyReadData()
{
  QByteArray data=d_socket->readAll();
  if(!data.isEmpty()) {
    write(data);
  }
}


void RDJsonFramer::disconnectedData()
{
  // Socket disconnected - stop processing immediately
  syslog(LOG_INFO,"RDJsonFramer: disconnected signal received (sockfd=%d), disconnecting readyRead",
          (int)d_socket->socketDescriptor());
  disconnect(d_socket,SIGNAL(readyRead()),this,SLOT(readyReadData()));
  d_empty_read_counter=0;
}
