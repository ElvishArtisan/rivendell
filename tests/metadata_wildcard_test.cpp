// metadata_wildcard_test.cpp
//
// Test the Rivendell multicast receiver routines
//
//   (C) Copyright 2018-2022 Fred Gleason <fredg@paravelsystems.com>
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

#include <rd.h>
#include <rdapplication.h>
#include <rdcmd_switch.h>
#include <rdlog_line.h>

#include "metadata_wildcard_test.h"

MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  unsigned cartnum=0;
  int cutnum=-1;
  QString code;
  bool ok=false;
  QString err_msg;

  //
  // Open the Database
  //
  rda=new RDApplication("metadata_wildcard_test","metadata_wildcard_test",
			METADATA_WILDCARD_TEST_USAGE,false,this);
  if(!rda->open(&err_msg,NULL,true)) {
    fprintf(stderr,"metadata_wildcard_test: %s\n",(const char *)err_msg.toUtf8());
    exit(1);
  }
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(rda->cmdSwitch()->key(i)=="--cart") {
      cartnum=rda->cmdSwitch()->value(i).toUInt(&ok);
      if((!ok)||(cartnum==0)||(cartnum>RD_MAX_CART_NUMBER)) {
	fprintf(stderr,"metadata_wildcard_test: invalid cart number\n");
	exit(1);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--cut") {
      cutnum=rda->cmdSwitch()->value(i).toInt(&ok);
      if((!ok)||(cutnum<=0)||(cartnum>RD_MAX_CUT_NUMBER)) {
	fprintf(stderr,"metadata_wildcard_test: invalid cut number\n");
	exit(1);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--code") {
      code=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(!rda->cmdSwitch()->processed(i)) {
      fprintf(stderr,"metadata_wildcard_test: unknown option \"%s\"\n",
	      rda->cmdSwitch()->value(i).toUtf8().constData());
      exit(256);
    }
  }
  if(cartnum==0) {
    fprintf(stderr,"metadata_wildcard_test: you must provide a --cart\n");
    exit(1);
  }
  if(code.isEmpty()) {
    fprintf(stderr,"metadata_wildcard_test: you must provide a --code\n");
    exit(1);
  }

  printf("Result: %s\n",
      (const char *)RDLogLine::resolveWildcards(cartnum,code,cutnum).toUtf8());

  exit(0);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
