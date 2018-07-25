// rdfeedlog.cpp
//
// Functions for manipulating RSS feed log tables.
//
//   (C) Copyright 2007,2016-2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <qdatetime.h>

#include <rdapplication.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdweb.h>

#include "rdfeedlog.h"

void RDDeleteFeedLog(QString keyname)
{
  QString sql=QString("delete from CAST_DOWNLOADS where ")+
    "FEED_KEY_NAME=\""+RDEscapeString(keyname)+"\"";
  RDSqlQuery::apply(sql);
}


void RDDeleteCastCount(QString keyname,unsigned cast_id)
{
  QString sql;

  sql=QString("delete from CAST_DOWNLOADS where ")+
    "FEED_KEY_NAME=\""+RDEscapeString(keyname)+"\" && "+
    QString().sprintf("CAST_ID=%u",cast_id);
  RDSqlQuery::apply(sql);
}


void RDDeleteCastCount(unsigned feed_id,unsigned cast_id)
{
  QString sql;
  RDSqlQuery *q;

  sql=QString().sprintf("select KEY_NAME from FEEDS where ID=%u",feed_id);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    RDDeleteCastCount(q->value(0).toString(),cast_id);
  }
  delete q;
}


void RDIncrementFeedCount(QString keyname)
{
  RDIncrementCastCount(keyname,0);
}


void RDIncrementCastCount(QString keyname,unsigned cast_id)
{
  QString sql;
  RDSqlQuery *q;
  QDate now=QDate::currentDate();

  sql=QString("select ACCESS_COUNT from CAST_DOWNLOADS where ")+
    "FEED_KEY_NAME=\""+RDEscapeString(keyname)+"\" && "+
    QString().sprintf("(CAST_ID=%u)&&",cast_id)+
    "(ACCESS_DATE=\""+RDEscapeString(now.toString("yyyy-MM-dd"))+"\")";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    sql=QString("update CAST_DOWNLOADS set ")+
      QString().sprintf("ACCESS_COUNT=%u where ",q->value(0).toUInt()+1)+
      "FEED_KEY_NAME=\""+RDEscapeString(keyname)+"\" && "+
      QString().sprintf("(CAST_ID=%u)&&",cast_id)+
      "(ACCESS_DATE=\""+RDEscapeString(now.toString("yyyy-MM-dd"))+"\")";
  }
  else {
    sql=QString("insert into CAST_DOWNLOADS set ")+
      "FEED_KEY_NAME=\""+RDEscapeString(keyname)+"\","+
      QString().sprintf("CAST_ID=%u,",cast_id)+
      "ACCESS_DATE=\""+RDEscapeString(now.toString("yyyy-MM-dd"))+"\","+
      "ACCESS_COUNT=1";
  }
  RDSqlQuery::apply(sql);
  delete q;
}
