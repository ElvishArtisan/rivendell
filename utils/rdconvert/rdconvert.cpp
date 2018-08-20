// rdconvert.cpp
//
// Rivendell file format converter.
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

#include <qapplication.h>

#include <rddb.h>
#include <rdcmd_switch.h>
#include <rdaudioconvert.h>

#include "rdconvert.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  unsigned schema=0;

  destination_settings=new RDSettings();
  start_point=-1;
  end_point=-1;
  speed_ratio=1.0;
  bool ok=false;
  RDAudioConvert::ErrorCode conv_err;

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"rdconvert",
  		    RDCONVERT_USAGE);
  if(cmd->keys()<1) {
    fprintf(stderr,"rdconvert: missing argument\n");
    exit(256);
  }
  source_filename=cmd->key(cmd->keys()-1);
  for(unsigned i=0;i<cmd->keys()-1;i++) {
    /*
    if(cmd->key(i)=="--source-file") {
      source_filename=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    */
    if(cmd->key(i)=="--destination-file") {
      destination_filename=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--start-point") {
      start_point=cmd->value(i).toInt(&ok);
      if(!ok) {
	fprintf(stderr,"rdconvert: invalid start point\n");
	exit(256);
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--end-point") {
      end_point=cmd->value(i).toInt(&ok);
      if(!ok) {
	fprintf(stderr,"rdconvert: invalid end point\n");
	exit(256);
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--destination-format") {
      RDSettings::Format format=(RDSettings::Format)cmd->value(i).toInt(&ok);
      if(!ok) {
	fprintf(stderr,"rdconvert: invalid destination format\n");
	exit(256);
      }
      switch(format) {
      case RDSettings::Pcm16:
      case RDSettings::Pcm24:
      case RDSettings::MpegL2:
      case RDSettings::MpegL2Wav:
      case RDSettings::MpegL3:
      case RDSettings::Flac:
      case RDSettings::OggVorbis:
	destination_settings->setFormat(format);
	cmd->setProcessed(i,true);
	break;

      default:
	fprintf(stderr,"rdconvert: invalid destination format\n");
	exit(256);
      }
      destination_settings->setFormat(format);
    }
    if(cmd->key(i)=="--destination-channels") {
      unsigned channels=cmd->value(i).toInt(&ok);
      if(!ok) {
	fprintf(stderr,"rdconvert: invalid destination channels\n");
	exit(256);
      }
      destination_settings->setChannels(channels);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--destination-sample-rate") {
      unsigned sample_rate=cmd->value(i).toInt(&ok);
      if(!ok) {
	fprintf(stderr,"rdconvert: invalid destination sample rate\n");
	exit(256);
      }
      destination_settings->setSampleRate(sample_rate);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--destination-bit-rate") {
      unsigned bit_rate=cmd->value(i).toInt(&ok);
      if(!ok) {
	fprintf(stderr,"rdconvert: invalid destination bit rate\n");
	exit(256);
      }
      destination_settings->setBitRate(bit_rate);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--quality") {
      unsigned quality=cmd->value(i).toInt(&ok);
      if(!ok) {
	fprintf(stderr,"rdconvert: invalid destination quality\n");
	exit(256);
      }
      destination_settings->setQuality(quality);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--normalization-level") {
      int normalization_level=cmd->value(i).toInt(&ok);
      if((!ok)||(normalization_level>0)) {
	fprintf(stderr,"rdconvert: invalid normalization level\n");
	exit(256);
      }
      destination_settings->setNormalizationLevel(normalization_level);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--speed-ratio") {
      speed_ratio=cmd->value(i).toFloat(&ok);
      if((!ok)||(speed_ratio<=0)) {
	fprintf(stderr,"rdconvert: invalid speed-ratio\n");
	exit(256);
      }
      cmd->setProcessed(i,true);
    }
  }
  if(source_filename.isEmpty()) {
    fprintf(stderr,"rdconvert: missing source-file\n");
    exit(256);
  }
  if(destination_filename.isEmpty()) {
    destination_filename=source_filename+"."+
      RDSettings::defaultExtension(destination_settings->format());
  }
  if((destination_settings->bitRate()!=0)&&
     (destination_settings->quality()!=0)) {
    fprintf(stderr,"rdconvert: --destination-bit-rate and --destination-quality are mutually exclusive\n");
    exit(256);
  }

  //
  // Read Configuration
  //
  rdconfig=new RDConfig();
  rdconfig->load();
  rdconfig->setModuleName("rdconvert");

  //
  // Open Database
  //
  QString err (tr("rdconvert: "));
  QSqlDatabase *db=RDInitDb(&schema,&err);
  if(!db) {
    fprintf(stderr,err.ascii());
    delete cmd;
    exit(256);
  }

  RDAudioConvert *conv=new RDAudioConvert(rdconfig,this);
  conv->setSourceFile(source_filename);
  conv->setDestinationFile(destination_filename);
  conv->setDestinationSettings(destination_settings);
  conv->setRange(start_point,end_point);
  conv->setSpeedRatio(speed_ratio);
  conv_err=conv->convert();
  if(conv_err!=RDAudioConvert::ErrorOk) {
    fprintf(stderr,"%s\n",(const char *)RDAudioConvert::errorText(conv_err));
    exit(256);
  }

  exit(0);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
