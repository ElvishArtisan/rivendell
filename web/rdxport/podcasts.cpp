// podcasts.cpp
//
// Rivendell web service portal -- Podcast services
//
//   (C) Copyright 2010-2020 Fred Gleason <fredg@paravelsystems.com>
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
#include <errno.h>

#include <rdapplication.h>
#include <rdconf.h>
#include <rdescape_string.h>
#include <rdformpost.h>
#include <rdgroup.h>
#include <rdpodcast.h>
#include <rduser.h>
#include <rdweb.h>

#include "rdxport.h"

void Xport::SavePodcast()
{
  int cast_id=0;
  QString keyname;
  QString destpath;
  QString err_msg;
  RDPodcast *cast=NULL;
  RDFeed *feed=NULL;
  QString filename;
  QString msg="OK";

  if(!xport_post->getValue("ID",&cast_id)) {
    XmlExit("Missing ID",400,"podcasts.cpp",LINE_NUMBER);
  }

  if(!xport_post->getValue("FILENAME",&filename)) {
    XmlExit("Missing FILENAME",400,"podcasts.cpp",LINE_NUMBER);
  }
  if(!xport_post->isFile("FILENAME")) {
    XmlExit("Missing file data",400,"podcasts.cpp",LINE_NUMBER);
  }

  cast=new RDPodcast(rda->config(),cast_id);
  if(!cast->exists()) {
    delete cast;
    XmlExit("No such podcast",404,"podcasts.cpp",LINE_NUMBER);
  }
  keyname=cast->keyName();
  if((!rda->user()->addPodcast())||(!rda->user()->feedAuthorized(keyname))) {
    delete cast;
    XmlExit("No such podcast",404,"podcasts.cpp",LINE_NUMBER);
  }
  feed=new RDFeed(keyname,rda->config(),this);
  destpath=QString(RD_AUDIO_ROOT)+"/"+cast->audioFilename();

  if(!RDCopy(filename,destpath)) {
    delete feed;
    delete cast;
    XmlExit("Internal server error [copy failed]",500,"podcasts.cpp",
	    LINE_NUMBER);
  }
  if(chmod(destpath.toUtf8(),S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP)!=0) {
    err_msg=QString().sprintf("Internal server error [%s]",strerror(errno));
    unlink(destpath.toUtf8());
    delete feed;
    delete cast;
    XmlExit(err_msg.toUtf8(),500,"podcasts.cpp",LINE_NUMBER);
  }

  printf("Content-type: text/html; charset: UTF-8\n");
  printf("Status: 200\n\n");
  printf("OK\n");

  rda->syslog(LOG_DEBUG,"saved podcast \"%s\"",destpath.toUtf8().constData());

  delete feed;
  delete cast;

  Exit(0);
}


void Xport::GetPodcast()
{
  int cast_id=0;
  QString keyname;
  QString destpath;
  QString err_msg;
  RDPodcast *cast=NULL;
  QString msg="OK";
  int fd=-1;
  struct stat st;
  ssize_t n=0;
  char *data=NULL;

  if(!xport_post->getValue("ID",&cast_id)) {
    XmlExit("Missing ID",400,"podcasts.cpp",LINE_NUMBER);
  }

  cast=new RDPodcast(rda->config(),cast_id);
  if(!cast->exists()) {
    delete cast;
    XmlExit("No such podcast",404,"podcasts.cpp",LINE_NUMBER);
  }
  keyname=cast->keyName();
  if((!rda->user()->editPodcast())||(!rda->user()->feedAuthorized(keyname))) {
    delete cast;
    XmlExit("No such podcast",404,"podcasts.cpp",LINE_NUMBER);
  }
  destpath=QString(RD_AUDIO_ROOT)+"/"+cast->audioFilename();

  if((fd=open(destpath.toUtf8(),O_RDONLY))<0) {
    err_msg=QString().sprintf("Internal server error [%s]",strerror(errno));
    delete cast;
    XmlExit(err_msg.toUtf8(),500,"podcasts.cpp",LINE_NUMBER);
  }
  memset(&st,0,sizeof(st));
  if(fstat(fd,&st)!=0) {
    err_msg=QString().sprintf("Internal server error [%s]",strerror(errno));
    delete cast;
    XmlExit(err_msg.toUtf8(),500,"podcasts.cpp",LINE_NUMBER);
  }

  printf("Content-type: audio/x-mpeg\n");
  printf("Content-length: %ld\n",st.st_size);
  printf("\n");
  fflush(stdout);
  data=new char[st.st_blksize];
  n=read(fd,data,st.st_blksize);
  while(n>0) {
    write(1,data,n);
    n=read(fd,data,st.st_blksize);
  }
  delete data;
  close(fd);

  rda->syslog(LOG_DEBUG,"served podcast \"%s\"",destpath.toUtf8().constData());

  delete cast;

  Exit(0);
}


void Xport::DeletePodcast()
{
  int cast_id=0;
  QString keyname;
  QString destpath;
  QString err_msg;
  RDPodcast *cast=NULL;
  QString msg="OK";

  if(!xport_post->getValue("ID",&cast_id)) {
    XmlExit("Missing ID",400,"podcasts.cpp",LINE_NUMBER);
  }

  cast=new RDPodcast(rda->config(),cast_id);
  if(!cast->exists()) {
    delete cast;
    XmlExit("No such podcast",404,"podcasts.cpp",LINE_NUMBER);
  }
  keyname=cast->keyName();
  if((!rda->user()->deletePodcast())||(!rda->user()->feedAuthorized(keyname))) {
    delete cast;
    XmlExit("No such podcast",404,"podcasts.cpp",LINE_NUMBER);
  }
  destpath=QString(RD_AUDIO_ROOT)+"/"+cast->audioFilename();

  if(unlink(destpath.toUtf8())!=0) {
    if(errno!=ENOENT) {
      err_msg=QString().sprintf("Internal server error [%s]",strerror(errno));
      delete cast;
      XmlExit(err_msg.toUtf8(),500,"podcasts.cpp",LINE_NUMBER);
    }
  }

  printf("Content-type: text/html; charset: UTF-8\n");
  printf("Status: 200\n\n");
  printf("OK\n");

  rda->syslog(LOG_DEBUG,"deleted podcast \"%s\"",destpath.toUtf8().constData());

  delete cast;

  Exit(0);
}
