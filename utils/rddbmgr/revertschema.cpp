// revertschema.cpp
//
// Revert Rivendell DB schema
//
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

#include <rddb.h>
#include <rdescape_string.h>

#include "rddbmgr.h"

bool MainObject::RevertSchema(int cur_schema,int set_schema,QString *err_msg) const
{
  QString sql;
  RDSqlQuery *q; 
  RDSqlQuery *q1; 
  QString tablename;

  //
  // Maintainer's Note:
  //
  // When adding a schema reversion here, be sure also to implement the
  // corresponding update in updateschema.cpp!
  //


  //
  // Revert 293
  //
  if((cur_schema==293)&&(set_schema<cur_schema)) {
    sql=QString("select NAME from SERVICES");
    q=new RDSqlQuery(sql);
    while(q->next()) {
      QString tablename=q->value(0).toString()+"_STACK";
      tablename.replace(" ","_");
      sql=QString("create table `")+tablename+"` ("+
	"SCHED_STACK_ID int unsigned not null primary key,"+
	"CART int unsigned not null,"+
	"ARTIST varchar(255),"+
	"SCHED_CODES varchar(255),"+
	"SCHEDULED_AT datetime default '1000-01-01 00:00:00')"+
	db_table_create_postfix;
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }
      sql=QString("select ")+
	"SCHED_STACK_ID,"+  // 00
	"CART,"+            // 01
	"ARTIST,"+          // 02
	"SCHED_CODES,"+     // 03
	"SCHEDULED_AT "+    // 04
	"from STACK_LINES where "+
	"SERVICE_NAME=\""+RDEscapeString(q->value(0).toString())+"\"";
      q1=new RDSqlQuery(sql,false);
      while(q1->next()) {
	sql=QString("insert into `")+tablename+"` set "+
	  QString().sprintf("SCHED_STACK_ID=%u,",q1->value(0).toUInt())+
	  QString().sprintf("CART=%u,",q1->value(1).toUInt())+
	  "ARTIST=\""+RDEscapeString(q1->value(2).toString())+"\","+
	  "SCHED_CODES=\""+RDEscapeString(q1->value(3).toString())+"\","+
	  "SCHEDULED_AT=\""+RDEscapeString(q1->value(4).toDateTime().
					  toString("yyyy-MM-dd hh:mm:ss"))+"\"";
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
      sql=QString("update `")+tablename+"` set "+
	"SCHEDULED_AT=\"1000-01-01 00:00:00\" where "+
	"SCHEDULED_AT=\"1752-09-14 00:00:00\"";
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }
    }
    delete q;

    if(!DropTable("STACK_LINES",err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 292
  //
  if((cur_schema==292)&&(set_schema<cur_schema)) {
    sql=QString("select NAME from CLOCKS");
    q=new RDSqlQuery(sql);
    while(q->next()) {
      QString tablename=q->value(0).toString()+"_RULES";
      tablename.replace(" ","_");
      sql=QString("create table `")+tablename+"` ("+
	"CODE varchar(10) not null primary key,"+
	"MAX_ROW int unsigned,"+
	"MIN_WAIT int unsigned,"+
	"NOT_AFTER varchar(10),"+
	"OR_AFTER varchar(10),"+
	"OR_AFTER_II varchar(10))"+
	db_table_create_postfix;
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }

      sql=QString("select ")+
	"CODE,"+         // 00
	"MAX_ROW,"+      // 01
	"MIN_WAIT,"+     // 02
	"NOT_AFTER,"+    // 03
	"OR_AFTER,"+     // 04
	"OR_AFTER_II "+  // 05
	"from RULE_LINES where "+
	"CLOCK_NAME=\""+RDEscapeString(q->value(0).toString())+"\" "+
	"order by CODE";
      q1=new RDSqlQuery(sql,false);
      while(q1->next()) {
	sql=QString("insert into `")+tablename+"` set "+
	  "CODE=\""+RDEscapeString(q1->value(0).toString())+"\","+
	  QString().sprintf("MAX_ROW=%u,",q1->value(1).toUInt())+
	  QString().sprintf("MIN_WAIT=%u,",q1->value(2).toUInt())+
	  "NOT_AFTER=\""+RDEscapeString(q1->value(3).toString())+"\","+
	  "OR_AFTER=\""+RDEscapeString(q1->value(4).toString())+"\","+
	  "OR_AFTER_II=\""+RDEscapeString(q1->value(5).toString())+"\"";
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
      delete q1;
    }

    if(!DropTable("RULE_LINES",err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }


  //
  // Revert 291
  //
  if((cur_schema==291)&&(set_schema<cur_schema)) {
    sql=QString("select NAME from CLOCKS");
    q=new RDSqlQuery(sql);
    while(q->next()) {
      QString tablename=q->value(0).toString()+"_CLK";
      tablename.replace(" ","_");
      sql=QString("create table `")+tablename+"` ("+
	"ID int unsigned auto_increment not null primary key,"+
	"EVENT_NAME char(64) not null,"+
	"START_TIME int not null,"+
	"LENGTH int not null,"+
	"INDEX EVENT_NAME_IDX (EVENT_NAME))"+
	db_table_create_postfix;
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }

      sql=QString("select ")+
	"EVENT_NAME,"+  // 00
	"START_TIME,"+  // 01
	"LENGTH "+      // 02
	"from CLOCK_LINES where "+
	"CLOCK_NAME=\""+RDEscapeString(q->value(0).toString())+"\" "+
	"order by START_TIME";
      q1=new RDSqlQuery(sql);
      while(q1->next()) {
	sql=QString("insert into `")+tablename+"` set "+
	  "EVENT_NAME=\""+RDEscapeString(q1->value(0).toString())+"\","+
	  QString().sprintf("START_TIME=%d,",q1->value(1).toInt())+
	  QString().sprintf("LENGTH=%d",q1->value(2).toInt());
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
      delete q1;
    }
    delete q;

    if(!DropTable("CLOCK_LINES",err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 290
  //
  if((cur_schema==290)&&(set_schema<cur_schema)) {
    sql=QString("select NAME from EVENTS");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      //
      // PreImport Events
      //
      QString tablename=q->value(0).toString()+"_PRE";
      tablename.replace(" ","_");
      sql=QString("create table if not exists `")+tablename+"` ("+
      "ID int not null primary key,"+
      "COUNT int not null,"+
      "TYPE int default 0,"+
      "SOURCE int not null,"+
      "START_TIME int,"+
      "GRACE_TIME int default 0,"+
      "CART_NUMBER int UNSIGNED not null default 0,"+
      "TIME_TYPE int not null,"+
      "POST_POINT enum('N','Y') default 'N',"+
      "TRANS_TYPE int not null,"+
      "START_POINT int not null default -1,"+
      "END_POINT int not null default -1,"+
      "FADEUP_POINT int default -1,"+
      QString().sprintf("FADEUP_GAIN int default %d,",RD_FADE_DEPTH)+
      "FADEDOWN_POINT int default -1,"+
      QString().sprintf("FADEDOWN_GAIN int default %d,",RD_FADE_DEPTH)+
      "SEGUE_START_POINT int not null default -1,"+
      "SEGUE_END_POINT int not null default -1,"+
      QString().sprintf("SEGUE_GAIN int default %d,",RD_FADE_DEPTH)+
      "DUCK_UP_GAIN int default 0,"+
      "DUCK_DOWN_GAIN int default 0,"+
      "COMMENT CHAR(255),"+
      "LABEL CHAR(64),"+
      "ORIGIN_USER char(255),"+
      "ORIGIN_DATETIME datetime,"+
      "EVENT_LENGTH int default -1,"+
      "LINK_EVENT_NAME char(64),"+
      "LINK_START_TIME int,"+
      "LINK_LENGTH int default 0,"+
      "LINK_START_SLOP int default 0,"+
      "LINK_END_SLOP int default 0,"+
      "LINK_ID int default -1,"+
      "LINK_EMBEDDED enum('N','Y') default 'N',"+
      "EXT_START_TIME time,"+
      "EXT_LENGTH int,"+
      "EXT_CART_NAME char(32),"+
      "EXT_DATA char(32),"+
      "EXT_EVENT_ID char(32),"+
      "EXT_ANNC_TYPE char(8),"+
      "index COUNT_IDX (COUNT),"+
      "index CART_NUMBER_IDX (CART_NUMBER),"+
      "index LABEL_IDX (LABEL))"+
      db_table_create_postfix;
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }
      sql=QString("select ")+
	"COUNT,"+           // 00
	"EVENT_TYPE,"+      // 01
	"CART_NUMBER,"+     // 02
	"TRANS_TYPE,"+      // 03
	"MARKER_COMMENT "+  // 04
	"from EVENT_LINES where "+
	"EVENT_NAME=\""+RDEscapeString(q->value(0).toString())+"\" && "+
	"TYPE=0";
      q1=new RDSqlQuery(sql,false);
      while(q1->next()) {
	sql=QString("insert into `")+tablename+"` set "+
	  QString().sprintf("ID=%d,",1+q1->value(0).toInt())+
	  QString().sprintf("COUNT=%d,",q1->value(0).toInt())+
	  QString().sprintf("TYPE=%d,",q1->value(1).toInt())+
	  QString().sprintf("CART_NUMBER=%u,",q1->value(2).toUInt())+
	  QString().sprintf("TRANS_TYPE=%d,",q1->value(3).toInt())+
	  "COMMENT=\""+RDEscapeString(q1->value(4).toString())+"\"";
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
      delete q1;

      //
      // PostImport Events
      //
      tablename=q->value(0).toString()+"_POST";
      tablename.replace(" ","_");
      sql=QString("create table if not exists `")+tablename+"` ("+
      "ID int not null primary key,"+
      "COUNT int not null,"+
      "TYPE int default 0,"+
      "SOURCE int not null,"+
      "START_TIME int,"+
      "GRACE_TIME int default 0,"+
      "CART_NUMBER int UNSIGNED not null default 0,"+
      "TIME_TYPE int not null,"+
      "POST_POINT enum('N','Y') default 'N',"+
      "TRANS_TYPE int not null,"+
      "START_POINT int not null default -1,"+
      "END_POINT int not null default -1,"+
      "FADEUP_POINT int default -1,"+
      QString().sprintf("FADEUP_GAIN int default %d,",RD_FADE_DEPTH)+
      "FADEDOWN_POINT int default -1,"+
      QString().sprintf("FADEDOWN_GAIN int default %d,",RD_FADE_DEPTH)+
      "SEGUE_START_POINT int not null default -1,"+
      "SEGUE_END_POINT int not null default -1,"+
      QString().sprintf("SEGUE_GAIN int default %d,",RD_FADE_DEPTH)+
      "DUCK_UP_GAIN int default 0,"+
      "DUCK_DOWN_GAIN int default 0,"+
      "COMMENT CHAR(255),"+
      "LABEL CHAR(64),"+
      "ORIGIN_USER char(255),"+
      "ORIGIN_DATETIME datetime,"+
      "EVENT_LENGTH int default -1,"+
      "LINK_EVENT_NAME char(64),"+
      "LINK_START_TIME int,"+
      "LINK_LENGTH int default 0,"+
      "LINK_START_SLOP int default 0,"+
      "LINK_END_SLOP int default 0,"+
      "LINK_ID int default -1,"+
      "LINK_EMBEDDED enum('N','Y') default 'N',"+
      "EXT_START_TIME time,"+
      "EXT_LENGTH int,"+
      "EXT_CART_NAME char(32),"+
      "EXT_DATA char(32),"+
      "EXT_EVENT_ID char(32),"+
      "EXT_ANNC_TYPE char(8),"+
      "index COUNT_IDX (COUNT),"+
      "index CART_NUMBER_IDX (CART_NUMBER),"+
      "index LABEL_IDX (LABEL))"+
      db_table_create_postfix;
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }
      sql=QString("select ")+
	"COUNT,"+           // 00
	"EVENT_TYPE,"+      // 01
	"CART_NUMBER,"+     // 02
	"TRANS_TYPE,"+      // 03
	"MARKER_COMMENT "+  // 04
	"from EVENT_LINES where "+
	"EVENT_NAME=\""+RDEscapeString(q->value(0).toString())+"\" && "+
	"TYPE=1";
      q1=new RDSqlQuery(sql,false);
      while(q1->next()) {
	sql=QString("insert into `")+tablename+"` set "+
	  QString().sprintf("ID=%d,",1+q1->value(0).toInt())+
	  QString().sprintf("COUNT=%d,",q1->value(0).toInt())+
	  QString().sprintf("TYPE=%d,",q1->value(1).toInt())+
	  QString().sprintf("CART_NUMBER=%u,",q1->value(2).toUInt())+
	  QString().sprintf("TRANS_TYPE=%d,",q1->value(3).toInt())+
	  "COMMENT=\""+RDEscapeString(q1->value(4).toString())+"\"";
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
      delete q1;
    }
    delete q;
    if(!DropTable("EVENT_LINES",err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 289
  //
  if((cur_schema==289)&&(set_schema<cur_schema)) {
    sql=QString("select NAME from SERVICES");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      QString tablename=q->value(0).toString()+"_SRT";
      tablename.replace(" ","_");

      sql=QString("create table if not exists `")+tablename+"` ("+
	"ID int unsigned auto_increment primary key,"+
	"LENGTH int,"+
	"LOG_NAME char(64),"+
	"LOG_ID int,"+
	"CART_NUMBER int unsigned,"+
	"CUT_NUMBER int,"+
	"TITLE char(255),"+
	"ARTIST char(255),"+
	"PUBLISHER char(64),"+
	"COMPOSER char(64),"+
	"USER_DEFINED char(255),"+
	"SONG_ID char(32),"+
	"ALBUM char(255),"+
	"LABEL char(64),"+
	"CONDUCTOR char(64),"+
	"USAGE_CODE int,"+
	"DESCRIPTION char(64),"+
	"OUTCUE char(64),"+
	"ISRC char(12),"+
	"ISCI char(32),"+
	"STATION_NAME char(64),"+
	"EVENT_DATETIME datetime,"+
	"SCHEDULED_TIME time,"+
	"EVENT_TYPE int,"+
	"EVENT_SOURCE int,"+
	"PLAY_SOURCE int,"+
	"START_SOURCE int default 0,"+
	"ONAIR_FLAG enum('N','Y') default 'N',"+
	"EXT_START_TIME time,"+
	"EXT_LENGTH int,"+
	"EXT_CART_NAME char(32),"+
	"EXT_DATA char(32),"+
	"EXT_EVENT_ID char(8),"+
	"EXT_ANNC_TYPE char(8),"+
	"index EVENT_DATETIME_IDX(EVENT_DATETIME))"+
	db_table_create_postfix;
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }

      sql=QString("select ")+
	"LENGTH,"+          // 00
	"LOG_NAME,"+        // 01
	"LOG_ID,"+          // 02
	"CART_NUMBER,"+     // 03
	"CUT_NUMBER,"+      // 04
	"TITLE,"+           // 05
	"ARTIST,"+          // 06
	"PUBLISHER,"+       // 07
	"COMPOSER,"+        // 08
	"USER_DEFINED,"+    // 09
	"SONG_ID,"+         // 10
	"ALBUM,"+           // 11
	"LABEL,"+           // 12
	"CONDUCTOR,"+       // 13
	"USAGE_CODE,"+      // 14
	"DESCRIPTION,"+     // 15
	"OUTCUE,"+          // 16
	"ISRC,"+            // 17
	"ISCI,"+            // 18
	"STATION_NAME,"+    // 19
	"EVENT_DATETIME,"+  // 20
	"SCHEDULED_TIME,"+  // 21
	"EVENT_TYPE,"+      // 22
	"EVENT_SOURCE,"+    // 23
	"PLAY_SOURCE,"+     // 24
	"START_SOURCE,"+    // 25
	"ONAIR_FLAG,"+      // 26
	"EXT_START_TIME,"+  // 27
	"EXT_LENGTH,"+      // 28
	"EXT_CART_NAME,"+   // 29
	"EXT_DATA,"+        // 30
	"EXT_EVENT_ID,"+    // 31
	"EXT_ANNC_TYPE "+   // 32
	"from ELR_LINES where "+
	"SERVICE_NAME=\""+RDEscapeString(q->value(0).toString())+"\"";
      q1=new RDSqlQuery(sql,false);
      while(q1->next()) {
	sql=QString("insert into `")+tablename+"` set "+
	  QString().sprintf("LENGTH=%d,",q1->value(0).toInt())+
	  "LOG_NAME=\""+RDEscapeString(q1->value(1).toString())+"\","+
	  QString().sprintf("LOG_ID=%d,",q1->value(2).toInt())+
	  QString().sprintf("CART_NUMBER=%u,",q1->value(3).toUInt())+
	  QString().sprintf("CUT_NUMBER=%d,",q1->value(4).toInt())+
	  "TITLE=\""+RDEscapeString(q1->value(5).toString())+"\","+
	  "ARTIST=\""+RDEscapeString(q1->value(6).toString())+"\","+
	  "PUBLISHER=\""+RDEscapeString(q1->value(7).toString())+"\","+
	  "COMPOSER=\""+RDEscapeString(q1->value(8).toString())+"\","+
	  "USER_DEFINED=\""+RDEscapeString(q1->value(9).toString())+"\","+
	  "SONG_ID=\""+RDEscapeString(q1->value(10).toString())+"\","+
	  "ALBUM=\""+RDEscapeString(q1->value(11).toString())+"\","+
	  "LABEL=\""+RDEscapeString(q1->value(12).toString())+"\","+
	  "CONDUCTOR=\""+RDEscapeString(q1->value(13).toString())+"\","+
	  "USAGE_CODE=\""+RDEscapeString(q1->value(14).toString())+"\","+
	  "DESCRIPTION=\""+RDEscapeString(q1->value(15).toString())+"\","+
	  "OUTCUE=\""+RDEscapeString(q1->value(16).toString())+"\","+
	  "ISRC=\""+RDEscapeString(q1->value(17).toString())+"\","+
	  "ISCI=\""+RDEscapeString(q1->value(18).toString())+"\","+
	  "STATION_NAME=\""+RDEscapeString(q1->value(19).toString())+"\","+
	  "EVENT_DATETIME=\""+RDEscapeString(q1->value(20).toDateTime().
				     toString("yyyy-MM-dd hh:mm:ss"))+"\","+
	  "SCHEDULED_TIME=\""+RDEscapeString(q1->value(21).toTime().
					     toString("hh:mm:ss"))+"\","+
	  QString().sprintf("EVENT_TYPE=%d,",q1->value(22).toInt())+
	  QString().sprintf("EVENT_SOURCE=%d,",q1->value(23).toInt())+
	  QString().sprintf("PLAY_SOURCE=%d,",q1->value(24).toInt())+
	  QString().sprintf("START_SOURCE=%d,",q1->value(25).toInt())+
	  "ONAIR_FLAG=\""+RDEscapeString(q1->value(26).toString())+"\","+
	  "EXT_START_TIME=\""+RDEscapeString(q1->value(27).toTime().
					     toString("hh:mm:ss"))+"\","+
	  QString().sprintf("EXT_LENGTH=%d,",q1->value(28).toInt())+
	  "EXT_CART_NAME=\""+RDEscapeString(q1->value(29).toString())+"\","+
	  "EXT_DATA=\""+RDEscapeString(q1->value(30).toString())+"\","+
	  "EXT_EVENT_ID=\""+RDEscapeString(q1->value(31).toString())+"\","+
	  "EXT_ANNC_TYPE=\""+RDEscapeString(q1->value(32).toString())+"\"";
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
      delete q1;
    }
    delete q;

    if(!DropTable("ELR_LINES",err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 288
  //
  if((cur_schema==288)&&(set_schema<cur_schema)) {
    sql=QString("select NAME from LOGS");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      QString tablename=q->value(0).toString()+"_LOG";
      tablename.replace(" ","_");
      sql=QString("create table if not exists `")+tablename+"` ("+
      "ID int not null primary key,"+
      "COUNT int not null,"+
      "TYPE int default 0,"+
      "SOURCE int not null,"+
      "START_TIME int,"+
      "GRACE_TIME int default 0,"+
      "CART_NUMBER int UNSIGNED not null default 0,"+
      "TIME_TYPE int not null,"+
      "POST_POINT enum('N','Y') default 'N',"+
      "TRANS_TYPE int not null,"+
      "START_POINT int not null default -1,"+
      "END_POINT int not null default -1,"+
      "FADEUP_POINT int default -1,"+
      QString().sprintf("FADEUP_GAIN int default %d,",RD_FADE_DEPTH)+
      "FADEDOWN_POINT int default -1,"+
      QString().sprintf("FADEDOWN_GAIN int default %d,",RD_FADE_DEPTH)+
      "SEGUE_START_POINT int not null default -1,"+
      "SEGUE_END_POINT int not null default -1,"+
      QString().sprintf("SEGUE_GAIN int default %d,",RD_FADE_DEPTH)+
      "DUCK_UP_GAIN int default 0,"+
      "DUCK_DOWN_GAIN int default 0,"+
      "COMMENT CHAR(255),"+
      "LABEL CHAR(64),"+
      "ORIGIN_USER char(255),"+
      "ORIGIN_DATETIME datetime,"+
      "EVENT_LENGTH int default -1,"+
      "LINK_EVENT_NAME char(64),"+
      "LINK_START_TIME int,"+
      "LINK_LENGTH int default 0,"+
      "LINK_START_SLOP int default 0,"+
      "LINK_END_SLOP int default 0,"+
      "LINK_ID int default -1,"+
      "LINK_EMBEDDED enum('N','Y') default 'N',"+
      "EXT_START_TIME time,"+
      "EXT_LENGTH int,"+
      "EXT_CART_NAME char(32),"+
      "EXT_DATA char(32),"+
      "EXT_EVENT_ID char(32),"+
      "EXT_ANNC_TYPE char(8),"+
      "index COUNT_IDX (COUNT),"+
      "index CART_NUMBER_IDX (CART_NUMBER),"+
      "index LABEL_IDX (LABEL))"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("select ")+
      "LINE_ID,"+            // 00
      "COUNT,"+              // 01
      "TYPE,"+               // 02
      "SOURCE,"+             // 03
      "START_TIME,"+         // 04
      "GRACE_TIME,"+         // 05
      "CART_NUMBER,"+        // 06
      "TIME_TYPE,"+          // 07
      "POST_POINT,"+         // 08
      "TRANS_TYPE,"+         // 09
      "START_POINT,"+        // 10
      "END_POINT,"+          // 11
      "FADEUP_POINT,"+       // 12
      "FADEUP_GAIN,"+        // 13
      "FADEDOWN_POINT,"+     // 14
      "FADEDOWN_GAIN,"+      // 15
      "SEGUE_START_POINT,"+  // 16
      "SEGUE_END_POINT,"+    // 17
      "SEGUE_GAIN,"+         // 18
      "DUCK_UP_GAIN,"+       // 19
      "DUCK_DOWN_GAIN,"+     // 20
      "COMMENT,"+            // 21
      "LABEL,"+              // 22
      "ORIGIN_USER,"+        // 23
      "ORIGIN_DATETIME,"+    // 24
      "EVENT_LENGTH,"+       // 25
      "LINK_EVENT_NAME,"+    // 26
      "LINK_START_TIME,"+    // 27
      "LINK_LENGTH,"+        // 28
      "LINK_START_SLOP,"+    // 29
      "LINK_END_SLOP,"+      // 30
      "LINK_ID,"+            // 31
      "LINK_EMBEDDED,"+      // 32
      "EXT_START_TIME,"+     // 33
      "EXT_LENGTH,"+         // 34
      "EXT_CART_NAME,"+      // 35
      "EXT_DATA,"+           // 36
      "EXT_EVENT_ID,"+       // 37
      "EXT_ANNC_TYPE "+      // 38
      "from LOG_LINES where "+
      "LOG_NAME=\""+RDEscapeString(q->value(0).toString())+"\" "+
      "order by COUNT";
      q1=new RDSqlQuery(sql);
      while(q1->next()) {
	sql=QString("insert into `")+tablename+"` set "+
	  QString().sprintf("ID=%d,",q1->value(0).toInt())+
	  QString().sprintf("COUNT=%d,",q1->value(1).toInt())+
	  QString().sprintf("TYPE=%d,",q1->value(2).toInt())+
	  QString().sprintf("SOURCE=%d,",q1->value(3).toInt())+
	  QString().sprintf("START_TIME=%d,",q1->value(4).toInt())+
	  QString().sprintf("GRACE_TIME=%d,",q1->value(5).toInt())+
	  QString().sprintf("CART_NUMBER=%u,",q1->value(6).toUInt())+
	  QString().sprintf("TIME_TYPE=%d,",q1->value(7).toInt())+
	  "POST_POINT=\""+RDEscapeString(q1->value(8).toString())+"\","+
	  QString().sprintf("TRANS_TYPE=%d,",q1->value(9).toInt())+
	  QString().sprintf("START_POINT=%d,",q1->value(10).toInt())+
	  QString().sprintf("END_POINT=%d,",q1->value(11).toInt())+
	  QString().sprintf("FADEUP_POINT=%d,",q1->value(12).toInt())+
	  QString().sprintf("FADEUP_GAIN=%d,",q1->value(13).toInt())+
	  QString().sprintf("FADEDOWN_POINT=%d,",q1->value(14).toInt())+
	  QString().sprintf("FADEDOWN_GAIN=%d,",q1->value(15).toInt())+
	  QString().sprintf("SEGUE_START_POINT=%d,",q1->value(16).toInt())+
	  QString().sprintf("SEGUE_END_POINT=%d,",q1->value(17).toInt())+
	  QString().sprintf("SEGUE_GAIN=%d,",q1->value(18).toInt())+
	  QString().sprintf("DUCK_UP_GAIN=%d,",q1->value(19).toInt())+
	  QString().sprintf("DUCK_DOWN_GAIN=%d,",q1->value(20).toInt())+
	  "COMMENT=\""+RDEscapeString(q1->value(21).toString())+"\","+
	  "LABEL=\""+RDEscapeString(q1->value(22).toString())+"\","+
	  "ORIGIN_USER=\""+RDEscapeString(q1->value(23).toString())+"\","+
	  "ORIGIN_DATETIME=\""+q1->value(24).toDateTime().
	  toString("yyyy-MM-dd hh:mm:ss")+"\","+
	  QString().sprintf("EVENT_LENGTH=%d,",q1->value(25).toInt())+
	  "LINK_EVENT_NAME=\""+RDEscapeString(q1->value(26).toString())+"\","+
	  QString().sprintf("LINK_START_TIME=%d,",q1->value(27).toInt())+
	  QString().sprintf("LINK_LENGTH=%d,",q1->value(28).toInt())+
	  QString().sprintf("LINK_START_SLOP=%d,",q1->value(29).toInt())+
	  QString().sprintf("LINK_END_SLOP=%d,",q1->value(30).toInt())+
	  QString().sprintf("LINK_ID=%d,",q1->value(31).toInt())+
	  "LINK_EMBEDDED=\""+RDEscapeString(q1->value(32).toString())+"\","+
	  "EXT_START_TIME=\""+q1->value(33).toTime().toString("hh:mm:ss")+"\","+
	  QString().sprintf("EXT_LENGTH=%d,",q1->value(34).toInt())+
	  "EXT_CART_NAME=\""+RDEscapeString(q1->value(35).toString())+"\","+
	  "EXT_DATA=\""+RDEscapeString(q1->value(36).toString())+"\","+
	  "EXT_EVENT_ID=\""+RDEscapeString(q1->value(37).toString())+"\","+
	  "EXT_ANNC_TYPE=\""+RDEscapeString(q1->value(38).toString())+"\"";
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
      delete q1;
    }
    delete q;

    if(!DropTable("LOG_LINES",err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 287
  //
  if((cur_schema==287)&&(set_schema<cur_schema)) {
    sql=QString("alter table STATIONS drop column JACK_PORTS");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 286
  //
  if((cur_schema==286)&&(set_schema<cur_schema)) {
    sql=QString("alter table SERVICES add ")+
      "column TFC_LENGTH_OFFSET int after TFC_LEN_SECONDS_LENGTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table SERVICES add ")+
      "column TFC_LENGTH_LENGTH int after TFC_LENGTH_OFFSET";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table SERVICES add ")+
      "column MUS_LENGTH_OFFSET int after MUS_LEN_SECONDS_LENGTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table SERVICES add ")+
      "column MUS_LENGTH_LENGTH int after MUS_LENGTH_OFFSET";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 285
  //
  if((cur_schema==285)&&(set_schema<cur_schema)) {
    sql=QString("alter table CART add column ISRC char(12) after YEAR");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table DECKS add ")+
      "column DEFAULT_SAMPRATE int default 44100 after DEFAULT_CHANNELS";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table RDLIBRARY add ")+
      "column INPUT_STREAM int default 0 after INPUT_CARD";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table RDLIBRARY add ")+
      "column OUTPUT_STREAM int default 0 after OUTPUT_CARD";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table RDLIBRARY add ")+
      "column RECORD_GPI int default -1 after TRIM_THRESHOLD";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table RDLIBRARY add ")+
      "column PLAY_GPI int default -1 after RECORD_GPI";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table RDLIBRARY add ")+
      "column STOP_GPI int default -1 after PLAY_GPI";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table RDLIBRARY add ")+
      "column DEFAULT_SAMPRATE int default 44100 after DEFAULT_CHANNELS";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table RDLOGEDIT add ")+
      "column SAMPRATE int unsigned default 44100 after FORMAT";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table SERVICES add ")+
      "column TFC_START_OFFSET int after TFC_TITLE_LENGTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table SERVICES add ")+
      "column TFC_START_LENGTH int after TFC_START_OFFSET";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table SERVICES add ")+
      "column MUS_START_OFFSET int after MUS_TITLE_LENGTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table SERVICES add ")+
      "column MUS_START_LENGTH int after MUS_START_OFFSET";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 284
  //
  if((cur_schema==284)&&(set_schema<cur_schema)) {
    /*
    sql=QString("alter table RDAIRPLAY add column INSTANCE int unsigned ")+
      "not null default 0 after STATION";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    */
    for(int i=9;i>=0;i--) {
      sql=QString("alter table RDAIRPLAY add column ")+
	QString().sprintf("STOP_RML%d char(255) after INSTANCE",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDAIRPLAY add column ")+
	QString().sprintf("START_RML%d char(255) after INSTANCE",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDAIRPLAY add column ")+
	QString().sprintf("PORT%d int default 0 after INSTANCE",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDAIRPLAY add column ")+
	QString().sprintf("CARD%d int default 0 after INSTANCE",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }

    sql=QString("alter table RDAIRPLAY add column ")+
      "OP_MODE int default 2 after TRANS_LENGTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table RDAIRPLAY add column ")+
      "START_MODE int default 0 after OP_MODE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table RDPANEL add column INSTANCE int unsigned ")+
      "not null default 0 after STATION";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    for(int i=9;i>=6;i--) {
      sql=QString("alter table RDPANEL add column ")+
	QString().sprintf("STOP_RML%d char(255) after INSTANCE",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDPANEL add column ")+
	QString().sprintf("START_RML%d char(255) after INSTANCE",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDPANEL add column ")+
	QString().sprintf("PORT%d int default 0 after INSTANCE",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDPANEL add column ")+
	QString().sprintf("CARD%d int default 0 after INSTANCE",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }

    for(int i=3;i>=2;i--) {
      sql=QString("alter table RDPANEL add column ")+
	QString().sprintf("STOP_RML%d char(255) after INSTANCE",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDPANEL add column ")+
	QString().sprintf("START_RML%d char(255) after INSTANCE",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDPANEL add column ")+
	QString().sprintf("PORT%d int default 0 after INSTANCE",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDPANEL add column ")+
	QString().sprintf("CARD%d int default 0 after INSTANCE",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }

    sql=QString("alter table MATRICES alter column PORT drop default");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table MATRICES alter column PORT_2 drop default");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table MATRICES alter column INPUTS drop default");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table MATRICES alter column OUTPUTS drop default");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table MATRICES alter column GPIS drop default");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table MATRICES alter column GPOS drop default");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table REPLICATORS alter column TYPE_ID drop default");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 283
  //
  if((cur_schema==283)&&(set_schema<cur_schema)) {
    sql=QString("create table if not exists AUDIO_PORTS (")+
      "ID int unsigned not null primary key AUTO_INCREMENT,"+
      "STATION_NAME char(64) not null,"+
      "CARD_NUMBER int not null,"+
      "CLOCK_SOURCE int default 0,"+
      "INPUT_0_LEVEL int default 0,"+
      "INPUT_0_TYPE int default 0,"+
      "INPUT_0_MODE int default 0,"+
      "INPUT_1_LEVEL int default 0,"+
      "INPUT_1_TYPE int default 0,"+
      "INPUT_1_MODE int default 0,"+
      "INPUT_2_LEVEL int default 0,"+
      "INPUT_2_TYPE int default 0,"+
      "INPUT_2_MODE int default 0,"+
      "INPUT_3_LEVEL int default 0,"+
      "INPUT_3_TYPE int default 0,"+
      "INPUT_3_MODE int default 0,"+
      "INPUT_4_LEVEL int default 0,"+
      "INPUT_4_TYPE int default 0,"+
      "INPUT_4_MODE int default 0,"+
      "INPUT_5_LEVEL int default 0,"+
      "INPUT_5_TYPE int default 0,"+
      "INPUT_5_MODE int default 0,"+
      "INPUT_6_LEVEL int default 0,"+
      "INPUT_6_TYPE int default 0,"+
      "INPUT_6_MODE int default 0,"+
      "INPUT_7_LEVEL int default 0,"+
      "INPUT_7_TYPE int default 0,"+
      "INPUT_7_MODE int default 0,"+
      "OUTPUT_0_LEVEL int default 0,"+
      "OUTPUT_1_LEVEL int default 0,"+
      "OUTPUT_2_LEVEL int default 0,"+
      "OUTPUT_3_LEVEL int default 0,"+
      "OUTPUT_4_LEVEL int default 0,"+
      "OUTPUT_5_LEVEL int default 0,"+
      "OUTPUT_6_LEVEL int default 0,"+
      "OUTPUT_7_LEVEL int default 0,"+
      "index STATION_NAME_IDX (STATION_NAME),"+
      "index CARD_NUMBER_IDX (CARD_NUMBER))"+
      db_table_create_postfix;
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }
    sql=QString("select NAME from STATIONS");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      for(int i=0;i<8;i++) {
	sql=QString("insert into AUDIO_PORTS set ")+
	  "STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	  QString().sprintf("CARD_NUMBER=%d",i);
	if(!RDSqlQuery::apply(sql,err_msg)) {
	  return false;
	}
      }
    }
    delete q;
    sql=QString("select ")+
      "STATION_NAME,"+  // 00
      "CARD_NUMBER,"+   // 01
      "PORT_NUMBER,"+   // 02
      "LEVEL,"+         // 03
      "TYPE,"+          // 04
      "MODE "+          // 05
      "from AUDIO_INPUTS where PORT_NUMBER<8";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update AUDIO_PORTS set ")+
	QString().sprintf("INPUT_%d_LEVEL=%d,",
			  q->value(2).toInt(),q->value(3).toInt())+
	QString().sprintf("INPUT_%d_TYPE=%d,",
			  q->value(2).toInt(),q->value(4).toInt())+
	QString().sprintf("INPUT_%d_MODE=%d  where ",
			  q->value(2).toInt(),q->value(5).toInt())+
	"STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\" && "+
	QString().sprintf("CARD_NUMBER=%d",q->value(1).toInt());
    }
    delete q;
    if(!DropTable("AUDIO_INPUTS",err_msg)) {
      return false;
    }

    sql=QString("select ")+
      "STATION_NAME,"+  // 00
      "CARD_NUMBER,"+   // 01
      "PORT_NUMBER,"+   // 02
      "LEVEL "+         // 03
      "from AUDIO_OUTPUTS where PORT_NUMBER<8";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update AUDIO_PORTS set ")+
	QString().sprintf("OUTPUT_%d_LEVEL=%d where ",
			  q->value(2).toInt(),q->value(3).toInt())+
	"STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\" && "+
	QString().sprintf("CARD_NUMBER=%d",q->value(1).toInt());
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }
    }
    delete q;
    if(!DropTable("AUDIO_OUTPUTS",err_msg)) {
      return false;
    }

    sql=QString("select ")+
      "STATION_NAME,"+  // 00
      "CARD_NUMBER,"+   // 01
      "CLOCK_SOURCE "+  // 02
      "from AUDIO_CARDS where CARD_NUMBER<8";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update AUDIO_PORTS set ")+
	QString().sprintf("CLOCK_SOURCE=%d where ",q->value(2).toInt())+
	"STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\" && "+
	QString().sprintf("CARD_NUMBER=%d",q->value(1).toInt());
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }
    }
    delete q;
    sql=QString("alter table AUDIO_CARDS drop column CLOCK_SOURCE");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 282
  //
  if((cur_schema==282)&&(set_schema<cur_schema)) {
    for(int i=7;i>=0;i--) {
      sql=QString("alter table STATIONS add column ")+
	QString().sprintf("CARD%d_OUTPUTS int default -1 after ALSA_VERSION",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table STATIONS add column ")+
	QString().sprintf("CARD%d_INPUTS int default -1 after ALSA_VERSION",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table STATIONS add column ")+
	QString().sprintf("CARD%d_NAME char(64) after ALSA_VERSION",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table STATIONS add column ")+
	QString().sprintf("CARD%d_DRIVER int default 0 after ALSA_VERSION",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }

    if(!DropTable("AUDIO_CARDS",err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 281
  //
  if((cur_schema==281)&&(set_schema<cur_schema)) {
    sql=QString("alter table RDAIRPLAY drop column VIRTUAL_EXIT_CODE");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 280
  //
  if((cur_schema==280)&&(set_schema<cur_schema)) {
    sql=QString("delete from LOG_MODES where MACHINE>=100");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 279
  //
  if((cur_schema==279)&&(set_schema<cur_schema)) {
    sql=QString("delete from RDAIRPLAY_CHANNELS where INSTANCE>=100");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 278
  //
  if((cur_schema==278)&&(set_schema<cur_schema)) {
    for(int i=2;i>=0;i--) {
      sql=QString("alter table RDAIRPLAY add column ")+
	QString().sprintf("LOG_RML%d char(255) after DESCRIPTION_TEMPLATE",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDAIRPLAY add column ")+
	QString().sprintf("UDP_STRING%d char(255) after DESCRIPTION_TEMPLATE",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDAIRPLAY add column ")+
	QString().sprintf("UDP_PORT%d int unsigned after DESCRIPTION_TEMPLATE",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDAIRPLAY add column ")+
	QString().sprintf("UDP_ADDR%d char(255) after DESCRIPTION_TEMPLATE",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDAIRPLAY add column ")+
	QString().sprintf("LOG%d_START_MODE int default 0 ",i)+
	"after AUDITION_PREROLL";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDAIRPLAY add column ")+
	QString().sprintf("LOG%d_NEXT_CART int unsigned default 0 ",i)+
	"after AUDITION_PREROLL";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDAIRPLAY add column ")+
	QString().sprintf("LOG%d_NOW_CART int unsigned default 0 ",i)+
	"after AUDITION_PREROLL";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDAIRPLAY add column ")+
	QString().sprintf("LOG%d_LOG_LINE int default -1 ",i)+
	"after AUDITION_PREROLL";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDAIRPLAY add column ")+
	QString().sprintf("LOG%d_LOG_ID int default -1 ",i)+
	"after AUDITION_PREROLL";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDAIRPLAY add column ")+
	QString().sprintf("LOG%d_RUNNING enum('N','Y') default 'N' ",i)+
	"after AUDITION_PREROLL";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDAIRPLAY add column ")+
	QString().sprintf("LOG%d_CURRENT_LOG char(64) ",i)+
	"after AUDITION_PREROLL";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDAIRPLAY add column ")+
	QString().sprintf("LOG%d_LOG_NAME char(64) ",i)+
	"after AUDITION_PREROLL";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }

      sql=QString("alter table RDAIRPLAY add column ")+
	QString().sprintf("LOG%d_AUTO_RESTART enum('N','Y') default 'N' ",i)+
	"after AUDITION_PREROLL";
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }

    sql=QString("select ")+
      "STATION_NAME,"+   // 00
      "MACHINE,"+        // 01
      "START_MODE,"+     // 02
      "AUTO_RESTART,"+   // 03
      "LOG_NAME,"+       // 04
      "CURRENT_LOG,"+    // 05
      "RUNNING,"+        // 06
      "LOG_ID,"+         // 07
      "LOG_LINE,"+       // 08
      "NOW_CART,"+       // 09
      "NEXT_CART,"+      // 10
      "UDP_ADDR,"+       // 11
      "UDP_PORT,"+       // 12
      "UDP_STRING,"+     // 13
      "LOG_RML "+        // 14
      "from LOG_MACHINES where "+
      "MACHINE<3";
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update RDAIRPLAY set ")+
	QString().sprintf("UDP_ADDR%d=\"",q->value(1).toInt())+
	RDEscapeString(q->value(11).toString())+"\","+
	QString().sprintf("UDP_PORT%d=%u,",
			  q->value(1).toInt(),q->value(12).toUInt())+
	QString().sprintf("UDP_STRING%d=\"",q->value(1).toInt())+
	RDEscapeString(q->value(13).toString())+"\","+
	QString().sprintf("LOG_RML%d=\"",q->value(1).toInt())+
	RDEscapeString(q->value(14).toString())+"\","+
	QString().sprintf("LOG%d_START_MODE=%d,",
			  q->value(1).toInt(),q->value(2).toInt())+
	QString().sprintf("LOG%d_AUTO_RESTART=\"",q->value(1).toInt())+
	RDEscapeString(q->value(3).toString())+"\","+
	QString().sprintf("LOG%d_LOG_NAME=\"",q->value(1).toInt())+
	RDEscapeString(q->value(4).toString())+"\","+
	QString().sprintf("LOG%d_CURRENT_LOG=\"",q->value(1).toInt())+
	RDEscapeString(q->value(5).toString())+"\","+
	QString().sprintf("LOG%d_RUNNING=\"",q->value(1).toInt())+
	RDEscapeString(q->value(6).toString())+"\","+
	QString().sprintf("LOG%d_LOG_ID=%d,",
			  q->value(1).toInt(),q->value(7).toInt())+
	QString().sprintf("LOG%d_LOG_LINE=%d,",
			  q->value(1).toInt(),q->value(8).toInt())+
	QString().sprintf("LOG%d_NOW_CART=%d,",
			  q->value(1).toInt(),q->value(9).toInt())+
	QString().sprintf("LOG%d_NEXT_CART=%d ",
			  q->value(1).toInt(),q->value(10).toInt())+
	"where STATION=\""+RDEscapeString(q->value(0).toString())+"\"";
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }
    }
    delete q;

    if(!DropTable("LOG_MACHINES")) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 277
  //
  if((cur_schema==277)&&(set_schema<cur_schema)) {
    sql=QString("alter table USERS drop column WEBGET_LOGIN_PRIV");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 276
  //
  if((cur_schema==276)&&(set_schema<cur_schema)) {
    sql=QString("alter table SYSTEM drop column NOTIFICATION_ADDRESS");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 275
  //
  if((cur_schema==275)&&(set_schema<cur_schema)) {
    sql=QString("alter table SERVICES drop column LOG_SHELFLIFE_ORIGIN");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 274
  //
  if((cur_schema==274)&&(set_schema<cur_schema)) {
    sql=QString("alter table LOGS drop index LOCK_GUID_IDX");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table LOGS drop column LOCK_GUID");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 273
  //
  if((cur_schema==273)&&(set_schema<cur_schema)) {
    sql=QString("alter table LOGS drop column LOCK_DATETIME");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table LOGS drop column LOCK_IPV4_ADDRESS");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table LOGS drop column LOCK_STATION_NAME");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table LOGS drop column LOCK_USER_NAME");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 272
  //
  if((cur_schema==272)&&(set_schema<cur_schema)) {
    if(!DropTable("USER_SERVICE_PERMS",err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 271
  //
  if((cur_schema==271)&&(set_schema<cur_schema)) {
    sql=QString("alter table DROPBOXES drop column SEGUE_LEVEL, ")+
      "drop column SEGUE_LENGTH";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 270
  //
  if((cur_schema==270)&&(set_schema<cur_schema)) {
    sql="alter table STATIONS drop column SHORT_NAME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }  

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 269
  //
  if((cur_schema==269)&&(set_schema<cur_schema)) {
    sql="alter table GROUPS drop column DEFAULT_CUT_LIFE";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }  

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 268
  //
  if((cur_schema==268)&&(set_schema<cur_schema)) {
    sql="alter table DROPBOXES drop column FORCE_TO_MONO";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }  

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 267
  //
  if((cur_schema==267)&&(set_schema<cur_schema)) {
    sql="alter table CUTS drop column ORIGIN_LOGIN_NAME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }  

    sql="alter table CUTS drop column SOURCE_HOSTNAME";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }  

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 266
  //
  if((cur_schema==266)&&(set_schema<cur_schema)) {
    // Nothing to do here as this is a pseudo-schema change.

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 265
  //
  if((cur_schema==265)&&(set_schema<cur_schema)) {
    sql="alter table SYSTEM drop column SHOW_USER_LIST";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }  

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 264
  //
  if((cur_schema==264)&&(set_schema<cur_schema)) {
    sql="alter table SYSTEM drop column FIX_DUP_CART_TITLES";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }  

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 263
  //
  if((cur_schema==263)&&(set_schema<cur_schema)) {
    // Nothing to do here as this is a pseudo-schema change.

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 262
  //
  if((cur_schema==262)&&(set_schema<cur_schema)) {
    sql=QString("alter table USERS drop column LOCAL_AUTH");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table USERS drop column PAM_SERVICE");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("drop index IPV4_ADDRESS_IDX on STATIONS");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }  

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 261
  //
  if((cur_schema==261)&&(set_schema<cur_schema)) {
    sql=QString("alter table CUTS drop column SHA1_HASH");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 260
  //
  if((cur_schema==260)&&(set_schema<cur_schema)) {
    sql=QString("alter table USERS drop column WEBAPI_AUTH_TIMEOUT");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    if(!DropTable("WEBAPI_AUTHS",err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 259
  //
  if((cur_schema==259)&&(set_schema<cur_schema)) {
    for(int i=0;i<168;i++) {
      sql=QString().sprintf("alter table SERVICES add column CLOCK%d char(64)",i);
      if(!RDSqlQuery::apply(sql,err_msg)) {
        return false;
      }
    }

    sql=QString("select SERVICE_NAME,HOUR,CLOCK_NAME from SERVICE_CLOCKS");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      sql=QString("update SERVICES set ")+
	QString().sprintf("CLOCK%d=",q->value(1).toInt());
      if(q->value(2).isNull()) {
	sql+="null ";
      }
      else {
	sql+="\""+RDEscapeString(q->value(2).toString())+"\" ";
      }
      sql+="where NAME=\""+RDEscapeString(q->value(0).toString())+"\"";
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }
    }
    delete q;

    if(!DropTable("SERVICE_CLOCKS",err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 258
  //
  if((cur_schema==258)&&(set_schema<cur_schema)) {
    sql=QString("select NAME from LOGS");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table ")+
	"`"+tablename+"_LOG` "+
	"modify column CART_NUMBER int unsigned not null";
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }
    }
    delete q;

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 257
  //
  if((cur_schema==257)&&(set_schema<cur_schema)) {
    sql=QString("alter table LOGS modify column LINK_DATETIME datetime not null");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table LOGS modify column START_DATE date not null");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table LOGS modify column END_DATE date not null");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 256
  //
  if((cur_schema==256)&&(set_schema<cur_schema)) {
    if(!DropTable("CUT_EVENTS",err_msg)) {
      return false;
    }

    if(!DropTable("DECK_EVENTS",err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 255
  //
  if((cur_schema==255)&&(set_schema<cur_schema)) {
    sql=QString("select NAME from SERVICES");
    q=new RDSqlQuery(sql,false);
    while(q->next()) {
      tablename=q->value(0).toString();
      tablename.replace(" ","_");
      sql=QString("alter table `")+tablename+"_SRT` drop column DESCRIPTION";
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }

      sql=QString("alter table `")+tablename+"_SRT` drop column OUTCUE";
      if(!RDSqlQuery::apply(sql,err_msg)) {
	return false;
      }
    }
    delete q;

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 254
  //
  if((cur_schema==254)&&(set_schema<cur_schema)) {
    sql=QString("alter table CUTS drop column PLAY_ORDER");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    sql=QString("alter table CART drop column USE_WEIGHTING");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 253
  //
  if((cur_schema==253)&&(set_schema<cur_schema)) {
    // Unimplemented reversion

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 252
  //
  if((cur_schema==252)&&(set_schema<cur_schema)) {
    // Unimplemented reversion

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 251
  //
  if((cur_schema==251)&&(set_schema<cur_schema)) {
    // Unimplemented reversion

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 250
  //
  if((cur_schema==250)&&(set_schema<cur_schema)) {
    // Unimplemented reversion

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 249
  //
  if((cur_schema==249)&&(set_schema<cur_schema)) {
    // Unimplemented reversion

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 248
  //
  if((cur_schema==248)&&(set_schema<cur_schema)) {
    // Unimplemented reversion

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 247
  //
  if((cur_schema==247)&&(set_schema<cur_schema)) {
    // Unimplemented reversion

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 246
  //
  if((cur_schema==246)&&(set_schema<cur_schema)) {
    // Unimplemented reversion

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 245
  //
  if((cur_schema==245)&&(set_schema<cur_schema)) {
    sql=QString("alter table RDLIBRARY drop column READ_ISRC");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 244
  //
  if((cur_schema==244)&&(set_schema<cur_schema)) {
    sql=QString("alter table JACK_CLIENTS modify column COMMAND_LINE ")+
      "char(255) not null";
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  //
  // Revert 243
  //
  if((cur_schema==243)&&(set_schema<cur_schema)) {
    sql=QString("alter table STATIONS drop column HAVE_MP4_DECODE");
    if(!RDSqlQuery::apply(sql,err_msg)) {
      return false;
    }

    WriteSchemaVersion(--cur_schema);
  }

  *err_msg="ok";
  return true;
}
