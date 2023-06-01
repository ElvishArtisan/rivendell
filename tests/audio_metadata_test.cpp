// audio_metadata_test.cpp
//
// Test the Rivendell audio file metadata reader.
//
//   (C) Copyright 2018-2022 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdapplication.h>
#include <rdwavefile.h>

#include "audio_metadata_test.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  QString err_msg;

  QString filename;

  //
  // Open the Database
  //
  rda=static_cast<RDApplication *>(new RDCoreApplication("audio_metadata_test",
		   "audio_metadata_test",AUDIO_METADATA_TEST_USAGE,false,this));
  if(!rda->open(&err_msg,NULL,true)) {
    fprintf(stderr,"audio_metadata_test: %s\n",(const char *)err_msg.toUtf8());
    exit(1);
  }
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(rda->cmdSwitch()->key(i)=="--filename") {
      filename=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
  }
  if(filename.isEmpty()) {
    fprintf(stderr,"audio_metadata_test: missing --filename\n");
    exit(256);
  }

  RDWaveData *wavedata=new RDWaveData();
  RDWaveFile *wavefile=new RDWaveFile(filename);
  if(!wavefile->openWave(wavedata)) {
    fprintf(stderr,"audio_metadata_test: unable to open \"%s\"\n",
	    (const char *)filename.toUtf8());
    exit(1);
  }
  printf("Type: %s\n",
	 (const char *)RDWaveFile::typeText(wavefile->type()).toUtf8());
  if(wavedata->metadataFound()) {
    printf("%s\n",(const char *)wavedata->dump().toUtf8());
  }
  else {
    printf("No metadata found.\n");
  }
  delete wavefile;
  delete wavedata;

  exit(0);
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
