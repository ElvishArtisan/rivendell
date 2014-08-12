// rdfeedlog.cpp
//
// Functions for manipulating RSS feed log tables.
//
//   (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdfeedlog.cpp,v 1.4 2010/07/29 19:32:33 cvs Exp $
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

#include <rdweb.h>
#include <rddb.h>
#include <rdfeedlog.h>


void RDCreateFeedLog(QString keyname)
{
  QString sql;
  RDSqlQuery *q;

  keyname.replace(" ","_");
  sql=QString().sprintf("create table if not exists %s_FLG (\
                         ID int unsigned primary key auto_increment,\
                         CAST_ID int unsigned,\
                         ACCESS_DATE date,\
                         ACCESS_COUNT int unsigned default 0,\
                         index CAST_ID_IDX(CAST_ID,ACCESS_DATE))",
			(const char *)keyname);
  q=new RDSqlQuery(sql);
  delete q;
}


void RDDeleteFeedLog(QString keyname)
{
  QString sql;
  RDSqlQuery *q;

  keyname.replace(" ","_");
  sql=QString().sprintf("drop table %s_FLG",(const char *)keyname);
  q=new RDSqlQuery(sql);
  delete q;
}


void RDDeleteCastCount(QString keyname,unsigned cast_id)
{
  QString sql;
  RDSqlQuery *q;

  keyname.replace(" ","_");
  sql=QString().sprintf("delete from %s_FLG where CAST_ID=%u",
			(const char *)keyname,cast_id);
  q=new RDSqlQuery(sql);
  delete q;
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
  QDate current_date=QDate::currentDate();

  keyname.replace(" ","_");
/*
  FIXME: Table locking kills updates.  Why?

  sql=QString().sprintf("lock tables %s_FLG read",(const char *)keyname);
  q=new RDSqlQuery(sql);
  delete q;
*/
  sql=QString().sprintf("select ACCESS_COUNT from %s_FLG where \
                         (CAST_ID=%u)&&(ACCESS_DATE=\"%s\")",
			(const char *)keyname,
			cast_id,
			(const char *)current_date.toString("yyyy-MM-dd"));
  q=new RDSqlQuery(sql);
  if(q->first()) {
    sql=QString().sprintf("update %s_FLG set ACCESS_COUNT=%u where \
                         (CAST_ID=%u)&&(ACCESS_DATE=\"%s\")",
			  (const char *)keyname,
			  q->value(0).toUInt()+1,cast_id,
			  (const char *)current_date.toString("yyyy-MM-dd"));
  }
  else {
    sql=QString().sprintf("insert into %s_FLG set \
                           CAST_ID=%u,\
                           ACCESS_DATE=\"%s\",\
                           ACCESS_COUNT=1",
			  (const char *)keyname,
			  cast_id,
			  (const char *)current_date.toString("yyyy-MM-dd"));
  }
  delete q;
  q=new RDSqlQuery(sql);
  delete q;

/*
  sql="unlock tables";
  q=new RDSqlQuery(sql);
  delete q;
*/
}
