// rdcatchd_socket.h
//
// A telephone services server for Mithlond
//
//   (C) Copyright 2002,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDCATCHD_SOCKET_H
#define RDCATCHD_SOCKET_H

#include <qobject.h>
#include <qstring.h>
#include <q3serversocket.h>
#include <qhostaddress.h>

class RDCatchdSocket : public Q3ServerSocket
{
  Q_OBJECT
  public:
   RDCatchdSocket(Q_UINT16 port,int backlog=0,QObject *parent=0);
   RDCatchdSocket(const QHostAddress &address,Q_UINT16 port,int backlog=0,
	     QObject *parent=0,const char *name=0);
   void newConnection(int socket);

  signals:
   void connection(int);

  private:
   Q3ServerSocket *socket;
};


#endif 
