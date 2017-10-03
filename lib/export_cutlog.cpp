// export_cutlog.cpp
//
// Export a Rivendell Cut Report.
//
//   (C) Copyright 2002-2016 Fred Gleason <fredg@paravelsystems.com>
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
#include <rddb.h>
#include <rdlog_line.h>
#include <rdairplay_conf.h>
#include <rdconf.h>
#include <rddatedecode.h>
#include <rdreport.h>

bool RDReport::ExportCutLog(const QDate &startdate,const QDate &enddate,
			    const QString &mixtable)
{
  QString sql;
  RDSqlQuery *q;
  FILE *f;
  QString cut;
  QString str;
  QString cart_fmt;
  QString cart_num;

#ifdef WIN32
  QString filename=
    RDDateDecode(exportPath(RDReport::Windows),startdate,serviceName());
#else
  QString filename=
    RDDateDecode(exportPath(RDReport::Linux),startdate,serviceName());
#endif

  QFile file(filename);
  if((f=fopen((const char *)filename,"w"))==NULL) {
    report_error_code=RDReport::ErrorCantOpen;
    return false;
  }
  if(useLeadingZeros()) {
    cart_fmt=QString().sprintf("%%0%uu",cartDigits());
  }
  else {
    cart_fmt="%6u";
  }
  sql=QString("select ")+
    "`"+mixtable+"_SRT`.LENGTH,"+          // 00
    "`"+mixtable+"_SRT`.CART_NUMBER,"+     // 01
    "`"+mixtable+"_SRT`.EVENT_DATETIME,"+  // 02
    "`"+mixtable+"_SRT`.EVENT_TYPE,"+      // 03
    "`"+mixtable+"_SRT`.EXT_START_TIME,"+  // 04
    "`"+mixtable+"_SRT`.EXT_LENGTH,"+      // 05
    "`"+mixtable+"_SRT`.EXT_DATA,"+        // 06
    "`"+mixtable+"_SRT`.EXT_EVENT_ID,"+    // 07
    "`"+mixtable+"_SRT`.TITLE,"+           // 08
    "CART.FORCED_LENGTH,"+                 // 09
    "`"+mixtable+"_SRT`.STATION_NAME,"+    // 10
    "`"+mixtable+"_SRT`.PLAY_SOURCE,"+     // 11
    "`"+mixtable+"_SRT`.CUT_NUMBER,"+      // 12
    "`"+mixtable+"_SRT`.DESCRIPTION "+     // 13
    "from `"+mixtable+"_SRT` left join CART "+
    "on `"+mixtable+"_SRT`.CART_NUMBER=CART.NUMBER "+
    "order by EVENT_DATETIME";
  q=new RDSqlQuery(sql);

  //
  // Write File Header
  //
  if(startdate==enddate) {
    fprintf(f,"                  Rivendell RDAirPlay Cut Report for %s\n",
	    (const char *)startdate.toString("MM/dd/yyyy"));
  }
  else {
    fprintf(f,"             Rivendell RDAirPlay Cut Report for %s - %s\n",
	    (const char *)startdate.toString("MM/dd/yyyy"),
	    (const char *)enddate.toString("MM/dd/yyyy"));
  }
  str=QString().sprintf("%s -- %s\n",(const char *)name(),
			(const char *)description());
  for(unsigned i=0;i<(80-str.length())/2;i++) {
    fprintf(f," ");
  }
  fprintf(f,"%s\n",(const char *)str);
  fprintf(f,"--Time--  -Cart-  --Title----------------  Cut  --Description-------  -Len-\n");

  //
  // Write Data Rows
  //
  while(q->next()) {
    if(q->value(12).toInt()>0) {
      cut=QString().sprintf("%03d",q->value(12).toInt());
    }
    else {
      if((RDAirPlayConf::TrafficAction)q->value(3).toInt()==
	 RDAirPlayConf::TrafficMacro) {
	cut="rml";
      }
      else {
	cut="   ";
      }
    }
    cart_num=QString().sprintf(cart_fmt,q->value(1).toUInt());
    QString desc=q->value(13).toString();
    if(desc.isEmpty()) {
      desc="                    ";
    }
    fprintf(f,"%8s  %6s  %-23s  %3s  %-20s  %5s",
	    (const char *)q->value(2).toTime().toString("hh:mm:ss"),
	    (const char *)cart_num,
	    (const char *)q->value(8).toString().left(23),
	    (const char *)cut,
	    (const char *)desc.left(20),
	    (const char *)RDGetTimeLength(q->value(9).toInt(),true,false).
	    right(5));
    fprintf(f,"\n");
  }
  delete q;
  fclose(f);
  report_error_code=RDReport::ErrorOk;
  return true;
}

