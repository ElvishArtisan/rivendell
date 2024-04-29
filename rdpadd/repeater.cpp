// rdpadd.cpp
//
// Rivendell PAD Data Repeater
//
//   (C) Copyright 2018-2024 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdio.h>
#include <stdlib.h>

#include <QHostAddress>

#include <rd.h>

#include "repeater.h"

Repeater::Repeater(const QString &src_unix_addr,uint16_t serv_port,
		   QObject *parent)
  : QObject(parent)
{
  pad_source_unix_address=src_unix_addr;
  pad_server_port=serv_port;

  //
  // Client Server
  //
  pad_client_disconnect_mapper=new QSignalMapper(this);
  connect(pad_client_disconnect_mapper,SIGNAL(mapped(int)),
	  this,SLOT(clientDisconnected(int)));

  pad_client_server=new QTcpServer(this);
  connect(pad_client_server,SIGNAL(newConnection()),
	  this,SLOT(newClientConnectionData()));
  if(!pad_client_server->listen(QHostAddress::Any,pad_server_port)) {
    fprintf(stderr,"rdpadd: unable to bind client port %d\n",pad_server_port);
    exit(1);
  }

  //
  // Source Server
  //
  pad_source_disconnect_mapper=new QSignalMapper(this);
  connect(pad_source_disconnect_mapper,SIGNAL(mapped(int)),
	  this,SLOT(sourceDisconnected(int)));

  pad_source_server=new RDUnixServer(this);
  connect(pad_source_server,SIGNAL(newConnection()),
	  this,SLOT(newSourceConnectionData()));
  if(!pad_source_server->listenToAbstract(pad_source_unix_address)) {
    fprintf(stderr,"rdpadd: unable to bind source socket [%s]\n",
	    pad_source_server->errorString().toUtf8().constData());
    exit(1);
  }
}


QString Repeater::sourceUnixAddress() const
{
  return pad_source_unix_address;
}


uint16_t Repeater::serverPort() const
{
  return pad_server_port;
}


void Repeater::newClientConnectionData()
{
  QTcpSocket *sock=pad_client_server->nextPendingConnection();
  connect(sock,SIGNAL(disconnected()),pad_client_disconnect_mapper,SLOT(map()));
  pad_client_disconnect_mapper->setMapping(sock,sock->socketDescriptor());
  pad_client_sockets[sock->socketDescriptor()]=sock;

  for(QMap<int,RDJsonFramer *>::const_iterator it=pad_framers.begin();
      it!=pad_framers.end();it++) {
    sock->write(it.value()->currentDocument());
  }
}


void Repeater::clientDisconnected(int id)
{
  QTcpSocket *sock=NULL;

  if((sock=pad_client_sockets.value(id))!=NULL) {
    sock->deleteLater();
    pad_client_sockets.remove(id);
  }
  else {
    fprintf(stderr,"unknown client connection %d attempted to close\n",id);
  }
}


void Repeater::newSourceConnectionData()
{
  QTcpSocket *sock=pad_source_server->nextPendingConnection();
  if(sock==NULL) {
    fprintf(stderr,"rdpadd: UNIX socket error [%s]\n",
	    (const char *)pad_source_server->errorString().toUtf8());
    exit(1);
  }
  connect(sock,SIGNAL(disconnected()),pad_source_disconnect_mapper,SLOT(map()));
  pad_source_disconnect_mapper->setMapping(sock,sock->socketDescriptor());

  RDJsonFramer *framer=new RDJsonFramer(sock,this);
  connect(framer,SIGNAL(documentReceived(const QByteArray &)),
	  this,SLOT(sendUpdate(const QByteArray &)));
  pad_framers[sock->socketDescriptor()]=framer;
}


void Repeater::sourceDisconnected(int id)
{
  if(pad_framers.value(id)!=NULL) {
    pad_framers.value(id)->deleteLater();
    pad_framers.remove(id);
  }
  else {
    fprintf(stderr,"unknown source connection %d attempted to close\n",id);
  }
}


void Repeater::sendUpdate(const QByteArray &jdoc)
{
  for(QMap<int,QTcpSocket *>::const_iterator it=pad_client_sockets.begin();
      it!=pad_client_sockets.end();it++) {
    it.value()->write(jdoc);
  }
}
