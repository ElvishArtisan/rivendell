// audio_export_test.cpp
//
// Test the Rivendell file format exporter.
//
//   (C) Copyright 2010-2023 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdapplication.h>
#include <rddb.h>
#include <rdaudioexport.h>

#include "audio_export_test.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  username="user";
  password="";
  destination_settings=new RDSettings();
  cart_number=0;
  cut_number=0;
  start_point=-1;
  end_point=-1;
  bool ok=false;
  RDAudioConvert::ErrorCode audio_conv_err;
  RDAudioExport::ErrorCode conv_err;
  QString err_msg;

  //
  // Open the Database
  //
  rda=static_cast<RDApplication *>(new RDCoreApplication("audio_export_test",
		       "audio_export_test",AUDIO_EXPORT_TEST_USAGE,false,this));
  if(!rda->open(&err_msg,NULL,true,false)) {
    fprintf(stderr,"audio_export_test: %s\n",err_msg.toUtf8().constData());
    exit(1);
  }

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
	fprintf(stderr,"audio_export_test: invalid cart number\n");
	exit(256);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--cut-number") {
      cut_number=rda->cmdSwitch()->value(i).toUInt(&ok);
      if((!ok)||(cut_number>999)) {
	fprintf(stderr,"audio_export_test: invalid cut number\n");
	exit(256);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--destination-file") {
      destination_filename=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--start-point") {
      start_point=rda->cmdSwitch()->value(i).toInt(&ok);
      if(!ok) {
	fprintf(stderr,"audio_export_test: invalid start point\n");
	exit(256);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--end-point") {
      end_point=rda->cmdSwitch()->value(i).toInt(&ok);
      if(!ok) {
	fprintf(stderr,"audio_export_test: invalid end point\n");
	exit(256);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--destination-format") {
      RDSettings::Format format=(RDSettings::Format)rda->cmdSwitch()->value(i).toInt(&ok);
      if(!ok) {
	fprintf(stderr,"audio_export_test: invalid destination format\n");
	exit(256);
      }
      switch(format) {
      case RDSettings::Pcm16:
      case RDSettings::MpegL2:
      case RDSettings::MpegL2Wav:
      case RDSettings::MpegL3:
      case RDSettings::Flac:
      case RDSettings::OggVorbis:
	destination_settings->setFormat(format);
	rda->cmdSwitch()->setProcessed(i,true);
	break;

      default:
	fprintf(stderr,"audio_export_test: invalid destination format\n");
	exit(256);
      }
      destination_settings->setFormat(format);
    }
    if(rda->cmdSwitch()->key(i)=="--destination-channels") {
      unsigned channels=rda->cmdSwitch()->value(i).toInt(&ok);
      if(!ok) {
	fprintf(stderr,"audio_export_test: invalid destination channels\n");
	exit(256);
      }
      destination_settings->setChannels(channels);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--destination-sample-rate") {
      unsigned sample_rate=rda->cmdSwitch()->value(i).toInt(&ok);
      if(!ok) {
	fprintf(stderr,"audio_export_test: invalid destination sample rate\n");
	exit(256);
      }
      destination_settings->setSampleRate(sample_rate);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--destination-bit-rate") {
      unsigned bit_rate=rda->cmdSwitch()->value(i).toInt(&ok);
      if(!ok) {
	fprintf(stderr,"audio_export_test: invalid destination bit rate\n");
	exit(256);
      }
      destination_settings->setBitRate(bit_rate);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--quality") {
      unsigned quality=rda->cmdSwitch()->value(i).toInt(&ok);
      if(!ok) {
	fprintf(stderr,"audio_export_test: invalid destination quality\n");
	exit(256);
      }
      destination_settings->setQuality(quality);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--normalization-level") {
      int normalization_level=rda->cmdSwitch()->value(i).toInt(&ok);
      if((!ok)||(normalization_level>0)) {
	fprintf(stderr,"audio_export_test: invalid normalization level\n");
	exit(256);
      }
      destination_settings->setNormalizationLevel(normalization_level);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(!rda->cmdSwitch()->processed(i)) {
      fprintf(stderr,"audio_export_test: unknown command option \"%s\"\n",
	      rda->cmdSwitch()->key(i).toUtf8().constData());
      exit(2);
    }
  }

  //
  // Sanity Checks
  //
  if(cart_number==0) {
    fprintf(stderr,"audio_export_test: missing cart-number\n");
    exit(256);
  }
  if(cut_number==0) {
    fprintf(stderr,"audio_export_test: missing cut-number\n");
    exit(256);
  }
  if(destination_filename.isEmpty()) {
    fprintf(stderr,"audio_export_test: missing destination-file\n");
    exit(256);
  }
  if((destination_settings->bitRate()!=0)&&
     (destination_settings->quality()!=0)) {
    fprintf(stderr,"audio_export_test: --destination-bit-rate and --destination-quality are mutually exclusive\n");
    exit(256);
  }

  RDAudioExport *conv=new RDAudioExport(this);
  conv->setCartNumber(cart_number);
  conv->setCutNumber(cut_number);
  conv->setDestinationFile(destination_filename);
  conv->setDestinationSettings(destination_settings);
  conv->setRange(start_point,end_point);
  printf("Exporting...\n");
  conv_err=conv->runExport(username,password,&audio_conv_err);
  printf("Result: %s\n",
	 RDAudioExport::errorText(conv_err,audio_conv_err).toUtf8().
	 constData());
  delete conv;

  exit(0);
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
