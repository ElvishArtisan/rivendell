// rdservice.cpp
//
// Rivendell Services Manager
//
//   (C) Copyright 2018-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/types.h>

#include <qcoreapplication.h>

#include <rd.h>
#include <rdapplication.h>
#include <rdcmd_switch.h>
#include <rdconf.h>

#include "rdservice.h"

bool global_exiting=false;
bool global_reload_dropboxes=false;

void SigHandler(int signo)
{
  switch(signo) {
  case SIGTERM:
  case SIGINT:
    global_exiting=true;
    break;

  case SIGUSR1:
    global_reload_dropboxes=true;
    break;
  }
}


MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  QString err_msg;
  RDApplication::ErrorType err_type=RDApplication::ErrorOk;
  int initial_maintenance_interval=-1;
  bool ok=false;

  svc_startup_target=MainObject::TargetAll;
  svc_force_system_maintenance=false;

  //
  // Check for prior instance
  //
  if(RDGetPids("rdservice").size()>1) {
    rda->syslog(LOG_ERR,"prior instance found");
    exit(RDApplication::ExitPriorInstance);
  }

  //
  // Open the Database
  //
  rda=new RDApplication("rdservice","rdservice","\n\n",this);
  if(!rda->open(&err_msg,&err_type,false)) {
    rda->syslog(LOG_ERR,"unable to open database [%s]",
		(const char *)err_msg.utf8());
    exit(RDApplication::ExitNoDb);
  }
  rda->syslog(LOG_DEBUG,"starting up");

  //
  // Process Startup Options
  //
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    for(int j=0;j<MainObject::TargetAll;j++) {
      MainObject::StartupTarget target=(MainObject::StartupTarget)j;
      if(rda->cmdSwitch()->key(i)==TargetCommandString(target)) {
	svc_startup_target=target;
	rda->cmdSwitch()->setProcessed(i,true);
      }
      if(rda->cmdSwitch()->key(i)=="--force-system-maintenance") {
	svc_force_system_maintenance=true;
	rda->cmdSwitch()->setProcessed(i,true);
      }
      if(rda->cmdSwitch()->key(i)=="--initial-maintenance-interval") {
	initial_maintenance_interval=rda->cmdSwitch()->value(i).toInt(&ok);
	if(!ok) {
	  fprintf(stderr,
	      "rdservice: invalid \"--initial-maintenance-interval\" value\n");
	  exit(4);
	}
	rda->cmdSwitch()->setProcessed(i,true);
      }
    }
    if(!rda->cmdSwitch()->processed(i)) {
      fprintf(stderr,"rdservice: unknown command-line option\n");
      exit(RDApplication::ExitInvalidOption);
    }
  }

  //
  // Exit Timer
  //
  ::signal(SIGINT,SigHandler);
  ::signal(SIGTERM,SigHandler);
  ::signal(SIGUSR1,SigHandler);
  svc_exit_timer=new QTimer(this);
  connect(svc_exit_timer,SIGNAL(timeout()),this,SLOT(exitData()));
  svc_exit_timer->start(100);

  if(!Startup(&err_msg)) {
    Shutdown();
    rda->syslog(LOG_ERR,"unable to start service component [%s]",
		(const char *)err_msg.toUtf8());
    exit(RDApplication::ExitSvcFailed);
  }

  //
  // Maintenance Routine Timer
  //
  srandom(QTime::currentTime().msec());
  svc_maint_timer=new QTimer(this);
  svc_maint_timer->setSingleShot(true);
  connect(svc_maint_timer,SIGNAL(timeout()),this,SLOT(checkMaintData()));
  int interval=GetMaintInterval();
  if(!rda->config()->disableMaintChecks()) {
    if(initial_maintenance_interval>=0) {
      interval=initial_maintenance_interval;
    }
    rda->syslog(LOG_DEBUG,"initial maintenance run at %s [%s from now]",
		QDateTime::currentDateTime().addMSecs(interval).
		toString("hh:mm:ss").toUtf8().constData(),
		RDGetTimeLength(interval,false,false).toUtf8().constData());
    svc_maint_timer->start(interval);
  }
  else {
    rda->syslog(LOG_INFO,"maintenance checks disabled on this host");
  }

  if(!RDWritePid(RD_PID_DIR,"rdservice.pid",getuid())) {
    rda->syslog(LOG_WARNING,"unable to write pid file to \"%s/rdservice.pid\"",
		RD_PID_DIR);
  }
}


void MainObject::processFinishedData(int id)
{
  RDProcess *proc=svc_processes.value(id);
  if(proc->process()->exitStatus()!=QProcess::NormalExit) {
    rda->syslog(LOG_WARNING,"process \"%s\" crashed!",
		proc->prettyCommandString().toUtf8().constData());
  }
  else {
    if(proc->process()->exitCode()!=0) {
      rda->syslog(LOG_WARNING,"process \"%s\" exited with exit code %d",
		  proc->prettyCommandString().toUtf8().constData(),
		  proc->process()->exitCode());
    }
    else {
      rda->syslog(LOG_DEBUG,"process \"%s\" exited normally",
		  proc->prettyCommandString().toUtf8().constData());
    }
  }
  proc->process()->deleteLater();
  svc_processes.remove(id);
}


void MainObject::exitData()
{
  QString err_msg;

  if(global_exiting) {
    Shutdown();
    RDDeletePid(RD_PID_DIR,"rdservice.pid");
    rda->syslog(LOG_DEBUG,"shutting down normally");
    exit(RDApplication::ExitOk);
  }

  if(global_reload_dropboxes) {
    ShutdownDropboxes();
    StartDropboxes(&err_msg);
    ::signal(SIGUSR1,SigHandler);
    global_reload_dropboxes=false;
  }
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);
  new MainObject();
  return a.exec();
}
