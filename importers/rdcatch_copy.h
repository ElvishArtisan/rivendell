// rdcatch_copy.h
//
// An RDCatch event copier.
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcatch_copy.h,v 1.6 2010/07/29 19:32:32 cvs Exp $
//      $Date: 2010/07/29 19:32:32 $
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

#ifndef RDCATCH_COPY_H
#define RDCATCH_COPY_H

#include <qobject.h>
#include <qapplication.h>
#include <qsqldatabase.h>

#include <rdstation.h>
#include <rdripc.h>
#include <rdcatch_connect.h>
#include <rdstation.h>
#include <rdconfig.h>

#define RDCATCH_COPY_USAGE "-h <src-mysql-host> -s <src-rd-host> -H <dest-mysql-host> -S <dest-rd-host>\n"

class MainObject : public QObject
{
  Q_OBJECT
 public:
  MainObject(QObject *parent=0,const char *name=0);

 private:
  QSqlDatabase *src_db;
  QSqlDatabase *dest_db;
  RDConfig *rd_config;
};


#endif 
