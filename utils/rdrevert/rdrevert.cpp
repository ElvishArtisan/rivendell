// rdrevert.cpp
//
// Revert the Rivendell database schema to an earlier version.
//
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
      sql=RDCreateClockTableSql(RDClock::tableName(q->value(0).toString()));
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
      RDCreateLogTable(RDEvent::preimportTableName(q->value(0).toString()));
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

      RDCreateLogTable(RDEvent::postimportTableName(q->value(0).toString()));
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
  version_map["2.18"]=269;

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
