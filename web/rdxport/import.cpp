// import.cpp
//
// Rivendell web service portal -- Import service
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: import.cpp,v 1.12.2.2 2014/01/15 19:56:32 cvs Exp $
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <rdformpost.h>
#include <rdweb.h>
#include <rdcart.h>
#include <rdaudioconvert.h>
#include <rdsettings.h>
#include <rdconf.h>
#include <rdlibrary_conf.h>

#include <rdxport.h>

void Xport::Import()
{
  unsigned length_deviation=0;
  unsigned msecs;
  int resp_code=0;

  //
  // Verify Post
  //
  int cartnum=0;
  if(!xport_post->getValue("CART_NUMBER",&cartnum)) {
    XmlExit("Missing CART_NUMBER",400);
  }
  int cutnum=0;
  if(!xport_post->getValue("CUT_NUMBER",&cutnum)) {
    XmlExit("Missing CUT_NUMBER",400);
  }
  int channels=0;
  if(!xport_post->getValue("CHANNELS",&channels)) {
    XmlExit("Missing CHANNELS",400);
  }
  int normalization_level=0;
  if(!xport_post->getValue("NORMALIZATION_LEVEL",&normalization_level)) {
    XmlExit("Missing NORMALIZATION_LEVEL",400);
  }
  int autotrim_level=0;
  if(!xport_post->getValue("AUTOTRIM_LEVEL",&autotrim_level)) {
    XmlExit("Missing AUTOTRIM_LEVEL",400);
  }
  int use_metadata=0;
  if(!xport_post->getValue("USE_METADATA",&use_metadata)) {
    XmlExit("Missing USE_METADATA",400);
  }
  QString filename;
  if(!xport_post->getValue("FILENAME",&filename)) {
    XmlExit("Missing FILENAME",400);
  }
  if(!RDCart::exists(cartnum)) {
    XmlExit("No such cart",404);
  }
  if(!RDCut::exists(cartnum,cutnum)) {
    XmlExit("No such cut",404);
  }
  if(!xport_post->isFile("FILENAME")) {
    XmlExit("Missing file data",400);
  }

  //
  // Verify User Perms
  //
  if(!xport_user->cartAuthorized(cartnum)) {
    XmlExit("No such cart",404);
  }
  if(!xport_user->editAudio()) {
    XmlExit("Unauthorized",401);
  }

  //
  // Load Configuration
  //
  RDCart *cart=new RDCart(cartnum);
  RDCut *cut=new RDCut(cartnum,cutnum);
  RDLibraryConf *conf=new RDLibraryConf(xport_config->stationName(),0);
  RDSettings *settings=new RDSettings();
  switch(conf->defaultFormat()) {
  case 0:
    settings->setFormat(RDSettings::Pcm16);
    break;

  case 1:
    settings->setFormat(RDSettings::MpegL2Wav);
    break;
  }
  settings->setChannels(channels);
  settings->setSampleRate(xport_system->sampleRate());
  settings->setBitRate(channels*conf->defaultBitrate());
  settings->setNormalizationLevel(normalization_level);
  RDWaveFile *wave=new RDWaveFile(filename);
  if(!wave->openWave()) {
    delete wave;
    XmlExit("Format Not Supported",415);
  }
  msecs=wave->getExtTimeLength();
  delete wave;
  RDAudioConvert *conv=new RDAudioConvert(xport_config->stationName());
  conv->setSourceFile(filename);
  conv->setDestinationFile(RDCut::pathName(cartnum,cutnum));
  conv->setDestinationSettings(settings);
  RDAudioConvert::ErrorCode conv_err=conv->convert();
  switch(conv_err) {
  case RDAudioConvert::ErrorOk:
    cut->checkInRecording(xport_config->stationName(),settings,msecs);
    if(use_metadata>0) {
      cart->setMetadata(conv->sourceWaveData());
      cut->setMetadata(conv->sourceWaveData());
    }
    if(autotrim_level!=0) {
      cut->autoTrim(RDCut::AudioBoth,100*autotrim_level);
    }
    cart->updateLength();
    cart->resetRotation();
    cart->calculateAverageLength(&length_deviation);
    cart->setLengthDeviation(length_deviation);
    resp_code=200;
    break;

  case RDAudioConvert::ErrorFormatNotSupported:
  case RDAudioConvert::ErrorInvalidSettings:
    resp_code=415;
    break;

  case RDAudioConvert::ErrorNoSource:
  case RDAudioConvert::ErrorNoDestination:
  case RDAudioConvert::ErrorInvalidSource:
  case RDAudioConvert::ErrorInternal:
  case RDAudioConvert::ErrorNoSpace:
  case RDAudioConvert::ErrorNoDisc:
  case RDAudioConvert::ErrorNoTrack:
  case RDAudioConvert::ErrorInvalidSpeed:
    resp_code=500;
    break;

  case RDAudioConvert::ErrorFormatError:
    resp_code=400;
    break;
  }
  delete conv;
  delete settings;
  delete conf;
  delete cut;
  delete cart;
  XmlExit(RDAudioConvert::errorText(conv_err),resp_code,conv_err);
}
