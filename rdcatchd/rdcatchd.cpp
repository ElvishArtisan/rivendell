// rdcatchd.cpp
//
// The Rivendell Netcatcher Daemon
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

/*
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <math.h>
#include <netdb.h>
#include <ctype.h>
#include <pwd.h>
#include <syslog.h>
#include <grp.h>
#include <errno.h>
#include <sched.h>
*/
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <QCoreApplication>

#include <rdapplication.h>
#include <rddb.h>
#include <rdescape_string.h>

#include "rdcatchd.h"

void SigHandler(int signum)
{
  pid_t local_pid;

  switch(signum) {
  case SIGINT:
  case SIGTERM:
    rda->syslog(LOG_INFO,"rdcatchd exiting");
    exit(0);
    break;

  case SIGCHLD:
    local_pid=waitpid(-1,NULL,WNOHANG);
    while(local_pid>0) {
      local_pid=waitpid(-1,NULL,WNOHANG);
    }
    signal(SIGCHLD,SigHandler);
    return;
  }
}


MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  QString sql;
  RDSqlQuery *q;
  QString err_msg;
  RDApplication::ErrorType err_type=RDApplication::ErrorOk;

  //
  // Open the Database
  //
  rda=static_cast<RDApplication *>(new RDCoreApplication("rdcatchd","rdcatchd",
							 RDCATCHD_USAGE,this));
  if(!rda->open(&err_msg,&err_type,false)) {
    fprintf(stderr,"rdcatchd: %s [%s]\n",
	    RDCoreApplication::exitCodeText(RDCoreApplication::ExitNoDb).
	    toUtf8().constData(),
	    err_msg.toUtf8().constData());
    exit(RDCoreApplication::ExitNoDb);
  }

  //
  // Read Command Options
  //
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(!rda->cmdSwitch()->processed(i)) {
      fprintf(stderr,"rdcatchd: unknown command option \"%s\"\n",
	      rda->cmdSwitch()->key(i).toUtf8().constData());
      exit(RDCoreApplication::ExitInvalidOption);
    }
  }

  //
  // RIPCD Connection
  //
  rda->ripc()->connectHost("localhost",RIPCD_TCP_PORT,rda->config()->password());
  connect(rda->ripc(),SIGNAL(rmlReceived(RDMacro *)),
	  this,SLOT(rmlReceivedData(RDMacro *)));
  connect(rda->ripc(),SIGNAL(gpiStateChanged(int,int,bool)),
	  this,SLOT(gpiStateChangedData(int,int,bool)));
  connect(rda->ripc(),SIGNAL(notificationReceived(RDNotification *)),
	  this,SLOT(notificationReceivedData(RDNotification *)));

  //
  // Time Engine
  //
  d_engine=new RDTimeEngine(this);
  connect(d_engine,SIGNAL(timeout(int)),this,SLOT(engineData(int)));
  LoadEngine();

  ::signal(SIGINT,SigHandler);
  ::signal(SIGTERM,SigHandler);
  ::signal(SIGCHLD,SigHandler);

  //
  // Heartbeat Timer
  //
  d_heartbeat_timer=new QTimer(this);
  connect(d_heartbeat_timer,SIGNAL(timeout()),
	  this,SLOT(sysHeartbeatData()));
  LoadHeartbeat();

  //
  // Mark Interrupted Events
  //
  sql=QString("update `RECORDINGS` set ")+
    QString::asprintf("`EXIT_CODE`=%d where ",RDRecording::Interrupted)+
    QString::asprintf("((`EXIT_CODE`=%d)||",RDRecording::Uploading)+
    QString::asprintf("(`EXIT_CODE`=%d))||",RDRecording::Downloading)+
    QString::asprintf("(`EXIT_CODE`=%d)&&",RDRecording::RecordActive)+
    "(`STATION_NAME`='"+RDEscapeString(rda->config()->stationName())+"')";
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString("update `RECORDINGS` set ")+
    QString::asprintf("`EXIT_CODE`=%d where ",RDRecording::Ok)+
    QString::asprintf("((`EXIT_CODE`=%d)||",RDRecording::Waiting)+
    QString::asprintf("(`EXIT_CODE`=%d))&&",RDRecording::PlayActive)+
    "(`STATION_NAME`='"+RDEscapeString(rda->config()->stationName())+"')";
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Schedule Startup Cart
  //
  QTimer *timer=new QTimer(this);
  timer->setSingleShot(true);
  connect(timer,SIGNAL(timeout()),this,SLOT(startupCartData()));
  timer->start(10000);

  rda->syslog(LOG_INFO,"rdcatchd started");
}


void MainObject::notificationReceivedData(RDNotification *notify)
{
  CatchEvent *e=NULL;

  if(notify->type()==RDNotification::CatchEventType) {
    rda->syslog(LOG_NOTICE,"action: %u",notify->action());
    switch(notify->action()) {
    case RDNotification::AddAction:
    case RDNotification::ModifyAction:
      if((e=d_events.value(notify->id().toUInt()))==NULL) {
	rda->syslog(LOG_NOTICE,"failed to find record!");
	e=new CatchEvent();
	if(!e->load(notify->id().toUInt())) {
	  rda->syslog(LOG_WARNING,"unable to load catch event data for id=%u",
		      notify->id().toUInt());
	  return;
	}
	else {
	  d_events[e->id()]=e;
	  d_engine->addEvent(e->id(),e->startTime());
	  rda->syslog(LOG_DEBUG,"added catch event id=%u",e->id());
	}
      }
      else {
	d_engine->removeEvent(e->id());
	e->load(e->id());
	d_engine->addEvent(e->id(),e->startTime());
	rda->syslog(LOG_DEBUG,"updated catch event id=%u",e->id());
      }
      break;

    case RDNotification::DeleteAction:
      if((e=d_events.value(notify->id().toUInt()))==NULL) {
	rda->syslog(LOG_WARNING,
		    "unable to find catch event data for id=%u for deletion",
		    notify->id().toUInt());
      }
      else {
	d_engine->removeEvent(e->id());
	d_events.remove(e->id());
	delete e;
	rda->syslog(LOG_DEBUG,"deleted catch event id=%u",
		    notify->id().toUInt());
      }
      break;

    case RDNotification::NoAction:
    case RDNotification::LastAction:
      break;
    }
  }
}


void MainObject::rmlReceivedData(RDMacro *rml)
{
  /*
  if(rml->role()!=RDMacro::Cmd) {
    return;
  }
  RunLocalMacros(rml);
  */
}


void MainObject::gpiStateChangedData(int matrix,int line,bool state)
{
  /*
  // LogLine(QString::asprintf("gpiStateChangedData(%d,%d,%d)",
  //                           matrix,line,state));
  if(!state) {
    return;
  }

  std::vector<unsigned> handled_events;
  QTime current_time=QTime::currentTime();

  //
  // Start Events
  //
  for(unsigned i=0;i<catch_events.size();i++) {
    if(catch_events[i].startType()==RDRecording::GpiStart) {
      if((catch_events[i].startMatrix()==matrix)&&
	 (catch_events[i].startLine()==(line+1))&&
	 (catch_events[i].gpiStartTimer()->isActive())) {
	if(catch_events[i].startOffset()>0) {
	  catch_events[i].gpiOffsetTimer()->
	    start(catch_events[i].startOffset());
	  catch_events[i].gpiStartTimer()->stop();
	  BroadcastCommand(QString::asprintf("RE %d %d %d!",
					     catch_events[i].channel(),
					     RDDeck::Ready,
					     catch_events[i].id()).toUtf8());
	}
	else {
	  if(StartRecording(i)) {
	    catch_events[i].gpiStartTimer()->stop();
	  }
	}
	handled_events.push_back(i);
      }
    }
  }

  //
  // End Events
  //
  for(unsigned i=0;i<catch_events.size();i++) {
    if(catch_events[i].endType()==RDRecording::GpiEnd) {
      if((catch_events[i].endMatrix()==matrix)&&
	 (catch_events[i].endLine()==(line+1))&&
	 (catch_events[i].endTime()<=current_time)&&
	 (catch_events[i].endTime().addMSecs(catch_events[i].endLength())>
	  current_time)) {
	bool handled=false;
	for(unsigned j=0;j<handled_events.size();j++) {
	  if(handled_events[j]==i) {
	    handled=true;
	  }
	}
	if(!handled) {
	  rda->cae()->
	    stopRecord(catch_record_card[catch_events[i].channel()-1],
		       catch_record_stream[catch_events[i].channel()-1]);
	}
      }
    }
  }
  */
}


void MainObject::engineData(int id)
{
  QDateTime now=QDateTime::currentDateTime();
  CatchEvent *e=d_events.value(id);

  if(e==NULL) {
    rda->syslog(LOG_DEBUG,"cannot find event %d, ignoring!",id);
    return;
  }
  if(!e->isActive()) {
    rda->syslog(LOG_DEBUG,"event %d is marked inactive, ignoring",id);
    return;
  }
  if(!e->dayOfWeek(now.date().dayOfWeek())) {
    rda->syslog(LOG_DEBUG,"event %d is not valid for this DOW, ignoring",id);
    return;
  }

  rda->syslog(LOG_INFO,"would start event id=%u, type=%s",id,
	      RDRecording::typeString(e->type()).toUtf8().constData());
}


void MainObject::sysHeartbeatData()
{
  /*
  RDCart *cart=new RDCart(catch_heartbeat_cartnum);
  if(cart->exists()) {
    ExecuteMacroCart(cart);
  }
  delete cart;
  */
}


void MainObject::startupCartData()
{
  /*
  unsigned cartnum=rda->station()->startupCart();
  if(cartnum>0) {
    RDCart *cart=new RDCart(cartnum);
    if(cart->exists()) {
      ExecuteMacroCart(cart);
      rda->syslog(LOG_INFO,"ran startup cart %06u",cartnum);
    }
    else {
      rda->syslog(LOG_WARNING,"startup cart %06u was invalid",cartnum);
    }
    delete cart;
  }
  */
}


void MainObject::userChangedData()
{
  /*
  //
  // Dispatch Handler
  //
  switch(batch_event->type()) {
  case RDRecording::Recording:
    RunImport(batch_event);
    SendNotification(RDNotification::CartType,RDNotification::ModifyAction,
		     RDCut::cartNumber(batch_event->cutName()));
    break;

  case RDRecording::Download:
    RunDownload(batch_event);
    SendNotification(RDNotification::CartType,RDNotification::ModifyAction,
		     RDCut::cartNumber(batch_event->cutName()));
    break;

  case RDRecording::Upload:
    RunUpload(batch_event);
    break;

  default:
    fprintf(stderr,"rdcatchd: nothing to do for this event type\n");
    exit(256);
  }

  QTimer *timer=new QTimer(this);
  timer->setSingleShot(true);
  connect(timer,SIGNAL(timeout()),this,SLOT(exitData()));
  timer->start(5000);  // So notifications have a chance to propagate
  */
}


void MainObject::exitData()
{
  exit(0);
}


void MainObject::LoadEngine()
{
  QString sql=CatchEvent::sqlFields()+
    "where `RECORDINGS`.`STATION_NAME`='"+
    RDEscapeString(rda->station()->name())+"' ";
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    CatchEvent *e=new CatchEvent();
    e->loadFromQuery(q);
    d_events[e->id()]=e;
    d_engine->addEvent(e->id(),e->startTime());
  }
  delete q;
}


void MainObject::LoadHeartbeat()
{
  if(d_heartbeat_timer->isActive()) {
    d_heartbeat_timer->stop();
  }
  QString sql=QString("select ")+
    "`HEARTBEAT_CART`,"+      // 00
    "`HEARTBEAT_INTERVAL` "+  // 01
    "from `STATIONS` where "+
    "`NAME`='"+RDEscapeString(rda->station()->name())+"'";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    if((q->value(0).toUInt()!=0)&&(q->value(1).toUInt()!=0)) {
      d_heartbeat_cartnum=q->value(0).toUInt();
      sysHeartbeatData();
      d_heartbeat_timer->start(q->value(1).toUInt());
    }
  }
  delete q;
}


int main(int argc,char *argv[])
{
  QCoreApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
