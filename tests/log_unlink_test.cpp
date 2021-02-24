// log_unlink_test.cpp
//
// Test the Rivendell log unlinker methods.
//
//   (C) Copyright 2017-2021 Fred Gleason <fredg@paravelsystems.com>
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
  test_import_source=RDSvc::Traffic;
  int schema=0;

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=new RDCmdSwitch("log_unlink_test",LOG_UNLINK_TEST_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--log") {
      test_log_name=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--source") {
      if(cmd->value(i).toLower()=="traffic") {
	test_import_source=RDSvc::Traffic;
      }
      else {
	if(cmd->value(i).toLower()=="music") {
	  test_import_source=RDSvc::Music;
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
	      cmd->value(i).toUtf8().constData());
      exit(256);
    }
  }
  if(test_log_name.isEmpty()) {
    fprintf(stderr,"log_unlink_test: you must specify a log name with \"--log=\"\n");
    exit(1);
  }

  //
  // Load Configuration
  //
  test_config=new RDConfig();
  test_config->load();
  test_config->setModuleName("reserve_carts_test");

  //
  // Open Database
  //
  QString err (tr("upload_test: "));
  if(!RDOpenDb(&schema,&err,test_config)) {
    fprintf(stderr,err.toUtf8());
    delete cmd;
    exit(256);
  }
  test_station=new RDStation(test_config->stationName());
  test_ripc=new RDRipc(test_station,test_config,this);
  connect(test_ripc,SIGNAL(userChanged()),this,SLOT(userData()));
  test_ripc->connectHost("localhost",RIPCD_TCP_PORT,test_config->password());
}


void MainObject::userData()
{
  QString err_msg;

  //
  // Run the Test
  //
  if(!RDLog::exists(test_log_name)) {
    fprintf(stderr,"log_unlink_test: no such log\n");
    exit(1);
  }
  RDLog *log=new RDLog(test_log_name);
  RDSvc *svc=new RDSvc(log->service(),test_station,test_config,this);
  if(!svc->clearLogLinks(test_import_source,test_log_name,
			 new RDUser(test_ripc->user()),&err_msg)) {
    fprintf(stderr,"log_unlink_test: %s\n",err_msg.toUtf8().constData());
    exit(1);
  }

  exit(0);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
