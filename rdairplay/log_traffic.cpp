// log_traffic.cpp
//
// Add an entry to the reconciliation table.
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: log_traffic.cpp,v 1.20.8.2.2.3 2014/06/24 18:27:05 cvs Exp $
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

#include <rdconf.h>
#include <rddb.h>
#include <rdescape_string.h>

#include <log_traffic.h>
#include <globals.h>


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
  sql=QString("insert into `")+svcname+"_SRT` set "+
    QString().sprintf("LENGTH=%d,",length)+
    "LOG_NAME=\""+RDEscapeString(logname.utf8())+"\","+
    QString().sprintf("LOG_ID=%d,",logline->id())+
    QString().sprintf("CART_NUMBER=%u,",logline->cartNumber())+
    "STATION_NAME=\""+RDEscapeString(rdstation_conf->name().utf8())+"\","+
    "EVENT_DATETIME=\""+datetime.toString("yyyy-MM-dd")+" "+
    logline->startTime(RDLogLine::Actual).toString("hh:mm:ss")+"\","+
    QString().sprintf("EVENT_TYPE=%d,",action)+
    QString().sprintf("EVENT_SOURCE=%d,",logline->source())+
    "EXT_START_TIME=\""+logline->extStartTime().toString("hh:mm:ss")+"\","+
    QString().sprintf("EXT_LENGTH=%d,",logline->extLength())+
    "EXT_DATA=\""+RDEscapeString(logline->extData())+"\","+
    "EXT_EVENT_ID=\""+RDEscapeString(logline->extEventId())+"\","+
    "EXT_ANNC_TYPE=\""+RDEscapeString(logline->extAnncType())+"\","+
    QString().sprintf("PLAY_SOURCE=%d,",src)+
    QString().sprintf("CUT_NUMBER=%d,",logline->cutNumber())+
    "EXT_CART_NAME=\""+RDEscapeString(logline->extCartName().utf8())+"\","+
    "TITLE=\""+RDEscapeString(logline->title().utf8())+"\","+
    "ARTIST=\""+RDEscapeString(logline->artist().utf8())+"\","+
    "SCHEDULED_TIME=\""+RDEscapeString(logline->startTime(RDLogLine::Logged).
				       toString("hh:mm:ss"))+"\","+
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
    "ISCI=\""+RDEscapeString(logline->isci().utf8())+"\"";

  q=new RDSqlQuery(sql);
  delete q;
}
