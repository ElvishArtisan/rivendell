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

MetadataSource::MetadataSource(QTcpSocket *sock)
{
  meta_socket=sock;
  meta_curly_count=0;
  meta_quoted=false;
  meta_committed=true;
}


QByteArray MetadataSource::buffer() const
{
  return meta_buffer;
}


bool MetadataSource::appendBuffer(const QByteArray &data)
{
  if(meta_committed) {
    meta_buffer.clear();
  }
  meta_buffer+=data;

  for(int i=0;i<data.size();i++) {
    if(data.at(i)=='"') {
      meta_quoted=!meta_quoted;
    }
    if(!meta_quoted) {
      if(data.at(i)=='{') {
	meta_curly_count++;
      }
      if(data.at(i)=='}') {
	meta_curly_count--;
      }
    }
  }
  meta_committed=(meta_curly_count==0);
  if(meta_committed) {
    meta_buffer+="\n\n";
  }

  return meta_committed;
}


bool MetadataSource::isCommitted() const
{
  return meta_committed;
}


QTcpSocket *MetadataSource::socket() const
{
  return meta_socket;
}




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
  pad_source_ready_mapper=new QSignalMapper(this);
  connect(pad_source_ready_mapper,SIGNAL(mapped(int)),
	  this,SLOT(sourceReadyReadData(int)));

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

  SendState(sock->socketDescriptor());
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
  connect(sock,SIGNAL(readyRead()),pad_source_ready_mapper,SLOT(map()));
  pad_source_ready_mapper->setMapping(sock,sock->socketDescriptor());

  connect(sock,SIGNAL(disconnected()),pad_source_disconnect_mapper,SLOT(map()));
  pad_source_disconnect_mapper->setMapping(sock,sock->socketDescriptor());

  pad_sources[sock->socketDescriptor()]=new MetadataSource(sock);
}


void Repeater::sourceReadyReadData(int id)
{
  if(pad_sources[id]!=NULL) {
    if(pad_sources[id]->appendBuffer(pad_sources[id]->socket()->readAll())) {
      for(QMap<int,QTcpSocket *>::const_iterator it=pad_client_sockets.begin();
	  it!=pad_client_sockets.end();it++) {
	it.value()->write(pad_sources[id]->buffer());
      }
    }
  }
}


void Repeater::sourceDisconnected(int id)
{
  if(pad_sources.value(id)!=NULL) {
    pad_sources.value(id)->socket()->deleteLater();
    delete pad_sources.value(id);
    pad_sources.remove(id);
  }
  else {
    fprintf(stderr,"unknown source connection %d attempted to close\n",id);
  }
}


void Repeater::SendState(int id)
{
  for(QMap<int,MetadataSource *>::const_iterator it=pad_sources.begin();
      it!=pad_sources.end();it++) {
    if(it.value()->isCommitted()&&(!it.value()->buffer().trimmed().isEmpty())) {
      pad_client_sockets.value(id)->write(it.value()->buffer());
    }
  }
}
