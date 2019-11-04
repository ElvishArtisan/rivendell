// mmsync.cpp
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

#include "mmsync.h"

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
  mm_test=false;
  mm_schedmap="mmsync.ini";

  rda=new RDApplication("mmsync","mmsync",MMSYNC_USAGE,this);
  if(!rda->open(&err_msg)) {
    fprintf(stderr,"mmsync: %s\n",(const char *)err_msg);
    exit(1);
  }

  nexus = new RDNexus();

  nexus->nexusServer(server);

  //
  // Process Command Switches
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"mmsync",MMSYNC_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--debug") {
      mm_debug=true;
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--station") {
      mm_station=cmd->value(i);
      cmd->setProcessed(i,true);
    }
    if(cmd->key(i)=="--schedmap") {
      mm_schedmap=cmd->value(i);
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
    if(cmd->key(i)=="--test") {
      mm_test=true;
      cmd->setProcessed(i,true);
    }
    if(!cmd->processed(i)) {
      fprintf(stderr,"mmsync: unrecognized option \"%s\"\n",
	      (const char *)cmd->key(i));
      exit(1);
    }
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

  schedmap=new RDProfile();
  if(!schedmap->setSource(mm_schedmap)) {
    fprintf(stderr,"Could not open scheduler code map file '%s'\n",(const char *)mm_schedmap);
    exit(1);
  }

  missingFromMusicMaster();
//  removeMusicMasterOrphans();

  exit(0);
}

//
// Add missing carts to MusicMaster library
//
void MainObject::missingFromMusicMaster()
{
  QMap<QString,RDNexusSongInfoList > songlists;
  RDNexusStationMapList stationmaplist;
  QString sql;
  RDSqlQuery *q;

  print << "Adding missing songs to MusicMaster libraries:" << endl;

  sql="select NUMBER from CART order by NUMBER";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    unsigned cartnum=q->value(0).toUInt();

    RDCart *cart=new RDCart(cartnum);

    RDNexusStationMapList stationmaplist;
    nexus->groupStations(cart->groupName(),stationmaplist);

    for(int i=0;i<stationmaplist.size();i++) {
      if(stationmaplist.at(i).station.isEmpty()) {
        continue;
      }

      nexus->setStation(stationmaplist.at(i).station);

      //
      // If we have not loaded songs for this station, load them
      //
      RDNexusSongInfoList songlist;
      if(!songlists.contains(nexus->station())) {
        print << QString("  Loading song library from %1:%2").arg(server.address).arg(nexus->station()) << endl;

        if(nexus->getSongsByQuery(songlist)) {
          songlists.insert(nexus->station(),songlist);
        }
      }
      else {
        songlist=songlists.value(nexus->station());
      }

      if(!nexus->exists((int)cart->number(),RDNexus::CutId,songlist)) {
        print << QString("  Cart %1 missing from station %2").arg(cartnum).arg(nexus->station()) << endl;

        RDNexusSongInfo info;
        setSongInfo(cart,info);

        //
        // See if we can get a match by Artist/Title
        //
        int songid=nexus->exists(cart->artist(),cart->title(),songlist);

        if(songid!=-1) {
          print << QString("  Found %1/%2 with Song ID %3").arg(cart->artist()).arg(cart->title()).arg(songid) << endl;

          info.songid=songid;

          if(!mm_test) {
            nexus->updateSong(info);
          }

          continue;
        }

        //
        // Match First Scheduler Code Map
        //
        QStringList schedcodes=cart->schedCodesList();
        for(int s=0;s<schedcodes.size();s++) {
          QString category=schedmap->stringValue("SchedCodes",schedcodes.at(s).toUpper());
          if(!category.isEmpty()) {
            info.category=category;
            break;
          }
        }

        //
        // If category could not be found based on scheduler codes, try group
        //
        if(info.category.isEmpty()) {
          info.category=schedmap->stringValue("Groups",cart->groupName().toUpper());
        }

        //
        // If we could not find a suitable category, set category to default
        //
        if(info.category.isEmpty()) {
          info.category=stationmaplist.at(i).category;
        }

        if(!mm_test) {
          nexus->importSong(info);
        }
      }
    }

    delete cart;
  }

  delete q;
}

//
// Remove orphaned songs from MusicMaster library
//
void MainObject::removeMusicMasterOrphans()
{
  RDNexusStationMapList stationmaplist;
  nexus->stationMapList(stationmaplist);

  print << "Removing orphaned songs from MusicMaster libraries:" << endl;

  for(int i=0;i<stationmaplist.size();i++) {
    if(stationmaplist.at(i).station.isEmpty()) {
      continue;
    }

    nexus->setStation(stationmaplist.at(i).station);

    print << QString("  Loading song library from %1:%2").arg(server.address).arg(nexus->station()) << endl;

    RDNexusSongInfoList songlist;
    if(nexus->getSongsByQuery(songlist)) {
      for(int s=0;s<songlist.size();s++) {
        RDCart *cart=new RDCart(songlist.at(s).cutid);

        if(!cart->exists()) {
          if(songlist.at(s).cutid) {
            print << QString("  Cart %1 does not exist. Removing from songid %2 from %3.").arg(songlist.at(s).cutid).arg(songlist.at(s).songid).arg(nexus->station()) << endl;
          }
          else {
            print << QString("  SongId %1 does not have a Cart Number. Removing songid %1 from %2.").arg(songlist.at(s).songid).arg(nexus->station()) << endl;
          }
          if(!mm_test) {
            nexus->deleteSong(songlist.at(s));
          }
        }

        delete cart;
      }
    }
  }

#if 0
  RDNexusMap fieldmap;
  RDNexusMap songmap;
  QStringList songlist;
  QStringList deleteList;

  nexus->getSongsByQuery(mm_cartnum,fieldmap,songmap);

  songlist=songmap.keys();

  for(int i=0;i<songlist.size();i++) {
    RDCart *cart=new RDCart(songmap.value(songlist.at(i)).toInt());
    if (!cart->exists()) {
      printf("Removing orphan songid=%s cart=%s\n",(const char *)songlist.at(i),(const char *)songmap.value(songlist.at(i)));
      deleteList.append(songlist.at(i));
    }
    delete cart;
  }

  if(deleteList.size()) {
    nexus->deleteSongs(deleteList);
  }
#endif
}

void MainObject::setSongInfo(RDCart *cart,RDNexusSongInfo &info)
{
  info.clear();

  info.cutid=cart->number();
  info.artist=cart->artist();
  info.title=cart->title();
  if(int year=cart->year()) {
    info.year=QString::number(year);
  }
  info.runtime=RDGetTimeLength(cart->forcedLength(),false,false);
  info.intro=RDGetTimeLength(cart->minimumTalkLength(),false,false);
  info.ending=RDGetTimeLength(cart->averageSegueLength(),false,false);
  info.group=cart->groupName();
  info.notes=cart->notes();
}

void MainObject::importSong(int cartnum)
{
#if 0
  RDNexusMap fieldmap;
  RDCart *cart;
  int songid;

  cart=new RDCart(cartnum);

  printf("Importing song %06d to MusicMaster...",cartnum);

  if (!cart->exists()) {
    printf("Error: cart does not exist\n");
  }

  cartFields(cart,fieldmap);

  songid=nexus->importSong(fieldmap);

  if(songid) {
    printf("Added Song ID %d\n",songid);
  }
  else {
    printf("Error: %s\n",qPrintable(nexus->responseError()));
  }
#endif
}

void MainObject::cartFields(RDCart *cart,RDNexusMap &fieldmap)
{
#if 0
      fieldmap.clear();

      insertFieldMap(fieldmap,mm_cartnum,QString::number(cart->number()));
      insertFieldMap(fieldmap,"Artist",cart->artist());
      insertFieldMap(fieldmap,"Artist Keywords",cart->artist());
      insertFieldMap(fieldmap,"Title",cart->title());
      insertFieldMap(fieldmap,"Title Keywords",cart->title());
      insertFieldMap(fieldmap,"Album",cart->album());
      insertFieldMap(fieldmap,"Composer",cart->composer());
      insertFieldMap(fieldmap,"Notes",cart->notes());
      insertFieldMap(fieldmap,"Group",cart->groupName());
      insertFieldMap(fieldmap,"Year",QString::number(cart->year()));
      insertFieldMap(fieldmap,"Run Time",RDGetTimeLength(cart->forcedLength(),false,false));

      //
      // Match First Scheduler Code Map
      //
      QStringList schedcodes=cart->schedCodesList();
      for(int i=0;i<schedcodes.size();i++) {
        QString category=schedmap->stringValue("SchedCodes",schedcodes.at(i).toUpper());
        if(!category.isEmpty()) {
          insertFieldMap(fieldmap,"Category",category);
          break;
        }
      }

      //
      // If we could not find a suitable scheduler code, set category based on group
      //
      if(!fieldmap.count(mm_fieldmap.key("Category"))) {
        QString category=schedmap->stringValue("Groups",cart->groupName().toUpper());
        insertFieldMap(fieldmap,"Category",category);
      }

      if(cart->groupName().toUpper()!="MUSIC") {
        insertFieldMap(fieldmap,"Non-Music","true");
      }

      printf(" %-3.3s ",qPrintable(fieldmap.value(mm_fieldmap.key("Category"))));
#endif
}

void MainObject::insertFieldMap(RDNexusMap &fieldmap,QString mm_field,QString value)
{
#if 0
  if(!mm_fieldmap.key(mm_field,"").isEmpty()) {
    fieldmap.insert(mm_fieldmap.key(mm_field),value);
  }
#endif
}


void MainObject::songFields(QStringList &fieldlist)
{
#if 0
      fieldlist.append(mm_fieldmap.key(mm_cartnum));
      fieldlist.append(mm_fieldmap.key("Artist"));
      fieldlist.append(mm_fieldmap.key("Title"));
      fieldlist.append(mm_fieldmap.key("Group"));
      fieldlist.append(mm_fieldmap.key("Run Time"));
      fieldlist.append(mm_fieldmap.key("Year"));
#endif
}

void MainObject::publishCart(RDNexusMap *songmap)
{
#if 0
  int cartnum=songmap->value(mm_cartnum).toInt();
  printf("Updating cart %d\n",cartnum);
  if(cartnum) {
    RDCart *cart=new RDCart(cartnum);
    if(cart->exists()) {
      cart->setTitle(songmap->value("Artist"));
      cart->setTitle(songmap->value("Title"));
      cart->setTitle(songmap->value("Year"));
    }
  }
#endif
}

int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
