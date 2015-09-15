// audio_peaks_test.cpp
//
// Test the Rivendell audio peak routines.
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

#include <stdlib.h>
#include <stdio.h>

#include <qapplication.h>
#include <qvariant.h>

#include <rdcmd_switch.h>
#include <rdwavefile.h>

#include "audio_peaks_test.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  QString filename;
  unsigned frame=0;
  bool frame_used=false;
  bool ok=false;

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"audio_peaks_test",
		    AUDIO_PEAKS_TEST_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--filename") {
      filename=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--frame") {
      frame=cmd->value(i).toUInt(&ok);
      if(!ok) {
	fprintf(stderr,"audio_peaks_test: invalid --frame arguument\n");
	exit(256);
      }
      frame_used=true;
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"audio_peaks_test: unknown option \"%s\"\n",
	      (const char *)cmd->value(i));
      exit(256);
    }
  }
  if(filename.isEmpty()) {
    fprintf(stderr,"audio_peaks_test: you must specify --filename\n");
    exit(256);
  }

  RDWaveFile *wave=new RDWaveFile();
  wave->nameWave(filename);
  if(!wave->openWave()) {
    fprintf(stderr,"audio_peaks_test: unable to open \"%s\"\n",
	    (const char *)filename);
    exit(256);
  }
  if(wave->hasEnergy()) {
    printf("\"%s\" has energy, size: %u\n",(const char *)filename,
	   wave->energySize());
    if(frame_used) {
      if(wave->getChannels()==1) {
	printf("frame: %u: %d\n",frame,0xFFFF&wave->energy(frame));
      }
      else {
	printf("frame %u: left: %d  right: %d\n",frame,
	       0xFFFF&wave->energy(2*frame),
	       0xFFFF&wave->energy(2*frame+1));
      }
    }
  }
  else {
    printf("\"%s\" does NOT have energy\n",(const char *)filename);
  }
  wave->closeWave();
  delete wave;

  exit(0);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject(NULL);
  return a.exec();
}
