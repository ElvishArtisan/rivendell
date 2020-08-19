// delete_test.cpp
//
// Test Rivendell file deletion routines.
//
//   (C) Copyright 2010-2020 Fred Gleason <fredg@paravelsystems.com>
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

#include <qapplication.h>

#include <rdapplication.h>
#include <rddb.h>
#include <rddelete.h>

#include "delete_test.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  QString err_msg;

  username="";
  password="";
  RDDelete::ErrorCode conv_err;
  use_identity_file=false;

  //
  // Open the Database
  //
  rda=new RDApplication("delete_test","delete_test",DELETE_TEST_USAGE,this);
  if(!rda->open(&err_msg)) {
    fprintf(stderr,"delete_test: %s\n",(const char *)err_msg);
    exit(1);
  }

  //
  // Read Command Options
  //
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(rda->cmdSwitch()->key(i)=="--username") {
      username=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--password") {
      password=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--ssh-identity-filename") {
      ssh_identity_filename=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--use-identity-file") {
      use_identity_file=true;
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--target-url") {
      target_url=QUrl(rda->cmdSwitch()->value(i));
      if(target_url.isRelative()) {
	fprintf(stderr,"delete_test: URL's must be fully qualified\n");
	exit(1);
      }
      if(!target_url.isValid()) {
	fprintf(stderr,"delete_test: invalid URL\n");
	exit(1);
      }
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(!rda->cmdSwitch()->processed(i)) {
      fprintf(stderr,"delete_test: unknown command option \"%s\"\n",
	      (const char *)rda->cmdSwitch()->key(i));
      exit(2);
    }
  }

  //
  // Sanity Checks
  //
  if(target_url.isEmpty()) {
    fprintf(stderr,"delete_test: missing --target-url\n");
    exit(256);
  }

  //
  // Run the Test
  //
  RDDelete *conv=new RDDelete(rda->config(),this);
  if(!conv->urlIsSupported(target_url)) {
    fprintf(stderr,"delete_test: unsupported URL scheme\n");
    exit(1);
  }
  conv->setTargetUrl(target_url);
  printf("Deleting...\n");
  conv_err=conv->
    runDelete(username,password,ssh_identity_filename,use_identity_file,
	      rda->config()->logXloadDebugData());
  printf("Result: %s\n",(const char *)RDDelete::errorText(conv_err));
  delete conv;

  exit(0);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
