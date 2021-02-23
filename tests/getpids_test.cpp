// getpids_test.cpp
//
// Test the Rivendell RDGetPids() function.
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

#include <rdcmd_switch.h>
#include <rdconf.h>
#include <rddatedecode.h>
#include <rddb.h>

#include "getpids_test.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  QString program="";
  QList<pid_t> pids;

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"getpids_test",
		    GETPIDS_TEST_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--program") {
      program=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"getpids_test: unknown option \"%s\"\n",
	      cmd->value(i).toUtf8().constData());
      exit(256);
    }
  }
  if(program.isEmpty()) {
    fprintf(stderr,
	    "getpids_test: you must specify a --program\n");
    exit(256);
  }

  pids=RDGetPids(program);

  printf("Found PIDS:\n");
  for(int i=0;i<pids.size();i++) {
    printf("%d\n",pids[i]);
  }

  exit(0);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
