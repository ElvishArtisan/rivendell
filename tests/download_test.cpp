// download_test.cpp
//
// Test Rivendell file downloading.
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
//#include <rdcmd_switch.h>
#include <rddownload.h>

#include "download_test.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  QString err_msg;

  username="";
  password="";
  RDDownload::ErrorCode conv_err;
  use_identity_file=false;

  //
  // Open the Database
  //
  rda=new RDApplication("download_test","download_test",DOWNLOAD_TEST_USAGE,this);
  if(!rda->open(&err_msg)) {
    fprintf(stderr,"download_test: %s\n",(const char *)err_msg);
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
    if(rda->cmdSwitch()->key(i)=="--source-url") {
      source_url=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--destination-file") {
      destination_file=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(!rda->cmdSwitch()->processed(i)) {
      fprintf(stderr,"download_test: unknown command option \"%s\"\n",
	      (const char *)rda->cmdSwitch()->key(i));
      exit(2);
    }
  }

  //
  // Sanity Checks
  //
  if(source_url.isEmpty()) {
    fprintf(stderr,"download_test: missing source-url\n");
    exit(256);
  }
  if(destination_file.isEmpty()) {
    fprintf(stderr,"download_test: missing destination-filename\n");
    exit(256);
  }

  //
  // Run the Test
  //
  RDDownload *conv=new RDDownload(rda->config(),this);
  conv->setSourceUrl(source_url);
  conv->setDestinationFile(destination_file);
  printf("Downloading...\n");
  conv_err=conv->
    runDownload(username,password,ssh_identity_filename,use_identity_file,
		rda->config()->logXloadDebugData());
  printf("Result: %s\n",(const char *)RDDownload::errorText(conv_err));
  delete conv;

  exit(0);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
