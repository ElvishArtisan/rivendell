// wings_filter.h
//
// A Library import filter for the Crown Wings system
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: wings_filter.h,v 1.9 2010/07/29 19:32:33 cvs Exp $
//      $Date: 2010/07/29 19:32:33 $
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

#ifndef WINGS_FILTER_H
#define WINGS_FILTER_H

#include <stdio.h>

#include <qobject.h>
#include <qapplication.h>
#include <qsqldatabase.h>

#include <rdwavefile.h>
#include <rdstation.h>
#include <rdripc.h>
#include <rdcatch_connect.h>
#include <rdcae.h>
#include <rdgroup.h>

#define WINGS_RECORD_LENGTH 613
#define WINGS_FILTER_USAGE "-g <default-group> -d <db-file> -A <audio-dir> [-e <audio-ext>]\n"
#define WINGS_DEFAULT_AUDIO_EXT "ATX"
#define WINGS_XFER_BUFFER_SIZE 4096

struct WingsRecord {
  char filename[9];
  char extension[4];
  char title[32];
  char artist[32];
  char album[32];
  char group[2];
  unsigned length;
};

class MainObject : public QObject
{
  Q_OBJECT
 public:
  MainObject(QObject *parent=0,const char *name=0);

 private:
  bool ImportCut(RDGroup *group,struct WingsRecord *rec,RDWaveFile *wavefile);
  bool ReadLine(FILE *fp,struct WingsRecord *rec);
  void TrimSpaces(char *str);
  RDStation *filter_rdstation;
  RDRipc *filter_ripc;
  QSqlDatabase *filter_db;
};


#endif 
