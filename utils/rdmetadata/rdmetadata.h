// rdmarkerset.h
//
// Command-line tool for setting Rivendell Cart Metadata
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


#ifndef RDMETADATA_H
#define RDMETADATA_H

#include <stdlib.h>

#include <vector>

#include <QObject>

#include "rdnotification.h"

#define RDMETADATA_USAGE "--cart-number=<cart number> [options]\n\nThe following options are recognized:\n\n\
  --add_schedcode=<schedcode>\n\
  --agency=<agency>\n\
  --album=<album>\n\
  --artist=<artist>\n\
  --bpm=<bpm>\n\
  --composer=<composer>\n\
  --conductor=<conductor>\n\
  --label=<label>\n\
  --publisher=<publisher>\n\
  --rem-schedcode=<schedcode>\n\
  --songid=<songid>\n\
  --title=<title>\n\
  --year=<year>\n\
  --verbose\n\
\n"

class MainObject : public QObject
{
  Q_OBJECT;
 public:
  MainObject(QObject *parent=0);

 private slots:
  void userChangedData();

 private:
  void updateMetadata();
  void SendNotification(RDNotification::Action action,unsigned cartnum);
  void Print(const QString &msg);
  bool verbose;
  unsigned cartnum;
  QString cartstring;
  QString artist;
  QString title;
  QString album;
  int year;
  QString conductor;
  QString label;
  QString agency;
  QString publisher;
  QString composer;
  QString songid;
  int bpm;
  QString add_schedcode;
  QString rem_schedcode;
};


#endif  // RDMETADATA_H
