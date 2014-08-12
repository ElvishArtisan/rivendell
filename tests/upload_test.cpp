// upload_test.cpp
//
// Test Rivendell file uploading.
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: upload_test.cpp,v 1.3 2011/06/21 22:20:44 cvs Exp $
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

#include <qapplication.h>

#include <rddb.h>
#include <rdcmd_switch.h>
#include <rdupload.h>

#include <upload_test.h>

MainObject::MainObject(QObject *parent,const char *name)
  :QObject(parent,name)
{
  username="";
  password="";
  RDUpload::ErrorCode conv_err;
  unsigned schema=0;

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"upload_test",
  		    UPLOAD_TEST_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--username") {
      username=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--password") {
      password=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--source-file") {
      source_filename=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--destination-url") {
      destination_url=cmd->value(i);
      cmd->setProcessed(i,true);
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

  //
  // Read Configuration
  //
  rdconfig=new RDConfig();
  rdconfig->load();

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

  RDUpload *conv=new RDUpload(rdconfig->stationName(),this);
  conv->setSourceFile(source_filename);
  conv->setDestinationUrl(destination_url);
  printf("Uploading...\n");
  conv_err=conv->runUpload(username,password,rdconfig->logXloadDebugData());
  printf("Result: %s\n",(const char *)RDUpload::errorText(conv_err));
  delete conv;

  exit(0);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject(NULL,"main");
  return a.exec();
}
