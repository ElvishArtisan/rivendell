// commandline_ops.cpp
//
// Command Line Operations for RDLogManager
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: commandline_ops.cpp,v 1.1.2.6.2.1 2014/05/20 14:01:50 cvs Exp $
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

#include <rdsvc.h>
#include <rddatedecode.h>
#include <rdlog.h>
#include <rdreport.h>
#include <rdcreate_log.h>
#include <rdlog_event.h>
#include <dbversion.h>

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
  unsigned schema=0;

  QApplication a(argc,argv,false);

  //
  // Load Local Configs
  //
  RDConfig *config=new RDConfig();
  config->load();

  //
  // Open Database
  //
  QString err;
  QSqlDatabase *db=RDInitDb(&schema,&err);
  if(!db) {
    fprintf(stderr,"rdlogmanager: unable to connect to database\n");
    return 256;
  }
  if((schema!=RD_VERSION_DATABASE)&&(!skip_db_check)) {
    fprintf(stderr,
	    "rdlogmanager: database version mismatch, should be %u, is %u\n",
	    RD_VERSION_DATABASE,schema);
    exit(256);
  }

  //
  // Some Basic Structures
  //
  rdstation_conf=new RDStation(config->stationName());
#ifndef WIN32
  rduser=new RDUser(rdstation_conf->defaultName());
#endif  // WIN32
  RDSvc *svc=new RDSvc(svcname);
  if(!svc->exists()) {
    fprintf(stderr,"rdlogmanager: no such service\n");
    return 256;
  }
  QDate start_date=QDate::currentDate().addDays(1+start_offset);
  QString logname=RDDateDecode(svc->nameTemplate(),start_date);
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
    log->removeTracks(rdstation_conf,rduser,config);
    srand(QTime::currentTime().msec());
    sql=RDCreateStackTableSql(svcname_table);
    q=new RDSqlQuery(sql);
    if(!q->isActive()) {
      fprintf(stderr,"SQL: %s\n",(const char *)sql);
      fprintf(stderr,"SQL Error: %s\n",
	      (const char *)q->lastError().databaseText());
    }
    delete q;
    if(!svc->generateLog(start_date,
			 RDDateDecode(svc->nameTemplate(),start_date),
			 RDDateDecode(svc->nameTemplate(),start_date.addDays(1)),
			 &unused_report)) {
      fprintf(stderr,"rdlogmanager: unable to generate log\n");
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
    log->removeTracks(rdstation_conf,rduser,config);
    svc->clearLogLinks(RDSvc::Traffic,start_date,logname);
    svc->clearLogLinks(RDSvc::Music,start_date,logname);
    if(svc->linkLog(RDSvc::Music,start_date,logname,&report)) {
      printf("%s\n",(const char*)report);
    }
    else {
      fprintf(stderr,
	      "rdlogmanager: unable to open music schedule file at \"%s\"\n",
	      (const char *)svc->importFilename(RDSvc::Music,start_date));
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
    svc->clearLogLinks(RDSvc::Traffic,start_date,logname);
    if(svc->linkLog(RDSvc::Traffic,start_date,logname,&report)) {
      printf("%s\n",(const char*)report);
    }
    else {
      fprintf(stderr,
	      "rdlogmanager: unable to open traffic schedule file at \"%s\"\n",
	      (const char *)svc->importFilename(RDSvc::Traffic,start_date));
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
  unsigned schema=0;
  QString out_path;

  QApplication a(argc,argv,false);

  if(end_offset<start_offset) {
    fprintf(stderr,
      "rdlogmanager: end date offset must be larger than start date offset\n");
    return 256;
  }

  //
  // Load Local Configs
  //
  RDConfig *config=new RDConfig();
  config->load();

  //
  // Open Database
  //
  QString err;
  QSqlDatabase *db=RDInitDb(&schema,&err);
  if(!db) {
    fprintf(stderr,"rdlogmanager: unable to connect to database\n");
    return 256;
  }
  if((schema!=RD_VERSION_DATABASE)&&(!skip_db_check)) {
    fprintf(stderr,
	    "rdlogmanager: database version mismatch, should be %u, is %u\n",
	    RD_VERSION_DATABASE,schema);
    exit(256);
  }
  rdstation_conf=new RDStation(config->stationName());

  //
  // Open Report Generator
  //
  RDReport *report=new RDReport(rptname);
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
			     yesterday.addDays(end_offset),rdstation_conf,
			     &out_path)) {
    fprintf(stderr,"rdlogmanager: report generation failed [%s]\n",
	    (const char *)RDReport::errorText(report->errorCode()));
    return 256;
  }
  return 0;
}
