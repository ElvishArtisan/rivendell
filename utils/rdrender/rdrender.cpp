// rdrender.cpp
//
// Render a Rivendell log.
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

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include <qapplication.h>
#include <qdir.h>
#include <qfile.h>

#include <rd.h>
#include <rdaudioexport.h>
#include <rdcart.h>
#include <rdcmd_switch.h>
#include <rdconf.h>
#include <rdescape_string.h>
#include <rddbheartbeat.h>
#include <rdsettings.h>

#include "rdrender.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  render_verbose=false;
  render_channels=RDRENDER_DEFAULT_CHANNELS;
  render_start_date=QDate::currentDate();
  render_start_time=QTime::currentTime();

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"rdimport",RDRENDER_USAGE);
  if(cmd->keys()<1) {
    fprintf(stderr,
	    "rdrender: you must specify a logname and output filename\n");
    exit(256);
  }
  if(cmd->keys()<2) {
    fprintf(stderr,
	    "rdrender: you must specify an output filename\n");
    exit(256);
  }
  for(int i=0;i<(int)cmd->keys()-2;i++) {
    bool ok=false;
    if(cmd->key(i)=="--verbose") {
      render_verbose=true;
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--channels") {
      render_channels=cmd->value(i).toUInt(&ok);
      if((!ok)||(render_channels>2)) {
	fprintf(stderr,"rdrender: invalid --channels argument\n");
	exit(1);
      }
      cmd->setProcessed(i,true);
    }
    /*
    if(cmd->key(i)=="--start-date") {
      render_start_date=QDate::fromString(cmd->value(i),Qt::ISODate);
      if(!render_start_date.isValid()) {
	fprintf(stderr,"rdrender: invalid --start-date\n");
	exit(1);
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--start-time") {
      render_start_time=QTime::fromString(cmd->value(i));
      if(!render_start_time.isValid()) {
	fprintf(stderr,"rdrender: invalid --start-time\n");
	exit(1);
      }
      cmd->setProcessed(i,true);
    }
    */
    if(!cmd->processed(i)) {
      fprintf(stderr,"rdrender: unrecognized option\n");
      exit(256);
    }
  }
  render_logname=cmd->key(cmd->keys()-2);
  render_output_filename=cmd->key(cmd->keys()-1);

  //
  // Read Configuration
  //
  render_config=new RDConfig();
  render_config->load();

  //
  // Open Database
  //
  QSqlDatabase *db=QSqlDatabase::addDatabase(render_config->mysqlDriver());
  if(!db) {
    fprintf(stderr,"rdrender: unable to initialize connection to database\n");
    exit(256);
  }
  db->setDatabaseName(render_config->mysqlDbname());
  db->setUserName(render_config->mysqlUsername());
  db->setPassword(render_config->mysqlPassword());
  db->setHostName(render_config->mysqlHostname());
  if(!db->open()) {
    fprintf(stderr,"rdimport: unable to connect to database\n");
    db->removeDatabase(render_config->mysqlDbname());
    exit(256);
  }
  new RDDbHeartbeat(render_config->mysqlHeartbeatInterval(),this);

  //
  // RIPC Connection
  //
  render_ripc=new RDRipc(render_config->stationName());
  connect(render_ripc,SIGNAL(userChanged()),this,SLOT(userData()));
  render_ripc->
    connectHost("localhost",RIPCD_TCP_PORT,render_config->password());

  //
  // System Configuration
  //
  render_system=new RDSystem();

  //
  // Station Configuration
  //
  render_station=new RDStation(render_config->stationName());

  //
  // User
  //
  render_user=NULL;
}


void MainObject::userData()
{
  //
  // Get User Context
  //
  disconnect(render_ripc,SIGNAL(userChanged()),this,SLOT(userData()));
  if(render_user!=NULL) {
    delete render_user;
  }
  render_user=new RDUser(render_ripc->user());

  exit(MainLoop());
}


uint64_t MainObject::FramesFromMsec(uint64_t msec)
{
  return msec*render_system->sampleRate()/1000;
}


void MainObject::Warning(const QTime &time,int line,const QString &msg) const
{
  fprintf(stderr,"%s\n",(const char *)(QString().sprintf("%04d : ",line)+msg));
}


void MainObject::Verbose(const QString &msg)
{
  if(render_verbose) {
    fprintf(stderr,"%s\n",(const char *)msg);
  }
}


void MainObject::Verbose(const QTime &time,int line,const QString &msg)
{
  if(render_verbose) {
    Warning(time,line,msg);
  }
}


bool MainObject::GetCutFile(const QString &cutname,int start_pt,int end_pt,
			    QString *dest_filename) const
{
  bool ret=false;
  RDAudioConvert::ErrorCode conv_err;
  RDAudioExport::ErrorCode export_err;
  char tempdir[PATH_MAX];
  
  strncpy(tempdir,RDTempDir()+"/rdrenderXXXXXX",PATH_MAX);
  *dest_filename=QString(mkdtemp(tempdir))+"/"+cutname+".wav";
  RDAudioExport *conv=new RDAudioExport(render_station,render_config);
  conv->setDestinationFile(*dest_filename);
  conv->setCartNumber(RDCut::cartNumber(cutname));
  conv->setCutNumber(RDCut::cutNumber(cutname));
  RDSettings s;
  s.setFormat(RDSettings::Pcm16);
  s.setSampleRate(render_system->sampleRate());
  s.setChannels(render_channels);
  s.setNormalizationLevel(0);
  conv->setDestinationSettings(&s);
  conv->setRange(start_pt,end_pt);
  conv->setEnableMetadata(false);
  switch(export_err=conv->runExport(render_user->name(),
				    render_user->password(),&conv_err)) {
  case RDAudioExport::ErrorOk:
    ret=true;
    break;

  default:
    ret=false;
    printf("export err %d [%s]\n",export_err,
	   (const char *)RDAudioExport::errorText(export_err,conv_err));
    break;
  }

  delete conv;
  return ret;
}


void MainObject::DeleteCutFile(const QString &dest_filename) const
{
  unlink(dest_filename);
  QStringList f0=f0.split("/",dest_filename);
  f0.erase(f0.fromLast());
  rmdir("/"+f0.join("/"));
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
