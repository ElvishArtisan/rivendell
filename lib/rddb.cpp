// rddb.cpp
//
//   Database driver with automatic reconnect
//
//   (C) Copyright 2007 Dan Mills <dmills@exponent.myzen.co.uk>
//   (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdio.h>

#include <QObject>
#include <QString>
#include <QTextCodec>
#include <QTranslator>
#include <QSqlError>
#include <QVariant>

#include "rddb.h"
#include "rddbheartbeat.h"

bool RDOpenDb (unsigned *schema,QString *error,RDConfig *config)
{
  QSqlDatabase db;

  *schema=0;
  if (!db.isOpen()){
    db=QSqlDatabase::addDatabase(config->mysqlDriver());
    if(!db.isValid()) {
      *error+= QString(QObject::tr("Couldn't initialize QSql driver!"));
      return false;
    }
    db.setHostName(config->mysqlHostname());
    db.setDatabaseName(config->mysqlDbname());
    db.setUserName(config->mysqlUsername());
    db.setPassword(config->mysqlPassword());
    if(!db.open()) {
      *error+=QString(QObject::tr("Couldn't open mySQL connection!"));
      db.removeDatabase(config->mysqlDbname());
      db.close();
      return false;
    }
  }
  new RDDbHeartbeat(config->mysqlHeartbeatInterval());
  //  QSqlQuery *q=new QSqlQuery("set character_set_results='utf8'");
  //  delete q;

  QSqlQuery *q=new QSqlQuery("select DB from VERSION");
  if(q->first()) {
    *schema=q->value(0).toUInt();
  }
  delete q;

  return true;
}


RDSqlQuery::RDSqlQuery (const QString &query):
  QSqlQuery(query)
{
  if(!isActive()) {
    QString err=QObject::tr("invalid SQL or failed DB connection")+
      +"["+lastError().text()+"]: "+query;

    fprintf(stderr,"%s\n",(const char *)err);
#ifndef WIN32
    syslog(LOG_ERR,(const char *)err);
#endif  // WIN32
  }
}
