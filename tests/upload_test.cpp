// upload_test.cpp
//
// Test Rivendell file uploading.
//
//   (C) Copyright 2010,2016 Fred Gleason <fredg@paravelsystems.com>
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


#include <rdapplication.h>
#include <rddb.h>
#include <rdupload.h>

#include <QCoreApplication>

#include <upload_test.h>

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  new RDApplication(RDApplication::Console,"upload_test",UPLOAD_TEST_USAGE);

  username="";
  password="";
  RDUpload::ErrorCode conv_err;

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
    if(rda->cmdSwitch()->key(i)=="--source-file") {
      source_filename=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(rda->cmdSwitch()->key(i)=="--destination-url") {
      destination_url=rda->cmdSwitch()->value(i);
      rda->cmdSwitch()->setProcessed(i,true);
    }
  }
  if(source_filename.isEmpty()) {
    fprintf(stderr,"upload_test: missing source-file\n");
    exit(256);
  }
  if(destination_url.isEmpty()) {
    fprintf(stderr,"upload_test: missing destination-url\n");
    exit(256);
  }

  RDUpload *conv=new RDUpload(rda->config()->stationName(),this);
  conv->setSourceFile(source_filename);
  conv->setDestinationUrl(destination_url);
  printf("Uploading...\n");
  conv_err=conv->runUpload(username,password,rda->config()->logXloadDebugData());
  printf("Result: %s\n",(const char *)RDUpload::errorText(conv_err));
  delete conv;

  exit(0);
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
