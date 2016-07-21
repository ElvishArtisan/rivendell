// rdclilogedit.cpp
//
// A command-line log editor for Rivendell
//
//   (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <qapplication.h>
#include <qfile.h>
#include <qstringlist.h>

#include <rdcmd_switch.h>
#include <rdconf.h>
#include <rdweb.h>

#include "rdclilogedit.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  edit_log=NULL;
  edit_log_event=NULL;
  edit_modified=false;

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"rdimport",RDCLILOGEDIT_USAGE);
  for(int i=0;i<(int)cmd->keys()-1;i++) {
  }

  //
  // Read Configuration
  //
  edit_config=new RDConfig();
  edit_config->load();

  //
  // Open Database
  //
  QSqlDatabase *db=QSqlDatabase::addDatabase(edit_config->mysqlDriver());
  if(!db) {
    fprintf(stderr,"rdclilogedit: unable to initialize connection to database\n");
    exit(256);
  }
  db->setDatabaseName(edit_config->mysqlDbname());
  db->setUserName(edit_config->mysqlUsername());
  db->setPassword(edit_config->mysqlPassword());
  db->setHostName(edit_config->mysqlHostname());
  if(!db->open()) {
    fprintf(stderr,"rdclilogedit: unable to connect to database\n");
    db->removeDatabase(edit_config->mysqlDbname());
    exit(256);
  }

  //
  // RDAirPlay Configuration
  //
  edit_airplay_conf=new RDAirPlayConf(edit_config->stationName(),"RDAIRPLAY");

  //
  // RIPC Connection
  //
  edit_user=NULL;
  edit_ripc=new RDRipc(edit_config->stationName());
  connect(edit_ripc,SIGNAL(userChanged()),this,SLOT(userData()));
  edit_ripc->connectHost("localhost",RIPCD_TCP_PORT,edit_config->password());
}


void MainObject::userData()
{
  char data[1024];
  int n;

  //
  // Get User Context
  //
  disconnect(edit_ripc,SIGNAL(userChanged()),this,SLOT(userData()));
  if(edit_user!=NULL) {
    delete edit_user;
  }
  edit_user=new RDUser(edit_ripc->user());

  //
  // Start up command processor
  //
  PrintPrompt();
  while((n=read(0,data,1024))>0) {
    for(int i=0;i<n;i++) {
      switch(0xFF&data[i]) {
      case 10:
	DispatchCommand(edit_accum);
	edit_accum="";
	break;

      case 13:
	break;

      default:
	edit_accum+=data[i];
      }
    }
  }
  printf("\n");
  exit(0);
}


void MainObject::OverwriteError(const QString &cmd) const
{
  fprintf(stderr,"%s: buffer not saved (append \"!\" to override)\n",
	  (const char *)cmd);
}


void MainObject::PrintPrompt() const
{
  if(edit_log==NULL) {
    printf("logedit> ");
  }
  else {
    if(edit_modified) {
      printf("logedit[%s*]> ",(const char *)edit_log->name());
    }
    else {
      printf("logedit[%s]> ",(const char *)edit_log->name());
    }
  }
  fflush(stdout);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
