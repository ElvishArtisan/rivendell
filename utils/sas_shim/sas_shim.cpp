// sas_shim.cpp
//
// An RDCatch event import shim for the SAS64000
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: sas_shim.cpp,v 1.8 2010/07/29 19:32:40 cvs Exp $
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

#include <rdconf.h>
#include <rd.h>
#include <rdcmd_switch.h>
#include <rddbheartbeat.h>

#include <sas_shim.h>


void SigHandler(int signo)
{
  switch(signo) {
      case SIGTERM:
	unlink("/var/run/sas_shim.pid");
	exit(0);
	break;
  }
}


MainObject::MainObject(QObject *parent,const char *name)
  : QObject(parent,name)
{
  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=new RDCmdSwitch(qApp->argc(),qApp->argv(),"sas_shim","\n");
  delete cmd;

  rd_config=new RDConfig(RD_CONF_FILE);
  rd_config->load();

  //
  // Open Database
  //
  shim_db=QSqlDatabase::addDatabase(rd_config->mysqlDriver());
  if(!shim_db) {
    fprintf(stderr,"sas_shim: can't open mySQL database\n");
    exit(1);
  }
  shim_db->setDatabaseName(rd_config->mysqlDbname());
  shim_db->setUserName(rd_config->mysqlUsername());
  shim_db->setPassword(rd_config->mysqlPassword());
  shim_db->setHostName(rd_config->mysqlHostname());
  if(!shim_db->open()) {
    fprintf(stderr,"sas_shim: unable to connect to mySQL Server");
    shim_db->removeDatabase(rd_config->mysqlDbname());
    exit(1);
  }
  new RDDbHeartbeat(rd_config->mysqlHeartbeatInterval(),this);

  //
  // RIPCD Connection
  //
  shim_ripc=new RDRipc("");
  shim_ripc->connectHost("localhost",RIPCD_TCP_PORT,rd_config->password());

  //
  // Station Configuration
  //
  shim_rdstation=new RDStation(rd_config->stationName());
  shim_address=shim_rdstation->address();

  //
  // TTY Device
  //
  shim_tty=new RDTTYDevice();
  shim_tty->setName(rd_config->sasTtyDevice());
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
  RDDetach(rd_config->logCoreDumpDirectory());
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
  rml.setArgQuantity(3);
  rml.setArg(0,rd_config->sasMatrix());
  rml.setArg(1,input);
  rml.setArg(2,output);
  shim_ripc->sendRml(&rml);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject(NULL,"main");
  return a.exec();
}
