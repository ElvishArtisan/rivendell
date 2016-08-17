// rdcgiapplication.cpp
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
#include "rddb.h"
#include "rddbheartbeat.h"

#include "rdcgiapplication.h"

RDCgiApplication *rdcgi=NULL;

RDCgiApplication::RDCgiApplication(int argc,char **argv)
  : QApplication(argc,argv,false)
{
  unsigned schema=0;
  QString err;

  rdcgi=this;

  //
  // Command-line Parser
  //
  app_cmd_switch=new RDCmdSwitch(qApp->argc(),qApp->argv(),"CGI","");

  //
  // Open Global Configuration
  //
  app_config=new RDConfig();
  app_config->load();

  //
  // Open Database
  //
  QSqlDatabase db=RDInitDb(&schema,&err);
  if(!db.isOpen()) {
    printf("Content-type: text/html\n");
    printf("Status: 500\n");
    printf("\n");
    printf("Unable to connect to Rivendell database\n");
    db.close();
    exit(0);
  }
  if(RD_VERSION_DATABASE!=schema) { 
    printf("Content-type: text/html\n");
    printf("Status: 500\n");
    printf("\n");
    printf("Missing/invalid database version\n");
    db.close();
    exit(0);
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


RDAirPlayConf *RDCgiApplication::airplayConf() const
{
  return app_airplay_conf;
}


RDAirPlayConf *RDCgiApplication::panelConf() const
{
  return app_panel_conf;
}


RDLibraryConf *RDCgiApplication::libraryConf() const
{
  return app_library_conf;
}


RDLogeditConf *RDCgiApplication::logeditConf() const
{
  return app_logedit_conf;
}


RDCae *RDCgiApplication::cae() const
{
  return app_cae;
}


RDRipc *RDCgiApplication::ripc() const
{
  return app_ripc;
}


RDStation *RDCgiApplication::station() const
{
  return app_station;
}


RDSystem *RDCgiApplication::system() const
{
  return app_system;
}


RDUser *RDCgiApplication::user() const
{
  return app_user;
}


void RDCgiApplication::setUser(const QString &username)
{
  if(app_user!=NULL) {
    delete app_user;
  }
  app_user=new RDUser(username);
}


RDConfig *RDCgiApplication::config() const
{
  return app_config;
}


RDCmdSwitch *RDCgiApplication::cmdSwitch() const
{
  return app_cmd_switch;
}
