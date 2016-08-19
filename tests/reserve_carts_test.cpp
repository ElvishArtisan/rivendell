// reserve_carts_test.cpp
//
// Test the Rivendell db connection routines.
//
//   (C) Copyright 2012,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <QCoreApplication>
#include <QVariant>

#include <rdapplication.h>
#include <rdgroup.h>
#include <rddb.h>

#include "reserve_carts_test.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  new RDApplication(RDApplication::Console,"reserve_carts_test",
		    RESERVE_CARTS_TEST_USAGE);
  QString group_name;
  unsigned quantity=0;
  bool ok=false;
  RDGroup *group=NULL;
  std::vector<unsigned> cart_nums;

  //
  // Read Command Options
  //
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(rda->cmdSwitch()->key(i)=="--group") {
      group_name=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--quantity") {
      quantity=rda->cmdSwitch()->value(i).toUInt(&ok);
      if(!ok) {
	fprintf(stderr,"invalid --quantity specified\n");
	exit(256);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(!rda->cmdSwitch()->processed(i)) {
      fprintf(stderr,"reserve_carts_test: unknown option \"%s\"\n",
	      (const char *)rda->cmdSwitch()->value(i));
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
  // Run the Test
  //
  group=new RDGroup(group_name);
  if(!group->exists()) {
    fprintf(stderr,"group \"%s\" does not exist\n",
	    (const char *)group_name.utf8());
    exit(256);
  }
  if(group->reserveCarts(&cart_nums,rda->config()->stationName(),RDCart::Audio,
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
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
