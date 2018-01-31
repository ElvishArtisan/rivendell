// commandline_ops.cpp
//
// Command Line Operations for RDLogManager
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

#include <stdlib.h>

#include <qapplication.h>
#include <qfile.h>

#include <dbversion.h>
#include <rdapplication.h>
#include <rdcreate_log.h>
#include <rddatedecode.h>
#include <rdlog.h>
#include <rdlog_event.h>
#include <rdreport.h>
#include <rdsvc.h>

#include <rdlogmanager.h>
#include <globals.h>

int RunLogOperation(int argc,char *argv[],const QString &svcname,
		    int start_offset,bool protect_existing,bool gen_log,
		    bool merge_mus,bool merge_tfc)
{
  QString sql;
  RDSqlQuery *q;
  QString report;
  QString unused_report;
  QString svcname_table=svcname;
  svcname_table.replace(" ","_");
  QString err_msg;

  QApplication a(argc,argv,false);

  rda=new RDApplication("RDLogManager");
  if(!rda->open(&err_msg)) {
    fprintf(stderr,"rdlogmanager: %s\n",(const char *)err_msg);
    exit(1);
  }

  //
  // Some Basic Structures
  //
  RDSvc *svc=new RDSvc(svcname,rda->station(),rda->config());
  if(!svc->exists()) {
    fprintf(stderr,"rdlogmanager: no such service\n");
    return 256;
  }
  QDate start_date=QDate::currentDate().addDays(1+start_offset);
  QString logname=
    RDDateDecode(svc->nameTemplate(),start_date,rda->station(),rda->config(),
		 svc->name());
  RDLog *log=new RDLog(logname);

  //
  // Generate Log
  //
  if(gen_log) {
    if(protect_existing&&log->exists()) {
      fprintf(stderr,"log \"%s\" already exists\n",
	      (const char *)log->name().utf8());
      exit(256);
    }
    log->removeTracks(rda->station(),rda->user(),rda->config());
    srand(QTime::currentTime().msec());
    sql=RDCreateStackTableSql(svcname_table,rda->config());
    q=new RDSqlQuery(sql);
    if(!q->isActive()) {
      fprintf(stderr,"SQL: %s\n",(const char *)sql);
      fprintf(stderr,"SQL Error: %s\n",
	      (const char *)q->lastError().databaseText());
    }
    delete q;
    if(!svc->generateLog(start_date,
			 RDDateDecode(svc->nameTemplate(),start_date,
				      rda->station(),rda->config(),svc->name()),
			 RDDateDecode(svc->nameTemplate(),start_date.addDays(1),
				      rda->station(),rda->config(),svc->name()),
			 &unused_report,rda->user(),&err_msg)) {
      fprintf(stderr,"rdlogmanager: %s\n",(const char *)err_msg);
      return 256;
    }
    log->updateTracks();

    //
    // Generate Exception Report
    //
    RDLogEvent *event=
      new RDLogEvent(QString().sprintf("%s_LOG",(const char *)logname));
    event->load();
    if((event->validate(&report,start_date)!=0)||
       (!unused_report.isEmpty())) {
      printf("%s\n\n%s",(const char*)report,(const char*)unused_report);
    }
    delete event;
  }

  //
  // Merge Music
  //
  if(merge_mus) {
    if(!log->exists()) {
      fprintf(stderr,"rdlogmanager: log does not exist\n");
      return 256;
    }
    if(protect_existing&&
       (log->linkState(RDLog::SourceMusic)==RDLog::LinkDone)) {
      fprintf(stderr,"rdlogmanager: music for log \"%s\" is already imported\n",
	      (const char *)log->name().utf8());
      return 256;
    }
    report="";
    log->removeTracks(rda->station(),rda->user(),rda->config());
    if(!svc->clearLogLinks(RDSvc::Traffic,logname,rda->user(),&err_msg)) {
      fprintf(stderr,"rdlogmanager: %s\n",(const char *)err_msg);
      return 256;
    }
    if(!svc->clearLogLinks(RDSvc::Music,logname,rda->user(),&err_msg)) {
      fprintf(stderr,"rdlogmanager: %s\n",(const char *)err_msg);
      return 256;
    }
    if(svc->linkLog(RDSvc::Music,start_date,logname,&report,rda->user(),&err_msg)) {
      printf("%s\n",(const char*)report);
    }
    else {
      fprintf(stderr,"rdlogmanager: %s\n",(const char *)err_msg);
      exit(256);
    }
  }

  //
  // Merge Traffic
  //
  if(merge_tfc) {
    if(!log->exists()) {
      fprintf(stderr,"rdlogmanager: log does not exist\n");
      return 256;
    }
    if(protect_existing&&
       (log->linkState(RDLog::SourceTraffic)==RDLog::LinkDone)) {
      fprintf(stderr,
	      "rdlogmanager: traffic for log \"%s\" is already imported\n",
	      (const char *)log->name().utf8());
      return 256;
    }
    report="";
    if(!svc->clearLogLinks(RDSvc::Traffic,logname,rda->user(),&err_msg)) {
      fprintf(stderr,"rdlogmanager: %s\n",(const char *)err_msg);
      return 256;
    }
    if(svc->linkLog(RDSvc::Traffic,start_date,logname,&report,rda->user(),
		    &err_msg)) {
      printf("%s\n",(const char*)report);
    }
    else {
      fprintf(stderr,"rdlogmanager: %s\n",(const char *)err_msg);
    }
  }

  //
  // Clean Up
  //
  delete log;
  delete svc;
  return 0;
}


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

  rda=new RDApplication("RDLogManager");
  if(!rda->open(&err_msg)) {
    fprintf(stderr,"rdlogmanager: %s\n",(const char *)err_msg);
    exit(1);
  }

  //
  // Open Report Generator
  //
  RDReport *report=new RDReport(rptname,rda->station(),rda->config());
  if(!report->exists()) {
    fprintf(stderr,"rdlogmanager: no such report\n");
    return 256;
  }

  //
  // Generate Report
  //
  QDate yesterday=QDate::currentDate().addDays(-1);
  if(protect_existing&&report->outputExists(yesterday.addDays(start_offset))) {
    fprintf(stderr,"report \"%s\" for %s already exists\n",
	    (const char *)rptname.utf8(),
	    (const char *)yesterday.addDays(start_offset).toString());
    exit(256);
  }
  if(!report->generateReport(yesterday.addDays(start_offset),
			     yesterday.addDays(end_offset),rda->station(),
			     &out_path)) {
    fprintf(stderr,"rdlogmanager: report generation failed [%s]\n",
	    (const char *)RDReport::errorText(report->errorCode()));
    return 256;
  }
  return 0;
}
