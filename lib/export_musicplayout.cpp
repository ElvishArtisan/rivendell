// export_musicplayout.cpp
//
// Export a Rivendell Report to an ASCII Text File.
//
//   (C) Copyright 2012-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include "rdairplay_conf.h"
#include "rdconf.h"
#include "rddatedecode.h"
#include "rddb.h"
#include "rdescape_string.h"
#include "rdlog_line.h"
#include "rdreport.h"

bool RDReport::ExportMusicPlayout(const QString &filename,
				  const QDate &startdate,const QDate &enddate,
				  const QString &mixtable)
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
    "`ELR_LINES`.`EXT_EVENT_ID`,"+    // 03
    "`ELR_LINES`.`TITLE`,"+           // 04
    "`ELR_LINES`.`CUT_NUMBER`,"+      // 05
    "`ELR_LINES`.`ARTIST`,"+          // 06
    "`ELR_LINES`.`ALBUM`,"+           // 07
    "`ELR_LINES`.`LABEL` "+           // 08
    "from `ELR_LINES` left join `CART` "+
    "on `ELR_LINES`.`CART_NUMBER`=`CART`.`NUMBER` where "+
    "`ELR_LINES`.`SERVICE_NAME`='"+RDEscapeString(mixtable)+"' "+
    "order by `EVENT_DATETIME`";
  q=new RDSqlQuery(sql);

  //
  // Write File Header
  //
  if(startdate==enddate) {
    *strm << RDReport::center(QString("Rivendell RDAirPlay Music Playout Report for ")+
		    startdate.toString("MM/dd/yyyy"),144)+"\n";
  }
  else {
    *strm << RDReport::center(QString("Rivendell RDAirPlay Music Playout Report for ")+
		    startdate.toString("MM/dd/yyyy")+" - "+
		    enddate.toString("MM/dd/yyyy"),144)+"\n";
  }
  *strm << RDReport::center(name()+" -- "+description(),144)+"\n";
  *strm << "--Time--  -Cart-  Cut  A-Len  --Title-----------------------   --Artist----------------------   --Album------------------   --Label-------------\n";

  //
  // Write Data Rows
  //
  while(q->next()) {
    if(q->value(5).toInt()>0) {
      cut=QString().sprintf("%03d",q->value(5).toInt());
    }
    else {
      if((RDAirPlayConf::TrafficAction)q->value(6).toInt()==
	 RDAirPlayConf::TrafficMacro) {
	cut="rml";
      }
      else {
	cut="   ";
      }
    }
    cart_num=QString().sprintf(cart_fmt.toUtf8(),q->value(1).toUInt());
    *strm << q->value(2).toDateTime().time().toString("hh:mm:ss")+"  ";
    *strm << cart_num+"  ";
    *strm << cut+"  ";
    *strm << RDGetTimeLength(q->value(0).toInt(),true,false).right(5)+"  ";
    *strm << RDReport::leftJustify(q->value(4).toString(),30)+"   ";
    *strm << RDReport::leftJustify(q->value(6).toString(),30)+"   ";
    *strm << RDReport::leftJustify(q->value(7).toString(),25)+"   ";
    *strm << RDReport::leftJustify(q->value(8).toString(),20)+"\n";
  }
  delete q;
  delete strm;
  delete file;
  report_error_code=RDReport::ErrorOk;

  return true;
}
