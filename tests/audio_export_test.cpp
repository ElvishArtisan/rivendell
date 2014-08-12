// audio_export_test.cpp
//
// Test the Rivendell file format exporter.
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: audio_export_test.cpp,v 1.5.4.1 2013/11/13 23:36:38 cvs Exp $
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
#include <rdaudioexport.h>
#include <rdstation.h>

#include <audio_export_test.h>

MainObject::MainObject(QObject *parent,const char *name)
  :QObject(parent,name)
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
  unsigned schema=0;

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"audio_export_test",
  		    AUDIO_EXPORT_TEST_USAGE);
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
	fprintf(stderr,"audio_export_test: invalid cart number\n");
	exit(256);
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--cut-number") {
      cut_number=cmd->value(i).toUInt(&ok);
      if((!ok)||(cut_number>999)) {
	fprintf(stderr,"audio_export_test: invalid cut number\n");
	exit(256);
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--destination-file") {
      destination_filename=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--start-point") {
      start_point=cmd->value(i).toInt(&ok);
      if(!ok) {
	fprintf(stderr,"audio_export_test: invalid start point\n");
	exit(256);
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--end-point") {
      end_point=cmd->value(i).toInt(&ok);
      if(!ok) {
	fprintf(stderr,"audio_export_test: invalid end point\n");
	exit(256);
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--destination-format") {
      RDSettings::Format format=(RDSettings::Format)cmd->value(i).toInt(&ok);
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
	cmd->setProcessed(i,true);
	break;

      default:
	fprintf(stderr,"audio_export_test: invalid destination format\n");
	exit(256);
      }
      destination_settings->setFormat(format);
    }
    if(cmd->key(i)=="--destination-channels") {
      unsigned channels=cmd->value(i).toInt(&ok);
      if(!ok) {
	fprintf(stderr,"audio_export_test: invalid destination channels\n");
	exit(256);
      }
      destination_settings->setChannels(channels);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--destination-sample-rate") {
      unsigned sample_rate=cmd->value(i).toInt(&ok);
      if(!ok) {
	fprintf(stderr,"audio_export_test: invalid destination sample rate\n");
	exit(256);
      }
      destination_settings->setSampleRate(sample_rate);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--destination-bit-rate") {
      unsigned bit_rate=cmd->value(i).toInt(&ok);
      if(!ok) {
	fprintf(stderr,"audio_export_test: invalid destination bit rate\n");
	exit(256);
      }
      destination_settings->setBitRate(bit_rate);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--quality") {
      unsigned quality=cmd->value(i).toInt(&ok);
      if(!ok) {
	fprintf(stderr,"audio_export_test: invalid destination quality\n");
	exit(256);
      }
      destination_settings->setQuality(quality);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--normalization-level") {
      int normalization_level=cmd->value(i).toInt(&ok);
      if((!ok)||(normalization_level>0)) {
	fprintf(stderr,"audio_export_test: invalid normalization level\n");
	exit(256);
      }
      destination_settings->setNormalizationLevel(normalization_level);
      cmd->setProcessed(i,true);
    }
  }
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

  //
  // Read Configuration
  //
  rdconfig=new RDConfig();
  rdconfig->load();

  //
  // Open Database
  //
  QString err (tr("audio_export_test: "));
  QSqlDatabase *db=RDInitDb(&schema,&err);
  if(!db) {
    fprintf(stderr,err.ascii());
    delete cmd;
    exit(256);
  }

  RDStation *station=new RDStation(rdconfig->stationName());
  RDAudioExport *conv=new RDAudioExport(station,rdconfig,this);
  conv->setCartNumber(cart_number);
  conv->setCutNumber(cut_number);
  conv->setDestinationFile(destination_filename);
  conv->setDestinationSettings(destination_settings);
  conv->setRange(start_point,end_point);
  printf("Exporting...\n");
  conv_err=conv->runExport(username,password,&audio_conv_err);
  printf("Result: %s\n",
	 (const char *)RDAudioExport::errorText(conv_err,audio_conv_err));
  delete conv;

  exit(0);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject(NULL,"main");
  return a.exec();
}
