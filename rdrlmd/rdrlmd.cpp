// rdrlmd.cpp
//
// Rivendell RLM Consolidation Server
//
//   (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <qcoreapplication.h>
#include <qhostaddress.h>

#include <rd.h>
#include <rdcmd_switch.h>

#include "rdrlmd.h"

MetadataSource::MetadataSource(QTcpSocket *sock)
{
  meta_socket=sock;
  meta_committed=true;
}


QByteArray MetadataSource::buffer() const
{
  return meta_buffer;
}


bool MetadataSource::appendBuffer(const QByteArray &data)
{
  printf("data: %s\n",(const char *)data);

  if(meta_committed) {
    meta_buffer.clear();
  }
  meta_buffer+=data;
  meta_committed=meta_buffer.endsWith("\r\n\r\n");

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




MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  new RDCmdSwitch(qApp->argc(),qApp->argv(),"rdrlmd",RDRLMD_USAGE);

  //
  // Client Server
  //
  rlm_client_disconnect_mapper=new QSignalMapper(this);
  connect(rlm_client_disconnect_mapper,SIGNAL(mapped(int)),
	  this,SLOT(clientDisconnected(int)));

  rlm_client_server=new QTcpServer(this);
  connect(rlm_client_server,SIGNAL(newConnection()),
	  this,SLOT(newClientConnectionData()));
  if(!rlm_client_server->listen(QHostAddress::Any,RD_RLM2_CLIENT_TCP_PORT)) {
    fprintf(stderr,"rdrlmd: unable to bind client port %d\n",
	    RD_RLM2_CLIENT_TCP_PORT);
    exit(1);
  }

  //
  // Source Server
  //
  rlm_source_ready_mapper=new QSignalMapper(this);
  connect(rlm_source_ready_mapper,SIGNAL(mapped(int)),
	  this,SLOT(sourceReadyReadData(int)));

  rlm_source_disconnect_mapper=new QSignalMapper(this);
  connect(rlm_source_disconnect_mapper,SIGNAL(mapped(int)),
	  this,SLOT(sourceDisconnected(int)));

  rlm_source_server=new RDUnixServer(this);
  connect(rlm_source_server,SIGNAL(newConnection()),
	  this,SLOT(newSourceConnectionData()));
  if(!rlm_source_server->listenToAbstract(RD_RLM2_SOURCE_UNIX_ADDRESS)) {
    fprintf(stderr,"rdrlmd: unable to bind source socket [%s]\n",
	    (const char *)rlm_source_server->errorString().toUtf8());
    exit(1);
  }
}


void MainObject::newClientConnectionData()
{
  QTcpSocket *sock=rlm_client_server->nextPendingConnection();
  connect(sock,SIGNAL(disconnected()),rlm_client_disconnect_mapper,SLOT(map()));
  rlm_client_disconnect_mapper->setMapping(sock,sock->socketDescriptor());
  rlm_client_sockets[sock->socketDescriptor()]=sock;

  SendState(sock->socketDescriptor());
  //  printf("client connection %d opened\n",sock->socketDescriptor());
}


void MainObject::clientDisconnected(int id)
{
  QTcpSocket *sock=NULL;

  if((sock=rlm_client_sockets.value(id))!=NULL) {
    sock->deleteLater();
    rlm_client_sockets.remove(id);
    //    printf("client connection %d closed\n",id);
  }
  else {
    fprintf(stderr,"unknown client connection %d attempted to close\n",id);
  }
}


void MainObject::newSourceConnectionData()
{
  QTcpSocket *sock=rlm_source_server->nextPendingConnection();
  if(sock==NULL) {
    fprintf(stderr,"rdrlmd: UNIX socket error [%s]\n",
	    (const char *)rlm_source_server->errorString().toUtf8());
    exit(1);
  }
  connect(sock,SIGNAL(readyRead()),rlm_source_ready_mapper,SLOT(map()));
  rlm_source_ready_mapper->setMapping(sock,sock->socketDescriptor());

  connect(sock,SIGNAL(disconnected()),rlm_source_disconnect_mapper,SLOT(map()));
  rlm_source_disconnect_mapper->setMapping(sock,sock->socketDescriptor());

  rlm_sources[sock->socketDescriptor()]=new MetadataSource(sock);

  //  printf("source connection %d opened\n",sock->socketDescriptor());
}


void MainObject::sourceReadyReadData(int id)
{
  if(rlm_sources[id]!=NULL) {
    if(rlm_sources[id]->appendBuffer(rlm_sources[id]->socket()->readAll())) {
      for(QMap<int,QTcpSocket *>::const_iterator it=rlm_client_sockets.begin();
	  it!=rlm_client_sockets.end();it++) {
	it.value()->write(rlm_sources[id]->buffer());
      }
    }
  }
}


void MainObject::sourceDisconnected(int id)
{
  if(rlm_sources.value(id)!=NULL) {
    rlm_sources.value(id)->socket()->deleteLater();
    delete rlm_sources.value(id);
    rlm_sources.remove(id);
    //    printf("source connection %d closed\n",id);
  }
  else {
    fprintf(stderr,"unknown source connection %d attempted to close\n",id);
  }
}


void MainObject::SendState(int id)
{
  for(QMap<int,MetadataSource *>::const_iterator it=rlm_sources.begin();
      it!=rlm_sources.end();it++) {
    if(it.value()->isCommitted()) {
      rlm_client_sockets.value(id)->write(it.value()->buffer());
    }
  }
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);

  new MainObject();
  return a.exec();
}
