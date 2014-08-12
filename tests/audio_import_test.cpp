// audio_import_test.cpp
//
// Test Rivendell file importing.
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: audio_import_test.cpp,v 1.5.4.1 2013/11/13 23:36:38 cvs Exp $
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

#include <qapplication.h>

#include <rddb.h>
#include <rdcmd_switch.h>
#include <rdaudioimport.h>

#include <audio_import_test.h>

MainObject::MainObject(QObject *parent,const char *name)
  :QObject(parent,name)
{
  username="user";
  password="";
  destination_settings=new RDSettings();
  cart_number=0;
  cut_number=0;
  use_metadata=false;
  bool ok=false;
  RDAudioConvert::ErrorCode audio_conv_err;
  RDAudioImport::ErrorCode conv_err;
  unsigned schema=0;

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"audio_import_test",
  		    AUDIO_IMPORT_TEST_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--username") {
      username=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--password") {
      password=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--cart-number") {
      cart_number=cmd->value(i).toUInt(&ok);
      if((!ok)||(cart_number>999999)) {
	fprintf(stderr,"audio_import_test: invalid cart number\n");
	exit(256);
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--cut-number") {
      cut_number=cmd->value(i).toUInt(&ok);
      if((!ok)||(cut_number>999)) {
	fprintf(stderr,"audio_import_test: invalid cut number\n");
	exit(256);
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--source-file") {
      source_filename=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--destination-channels") {
      unsigned channels=cmd->value(i).toInt(&ok);
      if(!ok) {
	fprintf(stderr,"audio_import_test: invalid destination channels\n");
	exit(256);
      }
      destination_settings->setChannels(channels);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--normalization-level") {
      int normalization_level=cmd->value(i).toInt(&ok);
      if((!ok)||(normalization_level>0)) {
	fprintf(stderr,"audio_import_test: invalid normalization level\n");
	exit(256);
      }
      destination_settings->setNormalizationLevel(normalization_level);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--autotrim-level") {
      int autotrim_level=cmd->value(i).toInt(&ok);
      if((!ok)||(autotrim_level>0)) {
	fprintf(stderr,"audio_import_test: invalid autotrim level\n");
	exit(256);
      }
      destination_settings->setAutotrimLevel(autotrim_level);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--use-metadata") {
      use_metadata=true;
      cmd->setProcessed(i,true);
    }
  }
  if(cart_number==0) {
    fprintf(stderr,"audio_import_test: missing cart-number\n");
    exit(256);
  }
  if(cut_number==0) {
    fprintf(stderr,"audio_import_test: missing cut-number\n");
    exit(256);
  }
  if(source_filename.isEmpty()) {
    fprintf(stderr,"audio_import_test: missing source-file\n");
    exit(256);
  }

  //
  // Read Configuration
  //
  rdconfig=new RDConfig();
  rdconfig->load();

  //
  // Open Database
  //
  QString err (tr("audio_import_test: "));
  QSqlDatabase *db=RDInitDb(&schema,&err);
  if(!db) {
    fprintf(stderr,err.ascii());
    delete cmd;
    exit(256);
  }

  RDStation *station=new RDStation(rdconfig->stationName());
  RDAudioImport *conv=new RDAudioImport(station,rdconfig,this);
  conv->setCartNumber(cart_number);
  conv->setCutNumber(cut_number);
  conv->setSourceFile(source_filename);
  conv->setDestinationSettings(destination_settings);
  conv->setUseMetadata(use_metadata);
  printf("Importing...\n");
  conv_err=conv->runImport(username,password,&audio_conv_err);
  printf("Result: %s\n",
	 (const char *)RDAudioImport::errorText(conv_err,audio_conv_err));
  delete conv;

  exit(0);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject(NULL,"main");
  return a.exec();
}
