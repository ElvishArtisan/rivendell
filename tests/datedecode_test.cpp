// datedecode_test.cpp
//
// Test the Rivendell db connection routines.
//
//   (C) Copyright 2012-2021 Fred Gleason <fredg@paravelsystems.com>
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
#include <rddatedecode.h>
#include <rddb.h>

#include "datedecode_test.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  QString date;
  QString datetime;
  QString service="";
  int schema=0;

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=new RDCmdSwitch("datedecode_test",DATEDECODE_TEST_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--date") {
      date=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--datetime") {
      datetime=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--service") {
      service=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"datedecode_test: unknown option \"%s\"\n",
	      cmd->value(i).toUtf8().constData());
      exit(256);
    }
  }
  if((!date.isNull())&&(!datetime.isNull())) {
    fprintf(stderr,
	    "datedecode_test: --date and --datetime are mutually exclusive\n");
    exit(256);
  }
  if(date.isNull()&&datetime.isNull()) {
    fprintf(stderr,
	    "datedecode_test: you must specify either --date or --datetime\n");
    exit(256);
  }

  //
  // Open Config
  //
  RDConfig *config=new RDConfig();
  config->load();

  //
  // Open Database
  //
  QString err (tr("datedecode_test: "));
  if(!RDOpenDb(&schema,&err,config)) {
    fprintf(stderr,"%s\n",err.toUtf8().constData());
    delete cmd;
    exit(256);
  }
  RDStation *station=new RDStation(config->stationName());

  //
  // Process Code
  //
  if(!date.isNull()) {
    printf("%s\n",
	   RDDateDecode(date,QDate::currentDate(),station,config,service).
	   toUtf8().constData());;
  }
  if(!datetime.isNull()) {
    printf("%s\n",RDDateTimeDecode(datetime,QDateTime(QDate::currentDate(),
						      QTime::currentTime()),
				   station,config,service).toUtf8().constData());
  }
  exit(0);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
