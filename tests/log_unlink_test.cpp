// log_unlink_test.cpp
//
// Test the Rivendell log unlinker methods.
//
//   (C) Copyright 2017 Fred Gleason <fredg@paravelsystems.com>
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
#include <rdconfig.h>
#include <rddatedecode.h>
#include <rddb.h>
#include <rdlog.h>
#include <rdstation.h>
#include <rdsvc.h>

#include "log_unlink_test.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  QString log_name="";
  RDSvc::ImportSource import_source=RDSvc::Traffic;
  RDConfig *config=NULL;
  RDStation *station=NULL;
  unsigned schema=0;

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"log_unlink_test",
		    LOG_UNLINK_TEST_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--log") {
      log_name=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--source") {
      if(cmd->value(i).lower()=="traffic") {
	import_source=RDSvc::Traffic;
      }
      else {
	if(cmd->value(i).lower()=="music") {
	  import_source=RDSvc::Music;
	}
	else {
	  fprintf(stderr,
		  "log_unlink_test: you must specify a source to unlink\n");
	  exit(1);
	}
      }
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"log_unlink_test: unknown option \"%s\"\n",
	      (const char *)cmd->value(i));
      exit(256);
    }
  }
  if(log_name.isEmpty()) {
    fprintf(stderr,"log_unlink_test: you must specify a log name with \"--log=\"\n");
    exit(1);
  }

  //
  // Load Configuration
  //
  config=new RDConfig();
  config->load();
  config->setModuleName("reserve_carts_test");

  //
  // Open Database
  //
  QString err (tr("upload_test: "));
  QSqlDatabase *db=RDInitDb(&schema,&err);
  if(!db) {
    fprintf(stderr,err.ascii());
    delete cmd;
    exit(256);
  }
  station=new RDStation(config->stationName());

  //
  // Run the Test
  //
  if(!RDLog::exists(log_name)) {
    fprintf(stderr,"log_unlink_test: no such log\n");
    exit(1);
  }
  RDLog *log=new RDLog(log_name);
  RDSvc *svc=new RDSvc(log->service(),station,config,this);
  svc->clearLogLinks(import_source,log_name);

  exit(0);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
