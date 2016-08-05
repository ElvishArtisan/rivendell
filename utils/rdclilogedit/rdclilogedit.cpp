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

#include <qfile.h>
#include <qstringlist.h>

#include <rdapplication.h>
#include <rdconf.h>
#include <rddbheartbeat.h>
#include <rdweb.h>

#include "rdclilogedit.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  edit_quiet_option=false;

  edit_log=NULL;
  edit_log_event=NULL;
  edit_modified=false;
  edit_new_log=false;

  //
  // Read Command Options
  //
  for(int i=0;i<(int)rda->cmdSwitch()->keys();i++) {
    if((rda->cmdSwitch()->key(i)=="-n")||(rda->cmdSwitch()->key(i)=="--quiet")||
       (rda->cmdSwitch()->key(i)=="--silent")) {
      edit_quiet_option=true;
    }
  }

  //
  // RIPC Connection
  //
  connect(rda->ripc(),SIGNAL(userChanged()),this,SLOT(userData()));
  rda->ripc()->connectHost("localhost",RIPCD_TCP_PORT,rda->config()->password());
}


void MainObject::userData()
{
  char data[1024];
  int n;

  //
  // Get User Context
  //
  disconnect(rda->ripc(),SIGNAL(userChanged()),this,SLOT(userData()));
  rda->setUser(rda->ripc()->user());

  //
  // Start up command processor
  //
  PrintPrompt();
  while((n=read(0,data,1024))>0) {
    for(int i=0;i<n;i++) {
      switch(0xFF&data[i]) {
      case 10:
      case ';':
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
  if(!edit_quiet_option) {
    printf("\n");
  }
  exit(0);
}


void MainObject::OverwriteError(const QString &cmd) const
{
  fprintf(stderr,"%s: buffer not saved (append \"!\" to override)\n",
	  (const char *)cmd);
}


void MainObject::PrintPrompt() const
{
  if(!edit_quiet_option) {
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
}


int main(int argc,char *argv[])
{
  RDApplication a(argc,argv,"rdclilogedit",RDCLILOGEDIT_USAGE,false);
  new MainObject();
  return a.exec();
}
