// mcast_recv_test.cpp
//
// Test the Rivendell multicast receiver routines
//
//   (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdint.h>
#include <stdlib.h>

#include <qapplication.h>
#include <qhostaddress.h>
#include <qstringlist.h>

#include <rdcmd_switch.h>

#include "mcast_recv_test.h"

MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  QHostAddress from_addr;
  unsigned from_port=0;
  bool ok=false;

  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"mcast_recv_test",
		    MCAST_RECV_TEST_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--from") {
      QStringList f0=f0.split(":",cmd->value(i));
      if(f0.size()!=2) {
	fprintf(stderr,"mcast_recv_test: invalid argument to \"--from\"\n");
	exit(1);
      }
      if(!from_addr.setAddress(f0[0])) {
	fprintf(stderr,"mcast_recv_test: invalid address in \"--from\"\n");
	exit(1);
      }
      from_port=f0[1].toUInt(&ok);
      if(!ok) {
	fprintf(stderr,"mcast_recv_test: invalid port in \"--from\"\n");
	exit(1);
      }
      if((from_port==0)||(from_port>=65536)) {
	fprintf(stderr,"mcast_recv_test: invalid port in \"--from\"\n");
	exit(1);
      }
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"mcast_recv_test: unknown option \"%s\"\n",
	      (const char *)cmd->value(i));
      exit(256);
    }
  }
  if(from_addr.isNull()) {
    fprintf(stderr,"mcast_recv_test: you must specify a multicast address in \"--from\"\n");
    exit(1);
  }
  if(from_port==0) {
    fprintf(stderr,"mcast_recv_test: you must specify a UDP port in \"--from\"\n");
    exit(1);
  }
  mcast_multicaster=new RDMulticaster(this);
  connect(mcast_multicaster,
	  SIGNAL(received(const QString &,const QHostAddress &)),
	  this,SLOT(receivedData(const QString &,const QHostAddress &)));
  if(!mcast_multicaster->bind(from_port)) {
    fprintf(stderr,"mcast_recv_test: unable to bind port\n");
    exit(1);
  }
  mcast_multicaster->subscribe(from_addr);
  printf("listening for %s at %u\n",(const char *)from_addr.toString(),
	 0xFFFF&from_port);
}


void MainObject::receivedData(const QString &msg,const QHostAddress &src_addr)
{
  printf("%15s: %s\n",(const char *)src_addr.toString(),
	 (const char *)msg);
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
