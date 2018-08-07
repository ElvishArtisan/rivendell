// export_nprsoundex.cpp
//
// Export a Rivendell NPR SoundExchange Report to an ASCII Text File.
//
//   (C) Copyright 2002-2006,2013,2016-2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <qfile.h>
#include <qmessagebox.h>
#include <q3textstream.h>

#include "rdairplay_conf.h"
#include "rddatedecode.h"
#include "rddb.h"
#include "rdescape_string.h"
#include "rdget_ath.h"
#include "rdlog_line.h"
#include "rdreport.h"

//
// This implements a National Public Radio (NPR) standard.
// See http://soundexchange.digitalservices.npr.org/playlist-log-file-guidelines/
// for more information.
//

bool RDReport::ExportNprSoundEx(const QString &filename,const QDate &startdate,
				const QDate &enddate,const QString &mixtable)
{
  QString sql;
  RDSqlQuery *q;
  QString artist;
  QString title;
  QString album;
  QString label;
  QString service_name=serviceName();
  QString trans_category=stationFormat();
  QString channel_name=stationId();

  QFile *file=new QFile(filename);
  if(!file->open(QIODevice::WriteOnly|QIODevice::Truncate)) {
    report_error_code=RDReport::ErrorCantOpen;
    delete file;
    return false;
  }
  Q3TextStream *strm=new Q3TextStream(file);
  strm->setEncoding(Q3TextStream::UnicodeUTF8);

  //
  // Generate Header
  //
  *strm << QString("Start Time\tEnd Time\tTitle\tArtist\tAlbum\tLabel\x0d\x0a");

  //
  // Roll Up Records
  //
  sql=QString("select ")+
    "EVENT_DATETIME,"+  // 00
    "LENGTH,"+          // 01
    "TITLE,"+           // 02
    "ARTIST,"+          // 03
    "ALBUM,"+           // 04
    "LABEL "+           // 05
    "from ELR_LINES where "+
    "SERVICE_NAME=\""+RDEscapeString(mixtable)+"\" "+
    "order by EVENT_DATETIME";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    *strm << q->value(0).toDateTime().toString("MM/dd/yyyy hh:mm:ss")+"\t";
    *strm << q->value(0).toDateTime().addSecs(q->value(1).toInt()/1000).
      toString("MM/dd/yyyy hh:mm:ss")+"\t";
    *strm << q->value(2).toString()+"\t";
    *strm << q->value(3).toString()+"\t";
    *strm << q->value(4).toString()+"\t";
    *strm << q->value(5).toString()+"\x0d\x0a";
  }
  delete q;
  delete strm;
  delete file;
  report_error_code=RDReport::ErrorOk;
  return true;
}

