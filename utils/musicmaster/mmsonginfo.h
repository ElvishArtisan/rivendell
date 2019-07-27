// mmsonginfo.h
//
// MusicMaster song information utility
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

#ifndef MMSONGINFO_H
#define MMSONGINFO_H

#include <QObject>

#include "rdnotification.h"
#include "rdcart.h"
#include "rdprofile.h"
#include "rdnexus.h"

#define MMSONGINFO_USAGE "--server=<server> --port=<port> --station=<station> --songid=<songid> --cartnum=<cartnum> --debug"

class MainObject : public QObject
{
  Q_OBJECT;
  public:
    MainObject(QObject *parent=0);
    void songInfo();

  private:
    RDNexus *nexus;
    RDNexusServer server;
    bool mm_debug;
    QString mm_station;
    QString mm_songid;
    QString mm_cartnum;
};


#endif  // MMSONGINFO_H
