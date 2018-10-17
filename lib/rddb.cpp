// rddb.cpp
//
//   Database driver with automatic reconnect
//
//   (C) Copyright 2007 Dan Mills <dmills@exponent.myzen.co.uk>
//   (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <QObject>
#include <QString>
#include <QTextCodec>
#include <QTranslator>
#include <QSqlError>
#include <QStringList>
#include <QVariant>

#include "rddb.h"
#include "rddbheartbeat.h"

RDSqlQuery::RDSqlQuery (const QString &query,bool reconnect):
  QSqlQuery(query)
{
  QSqlDatabase db;
  QString err;
  sql_columns=0;

  if (!isActive() && reconnect) {
    db = QSqlDatabase::database();

    if (db.open()) {
      clear();
      exec(query);
      err=QObject::tr("DB connection re-established");
    }
    else {
      err=QObject::tr("Could not re-establish DB connection")+
      +"["+db.lastError().text()+"]";
    }

    fprintf(stderr,"%s\n",(const char *)err);
#ifndef WIN32
    syslog(LOG_ERR,(const char *)err);
#endif
  }

  if(isActive()) {
    QStringList f0=query.split(" ");
    if(f0[0].toLower()=="select") {
      for(int i=1;i<f0.size();i++) {
	if(f0[i].toLower()=="from") {
	  QString fields;
	  for(int j=1;j<i;j++) {
	    fields+=f0[j];
	  }
	  QStringList f1=fields.split(",");
	  sql_columns=f1.size();
	  continue;
	}
      }
    }
  }
  else {
    err=QObject::tr("invalid SQL or failed DB connection")+
      +"["+lastError().text()+"]: "+query;

    fprintf(stderr,"%s\n",(const char *)err);
#ifndef WIN32
    syslog(LOG_ERR,(const char *)err);
#endif  // WIN32
  }
}


int RDSqlQuery::columns() const
{
  return sql_columns;
}


QVariant RDSqlQuery::run(const QString &sql,bool *ok)
{
  QVariant ret;

  RDSqlQuery *q=new RDSqlQuery(sql);
  if(ok!=NULL) {
    *ok=q->isActive();
  }
  ret=q->lastInsertId();
  delete q;

  return ret;
}


bool RDSqlQuery::apply(const QString &sql,QString *err_msg)
{
  bool ret=false;

  RDSqlQuery *q=new RDSqlQuery(sql);
  ret=q->isActive();
  if((err_msg!=NULL)&&(!ret)) {
    *err_msg="sql error: "+q->lastError().text()+" query: "+sql;
  }
  delete q;

  return ret;
}


int RDSqlQuery::rows(const QString &sql)
{
  int ret=0;

  RDSqlQuery *q=new RDSqlQuery(sql);
  ret=q->size();
  delete q;

  return ret;
}


bool RDOpenDb (int *schema,QString *err_str,RDConfig *config)
{
  QSqlDatabase db;
  QString sql;
  QSqlQuery *q;

  if (!db.isOpen()){
    db=QSqlDatabase::addDatabase(config->mysqlDriver());
    if(!db.isValid()) {
      *err_str+= QString(QObject::tr("Couldn't initialize MySql driver!"));
      return false;
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
      return false;
    }
  }
  new RDDbHeartbeat(config->mysqlHeartbeatInterval());
  sql=QString("set NAMES utf8mb4 collate utf8mb4_general_ci");
  q=new QSqlQuery(sql);
  delete q;

  *schema=-1;
  sql=QString("show tables where ")+
    "Tables_in_"+config->mysqlDbname()+"=\"VERSION\"";
  q=new QSqlQuery(sql);
  if(q->first()) {
    delete q;
    q=new QSqlQuery("select DB from VERSION");
    if(q->first()) {
      *schema=q->value(0).toUInt();
    }
  }
  else {
    delete q;
    sql=QString("show tables");
    q=new QSqlQuery(sql);
    if(!q->first()) {
      *schema=0;
    }
  }
  delete q;

  return true;
}
