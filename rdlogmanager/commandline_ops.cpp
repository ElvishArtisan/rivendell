// commandline_ops.cpp
//
// Command Line Operations for RDLogManager
//
//   (C) Copyright 2012-2023 Fred Gleason <fredg@paravelsystems.com>
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

#include <QCoreApplication>

#include <dbversion.h>
#include <rdapplication.h>
#include <rddatedecode.h>
#include <rdlog.h>
#include <rdreport.h>
#include <rdsvc.h>

#include <rdlogmanager.h>
#include <globals.h>

int RunReportOperation(int argc,char *argv[],const QString &rptname,
		       bool protect_existing,int start_offset,int end_offset,
		       double aggregate_tuning_hours)
{
  QString out_path;
  QString err_msg;

  QCoreApplication a(argc,argv);

  if(end_offset<start_offset) {
    fprintf(stderr,
      "rdlogmanager: end date offset must be larger than start date offset\n");
    return 256;
  }

  rda=static_cast<RDApplication *>(new RDCoreApplication("RDLogManager",
			     "rdlogmanager",RDLOGMANAGER_USAGE,true,NULL));
  if(!rda->open(&err_msg,NULL,true,false)) {
    fprintf(stderr,"rdlogmanager: %s\n",err_msg.toUtf8().constData());
    exit(RDApplication::ExitNoDb);
  }

  //
  // Open Report Generator
  //
  RDReport *report=new RDReport(rptname,rda->station(),rda->config());
  if(!report->exists()) {
    fprintf(stderr,"rdlogmanager: no such report\n");
    return RDApplication::ExitNoReport;
  }

  //
  // Check for Aggregate Tuning Hours
  //
  if((aggregate_tuning_hours<0)&&(report->aggregateTuningHoursRequired())) {
    fprintf(stderr,"rdlogmanager: -h option required\n");
    return RDApplication::ExitInvalidOption;
  }

  //
  // Generate Report
  //
  QDate yesterday=QDate::currentDate().addDays(-1);
  if(protect_existing&&report->outputExists(yesterday.addDays(start_offset))) {
    fprintf(stderr,"report \"%s\" for %s already exists\n",
	    rptname.toUtf8().constData(),
	    yesterday.addDays(start_offset).toString().toUtf8().constData());
    exit(RDApplication::ExitOutputProtected);
  }
  if(!report->generateReport(yesterday.addDays(start_offset),
			     yesterday.addDays(end_offset),rda->station(),
			     &out_path,aggregate_tuning_hours)) {
    fprintf(stderr,"rdlogmanager: report generation failed [%s]\n",
	    RDReport::errorText(report->errorCode()).toUtf8().constData());
    return RDApplication::ExitReportFailed;
  }
  return RDApplication::ExitOk;
}
