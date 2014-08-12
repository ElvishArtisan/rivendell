// datedecode_test.cpp
//
// Test the Rivendell db connection routines.
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: datedecode_test.cpp,v 1.1.2.3 2013/10/16 21:14:37 cvs Exp $
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
#include <rddatedecode.h>

#include "datedecode_test.h"

MainObject::MainObject(QObject *parent,const char *name)
  :QObject(parent,name)
{
  QString date="";
  QString datetime="";

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"datedecode_test",
		    DATEDECODE_TEST_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--date") {
      date=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--datetime") {
      datetime=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"datedecode_test: unknown option \"%s\"\n",
	      (const char *)cmd->value(i));
      exit(256);
    }
  }
  if((!date.isEmpty())&&(!datetime.isEmpty())) {
    fprintf(stderr,
	    "datedecode_test: --date and --datetime are mutually exclusive\n");
    exit(256);
  }
  if(date.isEmpty()&&datetime.isEmpty()) {
    fprintf(stderr,
	    "datedecode_test: you must specify either --date or --datetime\n");
    exit(256);
  }

  //
  // Process Code
  //
  if(!date.isEmpty()) {
    printf("%s\n",
	   (const char *)RDDateDecode(date,QDate::currentDate()));
  }
  if(!datetime.isEmpty()) {
    printf("%s\n",(const char *)RDDateTimeDecode(datetime,QDateTime(QDate::currentDate(),QTime::currentTime())));
  }
  exit(0);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject(NULL,"main");
  return a.exec();
}
