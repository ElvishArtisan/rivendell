// export_spincount.cpp
//
// Export a Rivendell Spin Count Report to an ASCII Text File.
//
//   (C) Copyright 2015-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <map>

#include <QFile>
#include <QMessageBox>
#include <QTextStream>

#include "rdairplay_conf.h"
#include "rdapplication.h"
#include "rdconf.h"
#include "rddatedecode.h"
#include "rddb.h"
#include "rdescape_string.h"
#include "rdlog_line.h"
#include "rdreport.h"

bool RDReport::ExportSpinCount(const QString &filename,const QDate &startdate,
			       const QDate &enddate,const QString &mixtable)
{
  QString sql;
  RDSqlQuery *q;
  QString cut;
  QString str;
  QString cart_fmt;
  QString cart_num;
  std::map<unsigned,unsigned> carts;
  std::map<unsigned,QString> titles;
  std::map<unsigned,QString> artists;
  std::map<unsigned,QString> labels;
  std::map<unsigned,QString> albums;

  QFile *file=new QFile(filename);
  if(!file->open(QIODevice::WriteOnly|QIODevice::Truncate)) {
    report_error_code=RDReport::ErrorCantOpen;
    delete file;
    return false;
  }
  QTextStream *strm=new QTextStream(file);
  strm->setCodec("UTF-8");
  if(useLeadingZeros()) {
    cart_fmt=QString::asprintf("%%0%uu",cartDigits());
  }
  else {
    cart_fmt="%6u";
  }

  //
  // Generate Spin Counts
  //
  sql=QString("select ")+
    "`CART_NUMBER`,"+  // 00
    "`TITLE`,"+        // 01
    "`ARTIST`,"+       // 02
    "`ALBUM`,"+       // 03
    "`LABEL` "+       // 04
    "from `ELR_LINES` where "+
    "`SERVICE_NAME`='"+RDEscapeString(mixtable)+"' "+
    "order by `TITLE`";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    carts[q->value(0).toInt()]++;
    titles[q->value(0).toInt()]=q->value(1).toString();
    artists[q->value(0).toInt()]=q->value(2).toString();
    albums[q->value(0).toInt()]=q->value(3).toString();
    labels[q->value(0).toInt()]=q->value(4).toString();
  }
  delete q;

  //
  // Write File Header
  //
  if(startdate==enddate) {
    *strm << RDReport::center(QString("Rivendell Spin Count Report for ")+
			      rda->shortDateString(startdate),132)+"\n";
  }
  else {
    *strm << RDReport::center(QString("Rivendell Spin Count Report for ")+
			      rda->shortDateString(startdate)+" - "+
			      rda->shortDateString(enddate),132)+"\n";
  }
  *strm << RDReport::center(name()+" -- "+description(),132)+"\n";
  *strm << "--Title------------------------ --Artist----------------------- --Album------------------------ --Label----------------------- Spins\n";

  //
  // Write Data Rows
  //
  for(std::map<unsigned,unsigned>::const_iterator it=carts.begin();
      it!=carts.end();it++) {
    *strm << RDReport::leftJustify(titles[it->first],30)+"  ";
    *strm << RDReport::leftJustify(artists[it->first],30)+"  ";
    *strm << RDReport::leftJustify(albums[it->first],30)+"  ";
    *strm << RDReport::leftJustify(labels[it->first],29)+"  ";
    *strm << QString::asprintf("%5u",it->second);
    *strm << "\n";
  }
  delete strm;
  delete file;
  report_error_code=RDReport::ErrorOk;
  return true;
}

