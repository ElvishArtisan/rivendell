// rdrevert.cpp
//
// Revert the Rivendell database schema to an earlier version.
//
//   (C) Copyright 2016-2017 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include <map>

#include <qapplication.h>
#include <qstringlist.h>

#include <rdclock.h>
#include <rdcmd_switch.h>
#include <rdcreate_log.h>
#include <rdescape_string.h>
#include <rdevent.h>
#include <rdlog.h>
#include <rdsvc.h>

#include "rdrevert.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  bool ok=false;
  int set_schema=0;
  rev_use_deadzone=false;

  //
  // Check for Root Permissions
  //
  if(geteuid()!=0) {
    fprintf(stderr,"rdrevert: this program requires root permissions\n");
    exit(256);
  }

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"rdcollect",RDREVERT_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--set-schema") {
      set_schema=cmd->value(i).toInt(&ok);
      if((!ok)||(set_schema<0)) {
	fprintf(stderr,"rdrevert: invalid schema value\n");
	exit(256);
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--set-version") {
      if((set_schema=MapSchema(cmd->value(i)))==0) {
	fprintf(stderr,"rdrevert: invalid/unsupported Rivendell version\n");
	exit(256);
      }
      cmd->setProcessed(i,true);
    }
  }
  if(!cmd->allProcessed()) {
    fprintf(stderr,"rdrevert: unknown option\n");
    exit(256);
  }

  //
  // Load Local Configs
  //
  rev_config=new RDConfig();
  rev_config->load();
  rev_config->setModuleName("rdrevert");

  //
  // Open Database
  //
  rev_db=QSqlDatabase::addDatabase(rev_config->mysqlDriver());
  if(!rev_db) {
    fprintf(stderr,"rdrevert: unable to connect to mysql server\n");
    exit(256);
  }
  rev_db->setDatabaseName(rev_config->mysqlDbname());
  rev_db->setUserName(rev_config->mysqlUsername());
  rev_db->setPassword(rev_config->mysqlPassword());
  rev_db->setHostName(rev_config->mysqlHostname());
  if(!rev_db->open()) {
    fprintf(stderr,"rdrevert: unable to authenticate with mysql server\n");
    rev_db->removeDatabase(rev_config->mysqlDbname());
    exit(256);
  }

  int start_schema=GetVersion();
  if(set_schema==0) {
    printf("%d\n",start_schema);
    exit(0);
  }
  if((set_schema<RDREVERT_BASE_SCHEMA)||(set_schema>start_schema)) {
    fprintf(stderr,"rdrevert: unsupported schema\n");
    exit(256);
  }
  if((start_schema>=246)&&(start_schema<=253)) {
    rev_use_deadzone=true;
  }

  while(start_schema>set_schema) {
    Revert(start_schema--);
  }
  exit(0);
}


void MainObject::Revert(int schema) const
{  
  switch(schema) {
  case 243:
    Revert243();
    break;

  case 244:
    Revert244();
    break;

  case 245:
    Revert245();
    break;

  case 246:
    Revert246();
    break;

  case 247:
    Revert247();
    break;

  case 248:
    Revert248();
    break;

  case 249:
    Revert249();
    break;

  case 250:
    Revert250();
    break;

  case 251:
    Revert251();
    break;

  case 252:
    Revert252();
    break;

  case 253:
    Revert253();
    break;

  case 254:
    Revert254();
    break;

  case 255:
    Revert255();
    break;

  case 256:
    Revert256();
    break;

  case 257:
    Revert257();
    break;

  case 258:
    Revert258();
    break;

  case 259:
    Revert259();
    break;

  case 260:
    Revert260();
    break;

  case 261:
    Revert261();
    break;

  case 262:
    Revert262();
    break;

  case 263:
    Revert263();
    break;

  case 264:
    Revert264();
    break;

  case 265:
    Revert265();
    break;

  case 266:
    Revert266();
    break;

  case 267:
    Revert267();
    break;

  case 268:
    Revert268();
    break;

  case 269:
    Revert269();
    break;

  case 270:
    Revert270();
    break;

  case 271:
    Revert271();
    break;

  case 272:
    Revert272();
    break;

  case 273:
    Revert273();
    break;

  case 274:
    Revert274();
    break;

  case 275:
    Revert275();
    break;

  case 276:
    Revert276();
    break;

  case 277:
    Revert277();
    break;

  case 278:
    Revert278();
    break;

  case 279:
    Revert279();
    break;

  case 280:
    Revert280();
    break;

  case 281:
    Revert281();
    break;

  case 282:
    Revert282();
    break;

  case 283:
    Revert283();
    break;

  case 284:
    Revert284();
    break;

  case 285:
    Revert285();
    break;
  }
}


void MainObject::Revert243() const
{
  QString sql;
  QSqlQuery *q;

  sql=QString("alter table STATIONS drop column HAVE_MP4_DECODE");
  q=new QSqlQuery(sql);
  delete q;

  SetVersion(242);
}


void MainObject::Revert244() const
{
  QString sql;
  QSqlQuery *q;

  sql=QString("alter table JACK_CLIENTS modify column COMMAND_LINE ")+
    "char(255) not null";
  q=new QSqlQuery(sql);
  delete q;

  SetVersion(243);
}


void MainObject::Revert245() const
{
  QString sql;
  QSqlQuery *q;

  sql=QString("alter table RDLIBRARY drop column READ_ISRC");
  q=new QSqlQuery(sql);
  delete q;

  SetVersion(244);
}


void MainObject::Revert246() const
{
  if(rev_use_deadzone) {
  }
  SetVersion(245);
}


void MainObject::Revert247() const
{
  if(rev_use_deadzone) {
  }
  SetVersion(246);
}


void MainObject::Revert248() const
{
  if(rev_use_deadzone) {
  }
  SetVersion(247);
}


void MainObject::Revert249() const
{
  if(rev_use_deadzone) {
  }
  SetVersion(248);
}


void MainObject::Revert250() const
{
  if(rev_use_deadzone) {
  }
  SetVersion(249);
}


void MainObject::Revert251() const
{
  QString sql;
  QSqlQuery *q;
  QSqlQuery *q1;
  QSqlQuery *q2;

  if(rev_use_deadzone) {
    sql=QString("select NAME from CLOCKS");
    q=new QSqlQuery(sql);
    while(q->next()) {
      sql=RDCreateClockTableSql(RDClock::tableName(q->value(0).toString()),
				rev_config);
      q1=new QSqlQuery(sql);
      delete q1;
      sql=QString("select EVENT_NAME,START_TIME,LENGTH from CLOCK_METADATA ")+
	"where CLOCK_NAME=\""+RDEscapeString(q->value(0).toString())+"\" "+
	"order by START_TIME";
      q1=new QSqlQuery(sql);
      while(q1->next()) {
	sql=QString("insert into `")+
	  RDClock::tableName(q->value(0).toString())+"` set "+
	  "EVENT_NAME=\""+RDEscapeString(q1->value(0).toString())+"\","+
	  QString().sprintf("START_TIME=%d,",q1->value(1).toInt())+
	  QString().sprintf("LENGTH=%d",q1->value(2).toInt());
	q2=new QSqlQuery(sql);
	delete q2;
      }
      delete q1;
    }
    delete q;
  }
  SetVersion(250);
}


void MainObject::Revert252() const
{
  QString sql;
  QSqlQuery *q;
  QSqlQuery *q1;
  QSqlQuery *q2;

  if(rev_use_deadzone) {
    sql=QString("select NAME from EVENTS");
    q=new QSqlQuery(sql);
    while(q->next()) {
      RDCreateLogTable(RDEvent::preimportTableName(q->value(0).toString()),
		       rev_config);
      sql=QString("select COUNT,TYPE,TRANS_TYPE,CART_NUMBER,TEXT ")+
	"from EVENT_METADATA where "+
	"(EVENT_NAME=\""+RDEscapeString(q->value(0).toString())+"\")&&"+
	"(PLACE=0)";
      q1=new QSqlQuery(sql);
      while(q1->next()) {
	sql=QString("insert into `")+
	  RDEvent::preimportTableName(q->value(0).toString())+"` set "+
	  QString().sprintf("COUNT=%d,",q1->value(0).toInt())+
	  QString().sprintf("TYPE=%d,",q1->value(1).toInt())+
	  QString().sprintf("TRANS_TYPE=%d,",q1->value(2).toInt())+
	  QString().sprintf("CART_NUMBER=%u,",q1->value(3).toUInt())+
	  "COMMENT=\""+RDEscapeString(q1->value(4).toString())+"\"";
	q2=new QSqlQuery(sql);
	delete q2;
      }
      delete q1;

      RDCreateLogTable(RDEvent::postimportTableName(q->value(0).toString()),
		       rev_config);
      sql=QString("select COUNT,TYPE,TRANS_TYPE,CART_NUMBER,TEXT ")+
	"from EVENT_METADATA where "+
	"(EVENT_NAME=\""+RDEscapeString(q->value(0).toString())+"\")&&"+
	"(PLACE=1)";
      q1=new QSqlQuery(sql);
      while(q1->next()) {
	sql=QString("insert into `")+
	  RDEvent::postimportTableName(q->value(0).toString())+"` set "+
	  QString().sprintf("COUNT=%d,",q1->value(0).toInt())+
	  QString().sprintf("TYPE=%d,",q1->value(1).toInt())+
	  QString().sprintf("TRANS_TYPE=%d,",q1->value(2).toInt())+
	  QString().sprintf("CART_NUMBER=%u,",q1->value(3).toUInt())+
	  "COMMENT=\""+RDEscapeString(q1->value(4).toString())+"\"";
	q2=new QSqlQuery(sql);
	delete q2;
      }
      delete q1;
    }
    delete q;
  }
  sql=QString("drop table EVENT_METADATA");
  q=new QSqlQuery(sql);
  delete q;
  SetVersion(251);
}


void MainObject::Revert253() const
{
  QString sql;
  QSqlQuery *q;

  if(rev_use_deadzone) {
    sql=QString("alter table CART add column PLAY_ORDER int unsigned ")+
      "after LAST_CUT_PLAYED";
    q=new QSqlQuery(sql);
    delete q;

    sql=QString("alter table CART drop column USE_DAYPARTING");
    q=new QSqlQuery(sql);
    delete q;
  }

  SetVersion(252);
}


void MainObject::Revert254() const
{
  QString sql;
  QSqlQuery *q;

  sql=QString("alter table CUTS drop column PLAY_ORDER");
  q=new QSqlQuery(sql);
  delete q;

  sql=QString("alter table CART drop column USE_WEIGHTING");
  q=new QSqlQuery(sql);
  delete q;

  SetVersion(253);
}


void MainObject::Revert255() const
{
  QString sql;
  QSqlQuery *q;
  QSqlQuery *q1;

  sql=QString("select NAME from SERVICES");
  q=new QSqlQuery(sql);
  while(q->next()) {
    sql=QString("alter table `")+RDSvc::svcTableName(q->value(0).toString())+
      "` drop column DESCRIPTION";
    q1=new QSqlQuery(sql);
    delete q1;

    sql=QString("alter table `")+RDSvc::svcTableName(q->value(0).toString())+
      "` drop column OUTCUE";
    q1=new QSqlQuery(sql);
    delete q1;
  }
  delete q;

  SetVersion(254);
}


void MainObject::Revert256() const
{
  QString sql;
  QSqlQuery *q;

  sql=QString("drop table CUT_EVENTS");
  q=new QSqlQuery(sql);
  delete q;

  sql=QString("drop table DECK_EVENTS");
  q=new QSqlQuery(sql);
  delete q;

  SetVersion(255);
}


void MainObject::Revert257() const
{
  QString sql;
  QSqlQuery *q;

  sql=QString("alter table LOGS modify column LINK_DATETIME datetime not null");
  q=new QSqlQuery(sql);
  delete q;

  sql=QString("alter table LOGS modify column START_DATE date not null");
  q=new QSqlQuery(sql);
  delete q;

  sql=QString("alter table LOGS modify column END_DATE date not null");
  q=new QSqlQuery(sql);
  delete q;

  SetVersion(256);
}


void MainObject::Revert258() const
{
  QString sql;
  QSqlQuery *q;
  QSqlQuery *q1;

  sql=QString("select NAME from LOGS");
  q=new QSqlQuery(sql);
  while(q->next()) {
    sql=QString("alter table ")+
      "`"+RDLog::tableName(q->value(0).toString())+"` "+
      "modify column CART_NUMBER int unsigned not null";
    q1=new QSqlQuery(sql);
    delete q1;
  }
  delete q;

  SetVersion(257);
}


void MainObject::Revert259() const
{
  QString sql;
  QSqlQuery *q;
  QSqlQuery *q1;

  for(int i=0;i<168;i++) {
    sql=QString().sprintf("alter table SERVICES add column CLOCK%d char(64)",i);
    q=new QSqlQuery(sql);
    delete q;
  }

  sql=QString("select SERVICE_NAME,HOUR,CLOCK_NAME from SERVICE_CLOCKS");
  q=new QSqlQuery(sql);
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
    q1=new QSqlQuery(sql);
    delete q1;
  }
  delete q;

  sql=QString("drop table SERVICE_CLOCKS");
  q=new QSqlQuery(sql);
  delete q;

  SetVersion(258);
}


void MainObject::Revert260() const
{
  QString sql;
  QSqlQuery *q;

  sql=QString("alter table USERS drop column WEBAPI_AUTH_TIMEOUT");
  q=new QSqlQuery(sql);
  delete q;

  sql=QString("drop table WEBAPI_AUTHS");
  q=new QSqlQuery(sql);
  delete q;

  SetVersion(259);
}


void MainObject::Revert261() const
{
  QString sql;
  QSqlQuery *q;

  sql=QString("alter table CUTS drop column SHA1_HASH");
  q=new QSqlQuery(sql);
  delete q;

  SetVersion(260);
}


void MainObject::Revert262() const
{
  QString sql;
  QSqlQuery *q;

  sql=QString("alter table USERS drop column LOCAL_AUTH");
  q=new QSqlQuery(sql);
  delete q;

  sql=QString("alter table USERS drop column PAM_SERVICE");
  q=new QSqlQuery(sql);
  delete q;

  sql=QString("drop index IPV4_ADDRESS_IDX on STATIONS");
  q=new QSqlQuery(sql);
  delete q;  

  SetVersion(261);
}


void MainObject::Revert263() const
{
  // Nothing to do here as this is a pseudo-schema change.

  SetVersion(262);
}


void MainObject::Revert264() const
{
  QString sql;
  QSqlQuery *q;

  sql="alter table SYSTEM drop column FIX_DUP_CART_TITLES";
  q=new QSqlQuery(sql);
  delete q;  

  SetVersion(263);
}


void MainObject::Revert265() const
{
  QString sql;
  QSqlQuery *q;

  sql="alter table SYSTEM drop column SHOW_USER_LIST";
  q=new QSqlQuery(sql);
  delete q;  

  SetVersion(264);
}


void MainObject::Revert266() const
{
  // Nothing to do here as this is a pseudo-schema change.

  SetVersion(265);
}


void MainObject::Revert267() const
{
  QString sql;
  QSqlQuery *q;

  sql="alter table CUTS drop column ORIGIN_LOGIN_NAME";
  q=new QSqlQuery(sql);
  delete q;  

  sql="alter table CUTS drop column SOURCE_HOSTNAME";
  q=new QSqlQuery(sql);
  delete q;  

  SetVersion(266);
}


void MainObject::Revert268() const
{
  QString sql;
  QSqlQuery *q;

  sql="alter table DROPBOXES drop column FORCE_TO_MONO";
  q=new QSqlQuery(sql);
  delete q;  

  SetVersion(267);
}


void MainObject::Revert269() const
{
  QString sql;
  QSqlQuery *q;

  sql="alter table GROUPS drop column DEFAULT_CUT_LIFE";
  q=new QSqlQuery(sql);
  delete q;  

  SetVersion(268);
}

void MainObject::Revert270() const
{
  QString sql;
  QSqlQuery *q;

  sql="alter table STATIONS drop column SHORT_NAME";
  q=new QSqlQuery(sql);
  delete q;  

  SetVersion(269);
}


void MainObject::Revert271() const
{
  QString sql;
  QSqlQuery *q;
  sql=QString("alter table DROPBOXES drop column SEGUE_LEVEL, ")+
    "drop column SEGUE_LENGTH";
  q=new QSqlQuery(sql);
  delete q;

  SetVersion(270);
}


void MainObject::Revert272() const
{
  QString sql;
  QSqlQuery *q;
  sql=QString("drop table USER_SERVICE_PERMS");
  q=new QSqlQuery(sql);
  delete q;

  SetVersion(271);
}


void MainObject::Revert273() const
{
  QString sql;
  QSqlQuery *q;

  sql=QString("alter table LOGS drop column LOCK_DATETIME");
  q=new QSqlQuery(sql);
  delete q;

  sql=QString("alter table LOGS drop column LOCK_IPV4_ADDRESS");
  q=new QSqlQuery(sql);
  delete q;

  sql=QString("alter table LOGS drop column LOCK_STATION_NAME");
  q=new QSqlQuery(sql);
  delete q;

  sql=QString("alter table LOGS drop column LOCK_USER_NAME");
  q=new QSqlQuery(sql);
  delete q;

  SetVersion(272);
}


void MainObject::Revert274() const
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("alter table LOGS drop index LOCK_GUID_IDX");
  q=new RDSqlQuery(sql,false);
  delete q;

  sql=QString("alter table LOGS drop column LOCK_GUID");
  q=new RDSqlQuery(sql,false);
  delete q;

  SetVersion(273);
}


void MainObject::Revert275() const
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("alter table SERVICES drop column LOG_SHELFLIFE_ORIGIN");
  q=new RDSqlQuery(sql,false);
  delete q;

  SetVersion(274);
}


void MainObject::Revert276() const
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("alter table SYSTEM drop column NOTIFICATION_ADDRESS");
  q=new RDSqlQuery(sql,false);
  delete q;

  SetVersion(275);
}


void MainObject::Revert277() const
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("alter table USERS drop column WEBGET_LOGIN_PRIV");
  q=new RDSqlQuery(sql,false);
  delete q;

  SetVersion(276);
}


void MainObject::Revert278() const
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;

  for(int i=2;i>=0;i--) {
    sql=QString("alter table RDAIRPLAY add column ")+
      QString().sprintf("LOG_RML%d char(255) after DESCRIPTION_TEMPLATE",i);
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table RDAIRPLAY add column ")+
      QString().sprintf("UDP_STRING%d char(255) after DESCRIPTION_TEMPLATE",i);
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table RDAIRPLAY add column ")+
      QString().sprintf("UDP_PORT%d int unsigned after DESCRIPTION_TEMPLATE",i);
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table RDAIRPLAY add column ")+
      QString().sprintf("UDP_ADDR%d char(255) after DESCRIPTION_TEMPLATE",i);
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table RDAIRPLAY add column ")+
      QString().sprintf("LOG%d_START_MODE int default 0 ",i)+
      "after AUDITION_PREROLL";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table RDAIRPLAY add column ")+
      QString().sprintf("LOG%d_NEXT_CART int unsigned default 0 ",i)+
      "after AUDITION_PREROLL";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table RDAIRPLAY add column ")+
      QString().sprintf("LOG%d_NOW_CART int unsigned default 0 ",i)+
      "after AUDITION_PREROLL";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table RDAIRPLAY add column ")+
      QString().sprintf("LOG%d_LOG_LINE int default -1 ",i)+
      "after AUDITION_PREROLL";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table RDAIRPLAY add column ")+
      QString().sprintf("LOG%d_LOG_ID int default -1 ",i)+
      "after AUDITION_PREROLL";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table RDAIRPLAY add column ")+
      QString().sprintf("LOG%d_RUNNING enum('N','Y') default 'N' ",i)+
      "after AUDITION_PREROLL";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table RDAIRPLAY add column ")+
      QString().sprintf("LOG%d_CURRENT_LOG char(64) ",i)+
      "after AUDITION_PREROLL";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table RDAIRPLAY add column ")+
      QString().sprintf("LOG%d_LOG_NAME char(64) ",i)+
      "after AUDITION_PREROLL";
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table RDAIRPLAY add column ")+
      QString().sprintf("LOG%d_AUTO_RESTART enum('N','Y') default 'N' ",i)+
      "after AUDITION_PREROLL";
    q=new RDSqlQuery(sql,false);
    delete q;
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
    "from LOG_MACHINES";
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
    q1=new RDSqlQuery(sql,false);
    delete q1;
  }
  delete q;

  sql=QString("drop table LOG_MACHINES");
  q=new RDSqlQuery(sql,false);
  delete q;

  SetVersion(277);
}


void MainObject::Revert279() const
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("delete from RDAIRPLAY_CHANNELS where INSTANCE>=100");
  q=new RDSqlQuery(sql,false);
  delete q;

  SetVersion(278);
}


void MainObject::Revert280() const
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("delete from LOG_MODES where MACHINE>=100");
  q=new RDSqlQuery(sql,false);
  delete q;

  SetVersion(279);
}


void MainObject::Revert281() const
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("alter table RDAIRPLAY drop column VIRTUAL_EXIT_CODE");
  q=new RDSqlQuery(sql,false);
  delete q;

  SetVersion(280);
}


void MainObject::Revert282() const
{
  QString sql;
  RDSqlQuery *q;

  for(int i=7;i>=0;i--) {
    sql=QString("alter table STATIONS add column ")+
      QString().sprintf("CARD%d_OUTPUTS int default -1 after ALSA_VERSION",i);
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table STATIONS add column ")+
      QString().sprintf("CARD%d_INPUTS int default -1 after ALSA_VERSION",i);
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table STATIONS add column ")+
      QString().sprintf("CARD%d_NAME char(64) after ALSA_VERSION",i);
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table STATIONS add column ")+
      QString().sprintf("CARD%d_DRIVER int default 0 after ALSA_VERSION",i);
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  sql=QString("drop table AUDIO_CARDS");
  q=new RDSqlQuery(sql,false);
  delete q;

  SetVersion(281);
}


void MainObject::Revert283() const
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;

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
    rev_config->createTablePostfix();
  q=new RDSqlQuery(sql,false);
  delete q;
  sql=QString("select NAME from STATIONS");
  q=new RDSqlQuery(sql,false);
  while(q->next()) {
    for(int i=0;i<8;i++) {
      sql=QString("insert into AUDIO_PORTS set ")+
	"STATION_NAME=\""+RDEscapeString(q->value(0).toString())+"\","+
	QString().sprintf("CARD_NUMBER=%d",i);
      q1=new RDSqlQuery(sql,false);
      delete q1;
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
  sql=QString("drop table AUDIO_INPUTS");
  q=new RDSqlQuery(sql,false);
  delete q;

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
    q1=new RDSqlQuery(sql,false);
    delete q1;
  }
  delete q;
  sql=QString("drop table AUDIO_OUTPUTS");
  q=new RDSqlQuery(sql,false);
  delete q;

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
    q1=new RDSqlQuery(sql,false);
    delete q1;
  }
  delete q;
  sql=QString("alter table AUDIO_CARDS drop column CLOCK_SOURCE");
  q=new RDSqlQuery(sql,false);
  delete q;

  SetVersion(282);
}


void MainObject::Revert284() const
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("alter table RDAIRPLAY add column INSTANCE int unsigned ")+
    "not null default 0 after STATION";
  q=new RDSqlQuery(sql,false);
  delete q;

  for(int i=9;i>=0;i--) {
    sql=QString("alter table RDAIRPLAY add column ")+
      QString().sprintf("STOP_RML%d char(255) after INSTANCE",i);
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table RDAIRPLAY add column ")+
      QString().sprintf("START_RML%d char(255) after INSTANCE",i);
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table RDAIRPLAY add column ")+
      QString().sprintf("PORT%d int default 0 after INSTANCE",i);
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table RDAIRPLAY add column ")+
      QString().sprintf("CARD%d int default 0 after INSTANCE",i);
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  sql=QString("alter table RDAIRPLAY add column ")+
    "OP_MODE int default 2 after TRANS_LENGTH";
  q=new RDSqlQuery(sql,false);
  delete q;

  sql=QString("alter table RDAIRPLAY add column ")+
    "START_MODE int default 0 after OP_MODE";
  q=new RDSqlQuery(sql,false);
  delete q;

  sql=QString("alter table RDPANEL add column INSTANCE int unsigned ")+
    "not null default 0 after STATION";
  q=new RDSqlQuery(sql,false);
  delete q;

  for(int i=9;i>=6;i--) {
    sql=QString("alter table RDPANEL add column ")+
      QString().sprintf("STOP_RML%d char(255) after INSTANCE",i);
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table RDPANEL add column ")+
      QString().sprintf("START_RML%d char(255) after INSTANCE",i);
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table RDPANEL add column ")+
      QString().sprintf("PORT%d int default 0 after INSTANCE",i);
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table RDPANEL add column ")+
      QString().sprintf("CARD%d int default 0 after INSTANCE",i);
    q=new RDSqlQuery(sql,false);
    delete q;
  }
  for(int i=3;i>=2;i--) {
    sql=QString("alter table RDPANEL add column ")+
      QString().sprintf("STOP_RML%d char(255) after INSTANCE",i);
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table RDPANEL add column ")+
      QString().sprintf("START_RML%d char(255) after INSTANCE",i);
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table RDPANEL add column ")+
      QString().sprintf("PORT%d int default 0 after INSTANCE",i);
    q=new RDSqlQuery(sql,false);
    delete q;

    sql=QString("alter table RDPANEL add column ")+
      QString().sprintf("CARD%d int default 0 after INSTANCE",i);
    q=new RDSqlQuery(sql,false);
    delete q;
  }

  sql=QString("alter table MATRICES alter column PORT drop default");
  q=new RDSqlQuery(sql,false);
  delete q;

  sql=QString("alter table MATRICES alter column PORT_2 drop default");
  q=new RDSqlQuery(sql,false);
  delete q;

  sql=QString("alter table MATRICES alter column INPUTS drop default");
  q=new RDSqlQuery(sql,false);
  delete q;

  sql=QString("alter table MATRICES alter column OUTPUTS drop default");
  q=new RDSqlQuery(sql,false);
  delete q;

  sql=QString("alter table MATRICES alter column GPIS drop default");
  q=new RDSqlQuery(sql,false);
  delete q;

  sql=QString("alter table MATRICES alter column GPOS drop default");
  q=new RDSqlQuery(sql,false);
  delete q;

  sql=QString("alter table REPLICATORS alter column TYPE_ID drop default");
  q=new RDSqlQuery(sql,false);
  delete q;

  SetVersion(283);
}


void MainObject::Revert285() const
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("alter table CART add column ISRC char(12) after YEAR");
  q=new RDSqlQuery(sql,false);
  delete q;

  sql=QString("alter table DECKS add ")+
    "column DEFAULT_SAMPRATE int default 44100 after DEFAULT_CHANNELS";
  q=new RDSqlQuery(sql,false);
  delete q;

  sql=QString("alter table RDLIBRARY add ")+
    "column INPUT_STREAM int default 0 after INPUT_CARD";
  q=new RDSqlQuery(sql,false);
  delete q;

  sql=QString("alter table RDLIBRARY add ")+
    "column OUTPUT_STREAM int default 0 after OUTPUT_CARD";
  q=new RDSqlQuery(sql,false);
  delete q;

  sql=QString("alter table RDLIBRARY add ")+
    "column RECORD_GPI int default -1 after TRIM_THRESHOLD";
  q=new RDSqlQuery(sql,false);
  delete q;

  sql=QString("alter table RDLIBRARY add ")+
    "column PLAY_GPI int default -1 after RECORD_GPI";
  q=new RDSqlQuery(sql,false);
  delete q;

  sql=QString("alter table RDLIBRARY add ")+
    "column STOP_GPI int default -1 after PLAY_GPI";
  q=new RDSqlQuery(sql,false);
  delete q;

  sql=QString("alter table RDLIBRARY add ")+
    "column DEFAULT_SAMPRATE int default 44100 after DEFAULT_CHANNELS";
  q=new RDSqlQuery(sql,false);
  delete q;

  sql=QString("alter table RDLOGEDIT add ")+
    "column SAMPRATE int unsigned default 44100 after FORMAT";
  q=new RDSqlQuery(sql,false);
  delete q;

  sql=QString("alter table RECORDINGS add ")+
    "column SAMPRATE int unsigned default 44100 after CHANNELS";
  q=new RDSqlQuery(sql,false);
  delete q;

  sql=QString("alter table SERVICES add ")+
    "column TFC_START_OFFSET int after TFC_TITLE_LENGTH";
  q=new RDSqlQuery(sql,false);
  delete q;

  sql=QString("alter table SERVICES add ")+
    "column TFC_START_LENGTH int after TFC_START_OFFSET";
  q=new RDSqlQuery(sql,false);
  delete q;

  sql=QString("alter table SERVICES add ")+
    "column MUS_START_OFFSET int after MUS_TITLE_LENGTH";
  q=new RDSqlQuery(sql,false);
  delete q;

  sql=QString("alter table SERVICES add ")+
    "column MUS_START_LENGTH int after MUS_START_OFFSET";
  q=new RDSqlQuery(sql,false);
  delete q;

  SetVersion(284);
}


int MainObject::GetVersion() const
{
  QString sql;
  QSqlQuery *q;
  int ret=0;

  sql=QString("select DB from VERSION");
  q=new QSqlQuery(sql);
  if(q->first()) {
    ret=q->value(0).toInt();
  }

  return ret;
}


void MainObject::SetVersion(int schema) const
{
  QString sql;
  QSqlQuery *q;

  sql=QString().sprintf("update VERSION set DB=%d",schema);
  q=new QSqlQuery(sql);
  delete q;
}


int MainObject::MapSchema(const QString &ver)
{
  QString version=ver;
  bool ok=false;

  //
  // Version -> Schema Map
  //
  std::map<QString,int> version_map;
  version_map["2.10"]=242;
  version_map["2.11"]=245;
  version_map["2.12"]=254;
  version_map["2.13"]=255;
  version_map["2.14"]=258;
  version_map["2.15"]=259;
  version_map["2.16"]=263;
  version_map["2.17"]=268;
  version_map["2.18"]=272;
  version_map["2.19"]=275;
  version_map["2.20"]=285;

  //
  // Normalize String
  //
  if(version.left(1).lower()=="v") {
    version=version.right(version.length()-1);
  }
  QStringList f0=f0.split(".",version);
  if(f0.size()!=3) {
    return 0;
  }
  for(int i=0;i<3;i++) {
    f0[i].toInt(&ok);
    if(!ok) {
      return 0;
    }
  }

  //
  // Lookup Schema
  //
  if(version_map.count(f0[0]+"."+f0[1])==0) {
    return 0;
  }

  return version_map[f0[0]+"."+f0[1]];
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
