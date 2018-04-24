// audio_import_test.cpp
//
// Test Rivendell file importing.
//
//   (C) Copyright 2010,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdapplication.h>
#include <rddb.h>
#include <rdaudioimport.h>

#include <QCoreApplication>

#include <audio_import_test.h>

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  new RDApplication(RDApplication::Console,"audio_import_test",
		    AUDIO_IMPORT_TEST_USAGE);
  username="user";
  password="";
  destination_settings=new RDSettings();
  cart_number=0;
  cut_number=0;
  use_metadata=false;
  bool ok=false;
  RDAudioConvert::ErrorCode audio_conv_err;
  RDAudioImport::ErrorCode conv_err;

  //
  // Read Command Options
  //
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(rda->cmdSwitch()->key(i)=="--username") {
      username=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--password") {
      password=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--cart-number") {
      cart_number=rda->cmdSwitch()->value(i).toUInt(&ok);
      if((!ok)||(cart_number>999999)) {
	fprintf(stderr,"audio_import_test: invalid cart number\n");
	exit(256);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--cut-number") {
      cut_number=rda->cmdSwitch()->value(i).toUInt(&ok);
      if((!ok)||(cut_number>999)) {
	fprintf(stderr,"audio_import_test: invalid cut number\n");
	exit(256);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--source-file") {
      source_filename=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--destination-channels") {
      unsigned channels=rda->cmdSwitch()->value(i).toInt(&ok);
      if(!ok) {
	fprintf(stderr,"audio_import_test: invalid destination channels\n");
	exit(256);
      }
      destination_settings->setChannels(channels);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--normalization-level") {
      int normalization_level=rda->cmdSwitch()->value(i).toInt(&ok);
      if((!ok)||(normalization_level>0)) {
	fprintf(stderr,"audio_import_test: invalid normalization level\n");
	exit(256);
      }
      destination_settings->setNormalizationLevel(normalization_level);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--autotrim-level") {
      int autotrim_level=rda->cmdSwitch()->value(i).toInt(&ok);
      if((!ok)||(autotrim_level>0)) {
	fprintf(stderr,"audio_import_test: invalid autotrim level\n");
	exit(256);
      }
      destination_settings->setAutotrimLevel(autotrim_level);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--use-metadata") {
      use_metadata=true;
      rda->cmdSwitch()->setProcessed(i,true);
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

  RDAudioImport *conv=new RDAudioImport(rda->station(),rda->config(),this);
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
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
