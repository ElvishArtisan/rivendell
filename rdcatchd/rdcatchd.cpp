// rdcatchd.cpp
//
// The Rivendell Netcatcher Daemon
//
//   (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
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
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <sched.h>
#include <sys/mman.h>

#include <vector>

#include <qapplication.h>
#include <qtimer.h>
#include <qsignalmapper.h>
#include <qsessionmanager.h>

#include <rdapplication.h>
#include <rdconf.h>
#include <rdcut.h>
#include <rddatedecode.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdhash.h>
#include <rdlibrary_conf.h>
#include <rdmixer.h>
#include <rdpaths.h>
#include <rdpodcast.h>
#include <rdrecording.h>
#include <rdsettings.h>
#include <rdtempdirectory.h>
#include <rdurl.h>
#include <rdwavefile.h>

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


ServerConnection::ServerConnection(int id,QTcpSocket *sock)
{
  conn_id=id;
  conn_socket=sock;
  conn_authenticated=false;
  conn_meter_enabled=false;
  conn_is_closing=false;
  accum="";
}


ServerConnection::~ServerConnection()
{
  delete conn_socket;
}


int ServerConnection::id() const
{
  return conn_id;
}


bool ServerConnection::isAuthenticated() const
{
  return conn_authenticated;
}


void ServerConnection::setAuthenticated(bool state)
{
  conn_authenticated=state;
}


bool ServerConnection::meterEnabled() const
{
  return conn_meter_enabled;
}


void ServerConnection::setMeterEnabled(bool state)
{
  conn_meter_enabled=state;
}


QTcpSocket *ServerConnection::socket()
{
  return conn_socket;
}


bool ServerConnection::isClosing() const
{
  return conn_is_closing;
}


void ServerConnection::close()
{
  conn_is_closing=true;
}




MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  QString sql;
  RDSqlQuery *q;
  QString err_msg;
  RDApplication::ErrorType err_type=RDApplication::ErrorOk;
  debug=false;

  //
  // Open the Database
  //
  rda=new RDApplication("rdcatchd","rdcatchd",RDCATCHD_USAGE,this);
  if(!rda->open(&err_msg,&err_type,false)) {
    fprintf(stderr,"rdcatchd: %s\n",(const char *)err_msg);
    exit(1);
  }

  //
  // Read Command Options
  //
  for(unsigned i=0;i<rda->cmdSwitch()->keys();i++) {
    if(rda->cmdSwitch()->key(i)=="--event-id") {
      RunBatch(rda->cmdSwitch());
      return;
    }
    if(rda->cmdSwitch()->key(i)=="-d") {
      debug=true;
      rda->cmdSwitch()->setProcessed(i,true);
    }
    if(!rda->cmdSwitch()->processed(i)) {
      fprintf(stderr,"rdcatchdd: unknown command option \"%s\"\n",
	      (const char *)rda->cmdSwitch()->key(i));
      exit(2);
    }
  }

  //
  // Initialize Data Structures
  //
  for(int i=0;i<MAX_DECKS;i++) {
    catch_record_deck_status[i]=RDDeck::Offline;
    catch_playout_deck_status[i]=RDDeck::Offline;
    catch_record_status[i]=false;
    catch_record_id[i]=0;
    catch_record_aborting[i]=false;
    catch_playout_status[i]=false;
    catch_playout_event_id[i]=-1;
    catch_playout_id[i]=0;
    catch_playout_handle[i]=-1;
    catch_monitor_port[i]=-1;
    catch_monitor_state[i]=false;
    catch_record_pending_cartnum[i]=0;
    catch_record_pending_cutnum[i]=0;
    catch_record_pending_maxlen[i]=0;
  }

  //
  // Calculate Temporary Directory
  //
  catch_temp_dir=RDTempDirectory::basePath();

  //
  // Macro Event Handling
  //
  for(int i=0;i<RDCATCHD_MAX_MACROS;i++) {
    catch_event_pool[i]=NULL;
    catch_event_free[i]=true;
    catch_macro_event_id[i]=-1;
  }
  catch_event_mapper=new QSignalMapper(this);
  connect(catch_event_mapper,SIGNAL(mapped(int)),
	  this,SLOT(eventFinishedData(int)));
  QTimer *timer=new QTimer(this,"free_events_timer");
  connect(timer,SIGNAL(timeout()),this,SLOT(freeEventsData()));
  timer->start(RDCATCHD_FREE_EVENTS_INTERVAL);

  //
  // Command Server
  //
  server=new QTcpServer(this);
  if(!server->listen(QHostAddress::Any,RDCATCHD_TCP_PORT)) {
    fprintf(stderr,"rdcatchd: aborting - couldn't bind socket");
    exit(1);
  }
  connect(server,SIGNAL(newConnection()),this,SLOT(newConnectionData()));
  catch_ready_mapper=new QSignalMapper(this);
  connect(catch_ready_mapper,SIGNAL(mapped(int)),this,SLOT(socketReadyReadData(int)));
  catch_kill_mapper=new QSignalMapper(this);
  connect(catch_kill_mapper,SIGNAL(mapped(int)),this,SLOT(socketKillData(int)));
  catch_garbage_timer=new QTimer(this);
  catch_garbage_timer->setSingleShot(true);
  connect(catch_garbage_timer,SIGNAL(timeout()),this,SLOT(garbageData()));

  //  connect (RDDbStatus(),SIGNAL(logText(RDConfig::LogPriority,const QString &)),
  //	   this,SLOT(log(RDConfig::LogPriority,const QString &)));

  //
  // Create RDCatchConf
  //
  catch_conf=new RDCatchConf(rda->config()->stationName());

  //
  // GPI Mappers
  //
  catch_gpi_start_mapper=new QSignalMapper(this);
  connect(catch_gpi_start_mapper,SIGNAL(mapped(int)),
	  this,SLOT(startTimerData(int)));
  catch_gpi_offset_mapper=new QSignalMapper(this);
  connect(catch_gpi_offset_mapper,SIGNAL(mapped(int)),
	  this,SLOT(offsetTimerData(int)));

  //
  // Xload Timer
  //
  catch_xload_timer=new QTimer(this);
  connect(catch_xload_timer,SIGNAL(timeout()),this,SLOT(updateXloadsData()));

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
  // CAE Connection
  //
  connect(rda->cae(),SIGNAL(isConnected(bool)),
	  this,SLOT(isConnectedData(bool)));
  connect(rda->cae(),SIGNAL(recordLoaded(int,int)),
	  this,SLOT(recordLoadedData(int,int)));
  connect(rda->cae(),SIGNAL(recording(int,int)),
	  this,SLOT(recordingData(int,int)));
  connect(rda->cae(),SIGNAL(recordStopped(int,int)),
	  this,SLOT(recordStoppedData(int,int)));
  connect(rda->cae(),SIGNAL(recordUnloaded(int,int,unsigned)),
	  this,SLOT(recordUnloadedData(int,int,unsigned)));
  connect(rda->cae(),SIGNAL(playLoaded(int)),
	  this,SLOT(playLoadedData(int)));
  connect(rda->cae(),SIGNAL(playing(int)),
	  this,SLOT(playingData(int)));
  connect(rda->cae(),SIGNAL(playStopped(int)),
	  this,SLOT(playStoppedData(int)));
  connect(rda->cae(),SIGNAL(playUnloaded(int)),
	  this,SLOT(playUnloadedData(int)));
  rda->cae()->connectHost();

  //
  // Sound Initialization
  //
  sql=QString("select ")+
    "CHANNEL,"+      // 00
    "CARD_NUMBER,"+  // 01
    "PORT_NUMBER "+  // 02
    "from DECKS	where "+
    "(STATION_NAME=\""+RDEscapeString(rda->config()->stationName())+"\")&&"+
    "(CARD_NUMBER!=-1)&&(CHANNEL>0)&&(CHANNEL<9)";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if((q->value(1).toInt()>=0)&&(q->value(2).toInt()>=0)) {
      catch_record_deck_status[q->value(0).toUInt()-1]=RDDeck::Idle;
    }
  }
  delete q;
  LoadDeckList();

  //
  // Initialize Monitor Passthroughs
  //
  sql=QString("select ")+
    "CARD_NUMBER,"+      // 00
    "PORT_NUMBER,"+      // 01
    "MON_PORT_NUMBER,"+  // 02
    "CHANNEL from DECKS where "+
    "(STATION_NAME=\""+RDEscapeString(rda->config()->stationName())+"\")&&"+
    QString().sprintf("(CHANNEL<=%d) &&",MAX_DECKS)+
    "(CARD_NUMBER>=0)&&(MON_PORT_NUMBER>=0) && "+
    "(DEFAULT_MONITOR_ON=\"Y\")";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    rda->cae()->setPassthroughVolume(q->value(0).toInt(),q->value(1).toInt(),
				    q->value(2).toInt(),0);
    catch_monitor_state[q->value(3).toUInt()-1]=true;
  }
  delete q;

  //
  // Playout Event Players
  //
  for(unsigned i=0;i<MAX_DECKS;i++) {
    catch_playout_event_player[i]=new EventPlayer(rda->station(),i+129,this);
    connect(catch_playout_event_player[i],SIGNAL(runCart(int,int,unsigned)),
	    this,SLOT(runCartData(int,int,unsigned)));
  }

  //
  // Time Engine
  //
  catch_engine=new RDTimeEngine(this);
  catch_engine->setTimeOffset(rda->station()->timeOffset());
  connect(catch_engine,SIGNAL(timeout(int)),this,SLOT(engineData(int)));
  LoadEngine();

  if(qApp->argc()!=1) {
    debug=true;
  }

  ::signal(SIGINT,SigHandler);
  ::signal(SIGTERM,SigHandler);
  ::signal(SIGCHLD,SigHandler);

  //
  // Start Heartbeat Timer
  //
  timer=new QTimer(this);
  connect(timer,SIGNAL(timeout()),this,SLOT(heartbeatData()));
  timer->start(RDCATCHD_HEARTBEAT_INTERVAL);

  //
  // Meter Timer
  //
  timer=new QTimer(this);
  connect(timer,SIGNAL(timeout()),this,SLOT(meterData()));
  timer->start(RD_METER_UPDATE_INTERVAL);

  //
  // Heartbeat Timer
  //
  catch_heartbeat_timer=new QTimer(this);
  connect(catch_heartbeat_timer,SIGNAL(timeout()),
	  this,SLOT(sysHeartbeatData()));
  LoadHeartbeat();

  //
  // Mark Interrupted Events
  //
  sql=QString("update RECORDINGS set ")+
    QString().sprintf("EXIT_CODE=%d where ",RDRecording::Interrupted)+
    QString().sprintf("((EXIT_CODE=%d)||",RDRecording::Uploading)+
    QString().sprintf("(EXIT_CODE=%d))||",RDRecording::Downloading)+
    QString().sprintf("(EXIT_CODE=%d)&&",RDRecording::RecordActive)+
    "(STATION_NAME=\""+RDEscapeString(rda->config()->stationName())+"\")";
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString("update RECORDINGS set ")+
    QString().sprintf("EXIT_CODE=%d where ",RDRecording::Ok)+
    QString().sprintf("((EXIT_CODE=%d)||",RDRecording::Waiting)+
    QString().sprintf("(EXIT_CODE=%d))&&",RDRecording::PlayActive)+
    "(STATION_NAME=\""+RDEscapeString(rda->config()->stationName())+"\")";
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Schedule Startup Cart
  //
  timer=new QTimer(this,"startup_cart_timer");
  connect(timer,SIGNAL(timeout()),this,SLOT(startupCartData()));
  timer->start(10000,true);

  //
  // Set Realtime Permissions
  //
  if(rda->config()->useRealtime()) {
    struct sched_param sp;
    memset(&sp,0,sizeof(sp));
    if(rda->config()->realtimePriority()>0) {
      sp.sched_priority=rda->config()->realtimePriority()-1;
    }
    if(sched_setscheduler(getpid(),SCHED_FIFO,&sp)!=0) {
      rda->syslog(LOG_DEBUG,"unable to set realtime permissions, %s",
	     strerror(errno));
    }
    mlockall(MCL_CURRENT|MCL_FUTURE);
  }

  rda->syslog(LOG_INFO,"rdcatchd started");
}


void MainObject::notificationReceivedData(RDNotification *notify)
{
  if(notify->type()==RDNotification::CatchEventType) {
    switch(notify->action()) {
    case RDNotification::AddAction:
    case RDNotification::ModifyAction:
    case RDNotification::DeleteAction:
      UpdateEvent(notify->id().toUInt());
      break;

    case RDNotification::NoAction:
    case RDNotification::LastAction:
      break;
    }
  }
}


void MainObject::newConnectionData()
{
  int i=0;
  QTcpSocket *sock=server->nextPendingConnection();
  while((i<catch_connections.size())&&(catch_connections[i]!=NULL)) {
    i++;
  }
  if(i==catch_connections.size()) {      // Table full, create a new slot
    catch_connections.push_back(new ServerConnection(i,sock));
  }
  else {
    catch_connections[i]=new ServerConnection(i,sock);
  }
  connect(sock,SIGNAL(readyRead()),catch_ready_mapper,SLOT(map()));
  catch_ready_mapper->setMapping(sock,i);
  connect(sock,SIGNAL(connectionClosed()),catch_kill_mapper,SLOT(map()));
  catch_kill_mapper->setMapping(sock,i);
  rda->syslog(LOG_DEBUG,"created connection %d",i);
}


void MainObject::rmlReceivedData(RDMacro *rml)
{
  if(rml->role()!=RDMacro::Cmd) {
    return;
  }
  RunLocalMacros(rml);
}


void MainObject::gpiStateChangedData(int matrix,int line,bool state)
{
  // LogLine(QString().sprintf("gpiStateChangedData(%d,%d,%d)",
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
	    start(catch_events[i].startOffset(),true);
	  catch_events[i].gpiStartTimer()->stop();
	  BroadcastCommand(QString().sprintf("RE %d %d %d!",
					     catch_events[i].channel(),
					     RDDeck::Ready,
					     catch_events[i].id()));
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
}


void MainObject::startTimerData(int id)
{
  int event=GetEvent(id);
  unsigned deck=catch_events[event].channel()-1;

  catch_events[event].setStatus(RDDeck::Idle);
  WriteExitCodeById(id,RDRecording::Ok);
  catch_record_deck_status[deck]=RDDeck::Idle;
  catch_record_id[deck]=0;
  BroadcastCommand(QString().sprintf("RE %d %d %d!",
				     deck+1,catch_record_deck_status[deck],
				     id));
  rda->syslog(LOG_DEBUG,"gpi start window closes: event: %d, gpi: %d:%d",
	      id,catch_events[event].startMatrix(),
	      catch_events[event].startLine());
}


void MainObject::offsetTimerData(int id)
{
  int event=GetEvent(id);
  if(StartRecording(event)) {
    catch_events[event].gpiStartTimer()->stop();
  }
}


void MainObject::engineData(int id)
{
  // LogLine(QString().sprintf("engineData(%d)",id));
  QString sql;
  RDSqlQuery *q;
  RDStation *rdstation;
  int event=GetEvent(id);

  //
  // Generate Effective Date
  //
  QDate date=QDate::currentDate();
  QTime current_time=QTime::currentTime();
  if((current_time.msecsTo(QTime(23,59,59))+1000)<catch_engine->timeOffset()) {
    date=date.addDays(1);
  }

  //
  // Ignore inactive or non-existent events
  //
  if(event<0) {
    rda->syslog(LOG_DEBUG,"cannot find event %d, ignoring!",id);
    return;
  }
  if(!catch_events[event].isActive()) {
    rda->syslog(LOG_DEBUG,"event %d is marked inactive, ignoring",id);
    return;
  }
  if(!catch_events[event].dayOfWeek(date.dayOfWeek())) {
    rda->syslog(LOG_DEBUG,"event %d is not valid for this DOW, ignoring",id);
    return;
  }

  //
  // Check for Playout Deck Availability
  //
  if(catch_events[event].type()==RDRecording::Playout) {
    if(catch_playout_deck_status[catch_events[event].channel()-129]!=
       RDDeck::Idle) {
      rda->syslog(LOG_WARNING,"playout deck P%d is busy for event %d, skipping",
		  catch_events[event].channel()-128,
		  catch_events[event].id());
      WriteExitCode(event,RDRecording::DeviceBusy);
      BroadcastCommand(QString().sprintf("RE 0 %d %d %s!",RDDeck::Recording,
					 catch_events[event].id(),
				(const char *)catch_events[event].cutName()));
      return;
    }
  }

  //
  // Load Deck Parameters
  //
  switch(catch_events[event].type()) {
  case RDRecording::Recording:
    if(!RDCut::exists(catch_events[event].cutName())) {
      WriteExitCode(event,RDRecording::NoCut);
      BroadcastCommand(QString().
		       sprintf("RE %d %d %d!",
			       catch_events[event].channel(),
			       catch_record_deck_status[catch_events[event].
							channel()-1],
			       catch_events[event].id()));
      rda->syslog(LOG_WARNING,"record aborted: no such cut: %s, id: %d",
		  (const char *)catch_events[event].cutName(),
	     catch_events[event].id());
      return;
    }
    catch_record_card[catch_events[event].channel()-1]=-1;
    catch_record_stream[catch_events[event].channel()-1]=-1;
    sql=QString("select ")+
      "CARD_NUMBER,"+     // 00
      "PORT_NUMBER,"+     // 01
      "SWITCH_STATION,"+  // 02
      "SWITCH_MATRIX,"+   // 03
      "SWITCH_OUTPUT,"+   // 04
      "SWITCH_DELAY "+    // 05
      "from DECKS where "+
      "(STATION_NAME=\""+RDEscapeString(rda->config()->stationName())+"\")&&"+
      QString().sprintf("(CHANNEL=%d)",catch_events[event].channel());
    q=new RDSqlQuery(sql);
    if(q->first()) {
      catch_record_card[catch_events[event].channel()-1]=
	q->value(0).toInt();
      catch_record_stream[catch_events[event].channel()-1]=
	q->value(1).toInt();
      if(q->value(2).toString()==QString("[none]")) {
	catch_swaddress[catch_events[event].channel()-1]=QHostAddress();
      }
      else {
	rdstation=new RDStation(q->value(2).toString());
	catch_swaddress[catch_events[event].channel()-1]=
	  rdstation->address();
	delete rdstation;
      }
      catch_swmatrix[catch_events[event].channel()-1]=q->value(3).toInt();
      catch_swoutput[catch_events[event].channel()-1]=q->value(4).toInt();
      catch_swdelay[catch_events[event].channel()-1]=q->value(5).toInt();
    }
    else {
      rda->syslog(LOG_DEBUG,"id %d specified non-existent record deck, ignored",
		  catch_events[event].id());
      delete q;
      return;
    }
    delete q;
    
    sql=QString("delete from CUT_EVENTS where ")+
      "CUT_NAME=\""+catch_events[event].cutName()+"\"";
    q=new RDSqlQuery(sql);
    delete q;

    switch(catch_events[event].startType()) {
    case RDRecording::HardStart:
      StartRecording(event);
      break;

    case RDRecording::GpiStart:
      catch_events[event].gpiStartTimer()->
	start(catch_events[event].startLength()-
	      (QTime().msecsTo(current_time)-
	       QTime().msecsTo(catch_events[event].startTime())),true);
      catch_record_deck_status[catch_events[event].channel()-1]=
	RDDeck::Waiting;
      catch_record_id[catch_events[event].channel()-1]=
	catch_events[event].id();
      catch_events[event].setStatus(RDDeck::Waiting);
      WriteExitCode(event,RDRecording::Waiting);
      BroadcastCommand(QString().sprintf("RE %d %d %d!",
		       catch_events[event].channel(),
		       catch_record_deck_status[catch_events[event].
						channel()-1],
					 catch_events[event].id()));
      rda->syslog(LOG_DEBUG,"gpi start window opens: event: %d, gpi: %d:%d",
		  id,catch_events[event].startMatrix(),
		  catch_events[event].startLine());
      break;
    }
    break;

  case RDRecording::Playout:
    if(!RDCut::exists(catch_events[event].cutName())) {
      WriteExitCode(event,RDRecording::NoCut);
      BroadcastCommand(QString().
		       sprintf("RE %d %d %d!",
			       catch_events[event].channel(),
			       catch_playout_deck_status[catch_events[event].
							 channel()-129],
			       catch_events[event].id()));
      rda->syslog(LOG_WARNING,"playout aborted: no such cut: %s, id: %d",
		  (const char *)catch_events[event].cutName().toUtf8(),
		  catch_events[event].id());
      return;
    }
    catch_playout_card[catch_events[event].channel()-129]=-1;
    catch_playout_stream[catch_events[event].channel()-129]=-1;
    sql=QString("select ")+
      "CARD_NUMBER,"+  // 00
      "PORT_NUMBER,"+  // 01
      "PORT_NUMBER "+  // 02
      "from DECKS where "+
      "(STATION_NAME=\""+RDEscapeString(rda->config()->stationName())+"\")&&"+
      QString().sprintf("(CHANNEL=%d)",catch_events[event].channel());
    q=new RDSqlQuery(sql);
    if(q->first()) {
      catch_playout_id[catch_events[event].channel()-129]=id;
      catch_playout_card[catch_events[event].channel()-129]=
	q->value(0).toInt();
      catch_playout_stream[catch_events[event].channel()-129]=
	q->value(1).toInt();
      catch_playout_port[catch_events[event].channel()-129]=
	q->value(2).toInt();
    }
    else {
      rda->syslog(LOG_DEBUG,"id %d specified non-existent play deck, ignored",
		  catch_events[event].id());
      delete q;
      return;
    }
    delete q;
    StartPlayout(event);
    break;

  case RDRecording::MacroEvent:
    if(!RDCart::exists(catch_events[event].macroCart())) {
      WriteExitCode(event,RDRecording::NoCut);
      BroadcastCommand(QString().
		       sprintf("RE 0 0 %d!",catch_events[event].id()));
      rda->syslog(LOG_WARNING,"macro aborted: no such cart: %u, id: %d",
		  catch_events[event].macroCart(),
		  catch_events[event].id());
      return;
    }
    StartMacroEvent(event);
    break;
	
  case RDRecording::SwitchEvent:
    StartSwitchEvent(event);
    break;

  case RDRecording::Download:
    if(!RDCut::exists(catch_events[event].cutName())) {
      WriteExitCode(event,RDRecording::NoCut);
      BroadcastCommand(QString().
		       sprintf("RE 0 0 %d!",catch_events[event].id()));
      rda->syslog(LOG_WARNING,"download aborted: no such cut: %s, id: %d",
		  (const char *)catch_events[event].cutName().toUtf8(),
		  catch_events[event].id());
      return;
    }

    //
    // Load Import Parameters
    //
    sql=QString("select ")+
      "DEFAULT_FORMAT,"+    // 00
      "DEFAULT_CHANNELS,"+  // 01
      "DEFAULT_LAYER,"+     // 02
      "DEFAULT_BITRATE,"+   // 03
      "RIPPER_LEVEL "+      // 04
      "from RDLIBRARY where "+
      "STATION=\""+RDEscapeString(rda->config()->stationName())+"\"";
    q=new RDSqlQuery(sql);
    if(q->first())
      {
	catch_default_format=q->value(0).toInt();
	catch_default_channels=q->value(1).toInt();
	catch_default_layer=q->value(2).toInt();
	catch_default_bitrate=q->value(3).toInt();
	catch_ripper_level=q->value(4).toInt();
      }
    else {
      rda->syslog(LOG_WARNING,"unable to load import audio configuration");
      delete q;
      return;
    }
    delete q;
    catch_events[event].
      setResolvedUrl(RDDateTimeDecode(catch_events[event].url(),
	       QDateTime(date.addDays(catch_events[event].eventdateOffset()),
			 current_time),rda->station(),RDConfiguration()));
	StartDownloadEvent(event);
	break;

  case RDRecording::Upload:
    if(!RDCut::exists(catch_events[event].cutName())) {
      WriteExitCode(event,RDRecording::NoCut);
      BroadcastCommand(QString().
		       sprintf("RE 0 0 %d!",catch_events[event].id()));
      rda->syslog(LOG_WARNING,"upload aborted: no such cut: %s, id: %d",
		  (const char *)catch_events[event].cutName().toUtf8(),
		  catch_events[event].id());
      return;
    }
    StartUploadEvent(event);
    break;

  case RDRecording::LastType:
    break;
  }
}


void MainObject::socketReadyReadData(int ch)
{
  ParseCommand(ch);
}


void MainObject::socketKillData(int conn_id)
{
  if(catch_connections[conn_id]!=NULL) {
    catch_connections[conn_id]->close();
    catch_garbage_timer->start(1);
  }
}


void MainObject::garbageData()
{
  for(int i=0;i<catch_connections.size();i++) {
    if(catch_connections.at(i)!=NULL) {
      if(catch_connections.at(i)->isClosing()) {
	delete catch_connections.at(i);
	catch_connections[i]=NULL;
	rda->syslog(LOG_DEBUG,"closed connection %d",i);
      }
    }
  }
}


void MainObject::isConnectedData(bool state)
{
  if(!state) {
    rda->syslog(LOG_ERR,"aborting - unable to connect to Core AudioEngine");
    exit(1);
  }
}


void MainObject::recordLoadedData(int card,int stream)
{
  int deck=GetRecordDeck(card,stream);
  catch_record_deck_status[deck-1]=RDDeck::Ready;
  BroadcastCommand(QString().sprintf("RE %d %d %d!",
				     deck,catch_record_deck_status[deck-1],
				     catch_record_id[deck-1]));
  rda->syslog(LOG_DEBUG,"Loaded - Card: %d  Stream: %d\n",card,stream);
}


void MainObject::recordingData(int card,int stream)
{
  int deck=GetRecordDeck(card,stream);
  catch_record_deck_status[deck-1]=RDDeck::Recording;
  WriteExitCodeById(catch_record_id[deck-1],RDRecording::RecordActive);
  QString cutname;
  int event=GetEvent(catch_record_id[deck-1]);
  if(event>=0) {
    cutname=catch_events[event].cutName();
  }
  BroadcastCommand(QString().sprintf("RE %d %d %d %s!",
				     deck,catch_record_deck_status[deck-1],
				     catch_record_id[deck-1],
				     (const char *)cutname));
  catch_record_status[deck-1]=true;
  if(debug) {
    printf("Recording - Card: %d  Stream: %d,  Id: %d\n",card,stream,
	   catch_record_id[deck-1]);
  }
}


void MainObject::recordStoppedData(int card,int stream)
{
  int deck=GetRecordDeck(card,stream);
  short levels[2]={-10000,-10000};

  catch_record_status[deck-1]=false;
  if(debug) {
    printf("Stopped - Card: %d  Stream: %d\n",card,stream);
  }
  SendMeterLevel(deck-1,levels);
  rda->cae()->unloadRecord(card,stream);
}


void MainObject::recordUnloadedData(int card,int stream,unsigned msecs)
{
  int deck=GetRecordDeck(card,stream);
  if(deck<1) {
    rda->syslog(LOG_DEBUG,"invalid record deck:  Card: %d  Stream: %d",
		card,stream);
    return;
  }
  int event=GetEvent(catch_record_id[deck-1]);
  if(event<0) {
    catch_record_deck_status[deck-1]=RDDeck::Idle;
    catch_record_aborting[deck-1]=false;
    rda->syslog(LOG_DEBUG,"invalid record event:  Id: %d",
		catch_record_id[deck-1]);
    RunRmlRecordingCache(deck);
    return;
  }

  if(catch_events[event].normalizeLevel()==0) {
    CheckInRecording(catch_record_name[deck-1],&catch_events[event],msecs,
		     catch_record_threshold[deck-1]);
  }
  else {
    StartBatch(catch_events[event].id());
  }
  if(catch_record_aborting[deck-1]) {
    rda->syslog(LOG_INFO,"record aborted: cut %s",
		(const char *)catch_record_name[deck-1].toUtf8());
    WriteExitCodeById(catch_record_id[deck-1],RDRecording::Interrupted);
  }
  else {
    rda->syslog(LOG_INFO,"record complete: cut %s",
		(const char *)catch_record_name[deck-1].toUtf8());
    WriteExitCodeById(catch_record_id[deck-1],RDRecording::Ok);
  }
  BroadcastCommand(QString().sprintf("RE %d %d %d!",
				     deck,RDDeck::Idle,
				     catch_record_id[deck-1]));
  catch_record_id[deck-1]=0;
  if(debug) {
    printf("Unloaded - Card: %d  Stream: %d\n",card,stream);
  }
  if(catch_events[event].oneShot()) {
    PurgeEvent(event);
  }
  catch_record_deck_status[deck-1]=RDDeck::Idle;
  catch_events[event].setStatus(RDDeck::Idle);
  catch_record_aborting[deck-1]=false;

  //
  // Restart the event (if needed)
  //
  if((catch_events[event].type()==RDRecording::Recording)&&
     (catch_events[event].startType()==RDRecording::GpiStart)&&
     ((catch_events[event].endType()==RDRecording::GpiEnd)||
      (catch_events[event].endType()==RDRecording::LengthEnd))&&
     catch_events[event].allowMultipleRecordings()) {
    engineData(catch_events[event].id());
  }
  else {
    RunRmlRecordingCache(deck);
  }
}


void MainObject::playLoadedData(int handle)
{
  int deck=GetPlayoutDeck(handle);
  catch_playout_deck_status[deck-129]=RDDeck::Ready;
  BroadcastCommand(QString().sprintf("RE %d %d %d!",
				     deck,catch_playout_deck_status[deck-129],
				     catch_playout_id[deck-129]));
  if(debug) {
    printf("Play Loaded - Card: %d  Stream: %d\n",
	   catch_playout_card[deck-129],
	   catch_playout_stream[deck-129]);
  }
}


void MainObject::playingData(int handle)
{
  int deck=GetPlayoutDeck(handle);
  catch_playout_deck_status[deck-129]=RDDeck::Recording;
  WriteExitCodeById(catch_playout_id[deck-129],
		    RDRecording::PlayActive);
  BroadcastCommand(QString().sprintf("RE %d %d %d!",
				     deck,catch_playout_deck_status[deck-129],
				     catch_playout_id[deck-129]));
  catch_playout_status[GetPlayoutDeck(handle)]=true;
  if(debug) {
    printf("Playing - Card: %d  Stream: %d\n",
	   catch_playout_card[deck-129],
	   catch_playout_stream[deck-129]);
  }
}


void MainObject::playStoppedData(int handle)
{
  int deck=GetPlayoutDeck(handle);
  short levels[2]={-10000,-10000};

  catch_playout_status[deck-129]=false;
  catch_playout_event_player[deck-129]->stop();
  rda->syslog(LOG_INFO,"playout stopped: cut %s",
	      (const char *)catch_playout_name[deck-129].toUtf8());
  if(debug) {
    printf("Playout stopped - Card: %d  Stream: %d\n",
	   catch_playout_card[deck-129],
	   catch_playout_stream[deck-129]);
  }
  SendMeterLevel(deck,levels);
  rda->cae()->unloadPlay(handle);
}


void MainObject::playUnloadedData(int handle)
{
  int deck=GetPlayoutDeck(handle);

  rda->syslog(LOG_INFO,"play complete: cut %s",
	      (const char *)catch_playout_name[deck-129].toUtf8());
  catch_playout_deck_status[deck-129]=RDDeck::Idle;
  WriteExitCodeById(catch_playout_id[deck-129],RDRecording::Ok);
  BroadcastCommand(QString().sprintf("RE %d %d %d!",deck,
				     catch_playout_deck_status[deck-129],
				     catch_playout_id[deck-129]));
  if(debug) {
    printf("Play unloaded - Card: %d  Stream: %d\n",
	   catch_playout_card[deck-129],catch_playout_stream[deck-129]);
  }
  if(catch_events[catch_playout_event_id[deck-129]].oneShot()) {
    PurgeEvent(catch_playout_event_id[deck-129]);
  }
  catch_events[catch_playout_event_id[deck-129]].setStatus(RDDeck::Idle);
  catch_playout_event_id[deck-129]=-1;
  catch_playout_id[deck-129]=0;
}


void MainObject::runCartData(int chan,int number,unsigned cartnum)
{
  RDCart *cart=new RDCart(cartnum);
  if(cart->exists()&&(cart->type()==RDCart::Macro)) {
    ExecuteMacroCart(cart);
  }
  delete cart;
  SendDeckEvent(chan,number);
}


void MainObject::meterData()
{
  short levels[2];

  for(int i=0;i<MAX_DECKS;i++) {
    if(catch_record_deck_status[i]==RDDeck::Recording) {
      rda->cae()->inputMeterUpdate(catch_record_card[i],catch_record_stream[i],
				  levels);
      SendMeterLevel(i+1,levels);
    }
    if(catch_playout_deck_status[i]==RDDeck::Recording) {
      rda->cae()->
	outputMeterUpdate(catch_playout_card[i],catch_playout_port[i],
				  levels);
      SendMeterLevel(i+129,levels);
    }
  }
}


void MainObject::eventFinishedData(int id)
{
  if(catch_macro_event_id[id]>=0) {
    rda->syslog(LOG_DEBUG,"clearing event_id: %d",catch_macro_event_id[id]);
    if(catch_macro_event_id[id]<RDCATCHD_ERROR_ID_OFFSET) {
      int event=GetEvent(catch_macro_event_id[id]);
      if(event<0) {
	rda->syslog(LOG_DEBUG,"processed eventFinishedData for unknown ID");
	return;
      }
      catch_events[event].setStatus(RDDeck::Idle);
      BroadcastCommand(QString().sprintf("RE 0 %d %d!",
					 RDDeck::Idle,
					 catch_macro_event_id[id]));
      if(catch_events[event].oneShot()) {
	PurgeEvent(event);
      }
    }
    catch_macro_event_id[id]=-1;
  }
  catch_event_free[id]=true;
}


void MainObject::freeEventsData()
{
  for(int i=0;i<RDCATCHD_MAX_MACROS;i++) {
    if((catch_event_pool[i]!=NULL)&&catch_event_free[i]) {
      delete catch_event_pool[i];
      catch_event_pool[i]=NULL;
    }
  }
}


void MainObject::heartbeatData()
{
  BroadcastCommand("HB!");
}


void MainObject::sysHeartbeatData()
{
  RDCart *cart=new RDCart(catch_heartbeat_cart);
  if(cart->exists()) {
    ExecuteMacroCart(cart);
  }
  delete cart;
}


void MainObject::updateXloadsData()
{
  std::vector<int>::iterator it;
  for(unsigned i=0;i<catch_active_xloads.size();i++) {
    switch(ReadExitCode(catch_active_xloads[i])) {
	case RDRecording::Ok:
	case RDRecording::ServerError:
	case RDRecording::InternalError:
	  it=catch_active_xloads.begin()+i;
	  BroadcastCommand(QString().
			   sprintf("RE 0 %d %d!",RDDeck::Idle,
				   catch_events[catch_active_xloads[i]].id()));
	  catch_active_xloads.erase(it,it+1);
	  break;

	default:
	  break;
    }
  }
  if(catch_active_xloads.size()==0) {
    catch_xload_timer->stop();
  }
}


void MainObject::startupCartData()
{
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
}


bool MainObject::StartRecording(int event)
{
  QString str;
  int length=0;
  QTime current_time=QTime::currentTime();

  if((event<0)||(event>=(int)catch_events.size())) {
    rda->syslog(LOG_DEBUG,"invalid event offset received, ignored");
    return false;
  }

  //
  // Ensure the Deck is Valid
  //
  unsigned deck=catch_events[event].channel();
  if((catch_record_card[deck-1]<0)||
     (catch_record_stream[deck-1]<0)) {
    WriteExitCodeById(catch_events[event].id(),RDRecording::InternalError);
    BroadcastCommand(QString().sprintf("RE %d %d %d!",
				       deck,catch_record_deck_status[deck-1],
				       catch_events[event].id()));
    rda->syslog(LOG_WARNING,"invalid audio device for deck: %d, event: %d",
		deck,catch_events[event].id());
    return false;
  }

  //
  // Ensure the Deck is Available
  //
  if((catch_record_deck_status[deck-1]!=RDDeck::Idle)&&
     (catch_record_deck_status[deck-1]!=RDDeck::Waiting)) {
    WriteExitCodeById(catch_events[event].id(),RDRecording::DeviceBusy);
    BroadcastCommand(QString().sprintf("RE %d %d %d!",
				       deck,catch_record_deck_status[deck-1],
				       catch_events[event].id()));
    rda->syslog(LOG_WARNING,
		"device busy for deck: %d, event: %d | in use by event: %d",
		deck,catch_events[event].id(),catch_record_id[deck-1]);
    return false;
  }

  //
  // Get Record Length
  //
  switch(catch_events[event].endType()) {
      case RDRecording::LengthEnd:
	length=catch_events[event].length();
	break;

      case RDRecording::HardEnd:
	length=current_time.msecsTo(catch_events[event].endTime());
	break;

      case RDRecording::GpiEnd:
	length=current_time.msecsTo(catch_events[event].endTime().
				    addMSecs(catch_events[event].endLength()));
	if(catch_events[event].maxGpiRecordLength()<length) {
	  length=catch_events[event].maxGpiRecordLength();
	}
	break;
  }

  //
  // Switch the input
  //
  if(!(catch_swaddress[deck-1]==QHostAddress())) {
    RDMacro *rml=new RDMacro();
    rml->setRole(RDMacro::Cmd);
    rml->setAddress(catch_swaddress[deck-1]);
    rml->setEchoRequested(false);
    rml->setCommand(RDMacro::ST);
    rml->addArg(catch_swmatrix[deck-1]);
    rml->addArg(catch_events[event].switchInput());
    rml->addArg(catch_swoutput[deck-1]);
    QString str;
    str=rml->toString();
    rda->ripc()->sendRml(rml);
    rda->syslog(LOG_DEBUG,"sending switcher command: \"%s\"",
		(const char *)str.toUtf8());
    delete rml;
  }

  //
  // Set Temp Name
  //
  RDCae::AudioCoding format=catch_events[event].format();
  QString cut_name;
  if(catch_events[event].normalizeLevel()==0) {
    cut_name=catch_events[event].cutName();
  }
  else {
    cut_name=QString().sprintf("rdcatchd-record-%d",catch_events[event].id());
    catch_events[event].
      setTempName(GetTempRecordingName(catch_events[event].id()));
    catch_events[event].setDeleteTempFile(true);
    format=RDCae::Pcm24;
  }    

  //
  // Start the recording
  //
  rda->cae()->loadRecord(catch_record_card[deck-1],
			catch_record_stream[deck-1],
			cut_name,
			format,
			catch_events[event].channels(),
			catch_events[event].sampleRate(),
			catch_events[event].bitrate());
  rda->cae()->record(catch_record_card[deck-1],catch_record_stream[deck-1],
		    length,0);
  catch_events[event].setStatus(RDDeck::Recording);

  str=QString().sprintf("record started: deck: %d, event: %d",
			deck,catch_events[event].id());
  str+=QString().sprintf(" card: %d, stream: %d, cut: %s length: %d",
			 catch_record_card[deck-1],
			 catch_record_stream[deck-1],
			 (const char *)cut_name,length);
  rda->syslog(LOG_INFO,str);

  //
  // Cache Selected Fields
  //
  catch_record_name[deck-1]=catch_events[event].cutName();
  catch_record_threshold[deck-1]=catch_events[event].trimThreshold();
  catch_record_id[deck-1]=catch_events[event].id();

  //
  // Update Cut Record
  //
  RDCut *cut=new RDCut(catch_events[event].cutName());
  cut->setOriginDatetime(QDateTime::currentDateTime());
  cut->setOriginName(rda->config()->stationName());
  switch(catch_events[event].format()) {
      case RDCae::Pcm16:
	cut->setCodingFormat(0);
	break;

      case RDCae::MpegL2:
	cut->setCodingFormat(1);
	break;

      default:
	cut->setCodingFormat(0);
	break;
  }
  cut->setChannels(catch_events[event].channels());
  cut->setSampleRate(catch_events[event].sampleRate());
  cut->setBitRate(catch_events[event].bitrate());
  cut->setPlayCounter(0);
  cut->setSegueStartPoint(-1);
  cut->setSegueEndPoint(-1);
  cut->setTalkStartPoint(-1);
  cut->setTalkEndPoint(-1);
  cut->setHookStartPoint(-1);
  cut->setHookEndPoint(-1);
  cut->setFadeupPoint(-1);
  cut->setFadedownPoint(-1);
  bool valid;
  QDateTime datetime=cut->startDatetime(&valid);
  if(valid) {
    if(catch_events[event].startdateOffset()!=0) {
      datetime.setDate(QDate::currentDate().
		       addDays(catch_events[event].startdateOffset()));
      cut->setStartDatetime(datetime,true);
    }
    if(catch_events[event].enddateOffset()!=0) {
      datetime=cut->endDatetime(&valid);
      datetime.setDate(QDate::currentDate().
		       addDays(catch_events[event].enddateOffset()));
      cut->setEndDatetime(datetime,true);
    }
  }
  delete cut;

  return true;
}


void MainObject::StartPlayout(int event)
{
  unsigned deck=catch_events[event].channel();
  if((catch_playout_card[deck-129]<0)) {
    rda->syslog(LOG_WARNING,	"invalid audio device for deck: %d, event: %d",
		deck,catch_events[event].id());
    return;
  }

  //
  // Get cut parameters
  //
  QString sql=QString("select ")+
    "START_POINT,"+  // 00
    "END_POINT "+    // 01
    "from CUTS where "+
    "CUT_NAME=\""+RDEscapeString(catch_events[event].cutName())+"\"";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(!q->first()) {
    return;
  }
  int start=q->value(0).toInt();
  int end=q->value(1).toInt();
  delete q;

  //
  // Start the playout
  //
  catch_playout_event_player[deck-129]->load(catch_events[event].cutName());
  rda->cae()->loadPlay(catch_playout_card[deck-129],
		      catch_events[event].cutName(),
		      &catch_playout_stream[deck-129],
		      &catch_playout_handle[deck-129]);
  RDSetMixerOutputPort(rda->cae(),catch_playout_card[deck-129],
		       catch_playout_stream[deck-129],
		       catch_playout_port[deck-129]);
  rda->cae()->positionPlay(catch_playout_handle[deck-129],start);
  catch_playout_event_player[deck-129]->start(start);
  rda->cae()->
    play(catch_playout_handle[deck-129],end-start,RD_TIMESCALE_DIVISOR,0);
  rda->cae()->setPlayPortActive(catch_playout_card[deck-129],
			       catch_playout_port[deck-129],
			       catch_playout_stream[deck-129]);
  catch_events[event].setStatus(RDDeck::Recording);

  rda->syslog(LOG_DEBUG,
	     "playout started: deck: %d, event %d  card %d, stream %d , cut=%s",
	      deck,catch_events[event].id(),
	      catch_playout_card[deck-129],
	      catch_playout_stream[deck-129],
	      (const char *)catch_events[event].cutName().toUtf8());
  
  //
  // Cache Selected Fields
  //
  catch_playout_name[deck-129]=catch_events[event].cutName();
  catch_playout_event_id[deck-129]=event;
}


void MainObject::StartMacroEvent(int event)
{
  RDCart *cart=new RDCart(catch_events[event].macroCart());
  if(!cart->exists()) {
    rda->syslog(LOG_WARNING,"cart %u does not exist!",
		catch_events[event].macroCart());
    delete cart;
    return;
  }
  if(cart->type()!=RDCart::Macro) {
    rda->syslog(LOG_WARNING,"%u is not a macro cart!",
		catch_events[event].macroCart());
    delete cart;
    return;
  }
  if(ExecuteMacroCart(cart,catch_events[event].id(),event)) {
    rda->syslog(LOG_INFO,"executing macro cart: %u",
		catch_events[event].macroCart());
  }
  delete cart;
}


void MainObject::StartSwitchEvent(int event)
{
  RDMacro *rml=new RDMacro();
  rml->setAddress(rda->station()->address());
  rml->setRole(RDMacro::Cmd);
  rml->setEchoRequested(false);
  rml->setCommand(RDMacro::ST);
  rml->addArg(catch_events[event].channel());
  rml->addArg(catch_events[event].switchInput());
  rml->addArg(catch_events[event].switchOutput());
  QString str=rml->toString();
  rda->syslog(LOG_INFO,"sent switch event, rml: \"%s\"",
	      (const char *)str.toUtf8());
  rda->ripc()->sendRml(rml);
  delete rml;
  if(catch_events[event].oneShot()) {
    PurgeEvent(event);
  }
}


void MainObject::StartDownloadEvent(int event)
{
  WriteExitCode(event,RDRecording::Downloading);
  catch_active_xloads.push_back(event);
  if(!catch_xload_timer->isActive()) {
    catch_xload_timer->start(XLOAD_UPDATE_INTERVAL);
  } 
  BroadcastCommand(QString().sprintf("RE 0 %d %d!",
				     RDDeck::Recording,
				     catch_events[event].id()));
  StartBatch(catch_events[event].id());
}


void MainObject::StartUploadEvent(int event)
{
  WriteExitCode(event,RDRecording::Uploading);
  catch_active_xloads.push_back(event);
  if(!catch_xload_timer->isActive()) {
    catch_xload_timer->start(XLOAD_UPDATE_INTERVAL);
  } 
  BroadcastCommand(QString().sprintf("RE 0 %d %d!",RDDeck::Recording,
				     catch_events[event].id()));
  StartBatch(catch_events[event].id());
}


bool MainObject::ExecuteMacroCart(RDCart *cart,int id,int event)
{
  int event_id=GetFreeEvent();
  if(event_id<0) {
    rda->syslog(LOG_WARNING,"unable to allocate event context, id=%d",id);
    return false;
  }
  if(event!=-1) {
    catch_events[event].setStatus(RDDeck::Recording);
  }
  if(id!=-1) {
    BroadcastCommand(QString().sprintf("RE 0 %d %d!",RDDeck::Recording,id));
  }
  catch_macro_event_id[event_id]=id;
  catch_event_pool[event_id]=
    new RDMacroEvent(rda->station()->address(),rda->ripc(),this,"event");
  catch_event_mapper->setMapping(catch_event_pool[event_id],event_id);
  connect(catch_event_pool[event_id],SIGNAL(finished()),
	  catch_event_mapper,SLOT(map()));
  QString cmd=cart->macros();
  catch_event_pool[event_id]->load(cmd);
  catch_event_pool[event_id]->exec();
  return true;
}


void MainObject::SendFullStatus(int ch)
{
  for(unsigned i=0;i<catch_events.size();i++) {
    if(catch_events[i].status()!=RDDeck::Idle) {
      EchoCommand(ch,QString().sprintf("RE 0 %d %d!",
					 catch_events[i].status(),
					 catch_events[i].id()));
    }
  }
  for(unsigned i=0;i<catch_active_xloads.size();i++) {
    EchoCommand(ch,QString().sprintf("RE 0 %d %d",
				       RDDeck::Recording,
				       catch_events[catch_active_xloads[i]].
				       id()));
  }
  for(int i=0;i<MAX_DECKS;i++) {
    EchoCommand(ch,QString().sprintf("RE %d %d %d!",i+1,
				       catch_record_deck_status[i],
				       catch_record_id[i]));
    EchoCommand(ch,QString().sprintf("RE %d %d %d!",i+129,
				       catch_playout_deck_status[i],
				       catch_playout_id[i]));
    EchoCommand(ch,QString().sprintf("MN %u %d!",i+1,catch_monitor_state[i]));
  }
}


void MainObject::SendMeterLevel(int deck,short levels[2])
{
  for(int i=0;i<catch_connections.size();i++) {
    if(catch_connections.at(i)!=NULL) {
      if(catch_connections.at(i)->meterEnabled()) {
	EchoCommand(i,QString().sprintf("RM %d 0 %d!",deck,(int)levels[0]));
	EchoCommand(i,QString().sprintf("RM %d 1 %d!",deck,(int)levels[1]));
      }
    }
  }
}


void MainObject::SendDeckEvent(int deck,int number)
{
  BroadcastCommand(QString().sprintf("DE %d %d!",deck,number));
}


void MainObject::ParseCommand(int ch)
{
  char data[1501];
  int n;

  ServerConnection *conn=catch_connections.at(ch);
  if(conn!=NULL) {
    while((n=conn->socket()->read(data,1500))>0) {
      data[n]=0;
      QString line=QString::fromUtf8(data);
      for(int i=0;i<line.length();i++) {
	QChar c=line.at(i);
	switch(c.toAscii()) {
	case '!':
	  DispatchCommand(conn);
	  conn->accum="";
	  break;

	case '\r':
	case '\n':
	  break;

	default:
	  conn->accum+=c;
	  break;
	}
      }
    }
  }
}


void MainObject::DispatchCommand(ServerConnection *conn)
{
  int chan;
  int id;
  int event;
  int code;
  QString str;
  bool ok=false;

  QStringList cmds=conn->accum.split(" ");

  //
  // Common Commands
  // Authentication not required to execute these!
  //
  if(cmds.at(0)=="DC") {  // Drop Connection
    socketKillData(conn->id());
    return;
  }

  if((cmds.at(0)=="PW")&&(cmds.size()==2)) {  // Password Authenticate
    if(cmds.at(1)==rda->config()->password()) {
      conn->setAuthenticated(true);
      EchoCommand(conn->id(),"PW +!");
      return;
    }
    else {
      conn->setAuthenticated(false);
      EchoCommand(conn->id(),"PW -!");
      return;
    }
  }

  //
  // Priviledged Commands
  // Authentication required to execute these!
  //
  if(!conn->isAuthenticated()) {
    EchoArgs(conn->id(),'-');
    return;
  }

  if(cmds.at(0)=="RS") {  // Reset
    EchoArgs(conn->id(),'+');
    LoadEngine();
  }

  if(cmds.at(0)=="RD") {  // Load Deck List
    EchoArgs(conn->id(),'+');
    LoadDeckList();
  }

  if(cmds.at(0)=="RO") {  // Reload Time Offset
    EchoArgs(conn->id(),'+');
    catch_engine->setTimeOffset(rda->station()->timeOffset());
  }

  if((cmds.at(0)=="RE")&&(cmds.size()==2)) {  // Request Status
    chan=cmds.at(1).toInt(&ok);
    if(!ok) {
      EchoArgs(conn->id(),'-');
      return;
    }
    if(chan==0) {
      SendFullStatus(conn->id());
      return;
    }
    chan--;
    if(chan<MAX_DECKS) {
      if(catch_record_deck_status[chan]==RDDeck::Offline) {
	EchoArgs(conn->id(),'-');
	return;
      }
      EchoCommand(conn->id(),QString().sprintf("RE %u %d %d!",
					       chan+1,
					       catch_record_deck_status[chan],
					       catch_record_id[chan]));
      EchoCommand(conn->id(),QString().sprintf("MN %u %d!",chan+1,
					       catch_monitor_state[chan]));
      return;
    }
    if((chan>=128)&&(chan<(MAX_DECKS+128))) {
      if(catch_playout_deck_status[chan-128]==RDDeck::Offline) {
	EchoArgs(conn->id(),'-');
	return;
      }
      EchoCommand(conn->id(),
		  QString().sprintf("RE %u %d %d!",
				    chan+1,catch_playout_deck_status[chan-128],
				    catch_playout_id[chan-128]));
      return;
    }
    EchoArgs(conn->id(),'-');
    return;
  }

  if((cmds.at(0)=="RM")&&(cmds.size()==2)) {  // Enable/Disable Metering
    conn->setMeterEnabled(cmds.at(1).trimmed()!="0");
  }

  if((cmds.at(0)=="SR")&&(cmds.size()==2)) {  // Stop Recording
    chan=cmds.at(1).toInt(&ok);
    if(!ok) {
      return;
    }
    if((chan>0)&&(chan<(MAX_DECKS+1))) {
      switch(catch_record_deck_status[chan-1]) {
      case RDDeck::Recording:
	catch_record_aborting[chan-1]=true;
	rda->cae()->stopRecord(catch_record_card[chan-1],
			       catch_record_stream[chan-1]);
	break;

      case RDDeck::Waiting:
	startTimerData(catch_record_id[chan-1]);
	break;

      default:
	break;
      }
    }
    if((chan>128)&&(chan<(MAX_DECKS+129))) {
      switch(catch_playout_deck_status[chan-129]) {
      case RDDeck::Recording:
	rda->cae()->stopPlay(catch_playout_handle[chan-129]);
	break;

      default:
	break;
      }
    }
  }

  if(cmds.at(0)=="RH") {  // Reload Heartbeat Configuration
    LoadHeartbeat();
  }

  if((cmds.at(0)=="MN")&&(cmds.size()==3)) {  // Monitor State
    chan=cmds.at(1).toInt(&ok);
    if(!ok) {
      return;
    }
    if((chan>0)&&(chan<(MAX_DECKS+1))) {
      if(catch_monitor_port[chan-1]>=0) {
	if(cmds.at(2).toInt()!=0) {
	  rda->cae()->setPassthroughVolume(catch_record_card[chan-1],
					  catch_record_stream[chan-1],
					  catch_monitor_port[chan-1],0);
	  catch_monitor_state[chan-1]=true;
	  BroadcastCommand(QString().sprintf("MN %d 1!",chan));
	}
	else {
	  rda->cae()->setPassthroughVolume(catch_record_card[chan-1],
					  catch_record_stream[chan-1],
					  catch_monitor_port[chan-1],
					  RD_MUTE_DEPTH);
	  catch_monitor_state[chan-1]=false;
	  BroadcastCommand(QString().sprintf("MN %d 0!",chan));
	}
      }
    }
  }

  if((cmds.at(0)=="SC")&&(cmds.size()>=4)) {  // Set Exit Code
    id=cmds.at(1).toInt(&ok);
    if(!ok) {
      return;
    }
    code=cmds.at(2).toInt(&ok);
    if(!ok) {
      return;
    }
    str="";
    for(int i=3;i<cmds.size();i++) {
      str+=cmds.at(i)+" ";
      str=str.left(str.length()-1);
    }
    if((event=GetEvent(id))<0) {
      return;
    }
    WriteExitCode(event,(RDRecording::ExitCode)code,str);
    BroadcastCommand(QString().sprintf("RE 0 %d %d!",RDDeck::Idle,id));
    if((RDRecording::ExitCode)code==RDRecording::Ok) {
      BroadcastCommand(QString().sprintf("RE 0 %d %d!",RDDeck::Idle,id));
    }
    else {
      BroadcastCommand(QString().sprintf("RE 0 %d %d!",RDDeck::Offline,id));
    }
  }
}


void MainObject::EchoCommand(int ch,const char *command)
{
//  LogLine(RDConfig::LogDebug,QString().sprintf("rdcatchd: EchoCommand(%d,%s)",ch,command));
  ServerConnection *conn=catch_connections.at(ch);
  if(conn->socket()->state()==QAbstractSocket::ConnectedState) {
    conn->socket()->write(command,strlen(command));
  }
}


void MainObject::BroadcastCommand(const char *command,int except_ch)
{
//  LogLine(RDConfig::LogDebug,QString().sprintf("rdcatchd: BroadcastCommand(%s)",command));
  for(int i=0;i<catch_connections.size();i++) {
    if(catch_connections.at(i)!=NULL) {
      if(i!=except_ch) {
	EchoCommand(i,command);
      }
    }
  }
}


void MainObject::EchoArgs(int ch,const char append)
{
  ServerConnection *conn=catch_connections.at(ch);
  if(conn!=NULL) {
    QString cmd=conn->accum+append+"!";
    EchoCommand(ch,cmd);
  }
}


void MainObject::LoadEngine(bool adv_day)
{
  RDSqlQuery *q;
  QString sql;

  catch_events.clear();
  rda->syslog(LOG_DEBUG,"rdcatchd engine load starts...");
  sql=LoadEventSql()+QString(" where STATION_NAME=\"")+
    RDEscapeString(rda->station()->name())+"\"";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    catch_events.push_back(CatchEvent(rda->station(),RDConfiguration()));
    LoadEvent(q,&catch_events.back(),true);
  }
  rda->syslog(LOG_DEBUG,"loaded %d events",(int)catch_events.size());
  delete q;
  rda->syslog(LOG_DEBUG,"rdcatchd engine load ends");
}


QString MainObject::LoadEventSql()
{
  return QString("select ")+
    "ID,"+                    // 00
    "IS_ACTIVE,"+             // 01
    "TYPE,"+                  // 02
    "CHANNEL,"+               // 03
    "CUT_NAME,"+              // 04
    "SUN,"+                   // 05
    "MON,"+                   // 06
    "TUE,"+                   // 07
    "WED,"+                   // 08
    "THU,"+                   // 09
    "FRI,"+                   // 10
    "SAT,"+                   // 11
    "START_TIME,"+            // 12
    "LENGTH,"+                // 13
    "START_GPI,"+             // 14
    "END_GPI,"+               // 15
    "TRIM_THRESHOLD,"+        // 16
    "STARTDATE_OFFSET,"+      // 17
    "ENDDATE_OFFSET,"+        // 18
    "FORMAT,"                 // 19
    "CHANNELS,"+              // 20
    "SAMPRATE,"+              // 21
    "BITRATE,"+               // 22
    "MACRO_CART,"+            // 23
    "SWITCH_INPUT,"+          // 24
    "SWITCH_OUTPUT,"+         // 25
    "ONE_SHOT,"+              // 26
    "START_TYPE,"+            // 27
    "START_LENGTH,"+          // 28
    "START_MATRIX,"+          // 29
    "START_LINE,"+            // 30
    "START_OFFSET,"+          // 31
    "END_TYPE,"+              // 32
    "END_TIME,"+              // 33
    "END_LENGTH,"+            // 34
    "END_MATRIX,"+            // 35
    "END_LINE,"+              // 36
    "URL,"+                   // 37
    "URL_USERNAME,"+          // 38
    "URL_PASSWORD,"+          // 39
    "QUALITY,"+               // 40
    "NORMALIZE_LEVEL,"+       // 41
    "ALLOW_MULT_RECS,"+       // 42
    "MAX_GPI_REC_LENGTH,"+    // 43
    "DESCRIPTION,"+           // 44
    "FEED_ID,"+               // 45
    "EVENTDATE_OFFSET,"+      // 46
    "ENABLE_METADATA "+       // 47
    "from RECORDINGS";
}


void MainObject::LoadEvent(RDSqlQuery *q,CatchEvent *e,bool add)
{
  e->setId(q->value(0).toUInt());
  e->setIsActive(RDBool(q->value(1).toString()));
  e->setType((RDRecording::Type)q->value(2).toInt());
  e->setChannel(q->value(3).toUInt());
  e->setCutName(q->value(4).toString());
  e->setDayOfWeek(7,RDBool(q->value(5).toString()));
  e->setDayOfWeek(1,RDBool(q->value(6).toString()));
  e->setDayOfWeek(2,RDBool(q->value(7).toString()));
  e->setDayOfWeek(3,RDBool(q->value(8).toString()));
  e->setDayOfWeek(4,RDBool(q->value(9).toString()));
  e->setDayOfWeek(5,RDBool(q->value(10).toString()));
  e->setDayOfWeek(6,RDBool(q->value(11).toString()));
  e->setStartTime(q->value(12).toTime());
  e->setLength(q->value(13).toUInt());
  e->setStartGpi(q->value(14).toInt());
  e->setEndGpi(q->value(15).toInt());
  e->setTrimThreshold(q->value(16).toUInt());
  e->setStartdateOffset(q->value(17).toUInt());
  e->setEnddateOffset(q->value(18).toUInt());
  switch((RDSettings::Format)q->value(19).toInt()) {
  case RDSettings::Pcm16:
    e->setFormat(RDCae::Pcm16);
    break;

  case RDSettings::Pcm24:
    e->setFormat(RDCae::Pcm24);
    break;

  case RDSettings::MpegL2:
  case RDSettings::MpegL2Wav:
    e->setFormat(RDCae::MpegL2);
    break;

  case RDSettings::MpegL3:
    e->setFormat(RDCae::MpegL3);
    break;

  case RDSettings::MpegL1:
  case RDSettings::Flac:
  case RDSettings::OggVorbis:
    break;
  }
  e->setChannels(q->value(20).toInt());
  e->setSampleRate(q->value(21).toUInt());
  e->setBitrate(q->value(22).toInt());
  e->setMacroCart(q->value(23).toInt());
  e->setSwitchInput(q->value(24).toInt());
  e->setSwitchOutput(q->value(25).toInt());
  e->setStatus(RDDeck::Idle);
  e->setOneShot(RDBool(q->value(26).toString()));
  e->setStartType((RDRecording::StartType)q->value(26).toInt());
  e->setStartLength(q->value(28).toInt());
  e->setStartMatrix(q->value(29).toInt());
  e->setStartLine(q->value(30).toInt());
  e->setStartOffset(q->value(31).toInt());
  e->setEndType((RDRecording::EndType)q->value(32).toInt());
  e->setEndTime(q->value(33).toTime());
  e->setEndLength(q->value(34).toInt());
  e->setEndMatrix(q->value(35).toInt());
  e->setEndLine(q->value(36).toInt());
  e->setUrl(q->value(37).toString());
  e->setUrlUsername(q->value(38).toString());
  e->setUrlPassword(q->value(39).toString());
  e->setQuality(q->value(40).toInt());
  e->setNormalizeLevel(q->value(41).toInt());
  e->setAllowMultipleRecordings(RDBool(q->value(42).toString()));
  e->setMaxGpiRecordLength(q->value(43).toUInt());
  e->setDescription(q->value(44).toString());
  e->setFeedId(q->value(45).toUInt());
  e->setEventdateOffset(q->value(46).toInt());
  e->setEnableMetadata(RDBool(q->value(47).toString()));

  if(add) {
    if(e->startType()==RDRecording::GpiStart) {
      e->setGpiStartTimer(new QTimer(this));
      catch_gpi_start_mapper->setMapping(e->gpiStartTimer(),e->id());
      connect(e->gpiStartTimer(),SIGNAL(timeout()),
	      catch_gpi_start_mapper,SLOT(map()));
      e->setGpiOffsetTimer(new QTimer(this));
      catch_gpi_offset_mapper->setMapping(e->gpiOffsetTimer(),e->id());
      connect(e->gpiOffsetTimer(),SIGNAL(timeout()),
	      catch_gpi_offset_mapper,SLOT(map()));
    }
    catch_engine->addEvent(e->id(),e->startTime());
  }
}


void MainObject::LoadDeckList()
{
  RDDeck::Status status[MAX_DECKS];

  //
  // Record Decks
  //
  for(int i=0;i<MAX_DECKS;i++) {
    status[i]=RDDeck::Offline;
  }
  QString sql=QString("select ")+
    "CHANNEL,"+          // 00
    "CARD_NUMBER,"+      // 01
    "PORT_NUMBER,"+      // 02
    "MON_PORT_NUMBER "+  // 03
    "from DECKS where "+
    "(STATION_NAME=\""+RDEscapeString(rda->config()->stationName())+"\")&&"+
    "(CARD_NUMBER!=-1)&&(CHANNEL>0)&&(CHANNEL<9)";
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    status[q->value(0).toUInt()-1]=RDDeck::Idle;
    catch_record_card[q->value(0).toUInt()-1]=q->value(1).toInt();
    catch_record_stream[q->value(0).toUInt()-1]=q->value(2).toInt();
    catch_monitor_port[q->value(0).toUInt()-1]=q->value(3).toInt();
  }
  delete q;
  for(int i=0;i<MAX_DECKS;i++) {
    if(catch_record_deck_status[i]==RDDeck::Recording) {
      if(status[i]==RDDeck::Idle) {
	catch_record_deck_status[i]=RDDeck::Recording;
      }
      else {
	rda->cae()->stopRecord(catch_record_card[i],catch_record_stream[i]);
	catch_record_deck_status[i]=RDDeck::Offline;
      }
    }
    else {
      catch_record_deck_status[i]=status[i];
    }
  }

  //
  // Playout Decks
  //
  for(int i=0;i<MAX_DECKS;i++) {
    status[i]=RDDeck::Offline;
  }
  sql=QString("select CHANNEL from DECKS where ")+
    "(STATION_NAME=\""+RDEscapeString(rda->config()->stationName())+"\")&&"+
    "(CARD_NUMBER!=-1)&&(CHANNEL>128)&&(CHANNEL<137)";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    status[q->value(0).toUInt()-129]=RDDeck::Idle;
  }
  delete q;
  for(int i=0;i<MAX_DECKS;i++) {
    if(catch_playout_deck_status[i]==RDDeck::Recording) {
      if(status[i]==RDDeck::Idle) {
	catch_playout_deck_status[i]=RDDeck::Recording;
      }
      else {
	rda->cae()->stopPlay(catch_playout_handle[i]);
	catch_playout_deck_status[i]=RDDeck::Offline;
      }
    }
    else {
      catch_playout_deck_status[i]=status[i];
    }
  }
}


int MainObject::GetRecordDeck(int card,int stream)
{
  for(int i=0;i<MAX_DECKS;i++) {
    if((catch_record_card[i]==card)&&(catch_record_stream[i]==stream)) {
      return i+1;
    }
  }
  return -1;
}


int MainObject::GetPlayoutDeck(int handle)
{
  for(int i=0;i<MAX_DECKS;i++) {
    if(catch_playout_handle[i]==handle) {
      return i+129;
    }
  }
  return -1;
}


int MainObject::GetFreeEvent()
{
  for(int i=0;i<RDCATCHD_MAX_MACROS;i++) {
    if(catch_event_pool[i]==NULL) {
      catch_event_free[i]=false;
      return i;
    }
  }
  return -1;
}


bool MainObject::AddEvent(int id)
{
  RDSqlQuery *q;
  QString sql;
  QString day_name;


  //
  // Load Schedule
  //
  sql=LoadEventSql()+
    QString(" where ")+
    "(STATION_NAME=\""+RDEscapeString(rda->station()->name())+"\")&&"+
    QString().sprintf("(ID=%d)",id);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    catch_events.push_back(CatchEvent(rda->station(),RDConfiguration()));
    LoadEvent(q,&catch_events.back(),true);
    switch((RDRecording::Type)q->value(2).toInt()) {
    case RDRecording::Recording:
      rda->syslog(LOG_DEBUG,"loading event %d, Type: recording, Cut: %s",
		  id,(const char *)q->value(4).toString().toUtf8());
      break;

    case RDRecording::Playout:
      rda->syslog(LOG_DEBUG,"loading event %d, Type: playout, Cut: %s",
		  id,(const char *)q->value(4).toString().toUtf8());
      break;

    case RDRecording::MacroEvent:
      rda->syslog(LOG_DEBUG,"loading event %d, Type: macro, Cart: %d",
		  id,q->value(23).toUInt());
      break;

    case RDRecording::SwitchEvent:
      rda->syslog(LOG_DEBUG,
	     "loading event %d, Type: switch, Matrix: %d, Source: %d  Dest: %d",
		  id,q->value(3).toInt(),q->value(24).toInt(),
		  q->value(25).toInt());
      break;

    case RDRecording::Download:
      rda->syslog(LOG_DEBUG,"loading event %d, Type: download, Cut: %s",
		  id,(const char *)q->value(4).toString().toUtf8());
      break;

    case RDRecording::Upload:
      rda->syslog(LOG_DEBUG,"loading event %d, Type: upload, Cut: %s",
		  id,(const char *)q->value(4).toString().toUtf8());
      break;

    case RDRecording::LastType:
      break;
    }
    delete q;
    return true;
  }
  rda->syslog(LOG_DEBUG,"event %d not found, not loaded",id);
  delete q;
  return false;
}


void MainObject::RemoveEvent(int id)
{
  int event=GetEvent(id);
  if(event<0) {
    rda->syslog(LOG_DEBUG,"event %d not found, not removed",id);
    return;
  }
  switch(catch_events[event].type()) {
  case RDRecording::Recording:
    rda->syslog(LOG_DEBUG,"removed event %d, Type: recording, Cut: %s",
		id,(const char *)catch_events[event].cutName().toUtf8());
    break;
	
  case RDRecording::Playout:
    rda->syslog(LOG_DEBUG,"removed event %d, Type: playout, Cut: %s",
		id,(const char *)catch_events[event].cutName().toUtf8());
    break;
	
  case RDRecording::MacroEvent:
    rda->syslog(LOG_DEBUG,"removed event %d, Type: macro, Cart: %u",
		id,catch_events[event].macroCart());
    break;
	
  case RDRecording::SwitchEvent:
    rda->syslog(LOG_DEBUG,
	     "removed event %d, Type: switch, Matrix: %d, Source: %d  Dest: %d",
		id,
		catch_events[event].channel(),
		catch_events[event].switchInput(),
		catch_events[event].switchOutput());
    break;
	
  case RDRecording::Download:
    rda->syslog(LOG_DEBUG,"removed event %d, Type: download, Cut: %s",
		id,(const char *)catch_events[event].cutName().toUtf8());
    break;
	
  case RDRecording::Upload:
    rda->syslog(LOG_DEBUG,"removed event %d, Type: upload, Cut: %s",
		id,(const char *)catch_events[event].cutName().toUtf8());
    break;

  case RDRecording::LastType:
    break;
  }
  std::vector<CatchEvent>::iterator it=catch_events.begin()+event;
  catch_events.erase(it,it+1);
  catch_engine->removeEvent(id);
}


bool MainObject::UpdateEvent(int id)
{
  RemoveEvent(id);
  return AddEvent(id);
}


int MainObject::GetEvent(int id)
{
  for(unsigned i=0;i<catch_events.size();i++) {
    if((int)catch_events[i].id()==id) {
      return i;
    }
  }
  return -1;
}


void MainObject::PurgeEvent(int event)
{
  QString sql=QString().sprintf("delete from RECORDINGS where ID=%d",
				catch_events[event].id());
  RDSqlQuery *q=new RDSqlQuery(sql);
  delete q;
  BroadcastCommand(QString().sprintf("PE %d!",catch_events[event].id()));
  switch(catch_events[event].type()) {
  case RDRecording::Recording:
    rda->syslog(LOG_DEBUG,"purged event %d, Type: recording, Cut: %s",
		catch_events[event].id(),
		(const char *)catch_events[event].cutName().toUtf8());
    break;

  case RDRecording::Playout:
    rda->syslog(LOG_DEBUG,"purged event %d, Type: playout, Cut: %s",
		catch_events[event].id(),
		(const char *)catch_events[event].cutName().toUtf8());
    break;

  case RDRecording::MacroEvent:
    rda->syslog(LOG_DEBUG,"purged event %d, Type: macro, Cart: %u",
		catch_events[event].id(),
		catch_events[event].macroCart());
    break;

  case RDRecording::SwitchEvent:
    rda->syslog(LOG_DEBUG,
	      "purged event %d, Type: switch, Matrix: %d, Source: %d  Dest: %d",
		catch_events[event].id(),
		catch_events[event].channel(),
		catch_events[event].switchInput(),
		catch_events[event].switchOutput());
    break;

  case RDRecording::Download:
    rda->syslog(LOG_DEBUG,"purged event %d, Type: download, Cut: %s",
		catch_events[event].id(),
		(const char *)catch_events[event].cutName().toUtf8());
    break;

  case RDRecording::Upload:
    rda->syslog(LOG_DEBUG,"purged event %d, Type: upload, Cut: %s",
		catch_events[event].id(),
		(const char *)catch_events[event].cutName().toUtf8());
    break;

  case RDRecording::LastType:
    break;
  }
  catch_engine->removeEvent(catch_events[event].id());
  std::vector<CatchEvent>::iterator it=catch_events.begin()+event;
  catch_events.erase(it,it+1);
}


void MainObject::LoadHeartbeat()
{
  if(catch_heartbeat_timer->isActive()) {
    catch_heartbeat_timer->stop();
  }
  QString sql=QString("select ")+
    "HEARTBEAT_CART,"+      // 00
    "HEARTBEAT_INTERVAL "+  // 01
    "from STATIONS where "+
    "NAME=\""+RDEscapeString(rda->station()->name())+"\"";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    if((q->value(0).toUInt()!=0)&&(q->value(1).toUInt()!=0)) {
      catch_heartbeat_cart=q->value(0).toUInt();
      sysHeartbeatData();
      catch_heartbeat_timer->start(q->value(1).toUInt());
    }
  }
  delete q;
}


void MainObject::CheckInRecording(QString cutname,CatchEvent *evt,
				  unsigned msecs,unsigned threshold)
{
  RDCut *cut=new RDCut(cutname);
  RDSettings *s=new RDSettings();
  s->setFormat((RDSettings::Format)evt->format());
  s->setSampleRate(evt->sampleRate());
  s->setBitRate(evt->bitrate());
  s->setChannels(evt->channels());
  cut->checkInRecording(rda->config()->stationName(),"",
			rda->config()->stationName(),s,msecs);
  cut->setSha1Hash(RDSha1Hash(RDCut::pathName(cut->cutName())));
  delete s;
  cut->autoTrim(RDCut::AudioBoth,-threshold);
  RDCart *cart=new RDCart(cut->cartNumber());
  cart->updateLength();
  delete cart;
  delete cut;
  chown(RDCut::pathName(cutname),rda->config()->uid(),rda->config()->gid());
}


void MainObject::CheckInPodcast(CatchEvent *e) const
{
  QString sql;
  RDSqlQuery *q;

  //
  // Purge Stale Casts
  //
  sql=QString("delete from PODCASTS where ")+
    QString().sprintf("(FEED_ID=%d)&&",e->feedId())+
    "(AUDIO_FILENAME=\""+RDEscapeString(RDGetBasePart(e->resolvedUrl()))+"\")";
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Get Channel Parameters
  //
  sql=QString("select ")+
    "ENABLE_AUTOPOST,"+      // 00
    "CHANNEL_TITLE,"+        // 01
    "CHANNEL_DESCRIPTION,"+  // 02
    "CHANNEL_CATEGORY,"+     // 03
    "CHANNEL_LINK,"+         // 04
    "MAX_SHELF_LIFE "+       // 05
    "from FEEDS	where "+
    QString().sprintf("ID=%u",e->feedId());
  q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete q;
    return;
  }

  //
  // Add the Cast Entry
  //
  RDPodcast::Status status=RDPodcast::StatusPending;
  if(q->value(0).toString().lower()=="y") {
    status=RDPodcast::StatusActive;
  }
  sql=QString("insert into PODCASTS set ")+
    QString().sprintf("FEED_ID=%u,",e->feedId())+
    QString().sprintf("STATUS=%u,",status)+
    "ITEM_TITLE=\""+RDEscapeString(q->value(1).toString())+"\","+
    "ITEM_DESCRIPTION=\""+RDEscapeString(q->value(2).toString())+"\","+
    "ITEM_CATEGORY=\""+RDEscapeString(q->value(3).toString())+"\","+
    "ITEM_LINK=\""+RDEscapeString(q->value(4).toString())+"\","+
    "AUDIO_FILENAME=\""+RDEscapeString(RDGetBasePart(e->resolvedUrl()))+"\","+
    QString().sprintf("AUDIO_LENGTH=%u,",e->podcastLength())+
    QString().sprintf("AUDIO_TIME=%u,",e->podcastTime())+
    QString().sprintf("SHELF_LIFE=%u,",q->value(5).toUInt())+
    "EFFECTIVE_DATETIME=now(),"+
    "ORIGIN_DATETIME=now()";
  delete q;
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Update the Build Date
  //
  sql=QString("update FEEDS set ")+
    "LAST_BUILD_DATETIME=now() where "+
    QString().sprintf("ID=%u",e->feedId());
  q=new RDSqlQuery(sql);
  delete q;
}


RDRecording::ExitCode MainObject::ReadExitCode(int event)
{
  RDRecording::ExitCode code=RDRecording::InternalError;
  QString sql=QString("select EXIT_CODE from RECORDINGS where ")+
    QString().sprintf("ID=%d",catch_events[event].id());
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    code=(RDRecording::ExitCode)q->value(0).toInt();
  }
  delete q;
  
  return code;
}


void MainObject::WriteExitCode(int event,RDRecording::ExitCode code,
			       const QString &err_text)
{
  QString sql=QString("update RECORDINGS set ")+
    QString().sprintf("EXIT_CODE=%d,",code)+
    "EXIT_TEXT=\""+RDEscapeString(err_text)+"\" where "+
    QString().sprintf("ID=%d",catch_events[event].id());
  RDSqlQuery *q=new RDSqlQuery(sql);
  delete q;
  switch(code) {
  case RDRecording::Ok:
  case RDRecording::Downloading:
  case RDRecording::Uploading:
  case RDRecording::RecordActive:
  case RDRecording::PlayActive:
  case RDRecording::Waiting:
    break;

  case RDRecording::ServerError:
  case RDRecording::InternalError:
    SendErrorMessage(&catch_events[event],
		     RDRecording::exitString(code)+": "+err_text,
		     catch_conf->errorRml());
    break;

  default:
    SendErrorMessage(&catch_events[event],RDRecording::exitString(code),
		     catch_conf->errorRml());
    break;
  }
}


void MainObject::WriteExitCodeById(int id,RDRecording::ExitCode code,
				   const QString &err_text)
{
  QString sql=QString("update RECORDINGS set ")+
    QString().sprintf("EXIT_CODE=%d,",code)+
    "EXIT_TEXT=\""+RDEscapeString(err_text)+"\" where "+
    QString().sprintf("ID=%d",id);
  RDSqlQuery *q=new RDSqlQuery(sql);
  delete q;
}


QString MainObject::BuildTempName(int event,QString str)
{
  return BuildTempName(&catch_events[event],str);
}


QString MainObject::BuildTempName(CatchEvent *evt,QString str)
{
  return catch_temp_dir+"/rdcatchd="+str+
    QString().sprintf("%u-%u",evt->id(),getpid());
}


QString MainObject::GetFileExtension(QString filename)
{
  for(int i=filename.length()-1;i>=0;i--) {
    if(((const char *)filename)[i]=='/') {
      return QString();
    }
    if(((const char *)filename)[i]=='.') {
      return filename.right(filename.length()-i-1);
    }
  }
  return QString();
}

/* This is an overloaded virtual function to tell a session manager not to restart this daemon. */
void QApplication::saveState(QSessionManager &sm) {
  sm.setRestartHint(QSessionManager::RestartNever);
  return;
};


bool MainObject::SendErrorMessage(CatchEvent *event,const QString &err_desc,
				  QString rml)
{
  if(rml.isEmpty()) {
    return false;
  }
  ResolveErrorWildcards(event,err_desc,&rml);

  //
  // Execute the RML
  //
  int event_id=GetFreeEvent();
  if(event_id<0) {
    rda->syslog(LOG_WARNING,"unable to allocate event context, id=%d",
		event->id());
  }
  catch_macro_event_id[event_id]=event->id()+RDCATCHD_ERROR_ID_OFFSET;
  catch_event_pool[event_id]=
    new RDMacroEvent(rda->station()->address(),rda->ripc(),this,"event");
  catch_event_mapper->setMapping(catch_event_pool[event_id],event_id);
  connect(catch_event_pool[event_id],SIGNAL(finished()),
	  catch_event_mapper,SLOT(map()));
  bool res=catch_event_pool[event_id]->load(rml);
  catch_event_pool[event_id]->exec();
  rda->syslog(LOG_INFO,"executed error rml: id=%d, rml=\"%s\", res=%d",
	      event->id(),(const char *)rml.toUtf8(),res);
  return true;
}


void MainObject::ResolveErrorWildcards(CatchEvent *event,
				       const QString &err_desc,QString *rml)
{
  rml->replace("%d",event->description());
  rml->replace("%e",err_desc);  // Error Description
  rml->replace("%i",QString().sprintf("%u",event->id()));
  rml->replace("%t",event->startTime().toString("hh:mm:ss"));
  rml->replace("%y",RDRecording::typeString(event->type()));
  switch(event->type()) {
  case RDRecording::Recording:
    rml->replace("%k",QString().sprintf("%d",event->channel()));
    rml->replace("%n",event->cutName().left(6));
    rml->replace("%u","n/a");
    break;

  case RDRecording::Playout:
    rml->replace("%k",QString().sprintf("%d",event->channel()-128));
    rml->replace("%n",event->cutName().left(6));
    rml->replace("%u","n/a");
    break;

  case RDRecording::Upload:
    rml->replace("%k","n/a");
    rml->replace("%n",event->cutName().left(6));
    rml->replace("%u",event->resolvedUrl());
    break;

  case RDRecording::Download:
    rml->replace("%k","n/a");
    rml->replace("%n",event->cutName().left(6));
    rml->replace("%u",event->resolvedUrl());
    break;

  case RDRecording::MacroEvent:
    rml->replace("%k","n/a");
    rml->replace("%n",QString().sprintf("%06u",event->macroCart()));
    rml->replace("%u","n/a");
    break;

  case RDRecording::SwitchEvent:
    rml->replace("%k","n/a");
    rml->replace("%n",tr("n/a"));
    rml->replace("%u","n/a");
    break;

  case RDRecording::LastType:
    break;
  }
}


unsigned MainObject::GetNextDynamicId()
{
  unsigned id=RDCATCHD_DYNAMIC_BASE_ID;

  for(unsigned i=0;i<catch_events.size();i++) {
    if(catch_events[i].id()>=id) {
      id=catch_events[id].id()+1;
    }
  }
  return id;
}


void MainObject::RunRmlRecordingCache(int chan)
{
  if(catch_record_pending_cartnum[chan-1]==0) {
    return;
  }
  StartRmlRecording(chan,catch_record_pending_cartnum[chan-1],
		    catch_record_pending_cutnum[chan-1],
		    catch_record_pending_maxlen[chan-1]);
  catch_record_pending_cartnum[chan-1]=0;
}


void MainObject::StartRmlRecording(int chan,int cartnum,int cutnum,int maxlen)
{
  RDDeck *deck=new RDDeck(rda->config()->stationName(),chan);
  RDCut *cut=new RDCut(cartnum,cutnum);
  QDateTime dt=QDateTime(QDate::currentDate(),QTime::currentTime());
  catch_events.push_back(CatchEvent(rda->station(),RDConfiguration()));
  catch_events.back().setId(GetNextDynamicId());
  catch_events.back().setIsActive(true);
  catch_events.back().setOneShot(true);
  catch_events.back().setType(RDRecording::Recording);
  catch_events.back().setChannel(chan);
  catch_events.back().setCutName(cut->cutName());
  catch_events.back().setDayOfWeek(dt.date().dayOfWeek(),true);
  catch_events.back().setStartTime(dt.time());
  catch_events.back().setEndType(RDRecording::LengthEnd);
  catch_events.back().setLength(maxlen);
  catch_events.back().
    setFormat((RDCae::AudioCoding)deck->defaultFormat());
  catch_events.back().setChannels(deck->defaultChannels());
  catch_events.back().setSampleRate(rda->system()->sampleRate());
  catch_events.back().setBitrate(deck->defaultBitrate());
  catch_events.back().setNormalizeLevel(0);
  StartRecording(catch_events.size()-1);
  delete cut;
  delete deck;
}


void MainObject::StartBatch(int id)
{
  if((fork())==0) {
    QString bin=QString(RD_PREFIX)+"/"+"sbin/rdcatchd";
    execl(bin,(const char *)bin,
	  (const char *)QString().sprintf("--event-id=%d",id),
	  (char *)NULL);
    rda->syslog(LOG_ERR,"failed to exec %s --event-id=%d: %s",
		(const char *)bin,
		id,strerror(errno));
    exit(0);
  }
}


QString MainObject::GetTempRecordingName(int id) const
{
  return QString().sprintf("%s/rdcatchd-record-%d.%s",
			   RDConfiguration()->audioRoot().ascii(),id,
			   RDConfiguration()->audioExtension().ascii());
}


int main(int argc,char *argv[])
{
  QApplication a(argc,argv,false);
  new MainObject();
  return a.exec();
}
