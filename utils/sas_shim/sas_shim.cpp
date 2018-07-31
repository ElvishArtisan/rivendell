// sas_shim.cpp
//
// An RDCatch event import shim for the SAS64000
//
//   (C) Copyright 2002-2004,2016-2018 Fred Gleason <fredg@paravelsystems.com>
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
#include <signal.h>

#include <qapplication.h>
#include <qtimer.h>

#include <rd.h>
#include <rdapplication.h>
#include <rdconf.h>

#include "sas_shim.h"

void SigHandler(int signo)
{
  switch(signo) {
  case SIGTERM:
    unlink("/var/run/sas_shim.pid");
    exit(0);
    break;
  }
}


MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  QString err_msg;

  //
  // Open the Database
  //
  rda=new RDApplication("sas_shim","sas_shim",SAS_SHIM_USAGE,this);
  if(!rda->open(&err_msg)) {
    fprintf(stderr,"sas_shim: %s\n",(const char *)err_msg);
    exit(1);
  }

  //
  // Read Command Options
  //
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(!rda->cmdSwitch()->processed(i)) {
      fprintf(stderr,"sas_shim: unknown command option \"%s\"\n",
	      (const char *)rda->cmdSwitch()->key(i));
      exit(2);
    }
  }

  //
  // Station Configuration
  //
  shim_address=rda->station()->address();

  //
  // RIPCD Connection
  //
  rda->ripc()->connectHost("localhost",RIPCD_TCP_PORT,rda->config()->password());

  //
  // TTY Device
  //
  shim_tty=new RDTTYDevice();
  shim_tty->setName(rda->config()->sasTtyDevice());
  if(!shim_tty->open(IO_ReadOnly)) {
    fprintf(stderr,"sas_shim: unabled to open tty device\n");
    exit(1);
  }
  shim_tty->setSpeed(9600);
  shim_tty->setWordLength(8);
  shim_tty->setParity(RDTTYDevice::None);

  //
  // Poll Timer
  //
  QTimer *timer=new QTimer(this,"poll_timer");
  connect(timer,SIGNAL(timeout()),this,SLOT(readTtyData()));
  timer->start(POLL_INTERVAL);

  //
  // Detach
  //
  RDDetach(rda->config()->logCoreDumpDirectory());
  FILE *pidfile=fopen("/var/run/sas_shim.pid","w");
  fprintf(pidfile,"%d",getpid());
  fclose(pidfile);
  ::signal(SIGTERM,SigHandler);
}


void MainObject::readTtyData()
{
  static char cmd[3];
  static int input;
  static int output;
  static int istate=0;
  char buf[256];
  int n;

  while((n=shim_tty->readBlock(buf,255))>0) {
    for(int i=0;i<n;i++) {
      switch(istate) {
      case 0:   // Start of Command
	if(buf[i]=='D') {
	  istate=1;
	}
	break;

      case 1:   // Type Identifier
	if(buf[i]=='T') {
	  istate=2;
	}
	else {
	  istate=0;
	}
	break;

      case 2:   // Crosspoint State Byte
	if(buf[i]=='1') {
	  istate=3;
	}
	else {
	  istate=0;
	}
	break;

      case 3:   // First Input Digit
	if((buf[i]>='0')&&(buf[i]<='9')) {
	  cmd[0]=buf[i];
	  istate=4;
	}
	else {
	  istate=0;
	}
	break;

      case 4:   // Second Input Digit
	if((buf[i]>='0')&&(buf[i]<='9')) {
	  cmd[1]=buf[i];
	  cmd[2]=0;
	  sscanf(cmd,"%d",&input);
	  istate=5;
	}
	else {
	  istate=0;
	}
	break;

      case 5:   // First Output Digit
	if((buf[i]>='0')&&(buf[i]<='9')) {
	  cmd[0]=buf[i];
	  istate=6;
	}
	else {
	  istate=0;
	}
	break;

      case 6:   // Second Output Digit
	if((buf[i]>='0')&&(buf[i]<='9')) {
	  cmd[1]=buf[i];
	  cmd[2]=0;
	  sscanf(cmd,"%d",&output);
	  DispatchRml(input,output);
	  istate=0;
	}
	else {
	  istate=0;
	}
	break;
      }
    }
  }
}


void MainObject::DispatchRml(int input,int output)
{
  RDMacro rml;

  rml.setCommand(RDMacro::ST);
  rml.setRole(RDMacro::Cmd);
  rml.setAddress(shim_address);
  rml.setEchoRequested(false);
  rml.addArg(rda->config()->sasMatrix());
  rml.addArg(input);
  rml.addArg(output);
  rda->ripc()->sendRml(&rml);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
