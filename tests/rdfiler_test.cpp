// rdfiler_test.cpp
//
// Test the Rivendell RDFiler class.
//
//   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <QCoreApplication>
#include <QDir>

#include <rdapplication.h>
#include <rdcmd_switch.h>
#include <rdconfig.h>
#include <rdfiler.h>

#include "rdfiler_test.h"

MainObject::MainObject()
  : QObject()
{
  QString user;
  QString root_directory;
  QString operation;
  QString filename;

  //
  // Check that we are root
  //
  if(geteuid()!=0) {
    fprintf(stderr,"this program must be run as root\n");
    exit(RDApplication::ExitNoPerms);
  }

  //
  // Process command line switches
  //
  RDCmdSwitch *cmd=new RDCmdSwitch("rdfiler_test",RDFILER_TEST_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--filename") {
      filename=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--operation") {
      if((cmd->value(i)!="create")&&
	 (cmd->value(i)!="read")&&
	 (cmd->value(i)!="unlink")&&
	 (cmd->value(i)!="write")) {
	fprintf(stderr,"rdfiler_test: unknown operation \"%s\"\n",
		cmd->value(i).toUtf8().constData());
	exit(RDApplication::ExitInvalidOption);
      }
      operation=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--root-dir") {
      QDir dir(cmd->value(i));
      if(!dir.exists()) {
	fprintf(stderr,"rdfiler_test: no such root directory\n");
	exit(RDApplication::ExitInvalidOption);
      }
      root_directory=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--user") {
      user=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"unrecognized switch \"%s\"\n",
	      cmd->key(i).toUtf8().constData());
      exit(RDApplication::ExitInvalidOption);
    }
  }
  if(filename.isEmpty()) {
    fprintf(stderr,"rdfiler_test: you must specify \"--filename\"\n");
    exit(RDApplication::ExitInvalidOption);
  }
  if(operation.isEmpty()) {
    fprintf(stderr,"rdfiler_test: you must specify \"--operation\"\n");
    exit(RDApplication::ExitInvalidOption);
  }
  if(root_directory.isEmpty()) {
    fprintf(stderr,"rdfiler_test: you must specify \"--root-dir\"\n");
    exit(RDApplication::ExitInvalidOption);
  }
  if(user.isEmpty()) {
    fprintf(stderr,"rdfiler_test: you must specify \"--user\"\n");
    exit(RDApplication::ExitInvalidOption);
  }

  //
  // Get configuration
  //
  RDConfig *config=new RDConfig();
  if(!config->load()) {
    fprintf(stderr,"rdfiler_test: unable to open Rivendell configuration\n");
    exit(RDApplication::ExitNoConfig);
  }

  //
  // Create filer object
  //
  RDFiler *filer=new RDFiler(config->uid(),config->gid(),root_directory);
  if(!filer->start()) {
    fprintf(stderr,"rdfiler_test: unable to start filer object\n");
    delete filer;
    exit(RDApplication::ExitSyscallError);
  }

  //
  // Run the operation
  //
  if(operation=="read") {
    int fd=-1;

    printf("\n");
    printf("attempting to open \"%s/%s\": ",
	   root_directory.toUtf8().constData(),
	   filename.toUtf8().constData());
    fflush(stdout);
    if((fd=filer->open(filename,O_RDONLY))>=0) {
      printf("success\n");
    }
    else {
      printf("failure\n");
    }
  }
  if(operation=="unlink") {
    printf("\n");
    printf("attempting to unlink \"%s/%s\": ",
	   root_directory.toUtf8().constData(),
	   filename.toUtf8().constData());
    fflush(stdout);
    if(filer->unlink(filename)) {
      printf("success\n");
    }
    else {
      printf("failure\n");
    }
  }

  //
  // Wait for exit
  //
  printf("\n");
  printf("Hit CTRL-C to quit\n");
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);

  new MainObject();

  return a.exec();
}
