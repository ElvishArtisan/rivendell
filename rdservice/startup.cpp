// startup.cpp
//
// Startup routines for the Rivendell Services Manager
//
//   (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
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
  KillProgram("rdrepld");
  KillProgram("rdvairplayd");
  KillProgram("rdpadd");
  KillProgram("rdcatchd");
  KillProgram("ripcd");
  KillProgram("caed");

  //
  // caed(8)
  //
  svc_processes[RDSERVICE_CAED_ID]=new Process(RDSERVICE_CAED_ID,this);
  args.clear();
  svc_processes[RDSERVICE_CAED_ID]->start(QString(RD_PREFIX)+"/sbin/caed",args);
  if(!svc_processes[RDSERVICE_CAED_ID]->process()->waitForStarted(-1)) {
    *err_msg=tr("unable to start caed(8)")+": "+
      svc_processes[RDSERVICE_CAED_ID]->errorText();
    return false;
  }

  //
  // ripcd(8)
  //
  svc_processes[RDSERVICE_RIPCD_ID]=new Process(RDSERVICE_RIPCD_ID,this);
  args.clear();
  svc_processes[RDSERVICE_RIPCD_ID]->
    start(QString(RD_PREFIX)+"/sbin/ripcd",args);
  if(!svc_processes[RDSERVICE_RIPCD_ID]->process()->waitForStarted(-1)) {
    *err_msg=tr("unable to start ripcd(8)")+": "+
      svc_processes[RDSERVICE_RIPCD_ID]->errorText();
    return false;
  }

  //
  // rdcatchd(8)
  //
  svc_processes[RDSERVICE_RDCATCHD_ID]=new Process(RDSERVICE_RDCATCHD_ID,this);
  args.clear();
  svc_processes[RDSERVICE_RDCATCHD_ID]->
    start(QString(RD_PREFIX)+"/sbin/rdcatchd",args);
  if(!svc_processes[RDSERVICE_RDCATCHD_ID]->process()->waitForStarted(-1)) {
    *err_msg=tr("unable to start rdcatchd(8)")+": "+
      svc_processes[RDSERVICE_RDCATCHD_ID]->errorText();
    return false;
  }

  //
  // rdpadd(8)
  //
  svc_processes[RDSERVICE_RDPADD_ID]=new Process(RDSERVICE_RDPADD_ID,this);
  args.clear();
  svc_processes[RDSERVICE_RDPADD_ID]->
    start(QString(RD_PREFIX)+"/sbin/rdpadd",args);
  if(!svc_processes[RDSERVICE_RDPADD_ID]->process()->waitForStarted(-1)) {
    *err_msg=tr("unable to start rdpadd(8)")+": "+
      svc_processes[RDSERVICE_RDPADD_ID]->errorText();
    return false;
  }

  //
  // rdvairplayd(8)
  //
  svc_processes[RDSERVICE_RDVAIRPLAYD_ID]=new Process(RDSERVICE_RDVAIRPLAYD_ID,this);
  args.clear();
  svc_processes[RDSERVICE_RDVAIRPLAYD_ID]->
    start(QString(RD_PREFIX)+"/sbin/rdvairplayd",args);
  if(!svc_processes[RDSERVICE_RDVAIRPLAYD_ID]->process()->waitForStarted(-1)) {
    *err_msg=tr("unable to start rdvairplayd(8)")+": "+
      svc_processes[RDSERVICE_RDVAIRPLAYD_ID]->errorText();
    return false;
  }

  //
  // rdrepld(8)
  //
  sql=QString("select NAME from REPLICATORS where ")+
    "STATION_NAME=\""+RDEscapeString(rda->station()->name())+"\"";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    svc_processes[RDSERVICE_RDREPLD_ID]=new Process(RDSERVICE_RDREPLD_ID,this);
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
    "LOG_PATH,"+                 // 11
    "DELETE_SOURCE,"+            // 12
    "STARTDATE_OFFSET,"+         // 13
    "ENDDATE_OFFSET,"+           // 14
    "ID,"+                       // 15
    "IMPORT_CREATE_DATES,"+      // 16
    "CREATE_STARTDATE_OFFSET,"+  // 17
    "CREATE_ENDDATE_OFFSET,"+    // 18
    "SET_USER_DEFINED,"+         // 19
    "FORCE_TO_MONO,"+            // 20
    "SEGUE_LEVEL,"+              // 21
    "SEGUE_LENGTH "+             // 22
    "from DROPBOXES where "+
    "STATION_NAME=\""+RDEscapeString(rda->config()->stationName())+"\"";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    QStringList args;

    args.push_back(QString().sprintf("--persistent-dropbox-id=%d",
				     q->value(15).toInt()));
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
    if(q->value(21).toInt()<1) {
      args.push_back(QString().sprintf("--segue-level=%d",
				       q->value(21).toInt()));
      args.push_back(QString().sprintf("--segue-length=%u",
				       q->value(22).toUInt()));
    }
    if(q->value(7).toString()=="Y") {
      args.push_back("--title-from-cartchunk-cutid");
    }
    if(q->value(8).toString()=="Y") {
      args.push_back("--delete-cuts");
    }
    if(q->value(20).toString()=="Y") {
      args.push_back("--to-mono");
    }
    if(!q->value(9).toString().isEmpty()) {
      args.push_back(QString("--metadata-pattern=")+q->value(9).toString());
    }
    if(q->value(10).toString()=="Y") {
      args.push_back("--fix-broken-formats");
    }
    if(q->value(12).toString()=="Y") {
      args.push_back("--delete-source");
    }
    if(q->value(16).toString()=="Y") {
      args.push_back(QString().sprintf("--create-startdate-offset=%d",
				       q->value(17).toInt()));
      args.push_back(QString().sprintf("--create-enddate-offset=%d",
				       q->value(18).toInt()));
    }
    if(!q->value(19).toString().isEmpty()) {
      args.push_back(QString("--set-user-defined=")+q->value(19).toString());
    }
    args.push_back(QString().sprintf("--startdate-offset=%d",
				     q->value(13).toInt()));
    args.push_back(QString().sprintf("--enddate-offset=%d",
				     q->value(14).toInt()));
    if(!q->value(11).toString().isEmpty()) {
      QFileInfo *fileinfo=new QFileInfo(q->value(11).toString());
      args.push_back(QString().sprintf("--log-filename=%s",
				     (const char *)fileinfo->fileName()));
      args.push_back(QString().sprintf("--log-directory=%s",
				     (const char *)fileinfo->absolutePath()));
      args.push_back("--verbose");
    }
    args.push_back(q->value(1).toString());
    args.push_back(q->value(2).toString());

    svc_processes[id]=new Process(id,this);
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
      syslog(LOG_WARNING,"killing unresponsive program \"%s\" [PID: %d]",
	     (const char *)program.toUtf8(),pids.at(i));
    }
    sleep(1);
    pids=RDGetPids(program);
  }
}
