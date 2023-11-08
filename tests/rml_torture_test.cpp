// rml_torture_test.cpp
//
// Generate a series of Rivendell GPIO events.
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <QCoreApplication>
#include <QUdpSocket>

#include <rd.h>
#include <rdcmd_switch.h>
#include <rdcoreapplication.h>

#include "rml_torture_test.h"

MainObject::MainObject(QObject *parent)
{
  QStringList rmls;
  QHostAddress host_address("127.0.0.1");
  int interval=1000;
  bool verbose=false;
  bool ok=false;
  int offset=0;

  RDCmdSwitch *cmd=new RDCmdSwitch("rml_torture_test",RML_TORTURE_TEST_USAGE);

  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--rml") {
      rmls.push_back(cmd->value(i));
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--host-address") {
      if(!host_address.setAddress(cmd->value(i))) {
	fprintf(stderr,"rml_torture_test: invalid --host-address\n");
	exit(RDCoreApplication::ExitInvalidOption);
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--interval") {
      interval=cmd->value(i).toInt(&ok);
      if((!ok)||(interval<=0)) {
	fprintf(stderr,"rml_torture_test: invalid --interval\n");
	exit(RDCoreApplication::ExitInvalidOption);
      }
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--verbose") {
      verbose=true;
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"rml_torture_test: unknown option \"%s\"\n",
	      cmd->key(i).toUtf8().constData());
      exit(RDCoreApplication::ExitInvalidOption);
      cmd->setProcessed(i,true);
    }
  }
  if(rmls.size()==0) {
    fprintf(stderr,"rml_torture_test: no RML commands specified\n");
    exit(RDCoreApplication::ExitInvalidOption);
  }

  //
  // Send Socket
  //
  QUdpSocket *send_socket=new QUdpSocket(this);

  while(1==1) {
    if(verbose) {
      printf("%s\n",rmls.at(offset).toUtf8().constData());
    }
    send_socket->
      writeDatagram(rmls.at(offset).toUtf8(),host_address,RD_RML_NOECHO_PORT);
    offset+=1;
    offset=offset%rmls.size();
    usleep(1000*interval);
  }

  exit(0);
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
