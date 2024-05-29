// rdunixsocket.cpp
//
// UNIX Socket
//
//   (C) Copyright 2018-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <linux/un.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "rdunixsocket.h"

RDUnixSocket::RDUnixSocket(QObject *parent)
  : QTcpSocket(parent)
{
}


bool RDUnixSocket::connectToPathname(const QString &pathname,
				     QAbstractSocket::OpenMode mode)
{
  return false;
}


bool RDUnixSocket::connectToAbstract(const QString &addr,
				     QAbstractSocket::OpenMode mode)
{
  int sock;
  struct sockaddr_un sa;

  if((sock=::socket(AF_UNIX,SOCK_SEQPACKET,0))<0) {
    return false;
  }
  memset(&sa,0,sizeof(sa));
  sa.sun_family=AF_UNIX;
  strncpy(sa.sun_path+1,addr.toUtf8(),UNIX_PATH_MAX-2);
  if(::connect(sock,(struct sockaddr *)(&sa),sizeof(sa))<0) {
    return false;
  }
  setSocketDescriptor(sock,QAbstractSocket::ConnectedState,mode);

  return true;
}
