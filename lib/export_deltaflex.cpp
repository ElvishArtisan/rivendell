// export_deltaflex.cpp
//
// Export a Rivendell Report to CBSI DeltaFlex
//
//   (C) Copyright 2002-2005,2016-2018 Fred Gleason <fredg@paravelsystems.com>
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

//
// CBSI Settings
//
#define CBSI_DELTAFLEX_VERSION 201
#define CBSI_STATION_ID 1
#define CBSI_SCHED_FLAG "C"

#include <qfile.h>
#include <qmessagebox.h>
#include <qtextstream.h>

#include "rddatedecode.h"
#include "rddb.h"
#include "rdescape_string.h"
#include "rdreport.h"

bool RDReport::ExportDeltaflex(const QString &filename,const QDate &startdate,
			       const QDate &enddate,const QString &mixtable)
{
  QString sql;
  RDSqlQuery *q;
  QString air_fmt;

  QFile *file=new QFile(filename);
  if(!file->open(IO_WriteOnly|IO_Truncate)) {
    report_error_code=RDReport::ErrorCantOpen;
    delete file;
    return false;
  }
  QTextStream *strm=new QTextStream(file);
  strm->setEncoding(QTextStream::UnicodeUTF8);
  if(useLeadingZeros()) {
    air_fmt=QString().sprintf("%%0%uu",cartDigits());
  }
  else {
    air_fmt="%u";
  }
  sql=QString("select ")+
    "ELR_LINES.LENGTH,"+          // 00
    "ELR_LINES.CART_NUMBER,"+     // 01
    "ELR_LINES.EVENT_DATETIME,"+  // 02
    "ELR_LINES.EVENT_TYPE,"+      // 03
    "ELR_LINES.EXT_START_TIME,"+  // 04
    "ELR_LINES.EXT_LENGTH,"+      // 05
    "ELR_LINES.EXT_DATA,"+        // 06
    "ELR_LINES.EXT_EVENT_ID,"+    // 07
    "ELR_LINES.EXT_ANNC_TYPE,"+   // 08
    "ELR_LINES.TITLE,"+           // 09
    "ELR_LINES.EXT_CART_NAME "+   // 10
    "from ELR_LINES left join CART "+
    "on ELR_LINES.CART_NUMBER=CART.NUMBER where "+
    "SERVICE_NAME=\""+RDEscapeString(mixtable)+"\" "+
    "order by EVENT_DATETIME";
  q=new RDSqlQuery(sql);

  //
  // Write File Header
  //
  unsigned station_id=stationId().toUInt();
  if(station_id>99) {
    station_id=0;
  }
  *strm << QString("Air Log for CBSI ");
  *strm << QString().sprintf("%03d|",CBSI_DELTAFLEX_VERSION);
  *strm << startdate.toString("yy/MM/dd");
  *strm << QString().sprintf("|%02u|",station_id);
  *strm << QString().sprintf("%05d|",q->size());
  *strm << QString(CBSI_SCHED_FLAG)+"|\x0d\x0a";

  //
  // Write Data Rows
  //
  QString cart_title;
  QString play_length;
  QString tfc_length;
  QString tfc_time;
  QString air_cartnum;
  QString tfc_cartnum;
  QString ext_data;
  QString ext_annc_type;

  while(q->next()) {
    if(q->value(9).toString().length()>29) {
      cart_title=q->value(9).toString().left(29);
    }
    else {
      cart_title=q->value(9).toString();
    }
    if(q->value(8).toString().length()>3) {
      ext_annc_type=q->value(8).toString().left(3);
    }
    else {
      ext_annc_type=q->value(8).toString();
    }
    ext_data="";
    if(q->value(6).toString().length()>0) {
      if(q->value(6).toString().length()<=8) {
	for(unsigned i=0;i<(8-q->value(6).toString().stripWhiteSpace().length());
	    i++) {
	  ext_data+="0";
	}
	ext_data+=q->value(6).toString().stripWhiteSpace();
      }
      else {
	ext_data+=q->value(6).toString().stripWhiteSpace().left(8);
      }
    }
    if(q->value(0).toUInt()<=999000) {
      play_length=QString().sprintf("%03u",q->value(0).toUInt()/1000);
    }
    else {
      play_length=QString("999");
    }
    if(q->value(5).toInt()<0) {
      tfc_length="000";
    }
    else {
      if(q->value(5).toInt()<=999000) {
	tfc_length=QString().sprintf("%03d",q->value(5).toInt()/1000);
      }
      else {
	tfc_length=QString("999");
      }
    }
    if(!q->value(4).toTime().isNull()) {
      tfc_time=q->value(4).toTime().toString("hhmm");
    }
    else {
      tfc_time="";
    }
    air_cartnum=QString().sprintf(air_fmt,q->value(1).toUInt());
    tfc_cartnum=q->value(10).toString();

    *strm << q->value(2).toDateTime().toString("hhmm")+"|";
    *strm << LeftJustify(tfc_time,4)+"|";
    *strm << LeftJustify(cart_title,29)+"|";
    *strm << LeftJustify(air_cartnum,12)+"|";
    *strm << LeftJustify(tfc_cartnum,12)+"|";
    *strm << play_length+"|";
    *strm << tfc_length+"|";
    *strm << LeftJustify(ext_data,8)+"|";
    *strm << LeftJustify(q->value(8).toString(),3)+"|  |                |";
    *strm << LeftJustify(q->value(7).toString(),4)+"|\x0d\x0a";
  }

  delete q;
  delete strm;
  delete file;
  report_error_code=RDReport::ErrorOk;
  return true;
}

