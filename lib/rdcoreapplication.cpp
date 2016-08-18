// rdcoreapplication.cpp
//
// Base class for Rivendell CGI programs
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

#include "dbversion.h"
#include "rdcoreapplication.h"

RDCoreApplication *rdca=NULL;

RDCoreApplication::RDCoreApplication(int argc,char **argv,const char *modname,
				     const char *usage,bool skip_db_check)
  : QCoreApplication(argc,argv)
{
  rdca=this;

  unsigned schema=0;
  QString err;

  app_heartbeat=NULL;

  //
  // Command-line Parser
  //
  app_cmd_switch=new RDCmdSwitch(argc,argv,modname,usage);

  //
  // Open Global Configuration
  //
  app_config=new RDConfig();
  app_config->load();

  //
  // Open Database
  //
  if(!OpenDb()) {
    abort("unable to connect to database");
  }
  if((!skip_db_check)&&(RD_VERSION_DATABASE!=schema)) { 
    abort("skewed database schema");
  }

  //
  // Configuration Accessors
  //
  app_airplay_conf=new RDAirPlayConf(config()->stationName(),"RDAIRPLAY");
  app_panel_conf=new RDAirPlayConf(config()->stationName(),"RDPANEL");
  app_library_conf=new RDLibraryConf(config()->stationName());
  app_logedit_conf=new RDLogeditConf(config()->stationName());
  app_station=new RDStation(config()->stationName());
  app_ripc=new RDRipc(config()->stationName());
  app_cae=new RDCae(station(),config());
  app_system=new RDSystem();
  app_user=NULL;
}


RDAirPlayConf *RDCoreApplication::airplayConf() const
{
  return app_airplay_conf;
}


RDAirPlayConf *RDCoreApplication::panelConf() const
{
  return app_panel_conf;
}


RDLibraryConf *RDCoreApplication::libraryConf() const
{
  return app_library_conf;
}


RDLogeditConf *RDCoreApplication::logeditConf() const
{
  return app_logedit_conf;
}


RDCae *RDCoreApplication::cae() const
{
  return app_cae;
}


RDRipc *RDCoreApplication::ripc() const
{
  return app_ripc;
}


RDStation *RDCoreApplication::station() const
{
  return app_station;
}


RDSystem *RDCoreApplication::system() const
{
  return app_system;
}


RDUser *RDCoreApplication::user() const
{
  return app_user;
}


void RDCoreApplication::setUser(const QString &username)
{
  if(app_user!=NULL) {
    delete app_user;
  }
  app_user=new RDUser(username);
}


RDConfig *RDCoreApplication::config() const
{
  return app_config;
}


RDCmdSwitch *RDCoreApplication::cmdSwitch() const
{
  return app_cmd_switch;
}


QSqlDatabase RDCoreApplication::database() const
{
  return QSqlDatabase::database();
}


QString RDCoreApplication::dbHostname() const
{
  return app_db_hostname;
}


QString RDCoreApplication::dbDatabaseName() const
{
  return app_db_dbname;
}


QString RDCoreApplication::dbUsername() const
{
  return app_db_username;
}


QString RDCoreApplication::dbPassword() const
{
  return app_db_password;
}


void RDCoreApplication::abort(const QString &err_msg)
{
  printf("Content-type: text-html\n");
  printf("Status: 500\n");
  printf("\n");
  printf("%s\n",(const char *)err_msg.toUtf8());
  exit(0);
}


bool RDCoreApplication::OpenDb()
{
  //
  // Get Credentials
  //
  app_db_hostname=config()->mysqlHostname();
  app_db_dbname=config()->mysqlDbname();
  app_db_username=config()->mysqlUsername();
  app_db_password=config()->mysqlPassword();
  for(unsigned i=0;i<cmdSwitch()->keys();i++) {
    if(cmdSwitch()->key(i)=="--db-hostname") {
      app_db_hostname=cmdSwitch()->value(i);
    }
    if(cmdSwitch()->key(i)=="--db-dbname") {
      app_db_dbname=cmdSwitch()->value(i);
    }
    if(cmdSwitch()->key(i)=="--db-username") {
      app_db_username=cmdSwitch()->value(i);
    }
    if(cmdSwitch()->key(i)=="--db-password") {
      app_db_password=cmdSwitch()->value(i);
    }
  }

  app_schema=0;
  QSqlDatabase db=QSqlDatabase::database();

  if (!db.isOpen()) {
    db=QSqlDatabase::addDatabase(config()->mysqlDriver());
    db.setHostName(app_db_hostname);
    db.setDatabaseName(app_db_dbname);
    db.setUserName(app_db_username);
    db.setPassword(app_db_password);
    if(!db.open()) {
      db.removeDatabase(app_db_dbname);
      db.close();
      return false;
    }
  }
  if(app_heartbeat==NULL){
    app_heartbeat=new RDDbHeartbeat(config()->mysqlHeartbeatInterval());
  }
  //  QSqlQuery *q=new QSqlQuery("set character_set_results='utf8'");
  //  delete q;

  QSqlQuery *q=new QSqlQuery("select DB from VERSION");
  if(q->first()) {
    app_schema=q->value(0).toUInt();
  }
  delete q;

  return true;
}
