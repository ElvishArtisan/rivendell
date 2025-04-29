// sha1_test.h
//
// Test Rivendell SHA1 methods.
//
//   (C) Copyright 2025 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdio.h>
#include <stdlib.h>

#include <QCoreApplication>

#include <rdcmd_switch.h>
#include <rdconf.h>
#include <rdhash.h>

#include "sha1_test.h"

MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  QString verify_filename;
  QString verify_password;
  QString verify_string;
  QString verify_hash;

  RDCmdSwitch *cmd=new RDCmdSwitch("sha1_test",SHA1_TEST_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--verify-hash") {
      verify_hash=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--verify-filename") {
      verify_filename=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--verify-password") {
      verify_password=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--verify-string") {
      verify_string=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"sha1_test: unknown option\n");
      exit(1);
    }
  }

  if(!verify_password.isEmpty()) {
    if(verify_hash.isEmpty()) {
      QString hash=RDSha1HashPassword(verify_password);
      printf("HASH: %s\n",hash.toUtf8().constData());
      printf("VERIFIED: %s\n",
	     RDYesNo(RDSha1HashCheckPassword(verify_password,hash)).
	     toUtf8().constData());
    }
    else {
      printf("VERIFIED: %s\n",
	     RDYesNo(RDSha1HashCheckPassword(verify_password,verify_hash)).
	     toUtf8().constData());
    }
    exit(0);
  }

  if(!verify_string.isEmpty()) {
    printf("STRING: %s\n",verify_string.toUtf8().constData());
    printf("HASH: %s\n",RDSha1HashData(verify_string.toUtf8()).
	   toUtf8().constData());
    exit(0);
  }

  if(!verify_filename.isEmpty()) {
    printf("FILE: %s\n",verify_filename.toUtf8().constData());
    printf("HASH: %s\n",RDSha1HashFile(verify_filename.toUtf8()).
	   toUtf8().constData());
    exit(0);
  }

  exit(0);
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
