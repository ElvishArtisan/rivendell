// rddbmgr.cpp
//
// Rivendell database management utility
//
//   (C) Copyright 2018-2021 Fred Gleason <fredg@paravelsystems.com>
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
#include <unistd.h>
#include <sys/types.h>

#include <QCoreApplication>
#include <QFileInfo>
#include <QSqlDatabase>
#include <QSqlError>
#include <QStringList>

#include <dbversion.h>
#include <rdcmd_switch.h>
#include <rddb.h>
#include <rdescape_string.h>

#include "rddbmgr.h"

MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  bool ok=false;

  db_command=MainObject::NoCommand;
  int set_schema=0;
  QString set_version="";
  QString station_name;
  bool generate_audio=false;
  db_verbose=false;

  db_yes=false;
  db_no=false;
  db_relink_audio="";
  db_relink_audio_move=false;

  db_check_all=true;
  db_check_orphaned_audio=false;
  db_check_orphaned_carts=false;
  db_check_orphaned_cuts=false;
  db_check_orphaned_tracks=false;

  //
  // Check that we're 'root'
  //
  if(geteuid()!=0) {
    fprintf(stderr,"rddbmgr: this utility requires root privileges\n");
    exit(1);
  }

  //
  // Load default DB credentials
  //
  db_config=new RDConfig();
  db_config->load();
  db_mysql_hostname=db_config->mysqlHostname();
  db_mysql_loginname=db_config->mysqlUsername();
  db_mysql_password=db_config->mysqlPassword();
  db_mysql_database=db_config->mysqlDbname();
  db_mysql_driver=db_config->mysqlDriver();
  db_mysql_engine=db_config->mysqlEngine();
  station_name=db_config->stationName();

  InitializeSchemaMap();

  //
  // Process Command Switches
  //
  RDCmdSwitch *cmd=new RDCmdSwitch("rddbmgr",RDDBMGR_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--check") {
      MainObject::Command command=MainObject::CheckCommand;
      if((db_command!=MainObject::NoCommand)&&
	 (db_command!=MainObject::CheckCommand)) {
	fprintf(stderr,"rddbmgr: exactly one command must be specified\n");
	exit(1);
      }
      db_command=command;
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--create") {
      MainObject::Command command=MainObject::CreateCommand;
      if((db_command!=MainObject::NoCommand)&&
	 (db_command!=MainObject::CreateCommand)) {
	fprintf(stderr,"rddbmgr: exactly one command must be specified\n");
	exit(1);
      }
      db_command=command;
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--modify") {
      MainObject::Command command=MainObject::ModifyCommand;
      if((db_command!=MainObject::NoCommand)&&
	 (db_command!=MainObject::ModifyCommand)) {
	fprintf(stderr,"rddbmgr: exactly one command must be specified\n");
	exit(1);
      }
      db_command=command;
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--generate-audio") {
      generate_audio=true;
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--mysql-hostname") {
      db_mysql_hostname=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--mysql-loginname") {
      db_mysql_loginname=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--mysql-password") {
      db_mysql_password=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--mysql-database") {
      db_mysql_database=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--mysql-driver") {
      db_mysql_driver=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--mysql-engine") {
      db_mysql_engine=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--set-schema") {
      set_schema=cmd->value(i).toInt(&ok);
      if((!ok)||(set_schema<=0)) {
	fprintf(stderr,"rddbmgr: invalid schema\n");
	exit(1);
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--set-version") {
      bool ok2=false;
      set_version=cmd->value(i);
      QStringList f0=set_version.split(".",QString::KeepEmptyParts);
      if(f0.size()==3) {
	ok2=true;
	for(int i=0;i<3;i++) {
	  f0[i].toInt(&ok);
	  ok2=ok2&&ok;
	}
      }
      if(!ok2) {
	fprintf(stderr,"rddbmgr: invalid version\n");
	exit(1);
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--station-name") {
      station_name=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--verbose") {
      db_verbose=true;
      cmd->setProcessed(i,true);
    }

    //
    // --check options
    //
    if(cmd->key(i)=="--yes") {
      db_yes=true;
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--no") {
      db_no=true;
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--orphan-group") {
      db_orphan_group_name=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--dump-cuts-dir") {
      db_dump_cuts_dir=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--relink-audio") {
      db_relink_audio=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--relink-audio-move") {
      db_relink_audio_move=true;
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--rehash") {
      db_rehash=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--orphaned-audio") {
      db_check_all=false;
      db_check_orphaned_audio=true;
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--orphaned-carts") {
      db_check_all=false;
      db_check_orphaned_carts=true;
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--orphaned-cuts") {
      db_check_all=false;
      db_check_orphaned_cuts=true;
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--orphaned-tracks") {
      db_check_all=false;
      db_check_orphaned_tracks=true;
      cmd->setProcessed(i,true);
    }

    if(!cmd->processed(i)) {
      fprintf(stderr,"rddbmgr: unrecognized option \"%s\"\n",
	      cmd->key(i).toUtf8().constData());
      exit(1);
    }
  }

  //
  // Sanity Checks
  //
  if((cmd->keys()>0)&&(db_command==MainObject::NoCommand)) {
    fprintf(stderr,"rddbmgr: exactly one command must be specified\n");
    exit(1);
  }
  if(db_yes&&db_no) {
    fprintf(stderr,"rddbmgr: '--yes' and '--no' are mutually exclusive\n");
    exit(1);
  }
  delete cmd;

  //
  // Sanity Checks for --check
  //
  if(db_command==MainObject::CheckCommand) {
    // Check for dump cuts directory
    if(!db_dump_cuts_dir.isEmpty()) {
      QFileInfo file(db_dump_cuts_dir);
      if(!file.exists()) {
	fprintf(stderr,"rddbmgr: directory \"%s\" does not exist.\n",
		db_dump_cuts_dir.toUtf8().constData());
	exit(1);
      }
      if(!file.isDir()) {
	fprintf(stderr,"rddbmgr: \"%s\" is not a directory.\n",
		db_dump_cuts_dir.toUtf8().constData());
	exit(1);
      }
      if(!file.isWritable()) {
	fprintf(stderr,"rddbmgr: \"%s\" is not writable.\n",
		db_dump_cuts_dir.toUtf8().constData());
	exit(1);
      }
    }

    // Check that Orphan group exists
    if(!db_orphan_group_name.isEmpty()) {
      QString sql=QString("select `NAME` from `GROUPS` where ")+
	"NAME='"+RDEscapeString(db_orphan_group_name)+"'";
      RDSqlQuery *q=new RDSqlQuery(sql,false);
      if(!q->first()) {
	fprintf(stderr,"rddbmgr: invalid group \"%s\"\n",
		db_orphan_group_name.toUtf8().constData());
	delete q;
	exit(1);
      }
      delete q;
    }
  }

  if(db_verbose) {
    fprintf(stderr,"Using DB Credentials:\n");
    fprintf(stderr,"  Hostname: %s\n",db_mysql_hostname.toUtf8().constData());
    fprintf(stderr,"  Loginname: %s\n",db_mysql_loginname.toUtf8().constData());
    fprintf(stderr,"  Password: %s\n",db_mysql_password.toUtf8().constData());
    fprintf(stderr,"  Database: %s\n",db_mysql_database.toUtf8().constData());
    fprintf(stderr,"  Driver: %s\n",db_mysql_driver.toUtf8().constData());
    fprintf(stderr,"  Engine: %s\n",db_mysql_engine.toUtf8().constData());
  }

  //
  // Open Database
  //
  QSqlDatabase db=QSqlDatabase::addDatabase(db_mysql_driver);
  db.setDatabaseName(db_mysql_database);
  db.setUserName(db_mysql_loginname);
  db.setPassword(db_mysql_password);
  db.setHostName(db_mysql_hostname);
  if(!db.open()) {
    fprintf(stderr,"rddbmgr: unable to open database [%s]\n",
	    db.lastError().text().toUtf8().constData());
    exit(1);
  }
  db_table_create_postfix=RDConfig::createTablePostfix(db_mysql_engine);

  //
  // Resolve Target Schema
  //
  int schema=GetCurrentSchema();
  if(schema>RD_VERSION_DATABASE) {
    fprintf(stderr,"rddbmgr: unknown current schema [%d]\n",schema);
    exit(1);
  }
  if(set_schema>0) {
    if((set_schema<242)||(set_schema>RD_VERSION_DATABASE)) {
      fprintf(stderr,"rddbmgr: unsupported schema\n");
      exit(1);
    }
  }
  else {
    if(set_version.isEmpty()) {
      set_schema=RD_VERSION_DATABASE;
      if(set_schema<schema) {
	fprintf(stderr,"rddbmgr: reversion implied, you must explicitly specify the target schema\n");
	exit(1);
      }
    }
    else {
      set_schema=GetVersionSchema(set_version);
      if(set_schema==0) {
	fprintf(stderr,"invalid/unsupported Rivendell version\n");
	exit(1);
      }
    }
  }

  //
  // Run the Command
  //
  QString err_msg;
  switch(db_command) {
  case MainObject::CheckCommand:
    ok=Check(&err_msg);
    break;

  case MainObject::CreateCommand:
    if((ok=Create(station_name,generate_audio,&err_msg))) {
      ok=Modify(&err_msg,set_schema);
    }
    break;

  case MainObject::ModifyCommand:
    ok=Modify(&err_msg,set_schema);
    break;

  case MainObject::NoCommand:
    ok=PrintStatus(&err_msg);
    break;
  }

  if(!ok) {
    fprintf(stderr,"rddbmgr: %s\n",err_msg.toUtf8().constData());
    exit(1);
  }
  exit(0);
}


void MainObject::WriteSchemaVersion(int ver) const
{
  QString sql=QString("update `VERSION` set ")+
    QString().sprintf("`DB`=%d",ver);
  RDSqlQuery::apply(sql);
}


bool MainObject::TableExists(const QString &tbl_name) const
{
  QString sql;
  RDSqlQuery *q;
  bool ret=false;

  sql=QString("show tables where ")+
    "`Tables_in_"+db_config->mysqlDbname()+"`='"+RDEscapeString(tbl_name)+"'";
  q=new RDSqlQuery(sql,false);
  ret=q->first();
  delete q;

  return ret;
}


bool MainObject::DropTable(const QString &tbl_name,QString *err_msg) const
{
  QString sql;
  RDSqlQuery *q;
  bool ret=false;

  sql=QString("show tables where ")+
    "`Tables_in_"+db_config->mysqlDbname()+"`='"+RDEscapeString(tbl_name)+"'";
  q=new RDSqlQuery(sql,false);
  if(q->first()) {
    sql=QString("drop table `")+q->value(0).toString()+"`";
    ret=RDSqlQuery::apply(sql,err_msg);
  }
  else {
    if(err_msg!=NULL) {
      *err_msg="no such table";
    }
  }
  delete q;
  return ret;
}


bool MainObject::ColumnExists(const QString &tbl_name,
			      const QString &col_name) const
{
  QString sql;
  RDSqlQuery *q;
  bool ret=false;

  sql=QString("select * from `INFORMATION_SCHEMA`.`COLUMNS` where ")+
    "`TABLE_SCHEMA`='"+db_config->mysqlDbname()+"' && "+
    "`TABLE_NAME`='"+tbl_name+"' && "+
    "`COLUMN_NAME`='"+col_name+"'";
  q=new RDSqlQuery(sql,false);
  ret=q->first();
  delete q;

  return ret;
}


bool MainObject::DropColumn(const QString &tbl_name,const QString &col_name,
			    QString *err_msg) const
{
  QString sql;
  bool ret=false;

  if(ColumnExists(tbl_name,col_name)) {
    sql=QString("alter table `")+tbl_name+"` drop column "+col_name;
    ret=RDSqlQuery::apply(sql,err_msg);
  }

  return ret;
}


bool MainObject::DropIndex(const QString &tbl_name,const QString &idx_name,
			   QString *err_msg) const
{
  QString sql=QString("alter table `")+tbl_name+"` drop index `"+idx_name+"`";
  return RDSqlQuery::apply(sql,err_msg);
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);

  new MainObject();
  return a.exec();
}
