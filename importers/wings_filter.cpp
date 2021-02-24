// wings_filter.cpp
//
// A Library import filter for the Airforce Wings system
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include <QApplication>

#include <rd.h>
#include <rdapplication.h>
#include <rddb.h>
#include <rdcut.h>
#include <rdescape_string.h>

#include "wings_filter.h"

MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  WingsRecord wr;
  QString audioname;
  bool found;
  QString dbname;
  QString audiodir;
  QString audio_extension=WINGS_DEFAULT_AUDIO_EXT;
  QString groupname;
  RDWaveFile *wavefile=NULL;
  QString err_msg;

  //
  // Open the Database
  //
  rda=static_cast<RDApplication *>(new RDCoreApplication("wings_filter","wings_filter",WINGS_FILTER_USAGE,this));
  if(!rda->open(&err_msg)) {
    fprintf(stderr,"wings_filter: %s\n",err_msg.toUtf8().constData());
    exit(1);
  }

  //
  // RIPCD Connection
  //
  rda->ripc()->connectHost("localhost",RIPCD_TCP_PORT,rda->config()->password());

  //
  // Read Arguments
  //
  for(int i=1;i<(qApp->arguments().size()-1);i+=2) {
    found=false;
    if(qApp->arguments().at(i)=="-d") {
      dbname=qApp->arguments().at(i+1);
      found=true;
    }
    if(qApp->arguments().at(i)=="-A") {
      audiodir=qApp->arguments().at(i+1);
      found=true;
    }
    if(qApp->arguments().at(i)=="-g") {
      groupname=qApp->arguments().at(i+1);
      found=true;
    }
    if(qApp->arguments().at(i)=="-e") {
      audio_extension=qApp->arguments().at(i+1);
      found=true;
    }
    if(!found) {
      fprintf(stderr,"\nwings_filter %s\n",WINGS_FILTER_USAGE);
      exit(1);
    }
  }

  FILE *file=fopen(dbname.toUtf8(),"r");
  if(file==NULL) {
    perror("wings_filter");
    exit(1);
  }

  RDGroup *default_group=new RDGroup(groupname);
  if(!default_group->exists()) {
    fprintf(stderr,"wings_filter: no such default group\n");
    delete default_group;
    exit(256);
  }
  while(ReadLine(file,&wr)) {
    strcpy(wr.extension,audio_extension.toUtf8());
    audioname=QString().sprintf("%s/%s.%s",audiodir.toUtf8().constData(),
				wr.filename,
				audio_extension.toUtf8().constData());
    wavefile=new RDWaveFile(audioname);
    if(!wavefile->openWave()) {
      fprintf(stderr,"Unable to open %s, skipping...\n",
	      audioname.toUtf8().constData());
    }
    else {
      if(wavefile->type()!=RDWaveFile::Atx) {
	fprintf(stderr,"ATX header in %s appears corrupt, skipping...\n",
		audioname.toUtf8().constData());
      }
      else {
	RDGroup *group=new RDGroup(wr.group);
	if(group->exists()) {
	  ImportCut(group,&wr,wavefile);
	}
	else {
	  ImportCut(default_group,&wr,wavefile);
	}
	delete group;
      }
      wavefile->closeWave();
    }
    delete wavefile;
  }

  delete default_group;
  fclose(file);
  exit(0);
}


bool MainObject::ImportCut(RDGroup *group,struct WingsRecord *rec,
			   RDWaveFile *wavefile)
{
  unsigned cartnum=0;
  QString sql;
  RDSqlQuery *q;
  int format=0;
  RDWaveFile *destfile;
  int n;
  char buffer[WINGS_XFER_BUFFER_SIZE];

  if((cartnum=group->nextFreeCart())==0) {
    fprintf(stderr,"No more available carts in group %s, skipping %s...\n",
	    rec->group,rec->filename);
    delete group;
    return false;
  }
  destfile=new RDWaveFile(RDCut::pathName(QString().sprintf
					   ("%06u_001",cartnum)));
  switch(wavefile->getFormatTag()) {
      case WAVE_FORMAT_PCM:
	format=0;
	destfile->setBextChunk(true);
	destfile->setFormatTag(WAVE_FORMAT_PCM);
	break;

      case WAVE_FORMAT_MPEG:
	if(wavefile->getHeadLayer()==2) {
	  format=1;
	  destfile->setFormatTag(WAVE_FORMAT_MPEG);
	  destfile->setHeadLayer(2);
	  destfile->setHeadBitRate(wavefile->getHeadBitRate());
	  destfile->setHeadMode(wavefile->getHeadMode());
	  destfile->setMextChunk(true);
	  destfile->setMextHomogenous(true);
	  destfile->setMextAncillaryLength(3);
	  destfile->setMextLeftEnergyPresent(true);
	  if(wavefile->getChannels()>1) {
	    destfile->setMextRightEnergyPresent(true);
	  }
	}
	else {
	  return false;
	}
	break;

      default:
	return false;
	break;
  }
  destfile->setSamplesPerSec(wavefile->getSamplesPerSec());
  destfile->setBitsPerSample(wavefile->getBitsPerSample());
  destfile->setChannels(wavefile->getChannels());
  destfile->setBextChunk(true);
  destfile->setBextDescription(rec->title);
  if(!destfile->createWave()) {
    fprintf(stderr,"wings_filter: unable to write to audio directory\n");
    exit(256);
  }
  while((n=wavefile->readWave(buffer,WINGS_XFER_BUFFER_SIZE))>0) {
    destfile->writeWave(buffer,n);
  }
  destfile->closeWave();

  printf("Importing %s - %s to cart %u, group %s\n",
	 rec->filename,rec->title,cartnum,group->name().toUtf8().constData());
  
  sql=QString("insert into CART set ")+
    QString().sprintf("NUMBER=%u,",cartnum)+
    "GROUP_NAME=\""+RDEscapeString(group->name())+"\","+
    "TITLE=\""+RDEscapeString(rec->title)+"\","+
    "ARTIST=\""+RDEscapeString(rec->artist)+"\","+
    "ALBUM=\""+RDEscapeString(rec->album)+"\","+
    "CUT_QUANTITY=1,"+
    QString().sprintf("TYPE=%d,",RDCart::Audio)+
    QString().sprintf("FORCED_LENGTH=%u,",wavefile->getExtTimeLength())+
    QString().sprintf("AVERAGE_LENGTH=%u,",wavefile->getExtTimeLength())+
    "USER_DEFINED=\""+RDEscapeString(rec->filename)+"."+
    RDEscapeString(rec->extension)+"\"";
  q=new RDSqlQuery(sql);
  delete q;
  RDCut::create(cartnum,1);
  sql=QString("update CUTS set ")+
    "DESCRIPTION=\""+RDEscapeString(rec->title)+"\","+
    "ORIGIN_DATETIME=now(),"+
    "ORIGIN_NAME=\""+RDEscapeString(rda->config()->stationName())+"\","+
    QString().sprintf("CODING_FORMAT=%d,",format)+
    QString().sprintf("SAMPLE_RATE=%u,",wavefile->getSamplesPerSec())+
    QString().sprintf("CHANNELS=%d,",wavefile->getChannels())+
    QString().sprintf("BIT_RATE=%d,",wavefile->getHeadBitRate())+
    QString().sprintf("LENGTH=%u,",wavefile->getExtTimeLength())+
    "START_POINT=0,"+
    QString().sprintf("END_POINT=%d where ",wavefile->getExtTimeLength())+
    "CUT_NAME=\""+RDCut::cutName(cartnum,1)+"\"";
  q=new RDSqlQuery(sql);
  delete q;

  return true;
}


bool MainObject::ReadLine(FILE *fp,struct WingsRecord *rec)
{
  char buffer[WINGS_RECORD_LENGTH+1];

  memset(rec,0,sizeof(struct WingsRecord));
  if(fgets(buffer,WINGS_RECORD_LENGTH+1,fp)==NULL) {
    return false;
  }
  if(strlen(buffer)<WINGS_RECORD_LENGTH) {
    return false;
  }
  buffer[8]=0;                    // Fielname
  strcpy(rec->filename,buffer);
  TrimSpaces(rec->filename);

  rec->group[0]=buffer[9];        // Group
  rec->group[1]=0;

  buffer[17]=0;                   // Length
  sscanf(buffer+14,"%d",&rec->length);
  rec->length*=1000;

  buffer[47]=0;                   // Title
  strcpy(rec->title,buffer+24);
  TrimSpaces(rec->title);

  buffer[77]=0;                   // Artist
  strcpy(rec->artist,buffer+55);
  TrimSpaces(rec->artist);

  buffer[105]=0;
  strcpy(rec->album,buffer+78);
  TrimSpaces(rec->album);

  return true;
}


void MainObject::TrimSpaces(char *str)
{
  for(int i=strlen(str)-1;i>=0;i--) {
    if(isspace(str[i])) {
      str[i]=0;
    }
    else {
      i=-1;
    }
  }
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv,false);
  new MainObject(NULL);
  return a.exec();
}
