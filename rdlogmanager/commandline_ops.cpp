// commandline_ops.cpp
//
// Command Line Operations for RDLogManager
//
//   (C) Copyright 2012-2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdlib.h>

#include <qapplication.h>
#include <qfile.h>

#include <dbversion.h>
#include <rdapplication.h>
#include <rddatedecode.h>
#include <rdlog.h>
#include <rdlog_event.h>
#include <rdreport.h>
#include <rdsvc.h>

#include <rdlogmanager.h>
#include <globals.h>

int RunReportOperation(int argc,char *argv[],const QString &rptname,
		       bool protect_existing,int start_offset,int end_offset)
{
  QString out_path;
  QString err_msg;

  QApplication a(argc,argv,false);

  if(end_offset<start_offset) {
    fprintf(stderr,
      "rdlogmanager: end date offset must be larger than start date offset\n");
    return 256;
  }

  rda=new RDApplication("RDLogManager","rdlogmanager",RDLOGMANAGER_USAGE);
  if(!rda->open(&err_msg)) {
    fprintf(stderr,"rdlogmanager: %s\n",err_msg.toUtf8().constData());
    exit(RD_EXIT_NO_DB);
  }

  //
  // Open Report Generator
  //
  RDReport *report=new RDReport(rptname,rda->station(),rda->config());
  if(!report->exists()) {
    fprintf(stderr,"rdlogmanager: no such report\n");
    return RD_EXIT_NO_REPORT;
  }

  //
  // Generate Report
  //
  QDate yesterday=QDate::currentDate().addDays(-1);
  if(protect_existing&&report->outputExists(yesterday.addDays(start_offset))) {
    fprintf(stderr,"report \"%s\" for %s already exists\n",
	    (const char *)rptname.utf8(),
	    (const char *)yesterday.addDays(start_offset).toString());
    exit(RD_EXIT_OUTPUT_PROTECTED);
  }
  if(!report->generateReport(yesterday.addDays(start_offset),
			     yesterday.addDays(end_offset),rda->station(),
			     &out_path)) {
    fprintf(stderr,"rdlogmanager: report generation failed [%s]\n",
	    (const char *)RDReport::errorText(report->errorCode()));
    return RD_EXIT_REPORT_FAILED;
  }
  return RD_EXIT_OK;
}
