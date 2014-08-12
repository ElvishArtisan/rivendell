// panel_copy.h
//
// A utility for copying SoundPanel assignments between databases.
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: panel_copy.h,v 1.5 2010/07/29 19:32:32 cvs Exp $
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

#ifndef PANEL_COPY_H
#define PANEL_COPY_H

#include <qobject.h>
#include <qapplication.h>
#include <qsqldatabase.h>

#include <rdconfig.h>

#define PANEL_COPY_USAGE "-h <src-mysql-host> -H <dest-mysql-host>\n"

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
