// rdloglock.cpp
//
// Log locking routines for Rivendell
//
//   (C) Copyright 2017 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef WIN32
#include <syslog.h>
#endif  // WIN32

#include <qdatetime.h>

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
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;
  bool ret=false;
  QDateTime now=QDateTime::currentDateTime();

  sql=QString("update LOGS set ")+
    "LOCK_USER_NAME=\""+RDEscapeString(lock_user->name())+"\","+
    "LOCK_STATION_NAME=\""+RDEscapeString(lock_station->name())+"\","+
    "LOCK_IPV4_ADDRESS=\""+RDEscapeString(lock_station->address().toString())+
    "\","+
    "LOCK_DATETIME=now() where "+
    "(NAME=\""+RDEscapeString(lock_log_name)+"\")&&"+
    "((LOCK_DATETIME is null)||"+
    "(LOCK_DATETIME<\""+RDEscapeString(now.addSecs(-RD_LOG_LOCK_TIMEOUT/1000).toString("yyyy-MM-dd hh:mm:ss"))+"\"))";
  q=new RDSqlQuery(sql);
  if(q->numRowsAffected()>0) {
    lock_timer->start(RD_LOG_LOCK_TIMEOUT/2);
    lock_locked=true;
    ret=true;
  }
  else {
    sql=QString("select ")+
      "LOCK_USER_NAME,"+
      "LOCK_STATION_NAME,"+
      "LOCK_IPV4_ADDRESS "+
      "from LOGS where "+
      "NAME=\""+RDEscapeString(lock_log_name)+"\"";
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


void RDLogLock::clearLock()
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("update LOGS set ")+
    "LOCK_USER_NAME=null,"+
    "LOCK_STATION_NAME=null,"+
    "LOCK_IPV4_ADDRESS=null,"+
    "LOCK_DATETIME=null where "+
    "(NAME=\""+RDEscapeString(lock_log_name)+"\")&&"+
    "(LOCK_USER_NAME=\""+RDEscapeString(lock_user->name())+"\")&&"+
    "(LOCK_STATION_NAME=\""+RDEscapeString(lock_station->name())+"\")&&"+
    "(LOCK_IPV4_ADDRESS=\""+RDEscapeString(lock_station->address().toString())+
    "\")";
  q=new RDSqlQuery(sql);
  delete q;
  lock_timer->stop();
  lock_locked=false;
}


void RDLogLock::updateLock()
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("update LOGS set ")+
    "LOCK_DATETIME=now() where "+
    "(NAME=\""+RDEscapeString(lock_log_name)+"\")&&"+
    "(LOCK_USER_NAME=\""+RDEscapeString(lock_user->name())+"\")&&"+
    "(LOCK_STATION_NAME=\""+RDEscapeString(lock_station->name())+"\")&&"+
    "(LOCK_IPV4_ADDRESS=\""+RDEscapeString(lock_station->address().toString())+
    "\")";
  q=new RDSqlQuery(sql);
#ifndef WIN32
  if(q->numRowsAffected()==0) {
    syslog(LOG_WARNING,"lock on log \"%s\" has gone stale",
	   (const char *)lock_log_name);
  }
#endif  // WIN32
  delete q;
}
