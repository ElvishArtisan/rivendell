// mmpublish.cpp
//
// Send "publishSchedule" command to Nexus Server
//
//   Patrick Linstruth <patrick@deltecent.com>
//   (C) Copyright 2019 Fred Gleason <fredg@paravelsystems.com>
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

#include <errno.h>
#include <stdio.h>

#include <QApplication>
#include <QDomDocument>
#include <QDomElement>
#include <QMap>
#include <QDebug>

#include "rdapplication.h"
#include "rdcart.h"
#include "rdnexus.h"

#include "mmpublish.h"

MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  QString err_msg;

  mm_debug=false;

  rda=new RDApplication("mmpublish","mmpublish",MMPUBLISH_USAGE,this);
  if(!rda->open(&err_msg)) {
    fprintf(stderr,"mmpublish: %s\n",(const char *)err_msg);
    exit(1);
  }

  //
  // Load Nexus Server Information
  //
  nexus = new RDNexus();

  nexus->nexusServer(server);

  //
  // Process Command Switches
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"mmschedule",MMPUBLISH_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--server") {
      server.address=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--port") {
      server.port=cmd->value(i).toInt();
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--station") {
      mm_station=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--start") {
      mm_start=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--end") {
      mm_end=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--debug") {
      mm_debug=true;
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"mmschedule: unrecognized option \"%s\"\n",
	      (const char *)cmd->key(i));
      exit(1);
    }
  }

  if(server.address.isEmpty()) {
    fputs("You must specify a Nexus server address\n",stderr);
    exit(1);
  }

  if(server.port<1024||server.port>65535) {
    fputs("You must specify a Nexus server port\n",stderr);
    exit(1);
  }

  if(mm_station.isEmpty()) {
    fputs("You must specify a Nexus station\n",stderr);
    exit(1);
  }

  if(mm_start.isEmpty()) {
    fprintf(stderr,"You must specify a start date (%s)\n",RDNEXUS_DATETIME);
    exit(1);
  }

  if(mm_end.isEmpty()) {
    fprintf(stderr,"You must specify an end date (%s)\n",RDNEXUS_DATETIME);
    exit(1);
  }

  nexus->setDebug(mm_debug);

  nexus->setServer(server.address,server.port,3);
  nexus->setStation(mm_station);

  nexus->publishSchedule(
    QDateTime::fromString(mm_start,RDNEXUS_DATETIME_HOUR),
    QDateTime::fromString(mm_end,RDNEXUS_DATETIME));

  if (!nexus->isValid()) {
    qDebug() << "responseStatus:" << nexus->responseStatus();
    qDebug() << "responseCommand:" << nexus->responseCommand();
    qDebug() << "responseMessageId:" << nexus->responseMessageId();
    qDebug() << "responseError:" << nexus->responseError();
    exit(1);
  }

  exit(0);
}

int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
