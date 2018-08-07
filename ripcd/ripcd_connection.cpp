// ripcd_connection.cpp
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

#include <ripcd_connection.h>

RipcdConnection::RipcdConnection(int id,int fd)
{
  ripcd_id=id;
  ripcd_authenticated=false;
  accum="";
  ripcd_socket=new Q3Socket();
  ripcd_socket->setSocket(fd);
}


RipcdConnection::~RipcdConnection()
{
  delete ripcd_socket;
}


int RipcdConnection::id() const
{
  return ripcd_id;
}


Q3Socket *RipcdConnection::socket() const
{
  return ripcd_socket;
}


bool RipcdConnection::isAuthenticated() const
{
  return ripcd_authenticated;
}


void RipcdConnection::setAuthenticated(bool state)
{
  ripcd_authenticated=state;
}
