// rdclilogedit.cpp
//
// A command-line log editor for Rivendell
//
//   (C) Copyright 2016-2020 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdapplication.h>
#include <rdconf.h>
#include <rddbheartbeat.h>
#include <rdweb.h>

#include "rdclilogedit.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  QString err_msg;

  edit_quiet_option=false;

  edit_log=NULL;
  edit_log_model=NULL;
  edit_modified=false;
  edit_log_lock=NULL;

  //
  // Open the Database
  //
  rda=new RDApplication("rdclilogedit","rdclilogedit",RDCLILOGEDIT_USAGE,this);
  if(!rda->open(&err_msg)) {
    fprintf(stderr,"rdclilogedit: %s\n",(const char *)err_msg);
    exit(1);
  }

  //
  // Read Command Options
  //
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if((rda->cmdSwitch()->key(i)=="-n")||(rda->cmdSwitch()->key(i)=="--quiet")||
       (rda->cmdSwitch()->key(i)=="--silent")) {
      edit_quiet_option=true;
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(!rda->cmdSwitch()->processed(i)) {
      fprintf(stderr,"rdclilogedit: unknown command option \"%s\"\n",
	      (const char *)rda->cmdSwitch()->key(i));
      exit(2);
    }
  }

  //
  // RIPC Connection
  //
  connect(rda,SIGNAL(userChanged()),this,SLOT(userData()));
  rda->ripc()->
    connectHost("localhost",RIPCD_TCP_PORT,rda->config()->password());
}


void MainObject::userData()
{
  char data[1024];
  int n;

  //
  // Get User Context
  //
  disconnect(rda->ripc(),SIGNAL(userChanged()),this,SLOT(userData()));

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
  if(edit_log_lock!=NULL) {
    delete edit_log_lock;
  }
  exit(0);
}


bool MainObject::TryLock(RDLogLock *lock,const QString &logname)
{
  QString username;
  QString stationname;
  QHostAddress addr;
  bool ret;

  ret=lock->tryLock(&username,&stationname,&addr);
  if(!ret) {
    QString msg="log \""+logname+"\" in use by "+username+"@"+stationname;
    if(stationname!=addr.toString()) {
      msg+=" ["+addr.toString()+"]";
    }
    fprintf(stderr,"%s\n",(const char *)msg);
  }
  return ret;
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


void MainObject::SendNotification(RDNotification::Action action,
				  const QString &logname)
{
  RDNotification *notify=new RDNotification(RDNotification::LogType,
					    action,QVariant(logname));
  rda->ripc()->sendNotification(*notify);
  qApp->processEvents();
  delete notify;
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
