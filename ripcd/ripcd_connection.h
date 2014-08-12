// ripcd_connection.h
//
// Rivendell Interprocess Communication Daemon
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: ripcd_connection.h,v 1.1 2010/08/03 18:41:14 cvs Exp $
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
#include <qstring.h>

#include <rdsocket.h>
#include <rd.h>

class RipcdConnection
{
  public:
  RipcdConnection(int id,int fd);
   ~RipcdConnection();
   RDSocket *socket;
   char args[RD_RML_MAX_ARGS][RD_RML_MAX_LENGTH];
   int istate;
   int argnum;
   int argptr;
   bool auth;
};


#endif  // RIPCD_CONNECTION_H
