// rdapplication.cpp
//
// Base Application Class
//
//   (C) Copyright 2018-2019 Fred Gleason <fredg@paravelsystems.com>
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
#include <syslog.h>

#include <qapplication.h>
#include <qobject.h>
#include <qprocess.h>

#include "dbversion.h"
#include "rdapplication.h"
#include "rdcmd_switch.h"

RDApplication *rda=NULL;
QStringList __rdapplication_temp_files;

void __RDApplication_ExitCallback()
{
  for(int i=0;i<__rdapplication_temp_files.size();i++) {
    unlink(__rdapplication_temp_files.at(i).toUtf8());
  }
}


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

  atexit(__RDApplication_ExitCallback);
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


bool RDApplication::open(QString *err_msg,RDApplication::ErrorType *err_type,
			 bool check_svc)
{
  int schema=0;
  QString db_err;
  bool skip_db_check=false;
  int persistent_dropbox_id=-1;
  bool ok=false;

  if(err_type!=NULL) {
    *err_type=RDApplication::ErrorOk;
  }

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
    if(app_cmd_switch->key(i)=="--persistent-dropbox-id") {
      persistent_dropbox_id=app_cmd_switch->value(i).toUInt(&ok);
      if(ok) {
	app_command_name=QString().sprintf("dropbox[%u]",persistent_dropbox_id);
      }
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
  // Initialize Logging
  //
  if(app_cmd_switch->debugActive()) {
    openlog(app_command_name,LOG_PERROR,app_config->syslogFacility());
  }
  else {
    openlog(app_command_name,0,app_config->syslogFacility());
  }

  //
  // Check Rivendell Service Status
  //
  if(check_svc) {
    if(!CheckService(err_msg)) {
      if(err_type!=NULL) {
	*err_type=RDApplication::ErrorNoService;
      }
      return false;
    }
  }

  //
  // Open Database
  //
  if(!RDOpenDb(&schema,&db_err,app_config)) {
    *err_msg=QObject::tr("Unable to open database")+" ["+db_err+"]";
    return false;
  }
  if((RD_VERSION_DATABASE!=schema)&&(!skip_db_check)) {
    if(err_type!=NULL) {
      *err_type=RDApplication::ErrorDbVersionSkew;
    }
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
  app_station=new RDStation(app_config->stationName());
  app_system=new RDSystem();
  app_schemas=new RDRssSchemas();
  app_library_conf=new RDLibraryConf(app_config->stationName());
  app_logedit_conf=new RDLogeditConf(app_config->stationName());
  app_airplay_conf=new RDAirPlayConf(app_config->stationName(),"RDAIRPLAY");
  app_panel_conf=new RDAirPlayConf(app_config->stationName(),"RDPANEL");
  app_user=new RDUser();
  app_cae=new RDCae(app_station,app_config,this);
  app_ripc=new RDRipc(app_station,app_config,this);
  connect(app_ripc,SIGNAL(userChanged()),this,SLOT(userChangedData()));

  if(!app_station->exists()) {
    if(err_type!=NULL) {
      *err_type=RDApplication::ErrorNoHostEntry;
    }
    *err_msg=QObject::tr("This host")+" (\""+app_config->stationName()+"\") "+
      QObject::tr("does not have a Hosts entry in the database.")+"\n"+
      QObject::tr("Open RDAdmin->ManageHosts->Add to create one.");
    return false;
  }

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


RDRssSchemas *RDApplication::rssSchemas()
{
  return app_schemas;
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


void RDApplication::addTempFile(const QString &pathname)
{
  __rdapplication_temp_files.push_back(pathname);
}


void RDApplication::logAuthenticationFailure(const QHostAddress &orig_addr,
					     const QString &login_name)
{
  if(login_name.isEmpty()) {
    syslog(LOG_NOTICE,"failed WebAPI login from %s",
	   orig_addr.toString().toUtf8().constData());
  }
  else {
    syslog(LOG_NOTICE,"failed WebAPI login from %s for user \"%s\"",
	   orig_addr.toString().toUtf8().constData(),
	   login_name.toUtf8().constData());
  }
}


void RDApplication::syslog(int priority,const char *fmt,...) const
{
  va_list args;

  va_start(args,fmt);
  if((priority&248)==0) {  // So custom one-off facility numbers still work
    priority=priority|(app_config->syslogFacility()<<3);
  }
  vsyslog(priority,fmt,args);
  va_end(args);
}


void RDApplication::syslog(RDConfig *config,int priority,const char *fmt,...)
{
  va_list args;

  va_start(args,fmt);
  if((priority&248)==0) {  // So custom one-off facility numbers still work
    priority=priority|(config->syslogFacility()<<3);
  }
  vsyslog(priority,fmt,args);
  va_end(args);
}


void RDApplication::userChangedData()
{
  app_user->setName(app_ripc->user());
  emit userChanged();
}


 bool RDApplication::CheckService(QString *err_msg)
{
  bool ret=false;
  int trial=config()->serviceTimeout();

  if(trial<=0) {
    trial=1;
  }
  while((!ret)&&(trial>0)) {
    QStringList args;
    QProcess *proc=new QProcess(this);

    args.push_back("--property");
    args.push_back("ActiveState");
    args.push_back("show");
    args.push_back("rivendell");
    proc->start("systemctl",args);
    proc->waitForFinished();
    if(proc->exitStatus()!=QProcess::NormalExit) {
      *err_msg=tr("systemctl(1) crashed.");
    }
    else {
      if(proc->exitCode()!=0) {
	*err_msg=tr("systemctl(1) returned exit code")+
	  QString().sprintf(" %d:\n",proc->exitCode())+
	  proc->readAllStandardError();
      }
      else {
	*err_msg=tr("Rivendell service is not active.");
	QStringList f0=QString(proc->readAllStandardOutput()).
	  split("\n",QString::SkipEmptyParts);
	for(int i=0;i<f0.size();i++) {
	  QStringList f1=f0.at(i).trimmed().split("=");
	  if((f1.size()==2)&&(f1.at(0)=="ActiveState")) {
	    ret=f1.at(1).toLower()=="active";
	    if(ret) {
	      *err_msg=tr("OK");
	    }
	  }
	}
      }
    }
    delete proc;

    trial--;
    sleep(1);
  }

  return ret;
}
