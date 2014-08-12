// reserve_carts_test.cpp
//
// Test the Rivendell db connection routines.
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: reserve_carts_test.cpp,v 1.1.2.1 2014/05/30 00:26:31 cvs Exp $
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

#include <vector>

#include <qapplication.h>
#include <qvariant.h>

#include <rdconfig.h>
#include <rdgroup.h>
#include <rdcmd_switch.h>
#include <rddb.h>

#include "reserve_carts_test.h"

MainObject::MainObject(QObject *parent,const char *name)
  :QObject(parent,name)
{
  RDConfig *config;
  QString group_name;
  unsigned quantity=0;
  bool ok=false;
  RDGroup *group=NULL;
  std::vector<unsigned> cart_nums;
  unsigned schema=0;

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"reserve_carts_test",
		    RESERVE_CARTS_TEST_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--group") {
      group_name=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--quantity") {
      quantity=cmd->value(i).toUInt(&ok);
      if(!ok) {
	fprintf(stderr,"invalid --quantity specified\n");
	exit(256);
      }
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"reserve_carts_test: unknown option \"%s\"\n",
	      (const char *)cmd->value(i));
      exit(256);
    }
  }

  //
  // Sanity Checks
  //
  if(quantity<1) {
    fprintf(stderr,"reserve_carts_test: you must reserve at least one cart\n");
    exit(256);
  }
  if(group_name.isEmpty()) {
    fprintf(stderr,"you must specify a group\n");
    exit(256);
  }

  //
  // Load Configuration
  //
  config=new RDConfig();
  config->load();

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

  //
  // Run the Test
  //
  group=new RDGroup(group_name);
  if(!group->exists()) {
    fprintf(stderr,"group \"%s\" does not exist\n",
	    (const char *)group_name.utf8());
    exit(256);
  }
  if(group->reserveCarts(&cart_nums,config->stationName(),RDCart::Audio,
			 quantity)) {
    printf("reserved the following carts:\n");
    for(unsigned i=0;i<cart_nums.size();i++) {
      printf("%06u\n",cart_nums[i]);
    }
  }
  else {
    printf("reservation failed\n");
  }

  exit(0);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject(NULL,"main");
  return a.exec();
}
