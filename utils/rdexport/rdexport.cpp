// rdexport.cpp
//
// A Batch Exporter for Rivendell.
//
//   (C) Copyright 2016-2018 Fred Gleason <fredg@paravelsystems.com>
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
#include <rdapplication.h>
#include <rdaudioconvert.h>
#include <rdaudioexport.h>
#include <rdaudioinfo.h>
#include <rdcart.h>
#include <rdescape_string.h>
#include <rdgroup.h>
#include <rdschedcode.h>

#include "rdexport.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  QString err_msg;

  export_metadata_pattern="%n_%j";
  export_escape_string="_";
  export_continue_after_error=false;
  export_allow_clobber=false;
  export_samplerate=0;
  export_bitrate=0;
  export_channels=0;
  export_quality=3;
  export_xml=false;
  export_verbose=false;

  //
  // Open the Database
  //
  rda=new RDApplication("rdexport","rdexport",RDEXPORT_USAGE,this);
  if(!rda->open(&err_msg)) {
    fprintf(stderr,"rdexport: %s\n",(const char *)err_msg.toUtf8());
    exit(1);
  }

  //
  // Read Command Options
  //
  if(rda->cmdSwitch()->keys()<1) {
    fprintf(stderr,"rdexport: you must specify an output directory\n");
    exit(2);
  }
  for(unsigned i=0;i<rda->cmdSwitch()->keys()-1;i++) {
    if(rda->cmdSwitch()->key(i)=="--allow-clobber") {
      export_allow_clobber=true;
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--bitrate") {
      bool ok=false;
      export_bitrate=rda->cmdSwitch()->value(i).toUInt(&ok);
      if(!ok) {
	fprintf(stderr,"rdexport: invalid bitrate\n");
	exit(256);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--carts") {
      bool ok=false;
      bool valid=false;
      QStringList f0=rda->cmdSwitch()->value(i).split(":");
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
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--channels") {
      bool ok=false;
      export_channels=rda->cmdSwitch()->value(i).toUInt(&ok);
      if((export_channels<1)||(export_channels>2)||(!ok)) {
	fprintf(stderr,"rdexport: invalid --channels argument\n");
	exit(256);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--continue-after-error") {
      export_continue_after_error=true;
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--escape-string") {
      if(rda->cmdSwitch()->value(i)!=SanitizePath(rda->cmdSwitch()->value(i))) {
	fprintf(stderr,"rdxport: illegal character(s) in escape string\n");
	exit(256);
      }
      export_escape_string=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--format") {
      export_format=rda->cmdSwitch()->value(i);
      bool ok=false;
      if(export_format.lower()=="flac") {
	export_set_format=RDSettings::Flac;
	ok=true;
      }
      if(export_format.lower()=="mp2") {
	export_set_format=RDSettings::MpegL2;
	ok=true;
      }
      if(export_format.lower()=="mp3") {
	export_set_format=RDSettings::MpegL3;
	ok=true;
      }
      if(export_format.lower()=="pcm16") {
	export_set_format=RDSettings::Pcm16;
	ok=true;
      }
      if(export_format.lower()=="pcm24") {
	export_set_format=RDSettings::Pcm24;
	ok=true;
      }
      if(export_format.lower()=="vorbis") {
	export_set_format=RDSettings::OggVorbis;
	ok=true;
      }
      if(!ok) {
	fprintf(stderr,"rdexport: unknown format \"%s\"\n",
		(const char *)export_format.toUtf8());
	exit(256);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--group") {
      export_groups.push_back(rda->cmdSwitch()->value(i));
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--scheduler-code") {
      export_schedcodes.push_back(rda->cmdSwitch()->value(i));
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--metadata-pattern") {
      export_metadata_pattern=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--quality") {
      bool ok=false;
      export_quality=rda->cmdSwitch()->value(i).toInt(&ok);
      if((export_quality<-1)||(export_quality>10)||(!ok)) {
	fprintf(stderr,"rdexport: invalid --quality value\n");
	exit(256);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--samplerate") {
      bool ok=false;
      export_samplerate=rda->cmdSwitch()->value(i).toUInt(&ok);
      if(!ok) {
	fprintf(stderr,"rdexport: invalid samplerate\n");
	exit(256);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--title") {
      export_titles.push_back(rda->cmdSwitch()->value(i));
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--verbose") {
      export_verbose=true;
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--xml") {
      export_xml=true;
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(!rda->cmdSwitch()->processed(i)) {
      fprintf(stderr,"rdrepld: unknown command option \"%s\"\n",
	      (const char *)rda->cmdSwitch()->key(i).toUtf8());
      exit(2);
    }
  }
  export_output_to=rda->cmdSwitch()->key(rda->cmdSwitch()->keys()-1);

  QDir dir;
  if(!dir.exists(export_output_to)) {
    fprintf(stderr,"rdexport: no such output directory\n");
    exit(256);
  }

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
    fprintf(stderr,"rdexport: %s\n",(const char *)str.toUtf8());
    exit(256);
  }

  //
  // Validate Scheduler Code List
  //
  std::vector<QString> bad_schedcodes;
  for(unsigned i=0;i<export_schedcodes.size();i++) {
    RDSchedCode *sch=new RDSchedCode(export_schedcodes[i]);
    if(!sch->exists()) {
      bad_schedcodes.push_back(export_schedcodes[i]);
    }
    delete sch;
  }
  if(bad_schedcodes.size()>0) {
    QString str="no such scheduler code(s): ";
    for(unsigned i=0;i<bad_schedcodes.size();i++) {
      str+=bad_schedcodes[i]+", ";
    }
    str=str.left(str.length()-2);
    fprintf(stderr,"rdexport: %s\n",(const char *)str.toUtf8());
    exit(256);
  }

  //
  // RIPC Connection
  //
  connect(rda,SIGNAL(userChanged()),this,SLOT(userData()));
  rda->ripc()->
    connectHost("localhost",RIPCD_TCP_PORT,rda->config()->password());
}


void MainObject::userData()
{
  //
  // Get User Context
  //
  disconnect(rda->ripc(),SIGNAL(userChanged()),this,SLOT(userData()));

  //
  // Verify Permissions
  //
  if(!rda->user()->editAudio()) {
    fprintf(stderr,"rdexport: user \"%s\" has no edit audio permission\n",
	    (const char *)rda->user()->name().toUtf8());
    exit(256);
  }

  //
  // Process Titles
  //
  for(unsigned i=0;i<export_titles.size();i++) {
    Verbose("Processing title \""+export_titles[i]+"\"...");
    ExportTitle(export_titles[i]);
  }

  //
  // Process Groups
  //
  for(unsigned i=0;i<export_groups.size();i++) {
    Verbose("Processing group \""+export_groups[i]+"\"...");
    ExportGroup(export_groups[i]);
  }

  //
  // Process Scheduler Codes
  //
  for(unsigned i=0;i<export_schedcodes.size();i++) {
    Verbose("Processing scheduler code \""+export_schedcodes[i]+"\"...");
    ExportSchedCode(export_schedcodes[i]);
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


void MainObject::ExportTitle(const QString &title)
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("select NUMBER from CART where ")+
    "(TITLE=\""+RDEscapeString(title)+"\")&&"+
    QString().sprintf("(TYPE=%u) ",RDCart::Audio)+
    "order by NUMBER";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    ExportCart(q->value(0).toUInt());
  }
  delete q;
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


void MainObject::ExportSchedCode(const QString &schedcode)
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("select CART_NUMBER from CART_SCHED_CODES where ")+
    "SCHED_CODE=\""+RDEscapeString(schedcode)+"\" "+
    "order by CART_NUMBER";
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
  RDAudioExport *conv=new RDAudioExport(this);
  RDAudioExport::ErrorCode export_err;
  RDAudioConvert::ErrorCode conv_err;
  RDAudioInfo::ErrorCode info_err;

  //
  // Get Audio Parameters
  //
  RDAudioInfo *info=new RDAudioInfo(this);
  info->setCartNumber(cart->number());
  info->setCutNumber(RDCut::cutNumber(cut->cutName()));
  if((info_err=info->runInfo(rda->user()->name(),rda->user()->password()))!=
     RDAudioInfo::ErrorOk) {
    fprintf(stderr,"rdexport: error getting cut info [%s]\n",
	    (const char *)RDAudioInfo::errorText(info_err).toUtf8());
    if(export_continue_after_error) {
      return;
    }
    else {
      exit(256);
    }
  }
  RDSettings settings;
  if(export_format.isEmpty()) {
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
      if(export_continue_after_error) {
	return;
      }
      else {
	exit(256);
      }
    }
  }
  else {
    settings.setFormat(export_set_format);
  }
  if(export_channels==0) {
    settings.setChannels(info->channels());
  }
  else {
    settings.setChannels(export_channels);
  }
  if(export_samplerate==0) {
    settings.setSampleRate(info->sampleRate());
  }
  else {
    settings.setSampleRate(export_samplerate);
  }
  if(export_bitrate==0) {
    if(info->bitRate()==0) {
      settings.setBitRate(256000);
    }
    else {
      settings.setBitRate(info->bitRate());
    }
  }
  else {
    settings.setBitRate(export_bitrate);
  }
  settings.setQuality(export_quality);
  Verbose(QString("exporting cart/cut ")+
	  QString().sprintf("%06u/%03d",RDCut::cartNumber(cut->cutName()),
		    RDCut::cutNumber(cut->cutName()))+" ["+cart->title()+"]");
  conv->setCartNumber(cart->number());
  conv->setCutNumber(RDCut::cutNumber(cut->cutName()));
  conv->setDestinationSettings(&settings);
  conv->setDestinationFile(ResolveOutputName(cart,cut,
			   RDSettings::defaultExtension(settings.format())));
  conv->setEnableMetadata(true);

  if((export_err=conv->runExport(rda->user()->name(),rda->user()->password(),
				 &conv_err))==RDAudioExport::ErrorOk) {
    QStringList f0=conv->destinationFile().split("/");
    printf("%s\n",(const char *)f0.at(f0.size()-1).toUtf8());
    if(export_xml) {
      FILE *f=NULL;
      f0=conv->destinationFile().split(".",QString::KeepEmptyParts);
      QString filename;
      for(int i=0;i<f0.size()-1;i++) {
	filename+=f0[i]+".";
      }
      filename+="xml";
      if((f=fopen(filename,"w"))!=NULL) {
	fprintf(f,"%s\n",
		(const char *)cart->xml(true,true,&settings,cut->cutNumber()).
		toUtf8());
	fclose(f);
      }
    }
  }
  else {
    fprintf(stderr,"rdexport: exporter error for output file \"%s\" [%s]\n",
	    (const char *)conv->destinationFile().toUtf8(),
	    (const char *)RDAudioExport::errorText(export_err,conv_err).
	    toUtf8());
    if(!export_continue_after_error) {
      exit(256);
    }
  }

  delete conv;
  delete info;
}


QString MainObject::ResolveOutputName(RDCart *cart,RDCut *cut,
				      const QString &exten)
{
  QString name=
    RDLogLine::resolveWildcards(cart->number(),export_metadata_pattern,
				cut->cutNumber());
  QString ret=SanitizePath(name);
  if(!export_allow_clobber) {
    int count=1;
    while(QFile::exists(export_output_to+"/"+ret+"."+exten)) {
      ret=name+QString().sprintf("[%d]",count++);
    }
  }

  return export_output_to+"/"+ret+"."+exten;
}


QString MainObject::SanitizePath(const QString &pathname) const
{
  //
  // Remove illegal characters from the filepath.
  // (from https://msdn.microsoft.com/en-us/library/windows/desktop/aa365247%28v=vs.85%29.aspx#naming_conventions
  //
  QString ret=pathname;

  ret.replace("/",export_escape_string);
  ret.replace(":",export_escape_string);
  ret.replace("<",export_escape_string);
  ret.replace(">",export_escape_string);
  ret.replace("\"",export_escape_string);
  ret.replace("\\",export_escape_string);
  ret.replace("|",export_escape_string);
  ret.replace("?",export_escape_string);
  ret.replace("*",export_escape_string);

  return ret;
}


void MainObject::Verbose(const QString &msg)
{
  if(export_verbose) {
    fprintf(stderr,"%s\n",(const char *)msg.toUtf8());
  }
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
