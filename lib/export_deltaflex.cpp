// export_deltaflex.cpp
//
// Export a Rivendell Report to CBSI DeltaFlex
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: export_deltaflex.cpp,v 1.12.6.1 2013/02/08 21:41:44 cvs Exp $
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

#include <stdio.h>

#include <qfile.h>
#include <qmessagebox.h>
#include <rddb.h>
#include <rddatedecode.h>
#include <rdreport.h>


bool RDReport::ExportDeltaflex(const QDate &startdate,const QDate &enddate,
			       const QString &mixtable)
{
  QString sql;
  RDSqlQuery *q;
  FILE *f;
  QString air_fmt;

#ifdef WIN32
  QString filename=RDDateDecode(exportPath(RDReport::Windows),startdate);
#else
  QString filename=RDDateDecode(exportPath(RDReport::Linux),startdate);
#endif

  QFile file(filename);
  if((f=fopen((const char *)filename,"wb"))==NULL) {
    report_error_code=RDReport::ErrorCantOpen;
    return false;
  }

  if(useLeadingZeros()) {
    air_fmt=QString().sprintf("%%0%uu",cartDigits());
  }
  else {
    air_fmt="%u";
  }
  sql=QString().sprintf("select `%s_SRT`.LENGTH,`%s_SRT`.CART_NUMBER,\
                         `%s_SRT`.EVENT_DATETIME,`%s_SRT`.EVENT_TYPE,\
                         `%s_SRT`.EXT_START_TIME,`%s_SRT`.EXT_LENGTH,\
                         `%s_SRT`.EXT_DATA,`%s_SRT`.EXT_EVENT_ID,\
                         `%s_SRT`.EXT_ANNC_TYPE,`%s_SRT`.TITLE,\
                         `%s_SRT`.EXT_CART_NAME from `%s_SRT` \
                         left join CART on\
                         `%s_SRT`.CART_NUMBER=CART.NUMBER\
                         order by EVENT_DATETIME",
			(const char *)mixtable,
			(const char *)mixtable,
			(const char *)mixtable,
			(const char *)mixtable,
			(const char *)mixtable,
			(const char *)mixtable,
			(const char *)mixtable,
			(const char *)mixtable,
			(const char *)mixtable,
			(const char *)mixtable,
			(const char *)mixtable,
			(const char *)mixtable,
			(const char *)mixtable);
  q=new RDSqlQuery(sql);

  //
  // Write File Header
  //
  unsigned station_id=stationId().toUInt();
  if(station_id>99) {
    station_id=0;
  }
  fprintf(f,"Air Log for CBSI %03d|%s|%02u|%05d|%s|\x0d\x0a",
	  CBSI_DELTAFLEX_VERSION,
	  (const char *)startdate.toString("yy/MM/dd"),
	  station_id,
	  q->size(),
	  CBSI_SCHED_FLAG);

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

    fprintf(f,"%s|%4s|%-29s|%-12s|%-12s|%s|%s|%8s|%-3s|  |                |%4s|\x0d\x0a",
	    (const char *)q->value(2).toDateTime().toString("hhmm"),
	    (const char *)tfc_time,
	    (const char *)cart_title,
	    (const char *)air_cartnum,
	    (const char *)tfc_cartnum,
	    (const char *)play_length,
	    (const char *)tfc_length,
	    (const char *)ext_data,
	    (const char *)q->value(8).toString(),
	    (const char *)q->value(7).toString());
  }

  delete q;
  fclose(f);
  report_error_code=RDReport::ErrorOk;
  return true;
}

