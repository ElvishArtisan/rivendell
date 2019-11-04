// rdhttpserver.cpp
//
// A container class for simple (I mean, simple) HTTP server
//
//   Patrick Linstruth <patrick@deltecent.com>
//   (C) Copyright 2019 Fred Gleason <fredg@paravelsystems.com>
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

#include "rdhttpserver.h"

RDHttpServer::RDHttpServer()
{
  connected=false;

  tcp_server=new QTcpServer(this);
  connect(tcp_server,SIGNAL(newConnection()),this,SLOT(newConnectionSlot()));
}

RDHttpServer::~RDHttpServer()
{
}

void RDHttpServer::newConnectionSlot()
{
  fprintf(stderr,"newConnectionSlot (connected=%d)...\n",connected);
  if(connected) {
    return;
  }

  if((tcp_socket=tcp_server->nextPendingConnection())) {
    connect(tcp_socket,SIGNAL(connected()),this,SLOT(connectedSlot()));
    connect(tcp_socket,SIGNAL(disconnected()),this,SLOT(disconnectedSlot()));
    connect(tcp_socket,SIGNAL(readyRead()),this,SLOT(readyReadSlot()));
    connect(tcp_socket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(stateChangedSlot(QAbstractSocket::SocketState)));
    connect(tcp_socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(errorSlot(QAbstractSocket::SocketError)));

    connected=true;
    have_headers=false;
    request.method.clear();
    request.resource.clear();
    request.headers.clear();
    request.body.clear();
  }
}

void RDHttpServer::stateChangedSlot(QAbstractSocket::SocketState state)
{
  fprintf(stderr,"stateChanged(%d)...\n",state);
}

void RDHttpServer::errorSlot(QAbstractSocket::SocketError error)
{
  fprintf(stderr,"errorSlot(%d)...\n",error);
}

void RDHttpServer::connectedSlot()
{
  fputs("connectedSlot...\n",stderr);
}

void RDHttpServer::disconnectedSlot()
{
  fputs("disconnectedSlot...\n",stderr);

  connected=false;

  newConnectionSlot();
}

void RDHttpServer::readyReadSlot()
{
  fputs("readyReadSlot...\n",stderr);

  if(have_headers==false) {
    while(tcp_socket->canReadLine()) {
      QByteArray line=tcp_socket->readLine().simplified();
      fprintf(stderr,"line=%d\n",line.size());
      if(line.size()) {
        request.headers.insert(QString::fromAscii(line.left(line.indexOf(':'))).trimmed(),QString::fromAscii(line.mid(line.indexOf(':')+1)).trimmed());
      }
      else {
        have_headers=true;
        break;
      }
    }
  }

//  qDebug() << request.headers;

  int content_len=request.headers.value("Content-Length",0).toInt();

  if(have_headers&&content_len) {
    while(tcp_socket->bytesAvailable()&&request.body.size()<content_len) {
      request.body.append(tcp_socket->readAll());
    }

    if(request.body.size()>=content_len) {
      fputs("emit requestReceived()\n",stderr);
      emit requestReceived(request);
    }
  }
  else if(have_headers) {
    emit requestReceived(request);
  }

//  qDebug() << request.body;
}

void RDHttpServer::bytesWrittenSlot(qint64 bytes)
{
    fprintf(stderr,"bytesWrittenSlot(%d)...\n",(int)bytes);
}

bool RDHttpServer::listen(QHostAddress address,int port)
{
  bool r;

  r=tcp_server->listen(address,port);

  if(r) {
    fputs("Listening...\n",stderr);
  }

  return r;
}

void RDHttpServer::sendResponse(RDHttpResponse &response)
{
  fputs("sendResponse()\n",stderr);
  tcp_socket->write("HTTP/1.1 200 OK\r\n\r\n");
  tcp_socket->disconnectFromHost();
  fputs("Waiting for disconnect\n",stderr);
  tcp_socket->waitForDisconnected();
  fputs("Disconnected\n",stderr);
}

