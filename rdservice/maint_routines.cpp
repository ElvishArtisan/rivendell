// main_routines.cpp
//
// Rivendell Maintenance Routines
//
//   (C) Copyright 2008-2020 Fred Gleason <fredg@paravelsystems.com>
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
#include <syslog.h>

#include <rd.h>
#include <rdapplication.h>
#include <rdpaths.h>

#include "rdservice.h"

void MainObject::checkMaintData()
{
  QString sql;
  RDSqlQuery *q;
  QDateTime current_datetime=
    QDateTime(QDate::currentDate(),QTime::currentTime());
  bool run=false;

  //
  // Schedule Next Maintenance Run
  //
  svc_maint_timer->start(GetMaintInterval());

  RunLocalMaintRoutine();

  //
  // Should we try to run system maintenance?
  //
  if(!rda->station()->systemMaint()) {
    return;
  }

  //
  // Get the system-wide maintenance timestamp
  //
  sql="lock tables VERSION write";
  q=new RDSqlQuery(sql);
  delete q;
  sql="select LAST_MAINT_DATETIME from VERSION";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    run=1000*q->value(0).toDateTime().secsTo(current_datetime)>
      RD_MAINT_MAX_INTERVAL;
  }
  delete q;
  sql="unlock tables";
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Run the routines
  //
  if(run) {
    RunSystemMaintRoutine();
  }
}


void MainObject::RunSystemMaintRoutine()
{
  QStringList args;

  args.clear();
  args.push_back("--system");
  RunEphemeralProcess(RDSERVICE_SYSTEMMAINT_ID,
		      QString(RD_PREFIX)+"/bin/rdmaint",args);
  rda->syslog(LOG_INFO,"ran system-wide maintenance routines");
}


void MainObject::RunLocalMaintRoutine()
{
  RunEphemeralProcess(RDSERVICE_LOCALMAINT_ID,
		      QString(RD_PREFIX)+"/bin/rdmaint",QStringList());

  rda->syslog(LOG_INFO,"ran local maintenance routines");
}


int MainObject::GetMaintInterval() const
{
  return (int)(RD_MAINT_MIN_INTERVAL+
	       (RD_MAINT_MAX_INTERVAL-RD_MAINT_MIN_INTERVAL)*
	       (double)random()/(double)RAND_MAX);
}


void MainObject::RunEphemeralProcess(int id,const QString &program,
				     const QStringList &args)
{
  svc_processes[id]=new RDProcess(id,this);
  connect(svc_processes[id],SIGNAL(finished(int)),
	  this,SLOT(processFinishedData(int)));
  svc_processes[id]->start(program,args);
  if(!svc_processes[id]->process()->waitForStarted()) {
    QString err_msg=tr("unable to start")+"\""+program+"\": "+
      svc_processes[id]->errorText();
    rda->syslog(LOG_WARNING,"%s",(const char *)err_msg.toUtf8());
    delete svc_processes[id];
    svc_processes.remove(id);
  }
}
