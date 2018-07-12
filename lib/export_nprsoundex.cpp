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

#include <stdio.h>

#include <qfile.h>
#include <qmessagebox.h>

#include <rdairplay_conf.h>
#include <rddatedecode.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdget_ath.h>
#include <rdlog_line.h>
#include <rdreport.h>

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
  FILE *f=NULL;
  QString artist;
  QString title;
  QString album;
  QString label;
  QString service_name=serviceName();
  QString trans_category=stationFormat();
  QString channel_name=stationId();

  if((f=fopen(filename,"wb"))==NULL) {
    report_error_code=RDReport::ErrorCantOpen;
    return false;
  }

  //
  // Generate Header
  //
  fprintf(f,"Start Time\tEnd Time\tTitle\tArtist\tAlbum\tLabel\x0d\x0a");

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
    fprintf(f,"%s\t",(const char *)q->value(0).toDateTime().
	    toString("MM/dd/yyyy hh:mm:ss"));
    fprintf(f,"%s\t",(const char *)q->value(0).toDateTime().
	    addSecs(q->value(1).toInt()/1000).
	    toString("MM/dd/yyyy hh:mm:ss"));
    fprintf(f,"%s\t",(const char *)StringField(q->value(2).toString()));
    fprintf(f,"%s\t",(const char *)StringField(q->value(3).toString()));
    fprintf(f,"%s\t",(const char *)StringField(q->value(4).toString()));
    fprintf(f,"%s\x0d\x0a",(const char *)StringField(q->value(5).toString()));
  }
  fclose(f);
  report_error_code=RDReport::ErrorOk;
  return true;
}

