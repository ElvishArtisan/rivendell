// export_radiotraffic.cpp
//
// Export a Rivendell Report to RadioTraffic.com
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

#include "rddb.h"
#include "rdconf.h"
#include "rddatedecode.h"
#include "rdescape_string.h"
#include "rdreport.h"

bool RDReport::ExportRadioTraffic(const QString &filename,
				  const QDate &startdate,const QDate &enddate,
				  const QString &mixtable,int version)
{
  //
  // Maintainer's Note: 
  // This method implements the 'CounterPoint' reconciliation format as
  // documented in 'docs/misc/scheduler_formats.ods'.
  //
  // The 'version' field is for maintaining bug-for-bug backwards
  // compatibility. Recognized values are:
  //     0 - The original implementation. This is missing a leading '0'
  //         column 3 ['Scheduled event length'].
  //
  //     1 - Fixes the 'missing leading zero' error in column 3.
  //
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
  strm->setCodec("UTF-8");
  if(useLeadingZeros()) {
    air_fmt=QString().sprintf("%%0%uu ",cartDigits());
  }
  else {
    air_fmt=QString().sprintf("%%%-uu ",cartDigits());
  }
  sql=QString("select ")+
    "`ELR_LINES`.`LENGTH,"+          // 00
    "`ELR_LINES`.`CART_NUMBER`,"+     // 01
    "`ELR_LINES`.`EVENT_DATETIME`,"+  // 02
    "`ELR_LINES`.`EVENT_TYPE`,"+      // 03
    "`ELR_LINES`.`EXT_START_TIME`,"+  // 04
    "`ELR_LINES`.`EXT_LENGTH`,"+      // 05
    "`ELR_LINES`.`EXT_DATA`,"+        // 06
    "`ELR_LINES`.`EXT_EVENT_ID`,"+    // 07
    "`ELR_LINES`.`EXT_ANNC_TYPE`,"+   // 08
    "`ELR_LINES`.`TITLE`,"+           // 09
    "`ELR_LINES`.`EXT_CART_NAME` "+   // 10
    "from `ELR_LINES` left join `CART` "+
    "on `ELR_LINES`.`CART_NUMBER`=`CART`.`NUMBER` where "+
    "`SERVICE_NAME`='"+RDEscapeString(mixtable)+"' "+
    "order by `EVENT_DATETIME`";
  q=new RDSqlQuery(sql);

  //
  // Write Data Rows
  //
  while(q->next()) {
    if(q->value(4).isNull()) {
      *strm << "00:00:00 ";
    }
    else {
      *strm << q->value(4).toTime().toString("hh:mm:ss")+" ";
    }
    *strm << q->value(2).toDateTime().toString("hh:mm:ss")+" ";
    if(q->value(5).toInt()>0) {
      if(version>0) {
	*strm << QString("0");
      }
      *strm << RDGetTimeLength(q->value(5).toInt(),true,false)+" ";
    }
    else {
      *strm << "00:00:00 ";
    }
    if(q->value(0).toInt()>0) {
      *strm << QString("0")+RDGetTimeLength(q->value(0).toInt(),true,false)+" ";
    }
    else {
      *strm << "00:00:00 ";
    }
    *strm << QString().sprintf(air_fmt.toUtf8(),q->value(1).toUInt());
    *strm << RDReport::leftJustify(q->value(9).toString(),34)+" ";
    *strm << RDReport::leftJustify(q->value(6).toString(),32);
    *strm << "\x0d\x0a";
  }

  delete q;
  delete strm;
  delete file;
  report_error_code=RDReport::ErrorOk;

  return true;
}

