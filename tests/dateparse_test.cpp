// dateparse_test.cpp
//
// Test the Rivendell date/time parser routines.
//
//   (C) Copyright 2019 Fred Gleason <fredg@paravelsystems.com>
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
#include <rddatetime.h>
#include <rdweb.h>

#include "dateparse_test.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  QString time="";
  QString datetime="";
  MainObject::Format format=MainObject::Unknown;
  MainObject::PrintType type=MainObject::None;

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"dateparse_test",
		    DATEPARSE_TEST_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--datetime") {
      datetime=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--print") {
      if(cmd->value(i).toLower()=="date") {
	type=MainObject::Date;
	cmd->setProcessed(i,true);
      }
      if(cmd->value(i).toLower()=="time") {
	type=MainObject::Time;
	cmd->setProcessed(i,true);
      }
      if(cmd->value(i).toLower()=="datetime") {
	type=MainObject::DateTime;
	cmd->setProcessed(i,true);
      }
      if(!cmd->processed(i)) {
	fprintf(stderr,"dataparse_test: unknown --print value\n");
	exit(1);
      }
    }
    if(cmd->key(i)=="--format") {
      if(cmd->value(i).toLower()=="rfc822") {
	format=MainObject::Rfc822;
	cmd->setProcessed(i,true);
      }
      if(cmd->value(i).toLower()=="xml") {
	format=MainObject::Xml;
	cmd->setProcessed(i,true);
      }
      if(cmd->value(i).toLower()=="auto") {
	format=MainObject::Auto;
	cmd->setProcessed(i,true);
      }
      if(!cmd->processed(i)) {
	fprintf(stderr,"dataparse_test: unknown --format value\n");
	exit(1);
      }
    }
    if(cmd->key(i)=="--time") {
      time=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"dateparse_test: unknown option \"%s\"\n",
	      (const char *)cmd->value(i));
      exit(256);
    }
  }
  if(format==MainObject::Unknown) {
    fprintf(stderr,"dateparse_test: no --format specified\n");
    exit(1);
  }
  if(type!=MainObject::None) {
    if(format==MainObject::Auto) {
      fprintf(stderr,"dateparse_test: auto not supported for printing\n");
      exit(1);
    }
    switch(type) {
    case MainObject::Date:
      if(format!=MainObject::Xml) {
	fprintf(stderr,"dateparse_test: date unsupported for RFC822 format\n");
	exit(1);
      }
      printf("XML xs:date: %s\n",
	     (const char *)RDWriteXmlDate(QDate::currentDate()));
      break;

    case MainObject::Time:
      if(format!=MainObject::Xml) {
	fprintf(stderr,"dateparse_test: time unsupported for RFC822 format\n");
	exit(1);
      }
      printf("XML xs:time: %s\n",
	     (const char *)RDWriteXmlTime(QTime::currentTime()));
      break;

    case MainObject::DateTime:
      switch(format) {
      case MainObject::Xml:
	printf("XML xs:dateTime: %s\n",
	       (const char *)RDWriteXmlDateTime(QDateTime::currentDateTime()));
	break;

      case MainObject::Rfc822:
	printf("RFC822: %s\n",
	       (const char *)RDWriteRfc822DateTime(QDateTime::currentDateTime()));
	break;

      case MainObject::Auto:
      case MainObject::Unknown:
	break;
      }
      break;

    case MainObject::None:
      break;
    }
    exit(0);
  }
  if((!datetime.isEmpty())&&(!time.isEmpty())) {
    fprintf(stderr,
	    "dateparse_test: --datetime and --time are mutually exclusive\n");
    exit(256);
  }
  if(datetime.isEmpty()&&time.isEmpty()) {
    fprintf(stderr,
	    "dateparse_test: you must specify --datetime or --time\n");
    exit(256);
  }
  if((!time.isEmpty())&&(format==MainObject::Rfc822)) {
    fprintf(stderr,"dateparse_test: RFC822 format has no --time parser\n");
    exit(1);
  }

  if(!datetime.isEmpty()) {
    QDateTime dt;
    bool ok=false;

    switch(format) {
    case MainObject::Rfc822:
      dt=RDParseRfc822DateTime(datetime,&ok);
      break;

    case MainObject::Xml:
      dt=RDParseXmlDateTime(datetime,&ok);
      break;

    case MainObject::Auto:
      dt=RDParseDateTime(datetime,&ok);
      break;

    case MainObject::Unknown:
      break;
    }
    if(ok) {
      printf("DateTime: %s\n",(const char *)dt.toString("yyyy-MM-dd hh:mm:ss").toUtf8());
    }
    else {
      printf("invalid date/time string\n");
    }
  }

  if(!time.isEmpty()) {
    QTime t;
    int day_offset=0;
    bool ok=false;
    switch(format) {
    case MainObject::Xml:
    case MainObject::Auto:
      t=RDParseXmlTime(time,&ok,&day_offset);
      break;

    case MainObject::Rfc822:
    case MainObject::Unknown:
      break;
    }
    if(ok) {
      QString offset_str="";
      if(day_offset<0) {
	offset_str=QString().sprintf(" (lost %d day)",-day_offset);
      }
      if(day_offset>0) {
	offset_str=QString().sprintf(" (gained %d day)",day_offset);
      }
      printf("Time: %s\n",(const char *)(t.toString("hh:mm:ss")+offset_str).toUtf8());
    }
    else {
      printf("invalid time string\n");
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
