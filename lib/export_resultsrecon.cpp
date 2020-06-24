// export_resultsreport.cpp
//
// Export a Rivendell Report in 'results' format
//
//   (C) Copyright 2002-2020 Fred Gleason <fredg@paravelsystems.com>
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
#include <qtextstream.h>

#include "rdconf.h"
#include "rddatedecode.h"
#include "rddb.h"
#include "rdescape_string.h"
#include "rdreport.h"

bool RDReport::ExportResultsReport(const QString &filename,
				   const QDate &startdate,const QDate &enddate,
				   const QString &mixtable)
{
  QString sql;
  RDSqlQuery *q;
  QString air_fmt;

  QFile *file=new QFile(filename);
  if(!file->open(QIODevice::WriteOnly|QIODevice::Truncate)) {
    report_error_code=RDReport::ErrorCantOpen;
    delete file;
    return false;
  }
  QTextStream *strm=new QTextStream(file);
  strm->setEncoding(QTextStream::UnicodeUTF8);
  sql=QString("select ")+
    "ELR_LINES.EVENT_DATETIME,"+  // 00
    "ELR_LINES.EVENT_TYPE,"+      // 01
    "ELR_LINES.LENGTH,"+          // 02
    "ELR_LINES.CART_NUMBER,"+     // 03
    "ELR_LINES.CUT_NUMBER,"+      // 04
    "ELR_LINES.TITLE,"+           // 05
    "ELR_LINES.ARTIST,"+          // 06
    "ELR_LINES.EXT_START_TIME "+  // 07
    "from ELR_LINES left join CART "+
    "on ELR_LINES.CART_NUMBER=CART.NUMBER where "+
    "SERVICE_NAME=\""+RDEscapeString(mixtable)+"\" "+
    "order by EVENT_DATETIME";
  q=new RDSqlQuery(sql);

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
  int line=0;

  while(q->next()) {
    // DateTime
    *strm << q->value(0).toDateTime().toString("yy-MM-dd,hh:mm:dd,");

    *strm << "on-air,";

    // Cart Number
    *strm << QString().sprintf("%06u,",q->value(3).toUInt());

    // Cut Number
    *strm << QString().sprintf("%03d,",q->value(4).toInt());

    // Title / Artist
    *strm << QString().
      sprintf("\"%-23s  %-25s\",",
	      q->value(5).toString().left(23).toUtf8().constData(),
	      q->value(6).toString().left(25).toUtf8().constData());

    // Length
    *strm << RDGetTimeLength(q->value(2).toInt(),true,false).right(5)+",";

    // Scheduled Start Time
    *strm << q->value(7).toTime().toString("hh:mm:ss,");

    // Line Counts
    *strm << QString().sprintf("%05d|-|%05d|00",line,line);

    // EOL
    *strm << "\r\n";

    line++;
  }

  delete q;
  delete strm;
  delete file;
  report_error_code=RDReport::ErrorOk;

  return true;
}

