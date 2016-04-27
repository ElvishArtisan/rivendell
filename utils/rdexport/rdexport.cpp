// rdexport.cpp
//
// A Batch Exporter for Rivendell.
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
#include <qfile.h>

#include <rd.h>
#include <rdaudioconvert.h>
#include <rdaudioexport.h>
#include <rdaudioinfo.h>
#include <rdcart.h>
#include <rdcmd_switch.h>
#include <rdescape_string.h>
#include <rdgroup.h>
#include <rddbheartbeat.h>

#include "rdexport.h"

MainObject::MainObject(QObject *parent,const char *name)
  :QObject(parent,name)
{
  export_metadata_pattern="%n_%j";

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"rdimport",RDEXPORT_USAGE);
  if(cmd->keys()<1) {
    fprintf(stderr,"rdexport: you must specify an output directory\n");
    exit(256);
  }
  for(int i=0;i<(int)cmd->keys()-1;i++) {
    if(cmd->key(i)=="--carts") {
      bool ok=false;
      bool valid=false;
      QStringList f0=f0.split(":",cmd->value(i));
      if(f0.size()==2) {
	int start=f0[0].toUInt(&valid);
	if(valid&&(start>0)&&(valid<=RD_MAX_CART_NUMBER)) {
	  int end=f0[1].toUInt(&valid);
	  if(valid&&(start>0)&&(valid<=RD_MAX_CART_NUMBER)&&(end>=start)) {
	    export_start_carts.push_back(start);
	    export_end_carts.push_back(end);
	    ok=true;
	  }
	}
      }
      if(!ok) {
	fprintf(stderr,"rdexport: invalid --carts argument\n");
	exit(256);
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--group") {
      export_groups.push_back(cmd->value(i));
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--metadata-pattern") {
      export_metadata_pattern=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--verbose") {
      export_verbose=true;
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"rdexport: unrecognized option\n");
      exit(256);
    }
  }
  export_output_to=cmd->key(cmd->keys()-1);

  //
  // Read Configuration
  //
  export_config=new RDConfig();
  export_config->load();

  //
  // Open Database
  //
  QSqlDatabase *db=QSqlDatabase::addDatabase(export_config->mysqlDriver());
  if(!db) {
    fprintf(stderr,"rdexport: unable to initialize connection to database\n");
    exit(256);
  }
  db->setDatabaseName(export_config->mysqlDbname());
  db->setUserName(export_config->mysqlUsername());
  db->setPassword(export_config->mysqlPassword());
  db->setHostName(export_config->mysqlHostname());
  if(!db->open()) {
    fprintf(stderr,"rdimport: unable to connect to database\n");
    db->removeDatabase(export_config->mysqlDbname());
    exit(256);
  }
  new RDDbHeartbeat(export_config->mysqlHeartbeatInterval(),this);

  //
  // Validate Group List
  //
  std::vector<QString> bad_groups;
  for(unsigned i=0;i<export_groups.size();i++) {
    RDGroup *grp=new RDGroup(export_groups[i]);
    if(!grp->exists()) {
      bad_groups.push_back(export_groups[i]);
    }
    delete grp;
  }
  if(bad_groups.size()>0) {
    QString str="no such group(s): ";
    for(unsigned i=0;i<bad_groups.size();i++) {
      str+=bad_groups[i]+", ";
    }
    str=str.left(str.length()-2);
    fprintf(stderr,"rdexport: %s\n",(const char *)str);
    exit(256);
  }

  //
  // RIPC Connection
  //
  export_ripc=new RDRipc(export_config->stationName());
  connect(export_ripc,SIGNAL(userChanged()),this,SLOT(userData()));
  export_ripc->
    connectHost("localhost",RIPCD_TCP_PORT,export_config->password());

  //
  // Station Configuration
  //
  export_station=new RDStation(export_config->stationName());

  //
  // User
  //
  export_user=NULL;
}


void MainObject::userData()
{
  //
  // Get User Context
  //
  disconnect(export_ripc,SIGNAL(userChanged()),this,SLOT(userData()));
  if(export_user!=NULL) {
    delete export_user;
  }
  export_user=new RDUser(export_ripc->user());

  //
  // Verify Permissions
  //
  if(!export_user->editAudio()) {
    fprintf(stderr,"rdexport: user \"%s\" has no edit audio permission\n",
	    (const char *)export_user->name());
    exit(256);
  }

  //
  // Process Groups
  //
  for(unsigned i=0;i<export_groups.size();i++) {
    Verbose("Processing group \""+export_groups[i]+"\"...");
    ExportGroup(export_groups[i]);
  }

  //
  // Process Cart Ranges
  //
  for(unsigned i=0;i<export_start_carts.size();i++) {
    for(unsigned j=export_start_carts[i];j<=export_end_carts[i];j++) {
      ExportCart(j);
    }
  }

  //
  // Clean Up and Exit
  //
  exit(0);
}


void MainObject::ExportGroup(const QString &groupname)
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("select NUMBER from CART where ")+
    "(GROUP_NAME=\""+RDEscapeString(groupname)+"\")&&"+
    QString().sprintf("(TYPE=%u) ",RDCart::Audio)+
    "order by NUMBER";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    ExportCart(q->value(0).toUInt());
  }
  delete q;
}


void MainObject::ExportCart(unsigned cartnum)
{
  RDCart *cart=new RDCart(cartnum);
  QString sql;
  RDSqlQuery *q;

  if(cart->exists()&&(cart->type()==RDCart::Audio)) {
    sql=QString().sprintf("select CUT_NAME from CUTS where CART_NUMBER=%u",
			  cartnum);
    q=new RDSqlQuery(sql);
    while(q->next()) {
      RDCut *cut=new RDCut(q->value(0).toString());
      ExportCut(cart,cut);
    }
    delete q;
  }
  delete cart;
}


void MainObject::ExportCut(RDCart *cart,RDCut *cut)
{
  RDAudioExport *conv=new RDAudioExport(export_station,export_config,this);
  RDAudioExport::ErrorCode export_err;
  RDAudioConvert::ErrorCode conv_err;
  RDAudioInfo::ErrorCode info_err;

  //
  // Get Audio Parameters
  //
  RDAudioInfo *info=new RDAudioInfo(export_station,export_config);
  info->setCartNumber(cart->number());
  info->setCutNumber(RDCut::cutNumber(cut->cutName()));
  if((info_err=info->runInfo(export_user->name(),export_user->password()))!=
     RDAudioInfo::ErrorOk) {
    fprintf(stderr,"rdexport: error getting cut info [%s]\n",
	    (const char *)RDAudioInfo::errorText(info_err));
    exit(256);
  }
  RDSettings settings;
  switch(info->format()) {
  case RDWaveFile::Pcm16:
    settings.setFormat(RDSettings::Pcm16);
    break;

  case RDWaveFile::Pcm24:
    settings.setFormat(RDSettings::Pcm24);
    break;

  case RDWaveFile::MpegL2:
    settings.setFormat(RDSettings::MpegL2);
    break;

  default:
    fprintf(stderr,"rdexport: unsupported source audio format\n");
    exit(256);
  }
  settings.setChannels(info->channels());
  settings.setSampleRate(info->sampleRate());
  settings.setBitRate(info->bitRate());

  Verbose(QString("exporting cart/cut ")+
	  QString().sprintf("%06u/%03d",RDCut::cartNumber(cut->cutName()),
		    RDCut::cutNumber(cut->cutName()))+" ["+cart->title()+"]");
  conv->setCartNumber(cart->number());
  conv->setCutNumber(RDCut::cutNumber(cut->cutName()));
  conv->setDestinationSettings(&settings);
  conv->setDestinationFile(ResolveOutputName(cart,cut,
      RDSettings::defaultExtension(export_station->name(),settings.format())));
  conv->setEnableMetadata(true);

  if((export_err=conv->runExport(export_user->name(),export_user->password(),
				 &conv_err))!=RDAudioExport::ErrorOk) {
    fprintf(stderr,"rdexport: exporter error [%s]\n",
	    (const char *)RDAudioExport::errorText(export_err,conv_err));
    exit(256);
  }

  delete conv;
  delete info;
}


QString MainObject::ResolveOutputName(RDCart *cart,RDCut *cut,
				      const QString &exten)
{
  QString name=export_metadata_pattern;

  name.replace("%a",cart->artist());
  name.replace("%b",cart->label());
  name.replace("%c",cart->client());
  name.replace("%e",cart->agency());
  name.replace("%g",cart->groupName());
  name.replace("%h",QString().sprintf("%d",cut->length()));
  name.replace("%i",cut->description());
  name.replace("%j",QString().sprintf("%03d",RDCut::cutNumber(cut->cutName())));
  name.replace("%l",cart->album());
  name.replace("%m",cart->composer());
  name.replace("%n",QString().sprintf("%06u",cart->number()));
  name.replace("%o",cut->outcue());
  name.replace("%p",cart->publisher());
  name.replace("%r",cart->conductor());
  name.replace("%s",cart->songId());
  name.replace("%t",cart->title());
  name.replace("%u",cart->userDefined());
  name.replace("%y",QString().sprintf("%d",cart->year()));

  QString ret=name;
  int count=1;
  while(QFile::exists(export_output_to+"/"+ret+"."+exten)) {
    ret=name+QString().sprintf("[%d]",count++);
  }

  return export_output_to+"/"+ret+"."+exten;
}


void MainObject::Verbose(const QString &msg)
{
  if(export_verbose) {
    fprintf(stderr,"%s\n",(const char *)msg);
  }
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject(NULL);
  return a.exec();
}
