// ripcd_connection.cpp
//
// Rivendell Interprocess Communication Daemon
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: ripcd_connection.cpp,v 1.1 2010/08/03 18:41:14 cvs Exp $
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
  socket=NULL;
  istate=0;
  argnum=0;
  argptr=0;
  auth=false;
  socket=new RDSocket(id);
  socket->setSocket(fd);
}


RipcdConnection::~RipcdConnection()
{
  delete socket;
}
