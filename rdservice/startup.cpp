// startup.cpp
//
// Startup routines for the Rivendell Services Manager
//
//   (C) Copyright 2018-2020 Fred Gleason <fredg@paravelsystems.com>
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
#include <syslog.h>

#include <qstringlist.h>
#include <qfileinfo.h>

#include <rdapplication.h>
#include <rdconf.h>
#include <rdescape_string.h>
#include <rdpaths.h>

#include "rdservice.h"

bool MainObject::Startup(QString *err_msg)
{
  QStringList args;
  QString sql;
  RDSqlQuery *q;

  //
  // Kill Stale Programs
  //
  KillProgram("rdrssd");
  KillProgram("rdrepld");
  KillProgram("rdvairplayd");
  KillProgram("rdpadengined");
  KillProgram("rdpadd");
  KillProgram("rdcatchd");
  KillProgram("ripcd");
  KillProgram("caed");

  //
  // caed(8)
  //
  svc_processes[RDSERVICE_CAED_ID]=new RDProcess(RDSERVICE_CAED_ID,this);
  args.clear();
  svc_processes[RDSERVICE_CAED_ID]->start(QString(RD_PREFIX)+"/sbin/caed",args);
  if(!svc_processes[RDSERVICE_CAED_ID]->process()->waitForStarted(-1)) {
    *err_msg=tr("unable to start caed(8)")+": "+
      svc_processes[RDSERVICE_CAED_ID]->errorText();
    return false;
  }
  if(svc_startup_target==MainObject::TargetCaed) {
    fprintf(stderr,"Startup target caed(8) reached\n");
    return true;
  }

  //
  // ripcd(8)
  //
  svc_processes[RDSERVICE_RIPCD_ID]=new RDProcess(RDSERVICE_RIPCD_ID,this);
  args.clear();
  svc_processes[RDSERVICE_RIPCD_ID]->
    start(QString(RD_PREFIX)+"/sbin/ripcd",args);
  if(!svc_processes[RDSERVICE_RIPCD_ID]->process()->waitForStarted(-1)) {
    *err_msg=tr("unable to start ripcd(8)")+": "+
      svc_processes[RDSERVICE_RIPCD_ID]->errorText();
    return false;
  }
  if(svc_startup_target==MainObject::TargetRipcd) {
    fprintf(stderr,"Startup target ripcd(8) reached\n");
    return true;
  }

  //
  // rdcatchd(8)
  //
  svc_processes[RDSERVICE_RDCATCHD_ID]=new RDProcess(RDSERVICE_RDCATCHD_ID,this);
  args.clear();
  svc_processes[RDSERVICE_RDCATCHD_ID]->
    start(QString(RD_PREFIX)+"/sbin/rdcatchd",args);
  if(!svc_processes[RDSERVICE_RDCATCHD_ID]->process()->waitForStarted(-1)) {
    *err_msg=tr("unable to start rdcatchd(8)")+": "+
      svc_processes[RDSERVICE_RDCATCHD_ID]->errorText();
    return false;
  }
  if(svc_startup_target==MainObject::TargetRdcatchd) {
    fprintf(stderr,"Startup target rdcatchd(8) reached\n");
    return true;
  }

  //
  // rdpadd(8)
  //
  svc_processes[RDSERVICE_RDPADD_ID]=new RDProcess(RDSERVICE_RDPADD_ID,this);
  args.clear();
  svc_processes[RDSERVICE_RDPADD_ID]->
    start(QString(RD_PREFIX)+"/sbin/rdpadd",args);
  if(!svc_processes[RDSERVICE_RDPADD_ID]->process()->waitForStarted(-1)) {
    *err_msg=tr("unable to start rdpadd(8)")+": "+
      svc_processes[RDSERVICE_RDPADD_ID]->errorText();
    return false;
  }
  if(svc_startup_target==MainObject::TargetRdpadd) {
    fprintf(stderr,"Startup target rdpadd(8) reached\n");
    return true;
  }

  //
  // *** BAND-AID * BAND_AID * YEECH! ***
  // This Makes It Work, but I think we're going to need to implement
  // socket activation on all of these services.
  //
  sleep(1);

  //
  // rdpadengined(8)
  //
  svc_processes[RDSERVICE_RDPADENGINED_ID]=
    new RDProcess(RDSERVICE_RDPADENGINED_ID,this);
  args.clear();
  svc_processes[RDSERVICE_RDPADENGINED_ID]->
    start(QString(RD_PREFIX)+"/sbin/rdpadengined",args);
  if(!svc_processes[RDSERVICE_RDPADENGINED_ID]->process()->waitForStarted(-1)) {
    *err_msg=tr("unable to start rdpadengined(8)")+": "+
      svc_processes[RDSERVICE_RDPADENGINED_ID]->errorText();
    return false;
  }
  if(svc_startup_target==MainObject::TargetRdpadengined) {
    fprintf(stderr,"Startup target rdpadengined(8) reached\n");
    return true;
  }

  //
  // rdvairplayd(8)
  //
  svc_processes[RDSERVICE_RDVAIRPLAYD_ID]=new RDProcess(RDSERVICE_RDVAIRPLAYD_ID,this);
  args.clear();
  svc_processes[RDSERVICE_RDVAIRPLAYD_ID]->
    start(QString(RD_PREFIX)+"/sbin/rdvairplayd",args);
  if(!svc_processes[RDSERVICE_RDVAIRPLAYD_ID]->process()->waitForStarted(-1)) {
    *err_msg=tr("unable to start rdvairplayd(8)")+": "+
      svc_processes[RDSERVICE_RDVAIRPLAYD_ID]->errorText();
    return false;
  }
  if(svc_startup_target==MainObject::TargetRdvairplayd) {
    fprintf(stderr,"Startup target rdvairplayd(8) reached\n");
    return true;
  }

  //
  // rdrepld(8)
  //
  sql=QString("select NAME from REPLICATORS where ")+
    "STATION_NAME=\""+RDEscapeString(rda->station()->name())+"\"";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    svc_processes[RDSERVICE_RDREPLD_ID]=new RDProcess(RDSERVICE_RDREPLD_ID,this);
    args.clear();
    svc_processes[RDSERVICE_RDREPLD_ID]->
      start(QString(RD_PREFIX)+"/sbin/rdrepld",args);
    if(!svc_processes[RDSERVICE_RDREPLD_ID]->process()->waitForStarted(-1)) {
      *err_msg=tr("unable to start rdrepld(8)")+": "+
	svc_processes[RDSERVICE_RDREPLD_ID]->errorText();
      return false;
    }
  }
  delete q;
  if(svc_startup_target==MainObject::TargetRdrepld) {
    fprintf(stderr,"Startup target rdrepld(8) reached\n");
    return true;
  }

  //
  // rdrssd(8)
  //
  sql=QString("select RSS_PROCESSOR_STATION from SYSTEM");
  q=new RDSqlQuery(sql);
  if(q->first()) {
    if(q->value(0).toString().toLower()==rda->station()->name().toLower()) {
      svc_processes[RDSERVICE_RDRSSD_ID]=
	new RDProcess(RDSERVICE_RDRSSD_ID,this);
      args.clear();
      svc_processes[RDSERVICE_RDRSSD_ID]->
	start(QString(RD_PREFIX)+"/sbin/rdrssd",args);
      if(!svc_processes[RDSERVICE_RDRSSD_ID]->process()->waitForStarted(-1)) {
	*err_msg=tr("unable to start rdrssd(8)")+": "+
	  svc_processes[RDSERVICE_RDRSSD_ID]->errorText();
	return false;
      }
    }
    if(svc_startup_target==MainObject::TargetRdrssd) {
      fprintf(stderr,"Startup target rdrssd(8) reached\n");
      return true;
    }
  }
  delete q;

  if(!StartDropboxes(err_msg)) {
    return false;
  }

  return true;
}


bool MainObject::StartDropboxes(QString *err_msg)
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;
  int id=RDSERVICE_FIRST_DROPBOX_ID;

  //
  // Launch Dropbox Configurations
  //
  sql=QString("select ")+
    "ID,"+                       // 00
    "GROUP_NAME,"+               // 01
    "PATH,"+                     // 02
    "NORMALIZATION_LEVEL,"+      // 03
    "AUTOTRIM_LEVEL,"+           // 04
    "TO_CART,"+                  // 05
    "USE_CARTCHUNK_ID,"+         // 06
    "TITLE_FROM_CARTCHUNK_ID,"+  // 07
    "DELETE_CUTS,"+              // 08
    "METADATA_PATTERN,"+         // 09
    "FIX_BROKEN_FORMATS,"+       // 10
    "LOG_TO_SYSLOG,"+            // 11
    "LOG_PATH,"+                 // 12
    "DELETE_SOURCE,"+            // 13
    "STARTDATE_OFFSET,"+         // 14
    "ENDDATE_OFFSET,"+           // 15
    "ID,"+                       // 16
    "IMPORT_CREATE_DATES,"+      // 17
    "CREATE_STARTDATE_OFFSET,"+  // 18
    "CREATE_ENDDATE_OFFSET,"+    // 19
    "SET_USER_DEFINED,"+         // 20
    "FORCE_TO_MONO,"+            // 21
    "SEGUE_LEVEL,"+              // 22
    "SEGUE_LENGTH,"+             // 23
    "SEND_EMAIL "+               // 24
    "from DROPBOXES where "+
    "STATION_NAME=\""+RDEscapeString(rda->config()->stationName())+"\"";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    QStringList args;

    args.push_back(QString().sprintf("--persistent-dropbox-id=%d",
				     q->value(16).toInt()));
    args.push_back("--drop-box");
    sql=QString("select SCHED_CODE from DROPBOX_SCHED_CODES where ")+
      QString().sprintf("DROPBOX_ID=%d",q->value(0).toInt());
    q1=new RDSqlQuery(sql);
    while(q1->next()) {
      args.push_back(QString().sprintf("--add-scheduler-code=")+
		     q1->value(0).toString()+"");
    }
    delete q1;
    args.push_back(QString().sprintf("--normalization-level=%d",
				     q->value(3).toInt()/100));
    args.push_back(QString().sprintf("--autotrim-level=%d",
				     q->value(4).toInt()/100));
    if(q->value(5).toUInt()>0) {
      args.push_back(QString().sprintf("--to-cart=%u",q->value(5).toUInt()));
    }
    if(q->value(6).toString()=="Y") {
      args.push_back("--use-cartchunk-cutid");
    }
    if(q->value(22).toInt()<1) {
      args.push_back(QString().sprintf("--segue-level=%d",
				       q->value(22).toInt()));
      args.push_back(QString().sprintf("--segue-length=%u",
				       q->value(23).toUInt()));
    }
    if(q->value(7).toString()=="Y") {
      args.push_back("--title-from-cartchunk-cutid");
    }
    if(q->value(8).toString()=="Y") {
      args.push_back("--delete-cuts");
    }
    if(q->value(21).toString()=="Y") {
      args.push_back("--to-mono");
    }
    if(!q->value(9).toString().isEmpty()) {
      args.push_back(QString("--metadata-pattern=")+q->value(9).toString());
    }
    if(q->value(10).toString()=="Y") {
      args.push_back("--fix-broken-formats");
    }
    if(q->value(13).toString()=="Y") {
      args.push_back("--delete-source");
    }
    if(q->value(24).toString()=="Y") {
      args.push_back("--send-mail");
      args.push_back("--mail-per-file");
    }
    if(q->value(17).toString()=="Y") {
      args.push_back(QString().sprintf("--create-startdate-offset=%d",
				       q->value(18).toInt()));
      args.push_back(QString().sprintf("--create-enddate-offset=%d",
				       q->value(19).toInt()));
    }
    if(!q->value(20).toString().isEmpty()) {
      args.push_back(QString("--set-user-defined=")+q->value(20).toString());
    }
    args.push_back(QString().sprintf("--startdate-offset=%d",
				     q->value(14).toInt()));
    args.push_back(QString().sprintf("--enddate-offset=%d",
				     q->value(15).toInt()));
    if(RDBool(q->value(11).toString())) {
      args.push_back("--log-syslog");
    }
    else {
      if(!q->value(12).toString().isEmpty()) {
	args.push_back("--log-filename="+q->value(12).toString());
      }
    }
    args.push_back(q->value(1).toString());
    args.push_back(q->value(2).toString());

    svc_processes[id]=new RDProcess(id,this);
    svc_processes[id]->start(QString(RD_PREFIX)+"/bin/rdimport",args);
    if(!svc_processes[id]->process()->waitForStarted(-1)) {
      *err_msg=tr("unable to start dropbox")+": "+
	svc_processes[id]->errorText();
      return false;
    }
    id++;
  }
  delete q;
  return true;
}


void MainObject::KillProgram(const QString &program)
{
  QList<pid_t> pids=RDGetPids(program);

  while(pids.size()>0) {
    for(int i=0;i<pids.size();i++) {
      kill(pids.at(i),SIGKILL);
      rda->syslog(LOG_WARNING,"killing unresponsive program \"%s\" [PID: %d]",
		  (const char *)program.toUtf8(),pids.at(i));
    }
    sleep(1);
    pids=RDGetPids(program);
  }
}


QString MainObject::TargetCommandString(MainObject::StartupTarget target) const
{
  switch(target) {
  case MainObject::TargetCaed:
    return QString("--end-startup-after-caed");

  case MainObject::TargetRipcd:
    return QString("--end-startup-after-ripcd");

  case MainObject::TargetRdcatchd:
    return QString("--end-startup-after-rdcatchd");

  case MainObject::TargetRdpadd:
    return QString("--end-startup-after-rdpadd");

  case MainObject::TargetRdpadengined:
    return QString("--end-startup-after-rdpadengined");

  case MainObject::TargetRdvairplayd:
    return QString("--end-startup-after-rdvairplayd");

  case MainObject::TargetRdrepld:
    return QString("--end-startup-after-rdrepld");

  case MainObject::TargetRdrssd:
    return QString("--end-startup-after-rdrssd");

  case MainObject::TargetAll:
    break;
  }

  return QString();
}
