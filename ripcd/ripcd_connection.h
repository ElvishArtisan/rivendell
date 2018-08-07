// ripcd_connection.h
//
// Rivendell Interprocess Communication Daemon
//
//   (C) Copyright 2010,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RIPCD_CONNECTION_H
#define RIPCD_CONNECTION_H

#include <qobject.h>
#include <qtcpsocket.h>
#include <qstring.h>

#include <rd.h>

class RipcdConnection
{
  public:
   RipcdConnection(int id,QTcpSocket *sock);
   ~RipcdConnection();
   int id() const;
   QTcpSocket *socket() const;
   bool isAuthenticated() const;
   void setAuthenticated(bool state);
   bool isClosing() const;
   void close();
   QString accum;

 private:
   int ripcd_id;
   bool ripcd_authenticated;
   QTcpSocket *ripcd_socket;
   bool ripcd_closing;
};


#endif  // RIPCD_CONNECTION_H
