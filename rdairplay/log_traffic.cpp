// log_traffic.cpp
//
// Add an entry to the reconciliation table.
//
//   (C) Copyright 2002-2016-2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdapplication.h>
#include <rdconf.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdsvc.h>

#include "globals.h"
#include "log_traffic.h"

void LogTraffic(const QString &svcname,const QString &logname,
		RDLogLine *logline,RDLogLine::PlaySource src,
		RDAirPlayConf::TrafficAction action,bool onair_flag)
{
  QString sql;
  RDSqlQuery *q;
  QDateTime datetime=QDateTime(QDate::currentDate(),QTime::currentTime());
  int length=logline->startTime(RDLogLine::Actual).msecsTo(datetime.time());
  if(length<0) {  // Event crossed midnight!
    length+=86400000;
    datetime.setDate(datetime.date().addDays(-1));
  }

  if((logline==NULL)||(svcname.isEmpty())) {
    return;
  }

  QString eventDateTimeSQL = "NULL";

  if(datetime.isValid() && logline->startTime(RDLogLine::Actual).isValid())
    eventDateTimeSQL = RDCheckDateTime(QDateTime(datetime.date(),
          logline->startTime(RDLogLine::Actual)), "yyyy-MM-dd hh:mm:ss");

  sql=QString("insert into `")+RDSvc::svcTableName(svcname)+"` set "+
    QString().sprintf("LENGTH=%d,",length)+
    "LOG_NAME=\""+RDEscapeString(logname.utf8())+"\","+
    QString().sprintf("LOG_ID=%d,",logline->id())+
    QString().sprintf("CART_NUMBER=%u,",logline->cartNumber())+
    "STATION_NAME=\""+RDEscapeString(rda->station()->name().utf8())+"\","+
    "EVENT_DATETIME="+eventDateTimeSQL+","+
    QString().sprintf("EVENT_TYPE=%d,",action)+
    QString().sprintf("EVENT_SOURCE=%d,",logline->source())+
    "EXT_START_TIME="+RDCheckDateTime(logline->extStartTime(),"hh:mm:ss")+","+
    QString().sprintf("EXT_LENGTH=%d,",logline->extLength())+
    "EXT_DATA=\""+RDEscapeString(logline->extData())+"\","+
    "EXT_EVENT_ID=\""+RDEscapeString(logline->extEventId())+"\","+
    "EXT_ANNC_TYPE=\""+RDEscapeString(logline->extAnncType())+"\","+
    QString().sprintf("PLAY_SOURCE=%d,",src)+
    QString().sprintf("CUT_NUMBER=%d,",logline->cutNumber())+
    "EXT_CART_NAME=\""+RDEscapeString(logline->extCartName().utf8())+"\","+
    "TITLE=\""+RDEscapeString(logline->title().utf8())+"\","+
    "ARTIST=\""+RDEscapeString(logline->artist().utf8())+"\","+
    "SCHEDULED_TIME="+RDCheckDateTime(logline->startTime(RDLogLine::Logged),
				       "hh:mm:ss")+","+
    "ISRC=\""+RDEscapeString(logline->isrc().utf8())+"\","+
    "PUBLISHER=\""+RDEscapeString(logline->publisher().utf8())+"\","+
    "COMPOSER=\""+RDEscapeString(logline->composer().utf8())+"\","+
    QString().sprintf("USAGE_CODE=%d,",logline->usageCode())+
    QString().sprintf("START_SOURCE=%d,",logline->startSource())+
    "ONAIR_FLAG=\""+RDYesNo(onair_flag)+"\","+
    "ALBUM=\""+RDEscapeString(logline->album().utf8())+"\","+
    "LABEL=\""+RDEscapeString(logline->label().utf8())+"\","+
    "USER_DEFINED=\""+RDEscapeString(logline->userDefined().utf8())+"\","+
    "CONDUCTOR=\""+RDEscapeString(logline->conductor().utf8())+"\","+
    "SONG_ID=\""+RDEscapeString(logline->songId().utf8())+"\","+
    "DESCRIPTION=\""+RDEscapeString(logline->description().utf8())+"\","+
    "OUTCUE=\""+RDEscapeString(logline->outcue().utf8())+"\","+
    "ISCI=\""+RDEscapeString(logline->isci().utf8())+"\"";

  q=new RDSqlQuery(sql);
  delete q;
}
