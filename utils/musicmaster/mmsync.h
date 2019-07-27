// mmping.h
//
// MusicMaster library utility - Library Sync
//
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

#ifndef MMSYNC_H
#define MMSYNC_H

#include <QObject>

#include "rdnotification.h"
#include "rdcart.h"
#include "rdprofile.h"
#include "rdnexus.h"

#define MMSYNC_USAGE "--server=<server> --port=<port> --station=<station> --schedmap=<filename> --test"

class MainObject : public QObject
{
  Q_OBJECT;
  public:
    MainObject(QObject *parent=0);
    void missingFromMusicMaster();
    void removeMusicMasterOrphans();
    void importSong(int cartnum);

  private:
    RDNexus *nexus;
    RDNexusServer server;
    RDProfile *schedmap;
    RDNexusMap mm_fieldmap;
    bool mm_debug;
    bool mm_test;
    QString mm_schedmap;
    QString mm_station;
    QString mm_songid;
    QString mm_cartnum;
    QString mm_artist;
    QString mm_title;
    void insertFieldMap(RDNexusMap &fieldmap,QString mm_field,QString value);
    void cartFields(RDCart *cart,RDNexusMap &fieldmap);
    void songFields(QStringList &fieldlist);
    void publishCart(RDNexusMap*);
    void setSongInfo(RDCart *cart,RDNexusSongInfo &info);
};


#endif  // MMSYNC_H
