// createdb.h
//
// Create a Rivendell Database
//
//   (C) Copyright 2002-2018 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef CREATEDB_H
#define CREATEDB_H

#include <qsqldatabase.h>
#include <qsqlquery.h>

#include <rdconfig.h>

class CreateDb
{ 
  public:
   CreateDb(QString host,QString database,QString username,QString password);
   ~CreateDb();
   bool create(QObject *parent,QString *err_str, RDConfig *config);
   bool isOpen();

  private:
   QString db_host;
   QString db_name;
   QString db_user;
   QString db_pass;
   QSqlDatabase db;
};

#endif  // CREATEDB_H


