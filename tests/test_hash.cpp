// test_hash.cpp
//
// Test SHA1 hash generation
//
//   (C) Copyright 2017-2021 Fred Gleason <fredg@paravelsystems.com>
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
#include <rdconfig.h>
#include <rdcut.h>
#include <rddb.h>
#include <rdhash.h>

#include "test_hash.h"

MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  QString filename="";
  QString password="";
  QString hash="";
  
  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=new RDCmdSwitch("test_hash",TEST_HASH_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--filename") {
      filename=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--hash") {
      hash=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--password") {
      password=cmd->value(i);
      cmd->setProcessed(i,true);
    }
  }
  if(filename.isEmpty()&&password.isEmpty()) {
    fprintf(stderr,"test_hash: missing --filename or --password\n");
    exit(256);
  }
  if((!filename.isEmpty())&&(!password.isEmpty())) {
    fprintf(stderr,"test_hash: --filename and --password are mutually exclusive\n");
    exit(256);
  }

  if(!filename.isEmpty()) {  // Hash the specified file
    hash=RDSha1HashFile(filename);
    if(hash.isEmpty()) {
      fprintf(stderr,"test_hash: unable to open \"%s\"\n",
	      filename.toUtf8().constData());
      exit(256);
    }
    printf("%s\n",hash.toUtf8().constData());
    exit(0);
  }

  if((!hash.isEmpty())&&(!password.isEmpty())) {  // Check the specified hash
    if( RDSha1HashCheckPassword(password,hash)) {
      printf("Match!\n");
    }
    else {
      printf("No Match!\n");
    }
    exit(0);
  }

  if(!password.isEmpty()) {  // Generate password hash
    printf("%s\n",RDSha1HashPassword(password).toUtf8().constData());
    exit(0);
  }

  fprintf(stderr,"test_hash: inconsistent arguments given!\n");
  exit(256);
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
