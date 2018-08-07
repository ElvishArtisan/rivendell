// export_textlog.cpp
//
// Export a Rivendell Report to an Text File.
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

#include <qfile.h>
#include <qmessagebox.h>
#include <q3textstream.h>

#include "rdairplay_conf.h"
#include "rdconf.h"
#include "rddatedecode.h"
#include "rddb.h"
#include "rdescape_string.h"
#include "rdlog_line.h"
#include "rdreport.h"

bool RDReport::ExportTextLog(const QString &filename,const QDate &startdate,
			     const QDate &enddate,const QString &mixtable)
{
  QString sql;
  RDSqlQuery *q;
  QString cut;
  QString str;
  QString cart_fmt;
  QString cart_num;

  QFile *file=new QFile(filename);
  if(!file->open(QIODevice::WriteOnly|QIODevice::Truncate)) {
    report_error_code=RDReport::ErrorCantOpen;
    delete file;
    return false;
  }
  Q3TextStream *strm=new Q3TextStream(file);
  strm->setEncoding(Q3TextStream::UnicodeUTF8);
  if(useLeadingZeros()) {
    cart_fmt=QString().sprintf("%%0%uu",cartDigits());
  }
  else {
    cart_fmt="%6u";
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
    "ELR_LINES.TITLE,"+           // 08
    "CART.FORCED_LENGTH,"+        // 09
    "ELR_LINES.STATION_NAME,"+    // 10 
    "ELR_LINES.PLAY_SOURCE,"+     // 11
    "ELR_LINES.CUT_NUMBER "+      // 12
    "from ELR_LINES left join CART "+
    "on ELR_LINES.CART_NUMBER=CART.NUMBER where "+
    "SERVICE_NAME=\""+RDEscapeString(mixtable)+"\" "+
    "order by EVENT_DATETIME";
  q=new RDSqlQuery(sql);

  //
  // Write File Header
  //
  if(startdate==enddate) {
    *strm << RDReport::center(QString("Rivendell RDAirPlay Playout Report for ")+
		    startdate.toString("MM/dd/yyyy"),78)+"\n";
  }
  else {
    *strm << RDReport::center(QString("Rivendell RDAirPlay Playout Report for ")+
		    startdate.toString("MM/dd/yyyy")+" - "+
		    enddate.toString("MM/dd/yyyy"),78)+"\n";
  }
  *strm << RDReport::center(name()+" -- "+description(),78)+"\n";
  *strm << "--Time--  -Cart-  Cut  --Title----------------  A-Len  N-Len  --Host----  Srce\n";

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
    *strm << q->value(2).toTime().toString("hh:mm:ss")+"  ";
    *strm << RDReport::rightJustify(cart_num,6)+"  ";
    *strm << cut+"  ";
    *strm << RDReport::leftJustify(q->value(8).toString(),23)+"  ";
    *strm << RDGetTimeLength(q->value(0).toInt(),true,false).right(5)+"  ";
    *strm << RDGetTimeLength(q->value(8).toInt(),true,false).right(5)+"  ";
    *strm << RDReport::leftJustify(q->value(10).toString(),10)+"  ";
    switch((RDLogLine::PlaySource)q->value(11).toInt()) {
    case RDLogLine::MainLog:
      *strm << "Main";
      break;

    case RDLogLine::AuxLog1:
      *strm << "Aux1";
      break;

    case RDLogLine::AuxLog2:
      *strm << "Aux2";
      break;

    case RDLogLine::SoundPanel:
      *strm << "SPnl";
      break;

    case RDLogLine::CartSlot:
      *strm << "Slot";
      break;

    default:
      *strm << "    ";
      break;
    }
    *strm << "\n";
  }
  delete q;

  delete strm;
  delete file;
  report_error_code=RDReport::ErrorOk;
  return true;
}

