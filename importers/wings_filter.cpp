// wings_filter.cpp
//
// A Library import filter for the Airforce Wings system
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: wings_filter.cpp,v 1.13 2010/07/29 19:32:33 cvs Exp $
//      $Date: 2010/07/29 19:32:33 $
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

#include <qapplication.h>

#include <rddb.h>
#include <rd.h>
#include <rdconfig.h>
#include <rdcmd_switch.h>
#include <rdcut.h>
#include <wings_filter.h>


//
// Global Variables
//
RDConfig *rdconfig;


MainObject::MainObject(QObject *parent,const char *name)
  : QObject(parent,name)
{
  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"wings_filter",
		    WINGS_FILTER_USAGE);
  delete cmd;

  WingsRecord wr;
  QString audioname;
  bool found;
  QString dbname;
  QString audiodir;
  QString audio_extension=WINGS_DEFAULT_AUDIO_EXT;
  QString groupname;
  RDWaveFile *wavefile=NULL;

  rdconfig=new RDConfig(RD_CONF_FILE);
  rdconfig->load();

  //
  // Open Database
  //


  filter_db=QSqlDatabase::addDatabase(rdconfig->mysqlDriver());
  if(!filter_db) {
    fprintf(stderr,"wings_filter: can't open mySQL database\n");
    exit(1);
  }
  filter_db->setDatabaseName(rdconfig->mysqlDbname());
  filter_db->setUserName(rdconfig->mysqlUsername());
  filter_db->setPassword(rdconfig->mysqlPassword());
  filter_db->setHostName(rdconfig->mysqlHostname());
  if(!filter_db->open()) {
    fprintf(stderr,"wings_filter: unable to connect to mySQL Server\n");
    filter_db->removeDatabase(rdconfig->mysqlDbname());
    exit(1);
  }

  //
  // RIPCD Connection
  //
  filter_ripc=new RDRipc("");
  filter_ripc->connectHost("localhost",RIPCD_TCP_PORT,rdconfig->password());

  //
  // Station Configuration
  //
  filter_rdstation=new RDStation(rdconfig->stationName());

  //
  // Read Arguments
  //
  for(int i=1;i<(qApp->argc()-1);i+=2) {
    found=false;
    if(!strcmp("-d",qApp->argv()[i])) {
      dbname=qApp->argv()[i+1];
      found=true;
    }
    if(!strcmp("-A",qApp->argv()[i])) {
      audiodir=qApp->argv()[i+1];
      found=true;
    }
    if(!strcmp("-g",qApp->argv()[i])) {
      groupname=qApp->argv()[i+1];
      found=true;
    }
    if(!strcmp("-e",qApp->argv()[i])) {
      audio_extension=qApp->argv()[i+1];
      found=true;
    }
    if(!found) {
      fprintf(stderr,"\nwings_filter %s\n",WINGS_FILTER_USAGE);
      exit(1);
    }
  }

  FILE *file=fopen((const char *)dbname,"r");
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
    strcpy(wr.extension,audio_extension);
    audioname=QString().sprintf("%s/%s.%s",(const char *)audiodir,
				wr.filename,(const char *)audio_extension);
    wavefile=new RDWaveFile(audioname);
    if(!wavefile->openWave()) {
      fprintf(stderr,"Unable to open %s, skipping...\n",
	      (const char *)audioname);
    }
    else {
      if(wavefile->type()!=RDWaveFile::Atx) {
	fprintf(stderr,"ATX header in %s appears corrupt, skipping...\n",
	       (const char *)audioname);
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
	 rec->filename,rec->title,cartnum,(const char *)group->name());
  
  sql=QString().sprintf("insert into CART set NUMBER=%u,GROUP_NAME=\"%s\",\
                         TITLE=\"%s\",ARTIST=\"%s\",ALBUM=\"%s\",\
                         CUT_QUANTITY=1,TYPE=%d,FORCED_LENGTH=%u,\
                         AVERAGE_LENGTH=%u,USER_DEFINED=\"%s.%s\"",
			cartnum,(const char *)group->name(),
			rec->title,rec->artist,rec->album,
			RDCart::Audio,wavefile->getExtTimeLength(),
			wavefile->getExtTimeLength(),
			rec->filename,rec->extension);
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("insert into CUTS set CUT_NAME=\"%06u_001\",\
                         CART_NUMBER=%u,DESCRIPTION=\"%s\",\
                         ORIGIN_DATETIME=\"%s\",ORIGIN_NAME=\"%s\",\
                         CODING_FORMAT=%d,SAMPLE_RATE=%u,CHANNELS=%d,\
                         BIT_RATE=%d,LENGTH=%u,START_POINT=0,\
                         END_POINT=%d",
			cartnum,cartnum,(const char *)rec->title,
			(const char *)QDateTime::currentDateTime().
			toString("yyyy-MM-dd hh:mm:ss"),
			(const char *)rdconfig->stationName(),format,
			wavefile->getSamplesPerSec(),
			wavefile->getChannels(),wavefile->getHeadBitRate(),
			wavefile->getExtTimeLength(),
			wavefile->getExtTimeLength());
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
  QApplication a(argc,argv,false);
  new MainObject(NULL,"main");
  return a.exec();
}
