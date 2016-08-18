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

#include <Q3ServerSocket>
#include <QObject>
#include <QString>
#include <QTextCodec>
#include <QTranslator>
#include <QSqlError>

#include "rddb.h"
#include "rddbheartbeat.h"

static QSqlDatabase db;
static RDSqlDatabaseStatus * dbStatus = NULL;

bool RDOpenDb (unsigned *schema,QString *error,RDConfig *config)
{
  static bool firsttime = true;

  *schema=0;
  //  RDConfig *cf = RDConfiguration();
  //  cf->load();
  //  assert (cf);
  if (!db.isOpen()){
    db=QSqlDatabase::addDatabase(config->mysqlDriver());
    /*
    if(db.isOpen()) {
      *error+= QString(QObject::tr("Couldn't initialize QSql driver!"));
      return false;
    }
    */
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
  if (firsttime){
    new RDDbHeartbeat(config->mysqlHeartbeatInterval());
    firsttime=false;
  }
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
  //printf("lastQuery: %s\n",(const char *)lastQuery());

  // With any luck, by the time we get here, we have already done the biz...
  /*
  unsigned schema;
  if (!isActive()){ //DB Offline?
    QString err=QObject::tr("invalid SQL or failed DB connection")+
      +"["+lastError().text()+"]: "+query;

    fprintf(stderr,"%s\n",(const char *)err);
#ifndef WIN32
    syslog(LOG_ERR,(const char *)err);
#endif  // WIN32
    QSqlDatabase *ldb = QSqlDatabase::database();
    // Something went wrong with the DB, trying a reconnect
    ldb->removeDatabase(RDConfiguration()->mysqlDbname());
    ldb->close();
    db = NULL;
    RDInitDb (&schema);
    QSqlQuery::prepare (query);
    QSqlQuery::exec ();
    if (RDDbStatus()){
      if (isActive()){
	RDDbStatus()->sendRecon();
      } else {
	RDDbStatus()->sendDiscon(query);
      }
    }
  } else {
    RDDbStatus()->sendRecon();
  }
  */
}


void RDSqlDatabaseStatus::sendRecon()
{
  if (discon){
    discon = false;
    emit reconnected();
    fprintf (stderr,"Database connection restored.\n");
    emit logText(RDConfig::LogErr,QString(tr("Database connection restored.")));
  }
}

void RDSqlDatabaseStatus::sendDiscon(QString query)
{
  if (!discon){
    emit connectionFailed();
    fprintf (stderr,"Database connection failed: %s\n",(const char *)query);
    emit logText(RDConfig::LogErr,
		 QString(tr("Database connection failed : ")) + query);
    discon = true;
  }
}

RDSqlDatabaseStatus::RDSqlDatabaseStatus()
{
  discon = false;
}

RDSqlDatabaseStatus * RDDbStatus()
{
  if (!dbStatus){
    dbStatus = new RDSqlDatabaseStatus;
  }
  return dbStatus;
}
