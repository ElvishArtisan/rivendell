// metalibrary.cpp
//
// Abstract a library of metadata.
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: metalibrary.cpp,v 1.1.2.3 2013/12/04 22:22:49 cvs Exp $
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
#include <stdlib.h>

#include <rdconf.h>

#include <metalibrary.h>

MetaLibrary::MetaLibrary()
{
}


MetaLibrary::~MetaLibrary()
{
  for(unsigned i=0;i<meta_tracks.size();i++) {
    delete meta_tracks[i];
  }
}


unsigned MetaLibrary::totalTracks()
{
  return meta_tracks.size();
}


unsigned MetaLibrary::tracks(const QString &disc_id)
{
  unsigned ret=0;

  for(unsigned i=0;i<meta_tracks.size();i++) {
    if(meta_tracks[i]->discId()==disc_id) {
      ret++;
    }
  }
  return ret;
}


MetaRecord *MetaLibrary::track(const QString &disc_id,int track_num)
{
  for(unsigned i=0;i<meta_tracks.size();i++) {
    if((meta_tracks[i]->discId()==disc_id)&&
       (meta_tracks[i]->trackNumber()==track_num)){
      return meta_tracks[i];
    }
  }

  return NULL;
}


int MetaLibrary::load(const QString &filename)
{
  FILE *f=NULL;
  char line[32768];
  QStringList f0;

  //
  // Open Source File
  //
  if((f=fopen(filename,"r"))==NULL) {
    return -1;
  }

  //
  // Load Headers
  //
  if(fgets(line,32768,f)==NULL) {
    fclose(f);
    return -1;
  }
  meta_headers=Split(",",QString(line).lower());

  //
  // Load Lines
  //
  while(fgets(line,1024,f)!=NULL) {
    f0=Split(",",line);
    if(f0.size()==meta_headers.size()) {
      LoadLine(f0);
    }
  }

  fclose(f);

  return meta_tracks.size();
}


void MetaLibrary::clear()
{
  for(unsigned i=0;i<meta_tracks.size();i++) {
    delete meta_tracks[i];
  }
  meta_tracks.clear();
  meta_headers.clear();
}


void MetaLibrary::LoadLine(const QStringList fields)
{
  MetaRecord *m=new MetaRecord();
  meta_tracks.push_back(m);
  for(unsigned i=0;i<fields.size();i++) {
    if(meta_headers[i]=="disc") {
      m->setDiscId(fields[i]);
    }
    if(meta_headers[i]=="cut") {
      m->setTrackNumber(fields[i].toInt()-1);
    }
    if(meta_headers[i]=="songid") {
      m->setSongId(fields[i]);
    }
    if(meta_headers[i]=="year") {
      m->setYear(fields[i].toInt());
    }
    if(meta_headers[i]=="bpm") {
      m->setBeatsPerMinute(fields[i].toInt());
    }
    if(meta_headers[i]=="title") {
      m->setTitle(fields[i]);
    }
    if(meta_headers[i]=="artist") {
      m->setArtist(fields[i]);
    }
    if(meta_headers[i]=="version") {
      m->setVersion(fields[i]);
    }
    if(meta_headers[i]=="album") {
      m->setAlbum(fields[i]);
    }
    if(meta_headers[i]=="composer") {
      m->setComposer(fields[i]);
    }
    if(meta_headers[i]=="publisher") {
      m->setPublisher(fields[i]);
    }
    if(meta_headers[i]=="client") {
      m->setClient(fields[i]);
    }
    if(meta_headers[i]=="agency") {
      m->setAgency(fields[i]);
    }
    if(meta_headers[i]=="license") {
      m->setLicense(fields[i]);
    }
    if(meta_headers[i]=="recordlabel") {
      m->setLabel(fields[i]);
    }
    if(meta_headers[i]=="isrc") {
      m->setIsrc(fields[i]);
    }
    if(meta_headers[i]=="ending") {
      m->setEnding(fields[i]);
    }
    if(meta_headers[i]=="intro") {
      m->setIntroLength(RDSetTimeLength(fields[i]));
    }
    if(meta_headers[i]=="length") {
      m->setLength(RDSetTimeLength(fields[i]));
    }
    if(meta_headers[i]=="aux3") {
      m->setSegueStart(RDSetTimeLength(fields[i]));
    }
  }
}


QStringList MetaLibrary::Split(const QString &sep,const QString &str)
{
  QString buf;
  QStringList ret;
  bool quoted=false;

  for(unsigned i=0;i<str.length();i++) {
    switch(str[i]) {
    case ',':
      if(quoted) {
	buf+=str[i];
      }
      else {
	ret.push_back(buf);
	buf="";
      }
      break;

    case '"':
      quoted=!quoted;
      break;

    case '\n':
    case '\r':
      break;

    default:
      buf+=str[i];
      break;
    }
  }
  ret.push_back(buf);

  return ret;
}
