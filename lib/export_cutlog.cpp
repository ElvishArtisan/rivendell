// export_cutlog.cpp
//
// Export a Rivendell Cut Report.
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

#include <stdio.h>

#include <QFile>
#include <QMessageBox>
#include <QTextStream>

#include <rdairplay_conf.h>
#include <rdapplication.h>
#include <rdconf.h>
#include <rddatedecode.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdlog_line.h>
#include <rdreport.h>

bool RDReport::ExportCutLog(const QString &filename,const QDate &startdate,
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
  QTextStream *strm=new QTextStream(file);
  strm->setCodec("UTF-8");
  if(useLeadingZeros()) {
    cart_fmt=QString().sprintf("%%0%uu",cartDigits());
  }
  else {
    cart_fmt="%6u";
  }
  sql=QString("select ")+
    "`ELR_LINES`.`LENGTH`,"+          // 00
    "`ELR_LINES`.`CART_NUMBER`,"+     // 01
    "`ELR_LINES`.`EVENT_DATETIME`,"+  // 02
    "`ELR_LINES`.`EVENT_TYPE`,"+      // 03
    "`ELR_LINES`.`EXT_START_TIME`,"+  // 04
    "`ELR_LINES`.`EXT_LENGTH`,"+      // 05
    "`ELR_LINES`.`EXT_DATA`,"+        // 06
    "`ELR_LINES`.`EXT_EVENT_ID`,"+    // 07
    "`ELR_LINES`.`TITLE`,"+           // 08
    "`CART`.`FORCED_LENGTH`,"+        // 09
    "`ELR_LINES`.`STATION_NAME`,"+    // 10
    "`ELR_LINES`.`PLAY_SOURCE`,"+     // 11
    "`ELR_LINES`.`CUT_NUMBER`,"+      // 12
    "`ELR_LINES`.`DESCRIPTION` "+     // 13
    "from `ELR_LINES` left join `CART` "+
    "on `ELR_LINES`.`CART_NUMBER`=`CART`.`NUMBER` where "+
    "SERVICE_NAME='"+RDEscapeString(mixtable)+"' "+
    "order by EVENT_DATETIME";
  q=new RDSqlQuery(sql);

  //
  // Write File Header
  //
  if(startdate==enddate) {
    *strm << RDReport::center(QString("Rivendell RDAirPlay Cut Report for ")+
			      rda->shortDateString(startdate),78)+"\n";
  }
  else {
    *strm << RDReport::center(QString("Rivendell RDAirPlay Cut Report for ")+
			      rda->shortDateString(startdate)+" - "+
			      rda->shortDateString(enddate),78)+"\n";
  }
  *strm << RDReport::center(name()+" -- "+description(),78)+"\n";
  if(rda->showTwelveHourTime()) {
    *strm << "--Time-----  -Cart-  --Title----------------  Cut  --Description-------  -Len-\n";
  }
  else {
    *strm << "--Time--  -Cart-  --Title----------------  Cut  --Description-------  -Len-\n";
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
    cart_num=QString().sprintf(cart_fmt.toUtf8(),q->value(1).toUInt());
    QString desc=q->value(13).toString();
    if(desc.isEmpty()) {
      desc="                    ";
    }
    *strm << rda->timeString(q->value(2).toTime()," ")+"  ";
    *strm << cart_num+"  ";
    *strm << RDReport::leftJustify(q->value(8).toString(),23)+"  ";
    *strm << cut+"  ";
    *strm << RDReport::leftJustify(desc,20)+"  ";
    *strm << RDGetTimeLength(q->value(9).toInt(),true,false).right(5);
    *strm << "\n";
  }
  delete q;
  delete strm;
  delete file;
  report_error_code=RDReport::ErrorOk;
  return true;
}

