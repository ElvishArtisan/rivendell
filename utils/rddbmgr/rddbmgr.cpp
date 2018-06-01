// rddbmgr.cpp
//
// Rivendell database management utility
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include <qapplication.h>
#include <qsqldatabase.h>
#include <qstringlist.h>

#include <rdcmd_switch.h>

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
  db_mysql_charset=db_config->mysqlCharset();
  db_mysql_collation=db_config->mysqlCollation();
  station_name=db_config->stationName();

  //
  // Process Command Switches
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"rddbmgr",RDDBMGR_USAGE);
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
    if(cmd->key(i)=="--mysql-charset") {
      db_mysql_charset=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--mysql-collation") {
      db_mysql_collation=cmd->value(i);
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
      QStringList f0=f0.split(".",set_version);
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
    if(!cmd->processed(i)) {
      fprintf(stderr,"rddbmgr: unrecognized option \"%s\"\n",
	      (const char *)cmd->key(i));
      exit(1);
    }
  }
  delete cmd;

  //
  // Sanity Checks
  //
  if(db_command==MainObject::NoCommand) {
    fprintf(stderr,"rddbmgr: exactly one command must be specified\n");
    exit(1);
  }

  if(db_verbose) {
    fprintf(stderr,"Using DB Credentials:\n");
    fprintf(stderr,"  Hostname: %s\n",(const char *)db_mysql_hostname);
    fprintf(stderr,"  Loginname: %s\n",(const char *)db_mysql_loginname);
    fprintf(stderr,"  Password: %s\n",(const char *)db_mysql_password);
    fprintf(stderr,"  Database: %s\n",(const char *)db_mysql_database);
    fprintf(stderr,"  Driver: %s\n",(const char *)db_mysql_driver);
    fprintf(stderr,"  Engine: %s\n",(const char *)db_mysql_engine);
    fprintf(stderr,"  Charset: %s\n",(const char *)db_mysql_charset);
    fprintf(stderr,"  Collation: %s\n",(const char *)db_mysql_collation);
  }

  //
  // Open Database
  //
  QSqlDatabase *db=QSqlDatabase::addDatabase(db_mysql_driver);
  if(!db) {
    fprintf(stderr,"rddbmgr: unable to connect to database server\n");
    exit(1);
  }
  db->setDatabaseName(db_mysql_database);
  db->setUserName(db_mysql_loginname);
  db->setPassword(db_mysql_password);
  db->setHostName(db_mysql_hostname);
  if(!db->open()) {
    fprintf(stderr,"rddbmgr: unable to open database [%s]\n",
	    (const char *)db->lastError().text());
    db->removeDatabase(db_mysql_database);
    exit(1);
  }
  db_table_create_postfix=
    RDConfig::createTablePostfix(db_mysql_engine,db_mysql_charset,
				 db_mysql_collation);  

  //
  // Run the Command
  //
  QString err_msg;
  switch(db_command) {
  case MainObject::CheckCommand:
    ok=Check(&err_msg);
    break;

  case MainObject::CreateCommand:
    ok=Create(station_name,generate_audio,&err_msg);
    break;

  case MainObject::ModifyCommand:
    ok=Modify(&err_msg,set_schema,set_version);
    break;

  case MainObject::NoCommand:
    break;
  }

  if(!ok) {
    fprintf(stderr,"rddbmgr: %s\n",(const char *)err_msg);
    exit(1);
  }
  exit(0);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);

  new MainObject();
  return a.exec();
}
