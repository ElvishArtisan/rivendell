// rdrender.cpp
//
// Render a Rivendell log.
//
//   (C) Copyright 2017 Fred Gleason <fredg@paravelsystems.com>
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
#include <rdaudioconvert.h>
#include <rdaudioexport.h>
#include <rdaudioimport.h>
#include <rdcart.h>
#include <rdcmd_switch.h>
#include <rdconf.h>
#include <rdcut.h>
#include <rdescape_string.h>
#include <rddbheartbeat.h>
#include <rdlog.h>
#include <rdrenderer.h>
#include <rdsettings.h>

#include "rdrender.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  render_verbose=false;
  render_channels=RDRENDER_DEFAULT_CHANNELS;
  render_first_line=-1;
  render_last_line=-1;
  render_ignore_stops=false;
  render_cart_number=0;
  render_cut_number=-1;

  //
  // Initialize Audio Settings
  //
  render_settings.setChannels(RDRENDER_DEFAULT_CHANNELS);
  render_settings.setSampleRate(0);
  render_settings.setFormat(RDRENDER_DEFAULT_FORMAT);
  render_settings.setBitRate(RDRENDER_DEFAULT_BITRATE);
  render_settings.setQuality(RDRENDER_DEFAULT_BITRATE);
  render_settings.setNormalizationLevel(RDRENDER_DEFAULT_NORMALIZATION_LEVEL);

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"rdimport",RDRENDER_USAGE);
  if(cmd->keys()<1) {
    fprintf(stderr,
	    "rdrender: you must specify a logname\n");
    exit(256);
  }
  for(int i=0;i<(int)cmd->keys()-1;i++) {
    bool ok=false;
    if(cmd->key(i)=="--verbose") {
      render_verbose=true;
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--bitrate") {
      render_settings.setBitRate(cmd->value(i).toUInt(&ok));
      if(!ok) {
	fprintf(stderr,"rdrender: invalid --bitrate argument\n");
	exit(1);
      }
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
    if(cmd->key(i)=="--format") {
      QString format=cmd->value(i);
      ok=false;
      if(format.lower()=="flac") {
	render_settings.setFormat(RDSettings::Flac);
	ok=true;
      }
      if(format.lower()=="mp2") {
	render_settings.setFormat(RDSettings::MpegL2);
	ok=true;
      }
      if(format.lower()=="mp3") {
	render_settings.setFormat(RDSettings::MpegL3);
	ok=true;
      }
      if(format.lower()=="pcm16") {
	render_settings.setFormat(RDSettings::Pcm16);
	ok=true;
      }
      if(format.lower()=="pcm24") {
	render_settings.setFormat(RDSettings::Pcm24);
	ok=true;
      }
      if(format.lower()=="vorbis") {
	render_settings.setFormat(RDSettings::OggVorbis);
	ok=true;
      }
      if(!ok) {
	fprintf(stderr,"rdrender: unknown --format \"%s\"\n",
		(const char *)format);
	exit(1);
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--first-line") {
      render_first_line=cmd->value(i).toInt(&ok);
      if((!ok)|(render_first_line<0)) {
	fprintf(stderr,"rdrender: invalid --first-line argument\n");
	exit(1);
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--first-time") {
      render_first_time=QTime::fromString(cmd->value(i));
      if(!render_first_time.isValid()) {
	fprintf(stderr,"rdrender: invalid --first-time argument\n");
	exit(1);
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--ignore-stops") {
      render_ignore_stops=true;
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--last-time") {
      render_last_time=QTime::fromString(cmd->value(i));
      if(!render_last_time.isValid()) {
	fprintf(stderr,"rdrender: invalid --last-time argument\n");
	exit(1);
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--last-line") {
      render_last_line=cmd->value(i).toInt(&ok);
      if((!ok)||(render_last_line<0)) {
	fprintf(stderr,"rdrender: invalid --last-line argument\n");
	exit(1);
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--normalization-level") {
      render_settings.setNormalizationLevel(cmd->value(i).toInt(&ok));
      if(!ok) {
	fprintf(stderr,"rdrender: invalid --normalization-level argument\n");
	exit(1);
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--quality") {
      render_settings.setQuality(cmd->value(i).toUInt(&ok));
      if(!ok) {
	fprintf(stderr,"rdrender: invalid --quality argument\n");
	exit(1);
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--samplerate") {
      render_settings.setSampleRate(cmd->value(i).toUInt(&ok));
      if(!ok) {
	fprintf(stderr,"rdrender: invalid --samplerate argument\n");
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
    if(cmd->key(i)=="--to-cart") {
      QStringList f0=f0.split(":",cmd->value(i));
      if(f0.size()!=2) {
	fprintf(stderr,"rdrender: invalid --to-cart argument\n");
	exit(1);
      }
      render_cart_number=f0[0].toUInt(&ok);
      if((!ok)||(render_cart_number>RD_MAX_CART_NUMBER)) {
	fprintf(stderr,"rdrender: invalid cart number in --to-cart argument\n");
	exit(1);
      }
      render_cut_number=f0[1].toInt(&ok);
      if((!ok)||(render_cut_number>RD_MAX_CUT_NUMBER)||(render_cut_number<1)) {
	fprintf(stderr,"rdrender: invalid cut number in --to-cart argument\n");
	exit(1);
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--to-file") {
      render_to_file=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"rdrender: unrecognized option\n");
      exit(256);
    }
  }
  if((render_last_line>=0)&&(render_first_line>=0)&&
     (render_last_line<render_last_line)) {
    fprintf(stderr,"rdrender: --last-line must be greater than --first-line\n");
    exit(1);
  }
  if(!RDAudioConvert::settingsValid(&render_settings)) {
    fprintf(stderr,"rdrender: invalid audio settings\n");
    exit(1);
  }
  if(render_to_file.isEmpty()&&
     ((render_cart_number==0)||(render_cut_number==-1))) {
    fprintf(stderr,"rdrender: you must specify exactly one --to-cart or --to-file option\n");
    exit(1);
  }
  render_logname=cmd->key(cmd->keys()-1);
  if(render_start_time.isNull()) {
    render_start_time=QTime(0,0,0,1);
  }

  //
  // Read Configuration
  //
  render_config=new RDConfig();
  render_config->load();
  render_config->setModuleName("rdrender");

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
  if(render_settings.sampleRate()==0) {
    render_settings.setSampleRate(render_system->sampleRate());
  }

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

  //
  // Open Log
  //
  RDLog *log=new RDLog(render_logname);
  if(!log->exists()) {
    fprintf(stderr,"rdrender: no such log\n");
    exit(1);
  }
  RDLogEvent *log_event=new RDLogEvent(RDLog::tableName(render_logname));
  log_event->load();

  //
  // Render It
  //
  QString err_msg;
  RDRenderer *r=new RDRenderer(render_user,render_station,render_system,
			       render_config,this);
  connect(r,SIGNAL(progressMessageSent(const QString &)),
	  this,SLOT(printProgressMessage(const QString &)));
  if(render_to_file.isEmpty()) {
    if(!r->renderToCart(render_cart_number,render_cut_number,log_event,
			render_channels,&render_settings,render_start_time,
			render_ignore_stops,&err_msg,render_first_line,
			render_last_line,render_first_time,render_last_time)) {
      fprintf(stderr,"rdrender: %s\n",(const char *)err_msg);
      exit(1);
    }
  }
  else {
    if(!r->renderToFile(render_to_file,log_event,render_channels,
			&render_settings,render_start_time,render_ignore_stops,
			&err_msg,render_first_line,render_last_line,
			render_first_time,render_last_time)) {
      fprintf(stderr,"rdrender: %s\n",(const char *)err_msg);
      exit(1);
    }
  }
  QStringList warnings=r->warnings();
  for(unsigned i=0;i<warnings.size();i++) {
    fprintf(stderr,"%s: %s\n",(const char *)tr("WARNING"),
	    (const char *)warnings[i]);
  }

  exit(0);
}


void MainObject::printProgressMessage(const QString &msg)
{
  if(render_verbose) {
    fprintf(stderr,"%s\n",(const char *)msg);
  }
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
