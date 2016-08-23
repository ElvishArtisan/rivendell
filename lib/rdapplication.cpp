// rdapplication.cpp
//
// Base class for Rivendell GUI programs
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

#include <unistd.h>

#include <QMessageBox>

#include <dbversion.h>

#include "rdapplication.h"

RDApplication *rda=NULL;

RDApplication::RDApplication(RDApplication::AppType type,const char *modname,
			     const char *usage,bool skip_schema_check)
{
  rda=this;

  QString err;

  app_type=type;
  app_airplay_conf=NULL;
  app_panel_conf=NULL;
  app_library_conf=NULL;
  app_logedit_conf=NULL;
  app_station=NULL;
  app_ripc=NULL;
  app_cae=NULL;
  app_system=NULL;
  app_user=NULL;

  //
  // Command-line Parser
  //
  app_cmd_switch=new RDCmdSwitch(qApp->argc(),qApp->argv(),modname,usage);

  //
  // Open Global Configuration
  //
  app_config=new RDConfig(app_cmd_switch);
  app_config->load();

  //
  // Open Database
  //
  if(!RDOpenDb(&app_schema,&err,config())) {
    abend(256,err);
  }
  if((!skip_schema_check)&&(RD_VERSION_DATABASE!=app_schema)) { 
    abend(256,QString().
	  sprintf("Skewed database schema [schema is %d, should be %d].",
		  app_schema,RD_VERSION_DATABASE));
  }

  //
  // Configuration Accessors
  //
  if(!skip_schema_check) {
    startAccessors();
  }
}


RDApplication::AppType RDApplication::appType() const
{
  return app_type;
}


int RDApplication::schemaVersion() const
{
  return app_schema;
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


QSqlDatabase RDApplication::database() const
{
  return QSqlDatabase::database();
}


void RDApplication::startAccessors()
{
  //
  // Re-read the DB schema
  //
  RDSqlQuery *q=new RDSqlQuery("select DB from VERSION");
  if(!q->first()) {
    abend(256,QObject::tr("unable to re-read database schema."));
  }
  app_schema=q->value(0).toInt();
  delete q;

  //
  // Start Accessors
  //
  app_airplay_conf=new RDAirPlayConf(config()->stationName(),"RDAIRPLAY");
  app_panel_conf=new RDAirPlayConf(config()->stationName(),"RDPANEL");
  app_library_conf=new RDLibraryConf(config()->stationName());
  app_logedit_conf=new RDLogeditConf(config()->stationName());
  app_station=new RDStation(config()->stationName());
  app_system=new RDSystem();
  app_ripc=new RDRipc(config()->stationName());
  app_cae=new RDCae(station(),config());
}


void RDApplication::abend(int exit_code,const QString &err_msg) const
{
  switch(app_type) {
  case RDApplication::Gui:
    QMessageBox::information(NULL,"Rivendell - "+QObject::tr("Error"),err_msg);
    _exit(exit_code);
    break;

  case RDApplication::Console:
    fprintf(stderr,"%s: %s\n",qApp->argv()[0],(const char *)err_msg.toUtf8());
    _exit(exit_code);
    break;

  case RDApplication::Cgi:
    printf("Content-type: text-html\n");
    printf("Status: 500\n");
    printf("\n");
    printf("%s\n",(const char *)err_msg.toUtf8());
    _exit(0);
  }
  _exit(exit_code);
}
