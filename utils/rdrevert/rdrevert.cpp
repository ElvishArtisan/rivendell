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

#include <map>

#include <qapplication.h>
#include <qstringlist.h>

#include <rdcmd_switch.h>

#include "rdrevert.h"

MainObject::MainObject(QObject *parent,const char *name)
  :QObject(parent,name)
{
  bool ok=false;
  int set_schema=0;

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
  SetVersion(245);
}


void MainObject::Revert247() const
{
  SetVersion(246);
}


void MainObject::Revert248() const
{
  SetVersion(247);
}


void MainObject::Revert249() const
{
  SetVersion(248);
}


void MainObject::Revert250() const
{
  SetVersion(249);
}


void MainObject::Revert251() const
{
  SetVersion(250);
}


void MainObject::Revert252() const
{
  SetVersion(251);
}


void MainObject::Revert253() const
{
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
  new MainObject(NULL,"main");
  return a.exec();
}
