// rdunixserver.cpp
//
// UNIX Socket Server
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

#include <errno.h>
#include <linux/un.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "rdunixserver.h"

RDUnixServer::RDUnixServer(QObject *parent)
  : QObject(parent)
{
  unix_socket=-1;
  unix_notifier=NULL;
  unix_is_listening=false;
  unix_max_pending_connections=3;
  unix_error_string="ok";
}


RDUnixServer::~RDUnixServer()
{
  close();
  if(unix_notifier!=NULL) {
    delete unix_notifier;
  }
}


void RDUnixServer::close()
{
  if(unix_socket>=0) {
    shutdown(unix_socket,SHUT_RDWR);
    unix_socket=-1;
    unix_is_listening=false;
  }
}


QString RDUnixServer::errorString() const
{
  return unix_error_string;
}


bool RDUnixServer::hasPendingConnections() const
{
  return false;
}


bool RDUnixServer::isListening() const
{
  return unix_is_listening;
}


bool RDUnixServer::listenToPathname(const QString &pathname)
{
  struct sockaddr_un sa;

  if((unix_socket=socket(AF_UNIX,SOCK_STREAM,0))<0) {
    unix_error_string=QString("unable to create socket")+" ["+
      QString(strerror(errno))+"]";
    return false;
  }
  memset(&sa,0,sizeof(sa));
  sa.sun_family=AF_UNIX;
  strncpy(sa.sun_path,pathname.toUtf8(),UNIX_PATH_MAX);
  if(bind(unix_socket,(struct sockaddr *)(&sa),sizeof(sa))<0) {
    unix_error_string=QString("unable to bind address")+" ["+
      QString(strerror(errno))+"]";
    return false;
  }
  if(listen(unix_socket,unix_max_pending_connections)<0) {
    unix_error_string=QString("unable to listen")+" ["+
      QString(strerror(errno))+"]";
    return false;
  }
  unix_is_listening=true;
  unix_notifier=new QSocketNotifier(unix_socket,QSocketNotifier::Read,this);
  connect(unix_notifier,SIGNAL(activated(int)),
	  this,SLOT(newConnectionData(int)));

  return true;
}


bool RDUnixServer::listenToAbstract(const QString &addr)
{
  struct sockaddr_un sa;

  if((unix_socket=socket(AF_UNIX,SOCK_STREAM,0))<0) {
    unix_error_string=QString("unable to create socket")+" ["+
      QString(strerror(errno))+"]";
    return false;
  }
  memset(&sa,0,sizeof(sa));
  sa.sun_family=AF_UNIX;
  strncpy(sa.sun_path+1,addr.toUtf8(),UNIX_PATH_MAX-1);
  if(bind(unix_socket,(struct sockaddr *)(&sa),sizeof(sa))<0) {
    unix_error_string=QString("unable to bind address")+" ["+
      QString(strerror(errno))+"]";
    return false;
  }
  if(listen(unix_socket,unix_max_pending_connections)<0) {
    unix_error_string=QString("unable to listen")+" ["+
      QString(strerror(errno))+"]";
    return false;
  }
  unix_is_listening=true;
  unix_notifier=new QSocketNotifier(unix_socket,QSocketNotifier::Read,this);
  connect(unix_notifier,SIGNAL(activated(int)),
	  this,SLOT(newConnectionData(int)));

  return true;
}


QTcpSocket *RDUnixServer::nextPendingConnection()
{
  int sock;
  QTcpSocket *tcpsock=NULL;
  struct sockaddr_un sa;
  socklen_t sa_len=sizeof(sa);

  memset(&sa,0,sizeof(sa));
  
  if((sock=accept(unix_socket,(struct sockaddr *)(&sa),&sa_len))<0) {
    unix_error_string=QString("accept failed [")+QString(strerror(errno));
    return NULL;
  }
  tcpsock=new QTcpSocket(this);
  tcpsock->setSocketDescriptor(sock,QAbstractSocket::ConnectedState);

  return tcpsock;
}


int RDUnixServer::maxPendingConnections() const
{
  return unix_max_pending_connections;
}


void RDUnixServer::setMaxPendingConnections(int num)
{
  unix_max_pending_connections=num;
}


int RDUnixServer::socketDescriptor() const
{
  return unix_socket;
}


void RDUnixServer::setSocketDescriptor(int sock)
{
  unix_socket=sock;
  if(unix_notifier!=NULL) {
    delete unix_notifier;
  }
  unix_notifier=new QSocketNotifier(unix_socket,QSocketNotifier::Read,this);
  connect(unix_notifier,SIGNAL(activated(int)),
	  this,SLOT(newConnectionData(int)));
}


void RDUnixServer::newConnectionData(int fd)
{
  emit newConnection();
}
