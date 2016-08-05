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

#include <qstringlist.h>

#include <rdapplication.h>
#include <rdclock.h>
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
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(rda->cmdSwitch()->key(i)=="--set-schema") {
      set_schema=rda->cmdSwitch()->value(i).toInt(&ok);
      if((!ok)||(set_schema<0)) {
	fprintf(stderr,"rdrevert: invalid schema value\n");
	exit(256);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--set-version") {
      if((set_schema=MapSchema(rda->cmdSwitch()->value(i)))==0) {
	fprintf(stderr,"rdrevert: invalid/unsupported Rivendell version\n");
	exit(256);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }
  }
  if(!rda->cmdSwitch()->allProcessed()) {
    fprintf(stderr,"rdrevert: unknown option\n");
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
  RDApplication a(argc,argv,"rdrevert",RDREVERT_USAGE,false,true);
  new MainObject();
  return a.exec();
}
