// rdwavefile_test.cpp
//
// Test the Rivendell RDWaveFile class.
//
//   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <QCoreApplication>

#include <rdcmd_switch.h>
#include <rdwavedata.h>
#include <rdwavefile.h>

#include "rdwavefile_test.h"

MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  QString filename;
  RDWaveData *wavedata=new RDWaveData();
  RDWaveFile *wavefile=NULL;

  RDCmdSwitch *cmd=new RDCmdSwitch("rdwavefile_test",RDWAVEFILE_TEST_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--filename") {
      filename=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"rdwavefile_test: unrecognized option \"%s\"\n",
	      cmd->key(i).toUtf8().constData());
      exit(1);
    }
  }
  if(filename.isEmpty()) {
    fprintf(stderr,"rdwavefile_test: you must supply --filename=<name>\n");
    exit(1);
  }

  wavefile=new RDWaveFile(filename);
  if(!wavefile->openWave(wavedata)) {
    fprintf(stderr,"rdwavefile_test: unable to open file\n");
    exit(1);
  }

  printf("DONE\n");

  exit(0);
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);

  new MainObject();
  return a.exec();
}
