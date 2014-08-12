// export_musicsummary.cpp
//
// Export a Rivendell Report to an ASCII Text File.
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: export_musicsummary.cpp,v 1.1.2.1 2012/08/13 20:08:27 cvs Exp $
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


bool RDReport::ExportMusicSummary(const QDate &startdate,const QDate &enddate,
				  const QString &mixtable)
{
  QString sql;
  RDSqlQuery *q;
  FILE *f;
  QString cut;
  QString str;

#ifdef WIN32
  QString filename=RDDateDecode(exportPath(RDReport::Windows),startdate);
#else
  QString filename=RDDateDecode(exportPath(RDReport::Linux),startdate);
#endif

  QFile file(filename);
  if((f=fopen((const char *)filename,"w"))==NULL) {
    report_error_code=RDReport::ErrorCantOpen;
    return false;
  }
  sql=QString().sprintf("select `%s_SRT`.ARTIST,`%s_SRT`.TITLE,\
                        `%s_SRT`.ALBUM \
                         from `%s_SRT` left join CART on\
                         `%s_SRT`.CART_NUMBER=CART.NUMBER\
                         order by EVENT_DATETIME",
			(const char *)mixtable,
			(const char *)mixtable,
			(const char *)mixtable,
			(const char *)mixtable,
			(const char *)mixtable);
  q=new RDSqlQuery(sql);

  //
  // Write File Header
  //
  if(startdate==enddate) {
    fprintf(f,"            Rivendell RDAirPlay Music Summary Report for %s\n",
	    (const char *)startdate.toString("MM/dd/yyyy"));
  }
  else {
    fprintf(f,"       Rivendell RDAirPlay Music Summary Report for %s - %s\n",
	    (const char *)startdate.toString("MM/dd/yyyy"),
	    (const char *)enddate.toString("MM/dd/yyyy"));
  }
  str=QString().sprintf("%s -- %s\n",(const char *)name(),
			(const char *)description());
  for(unsigned i=0;i<(80-str.length())/2;i++) {
    fprintf(f," ");
  }
  fprintf(f,"%s\n",(const char *)str);

  //
  // Write Data Rows
  //
  while(q->next()) {
    if(!q->value(0).toString().isEmpty()) {
      fprintf(f,"%s - ",(const char *)q->value(0).toString());
    }
    fprintf(f,"%s",(const char *)q->value(1).toString());
    if(!q->value(2).toString().isEmpty()) {
      fprintf(f," [%s]",(const char *)q->value(2).toString());
    }
    fprintf(f,"\n");
  }
  delete q;
  fclose(f);
  report_error_code=RDReport::ErrorOk;
  return true;
}

