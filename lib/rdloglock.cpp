// rdloglock.cpp
//
// Log locking routines for Rivendell
//
//   (C) Copyright 2017-2019 Fred Gleason <fredg@paravelsystems.com>
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

#include <syslog.h>

#include <qdatetime.h>

#include "rdapplication.h"
#include "rddb.h"
#include "rdescape_string.h"
#include "rdloglock.h"

RDLogLock::RDLogLock(const QString &log_name,RDUser *user,RDStation *station,
		     QObject *parent)
  : QObject(parent)
{
  lock_log_name=log_name;
  lock_user=user;
  lock_station=station;
  lock_locked=false;

  lock_timer=new QTimer(this);
  connect(lock_timer,SIGNAL(timeout()),this,SLOT(updateLock()));
}


RDLogLock::~RDLogLock()
{
  if(lock_locked) {
    clearLock();
  }
  delete lock_timer;
}


bool RDLogLock::isLocked() const
{
  return lock_locked;
}


bool RDLogLock::tryLock(QString *username,QString *stationname,
			QHostAddress *addr)
{
  bool ret=false;
  QString guid=RDLogLock::makeGuid(lock_station->name());

  *username=lock_user->name();
  *stationname=lock_station->name();
  addr->setAddress(lock_station->address().toString());
  if(RDLogLock::tryLock(username,stationname,addr,lock_log_name,guid)) {
    lock_timer->start(RD_LOG_LOCK_TIMEOUT/2);
    lock_guid=guid;
    lock_locked=true;
    ret=true;
  }

  return ret;
}


void RDLogLock::clearLock()
{
  RDLogLock::clearLock(lock_guid);
  lock_guid=QString();
  lock_timer->stop();
  lock_locked=false;
}


void RDLogLock::updateLock()
{
  RDLogLock::updateLock(lock_log_name,lock_guid);
}


bool RDLogLock::tryLock(QString *username,QString *stationname,
			QHostAddress *addr,const QString &log_name,
			const QString &guid)
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;
  bool ret=false;
  QDateTime now=QDateTime::currentDateTime();

  sql=QString("update LOGS set ")+
    "LOCK_USER_NAME=\""+RDEscapeString(*username)+"\","+
    "LOCK_STATION_NAME=\""+RDEscapeString(*stationname)+"\","+
    "LOCK_IPV4_ADDRESS=\""+RDEscapeString(addr->toString())+
    "\","+
    "LOCK_GUID=\""+RDEscapeString(guid)+"\","+
    "LOCK_DATETIME=now() where "+
    "(NAME=\""+RDEscapeString(log_name)+"\")&&"+
    "((LOCK_DATETIME is null)||"+
    "(LOCK_DATETIME<\""+RDEscapeString(now.addSecs(-RD_LOG_LOCK_TIMEOUT/1000).toString("yyyy-MM-dd hh:mm:ss"))+"\"))";
  q=new RDSqlQuery(sql);
  if(q->numRowsAffected()>0) {
    ret=true;
  }
  else {
    sql=QString("select ")+
      "LOCK_USER_NAME,"+
      "LOCK_STATION_NAME,"+
      "LOCK_IPV4_ADDRESS "+
      "from LOGS where "+
      "NAME=\""+RDEscapeString(log_name)+"\"";
    q1=new RDSqlQuery(sql);
    if(q1->first()) {
      *username=q1->value(0).toString();
      *stationname=q1->value(1).toString();
      addr->setAddress(q1->value(2).toString());
    }
    delete q1;
  }
  delete q;

  return ret;
}


void RDLogLock::updateLock(const QString &log_name,const QString &guid)
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("update LOGS set ")+
    "LOCK_DATETIME=now() where "+
    "LOCK_GUID=\""+RDEscapeString(guid)+"\"";
  q=new RDSqlQuery(sql);
  if(q->numRowsAffected()==0) {
    rda->syslog(LOG_WARNING,"lock on log \"%s\" has evaporated!",
		(const char *)log_name);
  }
  delete q;
}


void RDLogLock::clearLock(const QString &guid)
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("update LOGS set ")+
    "LOCK_USER_NAME=null,"+
    "LOCK_STATION_NAME=null,"+
    "LOCK_IPV4_ADDRESS=null,"+
    "LOCK_GUID=null,"+
    "LOCK_DATETIME=null where "+
    "LOCK_GUID=\""+RDEscapeString(guid)+"\"";
  q=new RDSqlQuery(sql);
  delete q;
}


bool RDLogLock::validateLock(const QString &log_name,const QString &guid)
{
  QString sql;
  RDSqlQuery *q;
  bool ret=false;
  QDateTime now=QDateTime::currentDateTime();

  sql=QString("select NAME from LOGS where ")+
    "(NAME=\""+RDEscapeString(log_name)+"\")&&"+
    "(LOCK_GUID=\""+RDEscapeString(guid)+"\")&&"+
    "(LOCK_DATETIME>\""+RDEscapeString(now.addSecs(-RD_LOG_LOCK_TIMEOUT/1000).
				       toString("yyyy-MM-dd hh:mm:ss"))+"\")";
  q=new RDSqlQuery(sql);
  ret=q->first();
  delete q;

  return ret;
}


QString RDLogLock::makeGuid(const QString &stationname)
{
  return stationname+QDateTime::currentDateTime().
    toString("yyyyMMddhhmmsszzz");
}
