// export_soundex.cpp
//
// Export a Rivendell SoundExchange Report to an ASCII Text File.
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <QFile>
#include <QMessageBox>
#include <QTextStream>

#include "rdairplay_conf.h"
#include "rddatedecode.h"
#include "rddb.h"
#include "rdescape_string.h"
#include "rdget_ath.h"
#include "rdlog_line.h"
#include "rdreport.h"

bool RDReport::ExportSoundEx(const QString &filename,const QDate &startdate,
			     const QDate &enddate,const QString &mixtable)
{
  QString sql;
  RDSqlQuery *q;
  unsigned cartnum=0;
  QString artist;
  QString title;
  QString isrc;
  QString album;
  QString label;
  unsigned plays=0;
  QString service_name=serviceName();
  QString trans_category=stationFormat();
  QString channel_name=stationId();

  //
  // Get ATH Value
  //
  double ath=0.0;
  RDGetAth *getath=new RDGetAth(&ath);
  if(getath->exec()<0) {
    report_error_code=RDReport::ErrorCanceled;
    return false;
  }

  QFile *file=new QFile(filename);
  if(!file->open(QIODevice::WriteOnly|QIODevice::Truncate)) {
    report_error_code=RDReport::ErrorCantOpen;
    delete file;
    return false;
  }
  QTextStream *strm=new QTextStream(file);
  strm->setCodec("UTF-8");

  //
  // Generate Header
  //
  *strm << "\"NAME_OF_SERVICE\",\"TRANSMISSION_CATEGORY\",\"FEATURED_ARTIST\",\"SOUND_RECORDING_TITLE\",\"ISRC\",\"ALBUM_TITLE\",\"MARKETING_LABEL\",\"ACTUAL_TOTAL_PERFORMANCES\",\"AGGREGATE_TUNING_HOURS\",\"CHANNEL_OR_PROGRAM_NAME\",\"PLAY_FREQUENCY\"\r\n";

  //
  // Roll Up Records
  //
  sql=QString("select ")+
    "`CART_NUMBER`,"+  // 00
    "`ARTIST`,"+       // 01
    "`TITLE`,"+        // 02
    "`ISRC`,"+         // 03
    "`ALBUM`,"+        // 04
    "`LABEL` "+        // 05
    "from `ELR_LINES` where "+
    "`SERVICE_NAME`='"+RDEscapeString(mixtable)+"' "+
    "order by `CART_NUMBER`";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if(q->value(0).toUInt()==cartnum) {
      plays++;
    }
    else {
      if(cartnum!=0) {
	*strm << QString("\"")+service_name+"\",";
	*strm << QString("\"")+trans_category+"\",";
	*strm << QString("\"")+artist+"\",";
	*strm << QString("\"")+title+"\",";
	*strm << QString("\"")+isrc+"\",";
	*strm << QString("\"")+album+"\",";
	*strm << QString("\"")+label+"\",,";
	*strm << QString::asprintf("%9.2f,",ath);
	*strm << QString("\"")+channel_name+"\",";
	*strm << QString::asprintf("%d",plays);
	*strm << "\n";
      }
      plays=1;
      if(q->value(1).isNull()) {
	artist="";
      }
      else {
	artist=q->value(1).toString();
      }
      title=q->value(2).toString();
      if(q->value(3).isNull()) {
	isrc="";
      }
      else {
	isrc=q->value(3).toString();
      }
      if(q->value(4).isNull()) {
	album="";
      }
      else {
	album=q->value(4).toString();
      }
      if(q->value(5).isNull()) {
	label="";
      }
      else {
	label=q->value(5).toString();
      }
    }
    cartnum=q->value(0).toUInt();
  }
  delete q;
  if(cartnum!=0) {
    *strm << QString("\"")+service_name+"\",";
    *strm << QString("\"")+trans_category+"\",";
    *strm << QString("\"")+artist+"\",";
    *strm << QString("\"")+title+"\",";
    *strm << QString("\"")+isrc+"\",";
    *strm << QString("\"")+album+"\",";
    *strm << QString("\"")+label+"\",,";
    *strm << QString::asprintf("%9.2f,",ath);
    *strm << QString("\"")+channel_name+"\",";
    *strm << QString::asprintf("%d",plays);
    *strm << "\n";
  }
  delete strm;
  delete file;
  report_error_code=RDReport::ErrorOk;
  return true;
}

