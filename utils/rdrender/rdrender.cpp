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
  render_settings_modified=false;

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
      render_settings_modified=true;
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
	render_settings_modified=true;
	ok=true;
      }
      if(format.lower()=="mp2") {
	render_settings.setFormat(RDSettings::MpegL2);
	render_settings_modified=true;
	ok=true;
      }
      if(format.lower()=="mp3") {
	render_settings.setFormat(RDSettings::MpegL3);
	render_settings_modified=true;
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
	render_settings_modified=true;
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
      render_settings_modified=true;
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--quality") {
      render_settings.setQuality(cmd->value(i).toUInt(&ok));
      if(!ok) {
	fprintf(stderr,"rdrender: invalid --quality argument\n");
	exit(1);
      }
      render_settings_modified=true;
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--samplerate") {
      render_settings.setSampleRate(cmd->value(i).toUInt(&ok));
      if(!ok) {
	fprintf(stderr,"rdrender: invalid --samplerate argument\n");
	exit(1);
      }
      render_settings_modified=true;
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
  if((render_cart_number>0)&&(!RDCart::exists(render_cart_number))) {
    fprintf(stderr,"rdrender: no such cart\n");
    exit(1);
  }
  if((render_cut_number>0)&&
     (!RDCut::exists(render_cart_number,render_cut_number))) {
    fprintf(stderr,"rdrender: no such cut\n");
    exit(1);
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

  exit(MainLoop());
}


uint64_t MainObject::FramesFromMsec(uint64_t msec)
{
  return msec*render_system->sampleRate()/1000;
}


void MainObject::Verbose(const QString &msg)
{
  if(render_verbose) {
    fprintf(stderr,"%s\n",(const char *)msg);
  }
}


void MainObject::Verbose(const QTime &time,int line,const QString &trans,
			 const QString &msg)
{
  if(render_verbose) {
    fprintf(stderr,"%s\n",
	    (const char *)(QString().sprintf("%04d : ",line)+
			   time.toString("hh:mm:ss")+" : "+
			   QString().sprintf("%-5s",(const char *)trans)+msg));
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
  if(render_settings.format()==RDSettings::Pcm16) {
    s.setFormat(RDSettings::Pcm16);
  }
  else {
    s.setFormat(RDSettings::Pcm24);
  }
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


bool MainObject::ConvertAudio(const QString &srcfile,const QString &dstfile,
			      RDSettings *s,QString *err_msg)
{
  RDAudioConvert::ErrorCode err_code;

  RDAudioConvert *conv=new RDAudioConvert(render_station->name(),this);
  conv->setSourceFile(srcfile);
  conv->setDestinationFile(dstfile);
  conv->setDestinationSettings(s);
  err_code=conv->convert();
  *err_msg=RDAudioConvert::errorText(err_code);
  delete conv;

  return err_code==RDAudioConvert::ErrorOk;
}


bool MainObject::ImportCart(const QString &srcfile,unsigned cartnum,int cutnum,
			    QString *err_msg)
{
  RDAudioImport::ErrorCode err_import_code;
  RDAudioConvert::ErrorCode err_conv_code;
  RDSettings settings;
  
  settings.setNormalizationLevel(0);

  RDAudioImport *conv=new RDAudioImport(render_station,render_config,this);
  conv->setCartNumber(cartnum);
  conv->setCutNumber(cutnum);
  conv->setSourceFile(srcfile);
  conv->setUseMetadata(false);
  conv->setDestinationSettings(&settings);
  err_import_code=
    conv->runImport(render_user->name(),render_user->password(),&err_conv_code);
  *err_msg=RDAudioImport::errorText(err_import_code,err_conv_code);
  delete conv;
  return err_import_code==RDAudioImport::ErrorOk;
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
