// export_musicsummary.cpp
//
// Export a Rivendell Report to an ASCII Text File.
//
//   (C) Copyright 2012,2016-2018 Fred Gleason <fredg@paravelsystems.com>
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

#include "rdairplay_conf.h"
#include "rdconf.h"
#include "rddatedecode.h"
#include "rddb.h"
#include "rdescape_string.h"
#include "rdlog_line.h"
#include "rdreport.h"

bool RDReport::ExportMusicSummary(const QString &filename,
				  const QDate &startdate,const QDate &enddate,
				  const QString &mixtable)
{
  QString sql;
  RDSqlQuery *q;
  QString cut;
  QString str;

  QFile *file=new QFile(filename);
  if(!file->open(IO_WriteOnly|IO_Truncate)) {
    report_error_code=RDReport::ErrorCantOpen;
    delete file;
    return false;
  }
  QTextStream *strm=new QTextStream(file);
  strm->setEncoding(QTextStream::UnicodeUTF8);
  sql=QString("select ")+
    "ELR_LINES.ARTIST,"+  // 00
    "ELR_LINES.TITLE,"+   // 01
    "ELR_LINES.ALBUM "+   // 02
    "from ELR_LINES left join CART "+
    "on ELR_LINES.CART_NUMBER=CART.NUMBER where "+
    "SERVICE_NAME=\""+RDEscapeString(mixtable)+"\" "+
    "order by EVENT_DATETIME";
  q=new RDSqlQuery(sql);

  //
  // Write File Header
  //
  if(startdate==enddate) {
    *strm << Center(QString("Rivendell RDAirPlay Music Summary Report for ")+
		    startdate.toString("MM/dd/yyyy"),75)+"\n";
  }
  else {
    *strm << Center(QString("Rivendell RDAirPlay Music Summary Report for ")+
		    startdate.toString("MM/dd/yyyy")+" - "+
		    enddate.toString("MM/dd/yyyy"),75)+"\n";
  }
  *strm << Center(name()+" -- "+description(),75)+"\n";

  //
  // Write Data Rows
  //
  while(q->next()) {
    if(!q->value(0).toString().isEmpty()) {
      *strm << q->value(0).toString()+" - ";
    }
    *strm << q->value(1).toString();
    if(!q->value(2).toString().isEmpty()) {
      *strm << QString("[")+q->value(2).toString()+"]";
    }
    *strm << "\n";
  }
  delete q;
  delete strm;
  delete file;
  report_error_code=RDReport::ErrorOk;
  return true;
}

