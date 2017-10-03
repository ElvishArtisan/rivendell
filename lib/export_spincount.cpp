// export_spincount.cpp
//
// Export a Rivendell Spin Count Report to an ASCII Text File.
//
//   (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
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

#include <map>

#include <qfile.h>
#include <qmessagebox.h>
#include <rddb.h>
#include <rdlog_line.h>
#include <rdairplay_conf.h>
#include <rdconf.h>
#include <rddatedecode.h>
#include <rdreport.h>


bool RDReport::ExportSpinCount(const QDate &startdate,const QDate &enddate,
			       const QString &mixtable)
{
  QString sql;
  RDSqlQuery *q;
  FILE *f;
  QString cut;
  QString str;
  QString cart_fmt;
  QString cart_num;
  std::map<unsigned,unsigned> carts;
  std::map<unsigned,QString> titles;
  std::map<unsigned,QString> artists;
  std::map<unsigned,QString> labels;
  std::map<unsigned,QString> albums;


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

  //
  // Generate Spin Counts
  //
  sql=QString("select CART_NUMBER,TITLE,ARTIST,ALBUM,LABEL ")+
    "from `"+mixtable+"_SRT` order by TITLE";
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
    fprintf(f,"                                          Rivendell Spin Count Report for %s\n",
	    (const char *)startdate.toString("MM/dd/yyyy"));
  }
  else {
    fprintf(f,"                                     Rivendell Spin Count Report for %s - %s\n",
	    (const char *)startdate.toString("MM/dd/yyyy"),
	    (const char *)enddate.toString("MM/dd/yyyy"));
  }
  str=QString().sprintf("%s -- %s\n",(const char *)name(),
			(const char *)description());
  for(unsigned i=0;i<(132-str.length())/2;i++) {
    fprintf(f," ");
  }
  fprintf(f,"%s\n",(const char *)str);
  //  fprintf(f,"------------------------------------------------------------------------------------------------------------------------------------\n");
  fprintf(f,"--Title------------------------ --Artist----------------------- --Album------------------------ --Label----------------------- Spins\n");

  //
  // Write Data Rows
  //
  for(std::map<unsigned,unsigned>::const_iterator it=carts.begin();
      it!=carts.end();it++) {
    fprintf(f,"%-30s  %-30s  %-30s  %-29s  %5u\n",
	    (const char *)titles[it->first],
	    (const char *)artists[it->first],
	    (const char *)albums[it->first],
	    (const char *)labels[it->first],
	    it->second);
  }
  fclose(f);
  report_error_code=RDReport::ErrorOk;
  return true;
}

