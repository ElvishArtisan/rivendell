// rdpadengined.cpp
//
// Rivendell PAD Consolidation Server
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

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <syslog.h>

#include <QCoreApplication>

#include <rdapplication.h>
#include <rdconf.h>
#include <rdescape_string.h>
#include <rdpaths.h>

#include "rdpadengined.h"

bool global_pad_exiting=false;

void SigHandler(int signo)
{
  switch(signo) {
  case SIGINT:
  case SIGTERM:
    global_pad_exiting=true;
    break;
  }
}


MainObject::MainObject(QObject *parent)
  : QObject(parent)
{
  QString err_msg;
  RDApplication::ErrorType err_type=RDApplication::ErrorOk;

  //
  // Open the Database
  //
  rda=static_cast<RDApplication *>(new RDCoreApplication("rdpadengined","rdpadengined",RDPADENGINED_USAGE,this));
  if(!rda->open(&err_msg,&err_type,false)) {
    fprintf(stderr,"rdpadengined: %s\n",err_msg.toUtf8().constData());
    exit(1);
  }

  //
  // Shed root permissions
  //
  if(getuid()==0) {
    if(setgid(rda->config()->pypadGid())!=0) {
      rda->syslog(LOG_ERR,"unable to set GID to %d [%s], exiting",
		  rda->config()->pypadGid(),strerror(errno));
      exit(1);
    }
    if(setuid(rda->config()->pypadUid())!=0) {
      rda->syslog(LOG_ERR,"unable to set UID to %d [%s], exiting",
		  rda->config()->pypadUid(),strerror(errno));
      exit(1);
    }
  }

  //
  // Read Command Options
  //
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(!rda->cmdSwitch()->processed(i)) {
      rda->syslog(LOG_ERR,"unknown command option \"%s\"",
		  (const char *)rda->cmdSwitch()->key(i).toUtf8());
      exit(2);
    }
  }

  //
  // RIPCD Connection
  //
  connect(rda->ripc(),SIGNAL(connected(bool)),
	  this,SLOT(ripcConnectedData(bool)));
  connect(rda->ripc(),SIGNAL(notificationReceived(RDNotification *)),
	  this,SLOT(notificationReceivedData(RDNotification *)));
  rda->
    ripc()->connectHost("localhost",RIPCD_TCP_PORT,rda->config()->password());

  //
  // Exit Timer
  //
  pad_exit_timer=new QTimer(this);
  connect(pad_exit_timer,SIGNAL(timeout()),this,SLOT(exitData()));
  pad_exit_timer->start(100);

  ::signal(SIGINT,SigHandler);
  ::signal(SIGTERM,SigHandler);
}


void MainObject::ripcConnectedData(bool state)
{
  QStringList args;
  QString sql;
  RDSqlQuery *q;

  //
  // Clear DB Records
  //
  sql=QString("update `PYPAD_INSTANCES` set ")+
    "`IS_RUNNING`='N',"+
    "`EXIT_CODE`=0,"+
    "`ERROR_TEXT`=null "+
    "where `STATION_NAME`='"+RDEscapeString(rda->station()->name())+"'";
  RDSqlQuery::apply(sql);

  //
  // Start Scripts
  //
  sql=QString("select ")+
    "`ID` "           // 00
    "from `PYPAD_INSTANCES` where "+
    "`STATION_NAME`='"+RDEscapeString(rda->station()->name())+"'";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    StartScript(q->value(0).toUInt());
  }
  delete q;
}


void MainObject::notificationReceivedData(RDNotification *notify)
{
  QString sql;
  RDSqlQuery *q;

  if(notify->type()==RDNotification::PypadType) {
    int id=notify->id().toUInt();
    switch(notify->action()) {
    case RDNotification::AddAction: 
      sql=QString("select `ID` from `PYPAD_INSTANCES` where ")+
	QString::asprintf("`ID`=%u && ",id)+
	"STATION_NAME='"+RDEscapeString(rda->station()->name())+"'";
      q=new RDSqlQuery(sql);
      if(q->first()) {
	StartScript(id);
      }
      delete q;
     break;

    case RDNotification::DeleteAction:
      pad_instances.value(id)->setPrivateData((void *)true);  // No Restart
      KillScript(id);
      break;

    case RDNotification::ModifyAction:
      if(ScriptIsActive(id)) {
	KillScript(id);
      }
      else {
	StartScript(id);
      }
      break;

    case RDNotification::NoAction:
    case RDNotification::LastAction:
      break;
    }
  }
}


void MainObject::instanceStartedData(int id)
{
  SetRunStatus(id,true);
}


void MainObject::instanceFinishedData(int id)
{
  RDProcess *proc=pad_instances.value(id);

  if(proc->process()->exitStatus()!=QProcess::NormalExit) {
    rda->syslog(LOG_WARNING,"PyPAD script %d crashed\n",id);
    SetRunStatus(id,false,-1,proc->standardErrorData());
    proc->deleteLater();
    pad_instances.remove(id);
    return;
  }
  if(proc->process()->exitCode()==0) {
    SetRunStatus(id,false);
    bool no_restart=(bool)proc->privateData();
    proc->deleteLater();
    pad_instances.remove(id);
    if(!no_restart) {
      StartScript(id);
    }
  }
  else {
    if(!global_pad_exiting) {
      rda->syslog(LOG_WARNING,"PyPAD script ID %d exited with code %d",
		  id,proc->process()->exitCode());
      SetRunStatus(id,false,proc->process()->exitCode(),
		   proc->standardErrorData());
    }
  }
}


void MainObject::exitData()
{
  if(global_pad_exiting) {
    //
    // Shut down all instances
    //
    for(QMap<unsigned,RDProcess *>::const_iterator it=pad_instances.begin();
	it!=pad_instances.end();it++) {
      it.value()->setPrivateData((void *)true);  // No Restart
      it.value()->process()->terminate();
    }

    //
    // Update Database
    //
    QString sql=QString("update `PYPAD_INSTANCES` set ")+
      "`IS_RUNNING`='N' where "+
      "`STATION_NAME`='"+RDEscapeString(rda->station()->name())+"'";
    RDSqlQuery::apply(sql);
    exit(0);
  }
}


bool MainObject::ScriptIsActive(unsigned id) const
{
  bool ret=false;

  if(pad_instances.value(id)!=NULL) {
    ret=pad_instances.value(id)->process()->state()!=QProcess::NotRunning;
  }

  return ret;
}


void MainObject::StartScript(unsigned id)
{
  QString sql=QString("select `SCRIPT_PATH` from `PYPAD_INSTANCES` where ")+
    QString::asprintf("`ID`=%u && ",id)+
    "`STATION_NAME`='"+RDEscapeString(rda->station()->name())+"'";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    RDProcess *proc=new RDProcess(id,this);
    pad_instances[id]=proc;
    connect(proc,SIGNAL(started(int)),this,SLOT(instanceStartedData(int)));
    connect(proc,SIGNAL(finished(int)),this,SLOT(instanceFinishedData(int)));
    QStringList args;
    args.push_back(q->value(0).toString());
    args.push_back("localhost");
    args.push_back(QString::asprintf("%u",RD_PAD_CLIENT_TCP_PORT));
    args.push_back(QString::asprintf("$%u",id));
    pad_instances.value(id)->start(RD_PYPAD_PYTHON_PATH,args);
    rda->syslog(LOG_INFO,"%s",(QString("starting: ")+proc->program()+" "+
			       proc->arguments().join(" ")).toUtf8().constData());
  }
  delete q;
}


void MainObject::KillScript(unsigned id)
{
  pad_instances.value(id)->process()->terminate();
}


void MainObject::SetRunStatus(unsigned id,bool state,int exit_code,
			      const QString &err_text) const
{
  QString sql=QString("update `PYPAD_INSTANCES` set ")+
    "`IS_RUNNING`='"+RDYesNo(state)+"',"+
    QString::asprintf("`EXIT_CODE`=%u,",exit_code);
  if(err_text.isNull()) {
    sql+="`ERROR_TEXT`=null ";
  }
  else {
    sql+="`ERROR_TEXT`='"+RDEscapeString(err_text)+"' ";
  }
  sql+=QString::asprintf("where `ID`=%u",id);
  RDSqlQuery::apply(sql);
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv);

  new MainObject();
  return a.exec();
}
