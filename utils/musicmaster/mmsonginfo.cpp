// mmsonginfo.cpp
//
// MusicMaster Sync Utility
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
#include <QTextStream>
#include <QDebug>

#include "rdapplication.h"
#include "rdconf.h"
#include "rdgroup_list.h"

#include "mmsonginfo.h"

//
// Output streams
//
QTextStream print(stdout);
QTextStream error(stderr);

MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  QString err_msg;

  mm_debug=false;

  rda=new RDApplication("mmsonginfo","mmsonginfo",MMSONGINFO_USAGE,this);
  if(!rda->open(&err_msg)) {
    fprintf(stderr,"mmsonginfo: %s\n",(const char *)err_msg);
    exit(1);
  }

  nexus = new RDNexus();

  nexus->nexusServer(server);

  //
  // Process Command Switches
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"mmsonginfo",MMSONGINFO_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--debug") {
      mm_debug=true;
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--station") {
      mm_station=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--songid") {
      mm_songid=cmd->value(i).toInt();
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--cartnum") {
      mm_cartnum=cmd->value(i).toInt();
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--server") {
      server.address=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--port") {
      server.port=cmd->value(i).toInt();
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"mmsonginfo: unrecognized option \"%s\"\n",
	      (const char *)cmd->key(i));
      exit(1);
    }
  }

  if(mm_station.isEmpty()) {
    fputs("You must specify a Nexus station\n",stderr);
    exit(1);
  }

  if(server.address.isEmpty()) {
    fputs("You must specify a Nexus server\n",stderr);
    exit(1);
  }

  if(server.port<1024||server.port>65535) {
    fputs("You must specify a Nexus server port\n",stderr);
    exit(1);
  }

  nexus->setDebug(mm_debug);
  nexus->setServer(server.address,server.port,5);
  nexus->setStation(mm_station);

  songInfo();

  exit(0);
}

void MainObject::songInfo()
{
  RDNexusSongInfoList songlist;
  nexus->getSongsByQuery(songlist);

  for(int i=0;i<songlist.size();i++) {
    print << QString().sprintf("%06d %06d %-30.30s %-30.30s",
      songlist.at(i).songid,
      songlist.at(i).cutid,
      (const char *)songlist.at(i).artist,
      (const char *)songlist.at(i).title)
      << endl;
  }
}

int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
