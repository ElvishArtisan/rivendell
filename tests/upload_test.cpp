// upload_test.cpp
//
// Test Rivendell file uploading.
//
//   (C) Copyright 2010-2023 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdapplication.h>
#include <rddb.h>
//#include <rdcmd_switch.h>
#include <rdupload.h>

#include "upload_test.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  QString err_msg;

  username="";
  password="";
  RDUpload::ErrorCode conv_err;
  use_identity_file=false;
  create_dirs=false;

  //
  // Open the Database
  //
  rda=new RDApplication("upload_test","upload_test",UPLOAD_TEST_USAGE,false,
			this);
  if(!rda->open(&err_msg,NULL,true,false)) {
    fprintf(stderr,"upload_test: %s\n",err_msg.toUtf8().constData());
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
    if(rda->cmdSwitch()->key(i)=="--create-dirs") {
      create_dirs=true;
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--source-file") {
      source_filename=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--destination-url") {
      destination_url=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(!rda->cmdSwitch()->processed(i)) {
      fprintf(stderr,"rdrepld: unknown command option \"%s\"\n",
	      rda->cmdSwitch()->key(i).toUtf8().constData());
      exit(2);
    }
  }

  //
  // Sanity Checks
  //
  if(source_filename.isEmpty()) {
    fprintf(stderr,"upload_test: missing source-file\n");
    exit(256);
  }
  if(destination_url.isEmpty()) {
    fprintf(stderr,"upload_test: missing destination-url\n");
    exit(256);
  }

  //
  // Run the Test
  //
  err_msg="";
  RDUpload *conv=new RDUpload(rda->config(),this);
  conv->setSourceFile(source_filename);
  conv->setDestinationUrl(destination_url);
  conv->createDestinationDirs(create_dirs);
  printf("Uploading...\n");
  conv_err=conv->
    runUpload(username,password,ssh_identity_filename,use_identity_file,
	      &err_msg,rda->config()->logXloadDebugData());
  printf("Result: %s [%s]\n",RDUpload::errorText(conv_err).toUtf8().constData(),
	 err_msg.toUtf8().constData());
  delete conv;

  exit(0);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
