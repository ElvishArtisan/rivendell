// export.cpp
//
// Rivendell web service portal -- Export service
//
//   (C) Copyright 2010-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <rdapplication.h>
#include <rdaudioconvert.h>
#include <rdcart.h>
#include <rdconf.h>
#include <rdformpost.h>
#include <rdsettings.h>
#include <rdtempdirectory.h>
#include <rdweb.h>

#include "rdxport.h"

void Xport::Export()
{
  RDAudioConvert::ErrorCode conv_err=RDAudioConvert::ErrorOk;
  int resp_code=0;

  //
  // Verify Post
  //
  int cartnum=0;
  if(!xport_post->getValue("CART_NUMBER",&cartnum)) {
    XmlExit("Missing CART_NUMBER",400,"export.cpp",LINE_NUMBER);
  }
  int cutnum=0;
  if(!xport_post->getValue("CUT_NUMBER",&cutnum)) {
    XmlExit("Missing CUT_NUMBER",400,"export.cpp",LINE_NUMBER);
  }
  int format=0;
  if(!xport_post->getValue("FORMAT",&format)) {
    XmlExit("Missing FORMAT",400,"export.cpp",LINE_NUMBER);
  }
  int channels=0;
  if(!xport_post->getValue("CHANNELS",&channels)) {
    XmlExit("Missing CHANNELS",400,"export.cpp",LINE_NUMBER);
  }
  int sample_rate=0;
  if(!xport_post->getValue("SAMPLE_RATE",&sample_rate)) {
    XmlExit("Missing SAMPLE_RATE",400,"export.cpp",LINE_NUMBER);
  }
  int bit_rate=0;
  if(!xport_post->getValue("BIT_RATE",&bit_rate)) {
    XmlExit("Missing BIT_RATE",400,"export.cpp",LINE_NUMBER);
  }
  int quality=0;
  if(!xport_post->getValue("QUALITY",&quality)) {
    XmlExit("Missing QUALITY",400,"export.cpp",LINE_NUMBER);
  }
  int start_point=-1;
  if(!xport_post->getValue("START_POINT",&start_point)) {
    XmlExit("Missing START_POINT",400,"export.cpp",LINE_NUMBER);
  }
  int end_point=-1;
  if(!xport_post->getValue("END_POINT",&end_point)) {
    XmlExit("Missing END_POINT",400,"export.cpp",LINE_NUMBER);
  }
  int normalization_level=0;
  if(!xport_post->getValue("NORMALIZATION_LEVEL",&normalization_level)) {
    XmlExit("Missing NORMALIZATION_LEVEL",400,"export.cpp",LINE_NUMBER);
  }
  int enable_metadata=false;
  if(!xport_post->getValue("ENABLE_METADATA",&enable_metadata)) {
    XmlExit("Missing ENABLE_METADATA",400,"export.cpp",LINE_NUMBER);
  }
  if(!RDCart::exists(cartnum)) {
    XmlExit("No such cart",404,"export.cpp",LINE_NUMBER);
  }
  if(!RDCut::exists(cartnum,cutnum)) {
    XmlExit("No such cut",404,"export.cpp",LINE_NUMBER);
  }

  //
  // Verify User Perms
  //
  if(!rda->user()->cartAuthorized(cartnum)) {
    XmlExit("No such cart",404,"export.cpp",LINE_NUMBER);
  }

  //
  // Audio Settings
  //
  RDSettings *settings=new RDSettings();
  settings->setFormat((RDSettings::Format)format);
  settings->setChannels(channels);
  settings->setSampleRate(sample_rate);
  settings->setBitRate(bit_rate);
  settings->setQuality(quality);
  settings->setNormalizationLevel(normalization_level);

  //
  // Generate Metadata
  //
  RDWaveData *wavedata=NULL;
  QString rdxl;
  float speed_ratio=1.0;
  if(enable_metadata!=0) {
    wavedata=new RDWaveData();
  }
  if(wavedata!=NULL) {
    RDCart *cart=new RDCart(cartnum);
    RDCut *cut=new RDCut(cartnum,cutnum);
    cart->getMetadata(wavedata);
    cut->getMetadata(wavedata);
    if(cart->enforceLength()) {
      speed_ratio=(float)cut->length()/(float)cart->forcedLength();
    }
    rdxl=cart->xml(true,start_point<0,settings,cutnum);
    delete cut;
    delete cart;
  }

  //
  // Export Cut
  //
  int fd;
  ssize_t n;
  uint8_t data[2048];
  QString err_msg;
  RDTempDirectory *tempdir=new RDTempDirectory("rdxport-export");
  if(!tempdir->create(&err_msg)) {
    XmlExit("unable to create temporary directory ["+err_msg+"]",500);
  }
  QString tmpfile=tempdir->path()+"/exported_audio";
  RDAudioConvert *conv=new RDAudioConvert();
  conv->setSourceFile(RDCut::pathName(cartnum,cutnum));
  conv->setDestinationFile(tmpfile);
  conv->setDestinationSettings(settings);
  conv->setDestinationWaveData(wavedata);
  conv->setDestinationRdxl(rdxl);
  conv->setRange(start_point,end_point);
  conv->setSpeedRatio(speed_ratio);
  switch(conv_err=conv->convert()) {
  case RDAudioConvert::ErrorOk:
    switch(settings->format()) {
    case RDSettings::Pcm16:
    case RDSettings::Pcm24:
      printf("Content-type: audio/x-wav\n\n");
      break;

    case RDSettings::MpegL1:
    case RDSettings::MpegL2:
    case RDSettings::MpegL2Wav:
    case RDSettings::MpegL3:
      printf("Content-type: audio/x-mpeg\n\n");
      break;

    case RDSettings::OggVorbis:
      printf("Content-type: audio/ogg\n\n");
      break;

    case RDSettings::Flac:
      printf("Content-type: audio/flac\n\n");
      break;
    }
    fflush(NULL);
    if((fd=open(tmpfile.toUtf8(),O_RDONLY))>=0) {
      while((n=read(fd,data,2048))>0) {
	write(1,data,n);
      }
    }
    close(fd);
    unlink(tmpfile.toUtf8());
    //    rmdir(tmpdir);
    delete tempdir;
    Exit(0);
    break;

  case RDAudioConvert::ErrorFormatNotSupported:
  case RDAudioConvert::ErrorInvalidSettings:
    resp_code=415;
    break;

  case RDAudioConvert::ErrorNoSource:
    resp_code=404;
    break;

  case RDAudioConvert::ErrorNoDestination:
  case RDAudioConvert::ErrorInvalidSource:
  case RDAudioConvert::ErrorNoSpace:
  case RDAudioConvert::ErrorInternal:
  case RDAudioConvert::ErrorNoDisc:
  case RDAudioConvert::ErrorNoTrack:
  case RDAudioConvert::ErrorInvalidSpeed:
  case RDAudioConvert::ErrorFormatError:
    resp_code=500;
    break;
  }
  delete conv;
  delete settings;
  if(wavedata!=NULL) {
    delete wavedata;
  }
  delete tempdir;
  if(resp_code==200) {
    Exit(200);
  }
  else {
    XmlExit(RDAudioConvert::errorText(conv_err),resp_code,"export.cpp",
	    LINE_NUMBER,conv_err);
  }
}
