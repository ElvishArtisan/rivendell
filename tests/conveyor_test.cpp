// conveyor_test.cpp
//
// Test the Rivendell replicator conveyor routines.
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
#include <sys/types.h>
#include <unistd.h>

#include <qapplication.h>

#include <rdcmd_switch.h>
#include <rdconf.h>
#include <rdconfig.h>
#include <rddb.h>
#include <rdrepl_conveyor.h>
#include <rdreplicator.h>

#include "conveyor_test.h"

MainObject::MainObject(QObject *parent)
{
  unsigned schema=0;

  QString repl_name="";
  RDReplConveyor::Direction repl_direction=(RDReplConveyor::Direction)1000;
  QString add_file="";
  QString recv_file="";

  openlog("conveyor_test",LOG_PERROR,LOG_DAEMON);

  //
  // Read Configuration
  //
  RDConfig *rdconfig=new RDConfig();
  rdconfig->load();

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"conveyor_test",
		    CONVEYOR_TEST_USAGE);
  bool ok=false;
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--name") {
      repl_name=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--add-file") {
      add_file=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--recv-file") {
      recv_file=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--direction") {
      repl_direction=(RDReplConveyor::Direction)cmd->value(i).toInt(&ok);
      if((!ok)||((repl_direction!=RDReplConveyor::Inbound)&&
		 (repl_direction!=RDReplConveyor::Outbound))) {
	fprintf(stderr,"conveyor_test: invalid --direction argument\n");
	exit(256);
      }
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"conveyor_test: unknown option \"%s\"\n",
	      (const char *)cmd->value(i));
      exit(256);
    }
  }
  if(repl_name.isEmpty()) {
    fprintf(stderr,"conveyor_test: you must specify --name\n");
    exit(256);
  }
  if((repl_direction!=RDReplConveyor::Inbound)&&
     (repl_direction!=RDReplConveyor::Outbound)) {
    fprintf(stderr,"conveyor_test: missing or invalid --direction\n");
    exit(256);
  }
  if(add_file.isEmpty()&&recv_file.isEmpty()) {
    fprintf(stderr,
	    "conveyor_test: --add-file or --recv-file must be specified\n");
    exit(256);
  }
  if((!add_file.isEmpty())&&(!recv_file.isEmpty())) {
    fprintf(stderr,
	  "conveyor_test: --add-file and --recv-file are mutually exclusive\n");
    exit(256);
  }

  //
  // Drop root permissions
  //
  /*
  if(setgid(rdconfig->gid())!=0) {
    fprintf(stderr,"conveyor_test: unable to set rivendell group\n");
    exit(256);
  }
  if(setuid(rdconfig->uid())!=0) {
    fprintf(stderr,"conveyor_test: unable to set rivendell user\n");
    exit(256);
  }
  */

  //
  // Open Database
  //
  QString err (tr("conveyor_test: "));
  QSqlDatabase *db=RDInitDb(&schema,&err);
  if(!db) {
    fprintf(stderr,err.ascii());
    delete cmd;
    exit(256);
  }

  //
  // Run the test
  //
  RDReplicator *repl=new RDReplicator(repl_name);
  if(!repl->exists()) {
    fprintf(stderr,"conveyor_test: no such replicator\n");
    exit(256);
  }
  RDReplConveyor *conv=new RDReplConveyor(repl_name);
  if(!add_file.isEmpty()) {
    if(!conv->pushPackage(repl_direction,add_file)) {
      fprintf(stderr,"conveyor_test: --add-file failed\n");
    }
  }
  if(!recv_file.isEmpty()) {
    int id=0;
    if(conv->nextPackageReady(&id,repl_direction)) {
      RDCopy(RDReplConveyor::fileName(id),recv_file);
      conv->popNextPackage(repl_direction);
    }
    else {
      fprintf(stderr,"conveyor_test: no package available\n");
    }
  }

  exit(0);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
