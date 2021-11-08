// import.cpp
//
// Rivendell web service portal -- Import service
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <rdapplication.h>
#include <rdaudioconvert.h>
#include <rdcart.h>
#include <rdconf.h>
#include <rdformpost.h>
#include <rdgroup.h>
#include <rdhash.h>
#include <rdlibrary_conf.h>
#include <rdsettings.h>
#include <rdweb.h>

#include "rdxport.h"

void Xport::Import()
{
  unsigned length_deviation=0;
  unsigned msecs=0;
  int resp_code=0;
  QString remote_host;
  QString err_msg;

  if(getenv("REMOTE_HOST")==NULL) {
    if(getenv("REMOTE_ADDR")==NULL) {
      XmlExit("Internal server error",500,"import.cpp",LINE_NUMBER);
    }
    else {
      remote_host=getenv("REMOTE_ADDR");
    }
  }
  else {
    remote_host=getenv("REMOTE_HOST");
  }

  //
  // Verify Post
  //
  int cartnum=0;
  if(!xport_post->getValue("CART_NUMBER",&cartnum)) {
    XmlExit("Missing CART_NUMBER",400,"import.cpp",LINE_NUMBER);
  }
  int cutnum=0;
  if(!xport_post->getValue("CUT_NUMBER",&cutnum)) {
    XmlExit("Missing CUT_NUMBER",400,"import.cpp",LINE_NUMBER);
  }
  int channels=0;
  if(!xport_post->getValue("CHANNELS",&channels)) {
    XmlExit("Missing CHANNELS",400,"import.cpp",LINE_NUMBER);
  }
  int normalization_level=0;
  if(!xport_post->getValue("NORMALIZATION_LEVEL",&normalization_level)) {
    XmlExit("Missing NORMALIZATION_LEVEL",400,"import.cpp",LINE_NUMBER);
  }
  int autotrim_level=0;
  if(!xport_post->getValue("AUTOTRIM_LEVEL",&autotrim_level)) {
    XmlExit("Missing AUTOTRIM_LEVEL",400,"import.cpp",LINE_NUMBER);
  }
  int use_metadata=0;
  if(!xport_post->getValue("USE_METADATA",&use_metadata)) {
    XmlExit("Missing USE_METADATA",400,"import.cpp",LINE_NUMBER);
  }
  int create=0;
  if(!xport_post->getValue("CREATE",&create)) {
    create=0;
  }
  QString group_name;
  xport_post->getValue("GROUP_NAME",&group_name);
  QString title;
  xport_post->getValue("TITLE",&title);
  QString filename;
  if(!xport_post->getValue("FILENAME",&filename)) {
    XmlExit("Missing FILENAME",400,"import.cpp",LINE_NUMBER);
  }
  if(!xport_post->isFile("FILENAME")) {
    XmlExit("Missing file data",400,"import.cpp",LINE_NUMBER);
  }

  //
  // Verify User Perms
  //
  if(RDCart::exists(cartnum)) {
    if(!rda->user()->cartAuthorized(cartnum)) {
      XmlExit("No such cart",404,"import.cpp",LINE_NUMBER);
    }
  }
  else {
    if(create) {
      if(!rda->user()->groupAuthorized(group_name)) {
	XmlExit("No such group",404,"import.cpp",LINE_NUMBER);
      }
    }
    else {
      XmlExit("No such cart",404,"import.cpp",LINE_NUMBER);
    }
  }
  if(!rda->user()->editAudio()) {
    XmlExit("Forbidden",404,"import.cpp",LINE_NUMBER);
  }
  if(create&&(!rda->user()->createCarts())) {
    XmlExit("Forbidden",404,"import.cpp",LINE_NUMBER);
  }

  //
  // Verify Title Uniqueness
  //
  if(!title.isEmpty()) {
    if((!rda->system()->allowDuplicateCartTitles())&&
       (!rda->system()->fixDuplicateCartTitles())&&
       (!RDCart::titleIsUnique(cartnum,title))) {
      XmlExit("Duplicate Cart Title Not Allowed",404,"import.cpp",LINE_NUMBER);
    }
  }

  //
  // Load Configuration
  //
  RDCart *cart=NULL;
  RDCut *cut=NULL;
  if(cartnum==0) {
    RDGroup *group=new RDGroup(group_name);
    if(!group->exists()) {
      XmlExit("No such group",404,"import.cpp",LINE_NUMBER);
    }
    if((cartnum=group->nextFreeCart())==0) {
      XmlExit("No available carts for specified group",404,"import.cpp",LINE_NUMBER);
    }
    cart=new RDCart(cartnum);
    if(RDCart::create(group_name,RDCart::Audio,&err_msg,cartnum)==0) {
      delete cart;
      XmlExit("Unable to create cart ["+err_msg+"]",500,"import.cpp",
	      LINE_NUMBER);
    }
    SendNotification(RDNotification::CartType,RDNotification::AddAction,
		     QVariant(cartnum));
    cutnum=1;
    cut=new RDCut(cartnum,cutnum,true);
    delete group;
  }
  else {
    cart=new RDCart(cartnum);
    cut=new RDCut(cartnum,cutnum);
  }
  if(!RDCart::exists(cartnum)) {
    XmlExit("No such cart",404,"import.cpp",LINE_NUMBER);
  }
  if(!RDCut::exists(cartnum,cutnum)) {
    XmlExit("No such cut",404,"import.cpp",LINE_NUMBER);
  }
  RDLibraryConf *conf=new RDLibraryConf(rda->config()->stationName());
  RDSettings *settings=new RDSettings();
  switch(conf->defaultFormat()) {
  case 0:
    settings->setFormat(RDSettings::Pcm16);
    break;

  case 1:
    settings->setFormat(RDSettings::MpegL2Wav);
    break;

  case 2:
    settings->setFormat(RDSettings::Pcm24);
    break;
  }
  settings->setChannels(channels);
  settings->setSampleRate(rda->system()->sampleRate());
  settings->setBitRate(channels*conf->defaultBitrate());
  settings->setNormalizationLevel(normalization_level);
  RDWaveData wavedata;
  RDWaveFile *wave=new RDWaveFile(filename);
  if(!wave->openWave(&wavedata)) {
    delete wave;
    XmlExit("Format Not Supported",415,"import.cpp",LINE_NUMBER);
  }
  delete wave;
  if(use_metadata) {
    if((!rda->system()->allowDuplicateCartTitles())&&
       (!rda->system()->fixDuplicateCartTitles())&&
       (!RDCart::titleIsUnique(cartnum,wavedata.title()))) {
      XmlExit("Duplicate Cart Title Not Allowed",404,"import.cpp",LINE_NUMBER);
    }
  }
  RDAudioConvert *conv=new RDAudioConvert();
  conv->setSourceFile(filename);
  conv->setDestinationFile(RDCut::pathName(cartnum,cutnum));
  conv->setDestinationSettings(settings);
  RDAudioConvert::ErrorCode conv_err=conv->convert();
  switch(conv_err) {
  case RDAudioConvert::ErrorOk:
    wave=new RDWaveFile(RDCut::pathName(cartnum,cutnum));
    if(wave->openWave()) {
      msecs=wave->getExtTimeLength();
    }
    else {
      delete wave;
      XmlExit("Unable to access imported file",500,"import.cpp",LINE_NUMBER);
    }
    delete wave;
    cut->checkInRecording(rda->config()->stationName(),rda->user()->name(),
			  remote_host,settings,msecs);
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
  if(resp_code==200) {
    cut->setSha1Hash(RDSha1HashFile(RDCut::pathName(cut->cutName())));
    if(!title.isEmpty()) {
      cart->setTitle(title);
    }
    printf("Content-type: application/xml; charset=utf-8\n");
    printf("Status: %d\n",resp_code);
    printf("\n");
    printf("<RDWebResult>\r\n");
    printf("  <ResponseCode>%d</ResponseCode>\r\n",resp_code);
    printf("  <ErrorString>OK</ErrorString>\r\n");
    printf("  <CartNumber>%d</CartNumber>\r\n",cartnum);
    printf("  <CutNumber>%d</CutNumber>\r\n",cutnum);
    printf("</RDWebResult>\r\n");
    SendNotification(RDNotification::CartType,RDNotification::ModifyAction,
		     QVariant(cartnum));
    unlink(filename.toUtf8());
    rmdir(xport_post->tempDir().toUtf8());
    exit(0);
  }
  XmlExit(RDAudioConvert::errorText(conv_err),resp_code,"import.cpp",
	  LINE_NUMBER,conv_err);
}
