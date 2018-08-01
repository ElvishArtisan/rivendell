// export_bmiemr.cpp
//
// Export a Rivendell Report to BMI EMR Format
//
//   (C) Copyright 2002-2006,2016-2017 Fred Gleason <fredg@paravelsystems.com>
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
#include <qtextstream.h>

#include <rdcart.h>
#include <rddb.h>
#include <rddatedecode.h>
#include <rdescape_string.h>
#include <rdreport.h>

bool RDReport::ExportBmiEmr(const QString &filename,const QDate &startdate,
			    const QDate &enddate,const QString &mixtable)
{
  QString sql;
  RDSqlQuery *q;
  int records=0;
  QDateTime current_datetime=
    QDateTime(QDate::currentDate(),QTime::currentTime());
  QString type_code;
  QString usage_code;
  QString station_format=stationFormat();

  QFile *file=new QFile(filename);
  if(!file->open(IO_WriteOnly|IO_Truncate)) {
    report_error_code=RDReport::ErrorCantOpen;
    delete file;
    return false;
  }
  QTextStream *strm=new QTextStream(file);
  strm->setEncoding(QTextStream::UnicodeUTF8);

  //
  // Station Type
  //
  switch(stationType()) {
      case RDReport::TypeAm:
	type_code="AM";
	break;

      case RDReport::TypeFm:
	type_code="FM";
	break;

      default:
	type_code="OT";
	break;
  }

  sql=QString("select ")+
    "EVENT_DATETIME,"+  // 00
    "TITLE,"+           // 01
    "ARTIST,"+          // 02
    "COMPOSER,"+        // 03
    "LENGTH,"+          // 04
    "ISRC,"+            // 05
    "USAGE_CODE "+      // 06
    "from ELR_LINES where "+
    "SERVICE_NAME=\""+RDEscapeString(mixtable)+"\" "+
    "order by EVENT_DATETIME"; 
  q=new RDSqlQuery(sql);

  //
  // Write HEDR Record
  //
  *strm << QString("HEDRSTA")+LeftJustify(stationId(),25)+
    LeftJustify(current_datetime.toString("yyyyMMddhhmmssyyyyMMdd"),22)+
    "FMDT                                                   \x0d\x0a";
  records++;

  //
  // Write FMDT Records
  //
  while(q->next()) {
    switch((RDCart::UsageCode)q->value(6).toInt()) {
    case RDCart::UsageFeature:
      usage_code="F1";
      break;

    case RDCart::UsageOpen:
      usage_code="TO";
      break;

    case RDCart::UsageClose:
      usage_code="TC";
      break;

    case RDCart::UsageTheme:
      usage_code="TT";
      break;

    case RDCart::UsageBackground:
      usage_code="B ";
      break;

    case RDCart::UsagePromo:
      usage_code="JP";
      break;

    default:
      usage_code="F1";
      break;
    }
    *strm << QString("FMDT")+
      LeftJustify(stationId(),40)+
      type_code+
      LeftJustify(station_format,25)+
      startdate.toString("yyyyMM")+"01"+
      LeftJustify(q->value(0).toDateTime().toString("yyyyMMddhh:mm:ss"),16)+
      "000000001"+
      LeftJustify(q->value(1).toString(),40)+
      LeftJustify(q->value(2).toString(),40)+
      LeftJustify(q->value(3).toString(),40)+
      QTime().addMSecs(q->value(4).toInt()).toString("hh:mm:ss")+"           "+
      RightJustify(q->value(5).toString(),12)+
      usage_code+"                      \x0d\x0a";
    records++;
  }
  delete q;

  //
  // Write TRLR Record
  //
  *strm << QString("TRLR")+
    QString().sprintf("%012d                       \x0d\x0a",++records);

  delete strm;
  delete file;
  report_error_code=RDReport::ErrorOk;
  return true;
}

