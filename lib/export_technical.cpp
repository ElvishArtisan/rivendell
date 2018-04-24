// export_technical.cpp
//
// Export a Rivendell Technical Report to an ASCII Text File.
//
//   (C) Copyright 2002-2006,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <QFile>
#include <QMessageBox>

#include <rdairplay_conf.h>
#include <rdconf.h>
#include <rddatedecode.h>
#include <rddb.h>
#include <rdlog_line.h>
#include <rdreport.h>

bool RDReport::ExportTechnical(const QDate &startdate,const QDate &enddate,
			       bool incl_hdr,bool incl_crs,
			       const QString &mixtable)
{
  QString sql;
  RDSqlQuery *q;
  FILE *f;
  QString cut;
  QString str;
  QString cart_fmt;
  QString cart_num;
  char eol[3]="\n";

#ifdef WIN32
  QString filename=RDDateDecode(exportPath(RDReport::Windows),startdate);
#else
  QString filename=RDDateDecode(exportPath(RDReport::Linux),startdate);
  if(incl_crs) {
    strcpy(eol,"\r\n");
  }
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
    "`"+mixtable+"_SRT`.PLAY_SOURCE,"+   // 11
    "`"+mixtable+"_SRT`.CUT_NUMBER,"+      // 12
    "`"+mixtable+"_SRT`.START_SOURCE,"+    // 13
    "`"+mixtable+"_SRT`.ONAIR_FLAG "+      // 14
    "from `"+mixtable+"_SRT` left join CART "+
    "on `"+mixtable+"_SRT`.CART_NUMBER=CART.NUMBER "+
    "order by EVENT_DATETIME";
  q=new RDSqlQuery(sql);

  //
  // Write File Header
  //
  if(incl_hdr) {
    if(startdate==enddate) {
      fprintf(f,"           Rivendell RDAirPlay Technical Playout Report for %s%s",
	      (const char *)startdate.toString("MM/dd/yyyy"),eol);
    }
    else {
      fprintf(f,"      Rivendell RDAirPlay Technical Playout Report for %s - %s%s",
	      (const char *)startdate.toString("MM/dd/yyyy"),
	      (const char *)enddate.toString("MM/dd/yyyy"),eol);
    }
    str=QString().sprintf("%s -- %s%s",(const char *)name(),
			  (const char *)description(),eol);
    for(int i=0;i<(80-str.length())/2;i++) {
      fprintf(f," ");
    }
    fprintf(f,"%s%s",(const char *)str,eol);
    fprintf(f,"--Time--  -Cart-  Cut  --Title----------------  A-Len  N-Len  --Host----  Srce  StartedBy  OnAir%s",eol);
  }

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
    fprintf(f,"%8s  %6s  %3s  %-23s  %5s  %5s  %-10s  ",
	    (const char *)q->value(2).toTime().toString("hh:mm:ss"),
	    (const char *)cart_num,
	    (const char *)cut,
	    (const char *)q->value(8).toString().left(23),
	    (const char *)RDGetTimeLength(q->value(0).toInt(),true,false).
	    right(5),
	    (const char *)RDGetTimeLength(q->value(9).toInt(),true,false).
	    right(5),
	    (const char *)q->value(10).toString());
    switch((RDLogLine::PlaySource)q->value(11).toInt()) {
	case RDLogLine::MainLog:
	  fprintf(f,"Main   ");
	  break;

	case RDLogLine::AuxLog1:
	  fprintf(f,"Aux1   ");
	  break;

	case RDLogLine::AuxLog2:
	  fprintf(f,"Aux2   ");
	  break;

	case RDLogLine::SoundPanel:
	  fprintf(f,"SPnl   ");
	  break;

	case RDLogLine::CartSlot:
	  fprintf(f,"Slot   ");
	  break;

	default:
	  fprintf(f,"       ");
	  break;
    }
    fprintf(f,"%-7s  ",(const char *)RDLogLine::
	    startSourceText((RDLogLine::StartSource)q->value(13).toInt()));
    if(q->value(14).toString()=="Y") {
      fprintf(f,"  Yes ");
    }
    else {
      fprintf(f,"  No  ");
    }
    fprintf(f,"%s",eol);
  }
  delete q;
  fclose(f);
  report_error_code=RDReport::ErrorOk;
  return true;
}

