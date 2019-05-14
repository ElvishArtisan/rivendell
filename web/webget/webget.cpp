// webget.cpp
//
// Rivendell download utility
//
//   (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <openssl/sha.h>

#include <qapplication.h>

#include <rdapplication.h>
#include <rdescape_string.h>
#include <rdweb.h>

#include "webget.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  QString err_msg;

  //
  // Open the Database
  //
  rda=new RDApplication("webget.cgi","webget.cgi",WEBGET_CGI_USAGE,this);
  if(!rda->open(&err_msg)) {
    XmlExit(err_msg,500,"webget.cpp",LINE_NUMBER);
  }

  //
  // Read Command Options
  //
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(!rda->cmdSwitch()->processed(i)) {
      XmlExit("unknown command option",500,"webget.cpp",LINE_NUMBER);
    }
  }

  //
  // Drop root permissions
  //
  if(setgid(rda->config()->gid())<0) {
    XmlExit("Unable to set Rivendell group",500,"webget.cpp",LINE_NUMBER);
  }
  if(setuid(rda->config()->uid())<0) {
    XmlExit("Unable to set Rivendell user",500,"webget.cpp",LINE_NUMBER);
  }
  if(getuid()==0) {
    XmlExit("Rivendell user should never be \"root\"!",500,"webget.cpp",
	    LINE_NUMBER);
  }

  //
  // Determine Connection Type
  //
  if(getenv("REQUEST_METHOD")==NULL) {
    XmlExit("missing REQUEST_METHOD",500,"webget.cpp",LINE_NUMBER);
  }
  if(QString(getenv("REQUEST_METHOD")).lower()!="post") {
    XmlExit("invalid web method",500,"webget.cpp",LINE_NUMBER);
  }
  if(getenv("REMOTE_ADDR")!=NULL) {
    webget_remote_address.setAddress(getenv("REMOTE_ADDR"));
  }
  if(getenv("REMOTE_HOST")!=NULL) {
    webget_remote_hostname=getenv("REMOTE_HOST");
  }
  if(webget_remote_hostname.isEmpty()) {
    webget_remote_hostname=webget_remote_address.toString();
  }

  //
  // Generate Post
  //
  webget_post=new RDFormPost(RDFormPost::AutoEncoded,false);
  if(webget_post->error()!=RDFormPost::ErrorOk) {
    XmlExit(webget_post->errorString(webget_post->error()),400,"webget.cpp",
	    LINE_NUMBER);
    Exit(0);
  }

  //
  // Authenticate Connection
  //
  if(!Authenticate()) {
    printf("Content-type: text/html\n");
    printf("Status: 400\n");
    printf("\n");
    printf("Invalid User name or Password!\n");
    Exit(0);
  }

  //
  // Connect to ripcd(8)
  //
  connect(rda->ripc(),SIGNAL(connected(bool)),
	  this,SLOT(ripcConnectedData(bool)));
  rda->ripc()->
    connectHost("localhost",RIPCD_TCP_PORT,rda->config()->password());
}


void MainObject::ripcConnectedData(bool state)
{
  if(!state) {
    XmlExit("unable to connect to ripc service",500,"webget.cpp",LINE_NUMBER);
    Exit(0);
  }

  QString title;
  if(!webget_post->getValue("title",&title)) {
    XmlExit("missing \"title\"",400,"webget.cpp",LINE_NUMBER);
  }

  int channels;
  if(!webget_post->getValue("channels",&channels)) {
    XmlExit("missing \"channels\"",400,"webget.cpp",LINE_NUMBER);
  }

  int samprate;
  if(!webget_post->getValue("samprate",&samprate)) {
    XmlExit("missing \"samprate\"",400,"webget.cpp",LINE_NUMBER);
  }

  int format;
  if(!webget_post->getValue("format",&format)) {
    XmlExit("missing \"format\"",400,"webget.cpp",LINE_NUMBER);
  }

  int bitrate=0;
  if((format==RDSettings::MpegL2)||(format==RDSettings::MpegL3)) {
    if(!webget_post->getValue("bitrate",&bitrate)) {
      XmlExit("missing \"bitrate\"",400,"webget.cpp",LINE_NUMBER);
    }
  }

  int quality=0;
  if(format==RDSettings::OggVorbis) {
    if(!webget_post->getValue("quality",&quality)) {
      XmlExit("missing \"quality\"",400,"webget.cpp",LINE_NUMBER);
    }
  }

  unsigned cartnum=0;
  int cutnum=0;
  QString sql=QString("select ")+
    "CUTS.CUT_NAME from "+
    "CART left join CUTS on CART.NUMBER=CUTS.CART_NUMBER where "+
    "CART.TITLE=\""+RDEscapeString(title)+"\" && "+
    QString().sprintf("CART.TYPE=%d",RDCart::Audio);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    cartnum=RDCut::cartNumber(q->value(0).toString());
    cutnum=RDCut::cutNumber(q->value(0).toString());
  }
  delete q;
  if(cartnum==0) {
    printf("Content-type: text/html\n");
    printf("Status: 400\n");
    printf("\n");
    printf("No such cart!\n");
    Exit(0);
  }

  //
  // Audio Settings
  //
  RDSettings *settings=new RDSettings();
  settings->setFormat((RDSettings::Format)format);
  settings->setChannels(channels);
  settings->setSampleRate(samprate);
  settings->setBitRate(bitrate);
  settings->setQuality(quality);
  settings->setNormalizationLevel(-1);

  //
  // Generate Metadata
  //
  RDCart *cart=new RDCart(cartnum);
  RDCut *cut=new RDCut(cartnum,cutnum);
  RDWaveData *wavedata=NULL;
  QString rdxl;
  float speed_ratio=1.0;
  wavedata=new RDWaveData();
  if(wavedata!=NULL) {
    cart->getMetadata(wavedata);
    cut->getMetadata(wavedata);
    if(cart->enforceLength()) {
      speed_ratio=(float)cut->length()/(float)cart->forcedLength();
    }
    rdxl=cart->xml(true,false,settings,cutnum);
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
  RDAudioConvert *conv=new RDAudioConvert(this);
  conv->setSourceFile(RDCut::pathName(cartnum,cutnum));
  conv->setDestinationFile(tmpfile);
  conv->setDestinationSettings(settings);
  conv->setDestinationWaveData(wavedata);
  conv->setDestinationRdxl(rdxl);
  conv->setRange(cut->startPoint(),cut->endPoint());
  conv->setSpeedRatio(speed_ratio);
  delete cut;
  delete cart;
  RDAudioConvert::ErrorCode conv_err;
  int resp_code=0;
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
    if((fd=open(tmpfile,O_RDONLY))>=0) {
      while((n=read(fd,data,2048))>0) {
	write(1,data,n);
      }
    }
    close(fd);
    unlink(tmpfile);
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


bool MainObject::Authenticate()
{
  QString name;
  QString passwd;

  if(!webget_post->getValue("LOGIN_NAME",&name)) {
    return false;
  }
  if(!webget_post->getValue("PASSWORD",&passwd)) {
    return false;
  }
  RDUser *user=new RDUser(name);
  if((!user->exists())||
     (!user->checkPassword(passwd,false))||
     (!user->webgetLogin())) {
    return false;
  }

  return true;
}


void MainObject::Exit(int code)
{
  if(webget_post!=NULL) {
    delete webget_post;
  }
  exit(code);
}


void MainObject::XmlExit(const QString &str,int code,const QString &srcfile,
			 int srcline,RDAudioConvert::ErrorCode err)
{
  if(webget_post!=NULL) {
    delete webget_post;
  }
#ifdef RDXPORT_DEBUG
  if(srcline>0) {
    RDXMLResult(str+" \""+srcfile+"\" "+QString().sprintf("line %d",srcline),
		code,err);
  }
  else {
    RDXMLResult(str,code,err);
  }
#else
  RDXMLResult(str,code,err);
#endif  // RDXPORT_DEBUG
  exit(0);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
