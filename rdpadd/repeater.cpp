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
#include <syslog.h>
#include <errno.h>
#include <sys/socket.h>

#include <QHostAddress>
#include <QTimer>

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
  
  //
  // Health Check Timer - detect stuck sockets
  //
  pad_health_timer=new QTimer(this);
  connect(pad_health_timer,SIGNAL(timeout()),this,SLOT(checkSocketHealth()));
  pad_health_timer->start(1000);  // Check every second
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
    QTimer::singleShot(100,this,SLOT(retrySourceConnection()));
    return;
  }
  
  connect(sock,SIGNAL(disconnected()),pad_source_disconnect_mapper,SLOT(map()));
  pad_source_disconnect_mapper->setMapping(sock,sock->socketDescriptor());

  RDJsonFramer *framer=new RDJsonFramer(sock,this);
  connect(framer,SIGNAL(documentReceived(const QByteArray &)),
	  this,SLOT(sendUpdate(const QByteArray &)));
  
  int sockfd=sock->socketDescriptor();
  pad_source_sockets[sockfd]=sock;
  pad_framers[sockfd]=framer;
}


void Repeater::sourceDisconnected(int id)
{
  QTcpSocket *sock=pad_source_sockets.value(id);
  RDJsonFramer *framer=pad_framers.value(id);
  
  if(sock!=NULL && framer!=NULL) {
    // Disconnect from signal mapper to avoid stale mappings
    pad_source_disconnect_mapper->removeMappings(sock);
    
    sock->blockSignals(true);
    pad_framers.remove(id);
    pad_source_sockets.remove(id);
    delete framer;  // This also deletes the socket
  }
}


void Repeater::retrySourceConnection()
{
  pad_source_server->resetErrorState();
}


void Repeater::sendUpdate(const QByteArray &data)
{
  for(QMap<int,QTcpSocket *>::const_iterator it=pad_client_sockets.begin();
      it!=pad_client_sockets.end();it++) {
    it.value()->write(data);
  }
}


void Repeater::checkSocketHealth()
{
  for(QMap<int,QTcpSocket *>::iterator it=pad_source_sockets.begin();
      it!=pad_source_sockets.end();it++) {
    QTcpSocket *sock=it.value();
    int fd=it.key();
    
    char buf[1];
    ssize_t n=::recv(sock->socketDescriptor(),buf,1,MSG_PEEK|MSG_DONTWAIT);
    
    if(n==0) {
      // EOF detected - peer closed but Qt didn't fire disconnected signal
      sourceDisconnected(fd);
      return;
    }
  }
}
