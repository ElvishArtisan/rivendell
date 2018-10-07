// db.cpp
//
// Create Rivendell MySQL database RDDbConfig
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

#include <qsqldatabase.h>
#include <qsqlquery.h>

#include "db.h"

Db::Db(QString *err_str,RDConfig *config)
{
  QSqlQuery *q;
  db_schema=0;

  //
  // Open Database
  //
  if (!db.isOpen()){
    db=QSqlDatabase::addDatabase(config->mysqlDriver());
    if(!db.isValid()) {
      *err_str+= QString(QObject::tr("Couldn't initialize MySql driver!"));
      return;
    }
    db.setHostName(config->mysqlHostname());
    db.setDatabaseName(config->mysqlDbname());
    db.setUserName(config->mysqlUsername());
    db.setPassword(config->mysqlPassword());
    if(!db.open()) {
      *err_str+=QString(QObject::tr("Couldn't open MySQL connection on"))+
	" \""+config->mysqlHostname()+"\".";
      db.removeDatabase(config->mysqlDbname());
      db.close();
      return;
    }

    q=new QSqlQuery("select DB from VERSION");
    if(q->first()) {
      db_schema=q->value(0).toUInt();
    }
    delete q;
  }
}


Db::~Db()
{
  if(db.isOpen()) {
//    db.removeDatabase(config->mysqlDbname());
    db.close();
  }
}

bool Db::isOpen()
{
  return db.isOpen();
}

unsigned Db::schema()
{
  return db_schema;
}

