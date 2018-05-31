// rdapplication.cpp
//
// Base Application Class
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

#include <stdlib.h>

#include <qapplication.h>
#include <qobject.h>

#include "dbversion.h"
#include "rdapplication.h"
#include "rdcmd_switch.h"

RDApplication *rda=NULL;

RDApplication::RDApplication(const QString &module_name,const QString &cmdname,
			     const QString &usage,QObject *parent)
  : QObject(parent)
{
  app_module_name=module_name;
  app_command_name=cmdname;
  app_usage=usage;

  app_heartbeat=NULL;
  app_airplay_conf=NULL;
  app_cae=NULL;
  app_cmd_switch=NULL;
  app_config=NULL;
  app_library_conf=NULL;
  app_logedit_conf=NULL;
  app_panel_conf=NULL;
  app_ripc=NULL;
  app_station=NULL;
  app_system=NULL;
  app_user=NULL;
}


RDApplication::~RDApplication()
{
  if(app_heartbeat!=NULL) {
    delete app_heartbeat;
  }
  if(app_config!=NULL) {
    delete app_config;
  }
  if(app_system!=NULL) {
    delete app_system;
  }
  if(app_station!=NULL) {
    delete app_station;
  }
  if(app_library_conf!=NULL) {
    delete app_library_conf;
  }
  if(app_logedit_conf!=NULL) {
    delete app_logedit_conf;
  }
  if(app_airplay_conf!=NULL) {
    delete app_airplay_conf;
  }
  if(app_panel_conf!=NULL) {
    delete app_panel_conf;
  }
  if(app_user!=NULL) {
    delete app_user;
  }
  if(app_cae!=NULL) {
    delete app_cae;
  }
  if(app_cmd_switch!=NULL) {
    delete app_cmd_switch;
  }
  if(app_ripc!=NULL) {
    delete app_ripc;
  }
}


bool RDApplication::open(QString *err_msg)
{
  unsigned schema=0;
  QString db_err;
  bool skip_db_check=false;

  //
  // Read command switches
  //
  app_cmd_switch=new RDCmdSwitch(qApp->argc(),qApp->argv(),app_command_name,
				 app_usage);
  for(unsigned i=0;i<app_cmd_switch->keys();i++) {
    if(app_cmd_switch->key(i)=="--skip-db-check") {
      skip_db_check=true;
      app_cmd_switch->setProcessed(i,true);
    }
  }

  //
  // Open rd.conf(5)
  //
  app_config=new RDConfig();
  app_config->load();
  app_config->setModuleName(app_module_name);

  //
  // Open Database
  //
  QSqlDatabase *db=RDInitDb(&schema,&db_err);
  if(!db) {
    *err_msg=QObject::tr("Unable to open database")+" ["+db_err+"]";
    return false;
  }
  if((RD_VERSION_DATABASE!=schema)&&(!skip_db_check)) {
    *err_msg=QObject::tr("Database version mismatch, should be")+
      QString().sprintf(" %u, ",RD_VERSION_DATABASE)+
      QObject::tr("is")+
      QString().sprintf(" %u",schema);
    return false;
  }
  app_heartbeat=new RDDbHeartbeat(app_config->mysqlHeartbeatInterval(),this);

  //
  // Open Accessors
  //
  app_system=new RDSystem();
  app_station=new RDStation(app_config->stationName());
  app_library_conf=new RDLibraryConf(app_config->stationName());
  app_logedit_conf=new RDLogeditConf(app_config->stationName());
  app_airplay_conf=new RDAirPlayConf(app_config->stationName(),"RDAIRPLAY");
  app_panel_conf=new RDAirPlayConf(app_config->stationName(),"RDPANEL");
  app_user=new RDUser();
  app_cae=new RDCae(app_station,app_config,this);
  app_ripc=new RDRipc(app_station,app_config,this);
  connect(app_ripc,SIGNAL(userChanged()),this,SLOT(userChangedData()));

  return true; 
}


RDAirPlayConf *RDApplication::airplayConf()
{
  return app_airplay_conf;
}


RDCae *RDApplication::cae()
{
  return app_cae;
}


RDCmdSwitch *RDApplication::cmdSwitch()
{
  return app_cmd_switch;
}


RDConfig *RDApplication::config()
{
  return app_config;
}


RDLibraryConf *RDApplication::libraryConf()
{
  return app_library_conf;
}


RDLogeditConf *RDApplication::logeditConf()
{
  return app_logedit_conf;
}


RDAirPlayConf *RDApplication::panelConf()
{
  return app_panel_conf;
}


RDRipc *RDApplication::ripc()
{
  return app_ripc;
}


RDStation *RDApplication::station()
{
  return app_station;
}


RDSystem *RDApplication::system()
{
  return app_system;
}


RDUser *RDApplication::user()
{
  return app_user;
}


void RDApplication::log(RDConfig::LogPriority prio,const QString &msg)
{
  app_config->log(app_module_name,prio,msg);
}


bool RDApplication::dropTable(const QString &tbl_name)
{
  bool ret=false;

  QString sql=QString("show tables where ")+
    "Tables_in_"+config()->mysqlDbname()+"=\""+tbl_name+"\"";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    sql=QString("drop table `")+tbl_name+"`";
    RDSqlQuery *q1=new RDSqlQuery(sql);
    delete q1;
    ret=true;
  }
  delete q;

  return ret;
}


void RDApplication::userChangedData()
{
  app_user->setName(app_ripc->user());
  emit userChanged();
}
