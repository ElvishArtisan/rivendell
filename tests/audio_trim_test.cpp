// audio_trim_test.cpp
//
// Test Rivendell low-level audio trim level measurement code.
//
//   (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdwavefile.h>

#include "audio_trim_test.h"

MainObject::MainObject(QObject *parent,const char *name)
  :QObject(parent,name)
{
  bool ok;

  filename="";
  trim_level=1;

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"audio_trim_test",
  		    AUDIO_TRIM_TEST_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--filename") {
      filename=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--trim-level") {
      trim_level=cmd->value(i).toInt(&ok);
      if((!ok)||(trim_level>0)) {
	fprintf(stderr,"audio_trim_test: invalid --trim-level value\n");
	exit(256);
      }
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"audio_trim_test: invalid argument \"%s\"\n",
	      (const char *)cmd->key(i));
    }
  }
  if(filename.isEmpty()) {
    fprintf(stderr,"audio_trim_test: missing --filename\n");
    exit(256);
  }
  if(trim_level>0) {
    fprintf(stderr,"audio_trim_test: missing --trim-level\n");
    exit(256);
  }

  //
  // Process
  //
  RDWaveFile *wave=new RDWaveFile(filename);
  if(!wave->openWave()) {
    fprintf(stderr,"audio_trim_test: unable to open \"%s\"\n",
	    (const char *)filename);
    delete wave;
    exit(256);
  }
  if(wave->hasEnergy()) {
    double start=wave->startTrim(trim_level);
    double end=wave->endTrim(trim_level);

    printf("Filename: %s\n",(const char *)filename);
    printf("Start Point: %d frames, %9.2lf secs\n",(int)start,
	   start/(double)wave->getSamplesPerSec());
    printf("  End Point: %d frames, %9.2lf secs\n",(int)end,
	   end/(double)wave->getSamplesPerSec());
  }
  else {
    printf("\"%s\" contains no energy\n",(const char *)filename);
  }
  delete wave;
  exit(0);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject(NULL);
  return a.exec();
}
