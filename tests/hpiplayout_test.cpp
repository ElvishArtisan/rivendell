// hpiplayout_test.cpp
//
// Test the Rivendell multicast receiver routines
//
//   (C) Copyright 2018-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <QApplication>
#include <QStringList>

#include <rdcmd_switch.h>

#include "hpiplayout_test.h"

MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  QHostAddress from_addr;
  int card=0;
  int port=0;
  bool ok=false;
  
  RDCmdSwitch *cmd=new RDCmdSwitch("hpiplayout_test",HPIPLAYOUT_TEST_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--card") {
      card=cmd->value(i).toInt(&ok);
      if((!ok)||(card<0)||(card>=HPI_MAX_ADAPTERS)) {
	fprintf(stderr,"hpiplayout_test: invalid --card\n");
	exit(1);
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--filename") {
      d_filename=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--port") {
      port=cmd->value(i).toInt(&ok);
      if((!ok)||(port<0)||(port>=HPI_MAX_NODES)) {
	fprintf(stderr,"hpiplayout_test: invalid --port\n");
	exit(1);
      }
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"hpiplayout_test: unknown option \"%s\"\n",
	      cmd->key(i).toUtf8().constData());
      exit(256);
    }
  }
  if(d_filename.isEmpty()) {
    fprintf(stderr,"hpiplayout_test: you must provide a \"--filename\"\n");
    exit(1);
  }

  d_rdconfig=new RDConfig();
  d_rdconfig->load();

  d_soundcard=new RDHPISoundCard(d_rdconfig,this);
  d_wavedata=new RDWaveData();
  d_playstream=new RDHPIPlayStream(d_soundcard,this);
  connect(d_playstream,SIGNAL(isStopped(bool)),this,SLOT(isStoppedData(bool)));
  connect(d_playstream,SIGNAL(played()),this,SLOT(playedData()));
  connect(d_playstream,SIGNAL(stopped()),this,SLOT(stoppedData()));
  connect(d_playstream,SIGNAL(paused()),this,SLOT(pausedData()));
  connect(d_playstream,SIGNAL(position(int)),this,SLOT(positionData(int)));
  connect(d_playstream,SIGNAL(stateChanged(int,int,int)),
	  this,SLOT(stateChangedData(int,int,int)));

  d_playstream->setCard(card);
  if(d_playstream->openWave(d_filename)!=RDHPIPlayStream::Ok) {
    fprintf(stderr,"hpiplayout_test: failed to open \"%s\"\n",
	    d_filename.toUtf8().constData());
    exit(1);
  }
  printf("card: %d  stream: %d\n",d_playstream->getCard(),
	 d_playstream->getStream());
  d_soundcard->setOutputVolume(card,d_playstream->getStream(),port,0);
  d_playstream->play();
}


void MainObject::isStoppedData(bool state)
{
  printf("isStopped(%u)\n",state);
  if(state) {
    exit(0);
  }
}


void MainObject::playedData()
{
  printf("played()\n");
}


void MainObject::pausedData()
{
  printf("isPaused()\n");
}


void MainObject::stoppedData()
{
  printf("stopped()\n");
  exit(0);
}


void MainObject::positionData(int samples)
{
  printf("position(%d)\n",samples);

}


void MainObject::stateChangedData(int card,int stream,int state)
{
  printf("stateChanged(%d,%d,%d)\n",card,stream,state);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
