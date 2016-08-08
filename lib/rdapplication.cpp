// rdapplication.cpp
//
// Base class for Rivendell modules
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

#include <qmessagebox.h>

#include "dbversion.h"
#include "rdapplication.h"
#include "rddb.h"
#include "rddbheartbeat.h"

RDApplication *rda=NULL;

RDApplication::RDApplication(int argc,char **argv,const char *modname,
			     const char *usage,bool gui,bool skip_db_check)
  : QApplication(argc,argv,gui)
{
  unsigned schema=0;
  QString err;

  rda=this;

  //
  // Command-line Parser
  //
  app_cmd_switch=new RDCmdSwitch(qApp->argc(),qApp->argv(),modname,usage);
  for(unsigned i=0;i<app_cmd_switch->keys();i++) {
    if(app_cmd_switch->key(i)=="--skip-db-check") {
      skip_db_check=true;
      app_cmd_switch->setProcessed(i,true);
    }
  }

  //
  // Open Global Configuration
  //
  app_config=new RDConfig();
  app_config->load();

  //
  // Open Database
  //
  QSqlDatabase *db=RDInitDb(&schema,&err);
  if(!db) {
    if(gui) {
      QMessageBox::warning(NULL,"Rivendell - "+QObject::tr("DB Error"),
			   QObject::tr("Unable to connect to MySQL."),err);
    }
    else {
      fprintf(stderr,"%s: unable to connect to MySQL\n",argv[0]);
    }
    exit(256);
  }
  if((RD_VERSION_DATABASE!=schema)&&(!skip_db_check)) {
    if(gui) {
      QMessageBox::warning(NULL,"Rivendell - "+QObject::tr("DB Error"),
			   QObject::tr("database version mismatch, should be ")+
			   QString().sprintf("%u, ",RD_VERSION_DATABASE)+
			   QObject::tr("is")+QString().sprintf(" %u.",schema));
    }
    else {
      fprintf(stderr,"%s: database version mismatch, should be %u\n",
	      argv[0],schema);
    }
    exit(256);
  }
  new RDDbHeartbeat(config()->mysqlHeartbeatInterval());

  //
  // Configuration Accessors
  //
  app_airplay_conf=new RDAirPlayConf(config()->stationName(),"RDAIRPLAY");
  app_panel_conf=new RDAirPlayConf(config()->stationName(),"RDPANEL");
  app_library_conf=new RDLibraryConf(config()->stationName(),0);
  app_logedit_conf=new RDLogeditConf(config()->stationName());
  app_station=new RDStation(config()->stationName());
  app_ripc=new RDRipc(config()->stationName());
  app_cae=new RDCae(station(),config());
  app_system=new RDSystem();
  app_user=NULL;
}


RDAirPlayConf *RDApplication::airplayConf() const
{
  return app_airplay_conf;
}


RDAirPlayConf *RDApplication::panelConf() const
{
  return app_panel_conf;
}


RDLibraryConf *RDApplication::libraryConf() const
{
  return app_library_conf;
}


RDLogeditConf *RDApplication::logeditConf() const
{
  return app_logedit_conf;
}


RDCae *RDApplication::cae() const
{
  return app_cae;
}


RDRipc *RDApplication::ripc() const
{
  return app_ripc;
}


RDStation *RDApplication::station() const
{
  return app_station;
}


RDSystem *RDApplication::system() const
{
  return app_system;
}


RDUser *RDApplication::user() const
{
  return app_user;
}


void RDApplication::setUser(const QString &username)
{
  if(app_user!=NULL) {
    delete app_user;
  }
  app_user=new RDUser(username);
}


RDConfig *RDApplication::config() const
{
  return app_config;
}


RDCmdSwitch *RDApplication::cmdSwitch() const
{
  return app_cmd_switch;
}
