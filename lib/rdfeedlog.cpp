// rdfeedlog.cpp
//
// Functions for manipulating RSS feed log tables.
//
//   (C) Copyright 2007,2016 Fred Gleason <fredg@paravelsystems.com>
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
  sql=QString("create table if not exists `")+keyname+"_FLG` ("+
    "ID int unsigned primary key auto_increment,"+
    "CAST_ID int unsigned,"+
    "ACCESS_DATE date,"+
    "ACCESS_COUNT int unsigned default 0,"+
    "index CAST_ID_IDX(CAST_ID,ACCESS_DATE))";
  q=new RDSqlQuery(sql);
  delete q;
}


void RDDeleteFeedLog(QString keyname)
{
  QString sql;
  RDSqlQuery *q;

  keyname.replace(" ","_");
  sql=QString("drop table `")+keyname+"_FLG`";
  q=new RDSqlQuery(sql);
  delete q;
}


void RDDeleteCastCount(QString keyname,unsigned cast_id)
{
  QString sql;
  RDSqlQuery *q;

  keyname.replace(" ","_");
  sql=QString("delete from `")+keyname+"_FLG` where "+
    QString().sprintf("CAST_ID=%u",cast_id);
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
  sql=QString("select ACCESS_COUNT from `")+keyname+"_FLG` where "+
    QString().sprintf("(CAST_ID=%u)&&",cast_id)+
    "(ACCESS_DATE=\""+current_date.toString("yyyy-MM-dd")+"\")";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    sql=QString("update `")+keyname+"_FLG` set "+
      QString().sprintf("ACCESS_COUNT=%u where ",q->value(0).toUInt()+1)+
      QString().sprintf("(CAST_ID=%u)&&",cast_id)+
      "(ACCESS_DATE=\""+current_date.toString("yyyy-MM-dd")+"\")";
  }
  else {
    sql=QString("insert into `")+keyname+"_FLG` set "+
      QString().sprintf("CAST_ID=%u,",cast_id)+
      "ACCESS_DATE=\""+current_date.toString("yyyy-MM-dd")+"\","+
      "ACCESS_COUNT=1";
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
