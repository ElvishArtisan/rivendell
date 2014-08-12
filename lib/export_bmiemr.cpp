// export_bmiemr.cpp
//
// Export a Rivendell Report to BMI EMR Format
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: export_bmiemr.cpp,v 1.10.8.1.2.1 2014/03/19 23:50:20 cvs Exp $
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
#include <rddb.h>
#include <rddatedecode.h>
#include <rdreport.h>
#include <rdcart.h>


bool RDReport::ExportBmiEmr(const QDate &startdate,const QDate &enddate,
			    const QString &mixtable)
{
  QString sql;
  RDSqlQuery *q;
  FILE *f;
  int records=0;
  QDateTime current_datetime=
    QDateTime(QDate::currentDate(),QTime::currentTime());
  QString type_code;
  QString usage_code;
  QString station_format=stationFormat();

#ifdef WIN32
  QString filename=RDDateDecode(exportPath(RDReport::Windows),startdate);
#else
  QString filename=RDDateDecode(exportPath(RDReport::Linux),startdate);
#endif

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

  if((f=fopen((const char *)filename,"wb"))==NULL) {
    report_error_code=RDReport::ErrorCantOpen;
    return false;
  }
  /*
  sql=QString().sprintf("select EVENT_DATETIME,TITLE,ARTIST,COMPOSER,\
                         LENGTH,ISRC,USAGE_CODE from `%s_SRT` \
                         order by EVENT_DATETIME",
			(const char *)mixtable);
  */
  sql=QString("select EVENT_DATETIME,TITLE,ARTIST,COMPOSER,")+
    "LENGTH,ISRC,USAGE_CODE from `"+
    mixtable+"_SRT` order by EVENT_DATETIME"; 
  q=new RDSqlQuery(sql);

  //
  // Write HEDR Record
  //
  fprintf(f,"HEDRSTA%-25s%22sFMDT                                                   \x0d\x0a",
	  (const char *)stationId().utf8(),
	  (const char *)current_datetime.toString("yyyyMMddhhmmssyyyyMMdd"));
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
    fprintf(f,"FMDT%-40s%2s%-25s%6s01%14s000000001%-40s%-40s%-40s%8s           %12s%2s                      \x0d\x0a",
	    (const char *)stationId().utf8(),
	    (const char *)type_code.utf8(),
	    (const char *)station_format.utf8(),
	    (const char *)startdate.toString("yyyyMM"),
	    (const char *)q->value(0).toDateTime().
	    toString("yyyyMMddhh:mm:ss"),
	    (const char *)q->value(1).toString().utf8(),
	    (const char *)q->value(2).toString().utf8(),
	    (const char *)q->value(3).toString().utf8(),
	    (const char *)QTime().addMSecs(q->value(4).toInt()).
	    toString("hh:mm:ss"),
	    (const char *)q->value(5).toString().utf8(),
	    (const char *)usage_code.utf8());
    records++;
  }
  delete q;

  //
  // Write TRLR Record
  //
  fprintf(f,"TRLR%012d                       \x0d\x0a",++records);

  fclose(f);
  report_error_code=RDReport::ErrorOk;
  return true;
}

