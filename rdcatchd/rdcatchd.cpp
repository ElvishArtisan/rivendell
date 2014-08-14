// rdcatchd.cpp
//
// The Rivendell Netcatcher Daemon
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcatchd.cpp,v 1.142.4.2.2.1 2014/06/03 18:23:38 cvs Exp $
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

#include <rddb.h>
#include <rdconf.h>
#include <rdurl.h>
#include <rdwavefile.h>
#include <rdcut.h>
#include <rdcatchd_socket.h>
#include <rdcatchd.h>
#include <rdrecording.h>
#include <rdttyout.h>
#include <rdmixer.h>
#include <rdcheck_daemons.h>
#include <rddebug.h>
#include <rddatedecode.h>
#include <rdescape_string.h>
#include <rdpodcast.h>
#include <rdsettings.h>
#include <rdlibrary_conf.h>
#include <rdpaths.h>
#include <dbversion.h>

RDConfig *catch_config;


// Logging function that works within and outside the MainObject. 
//static RDConfig *rd_config = NULL;
void LogLine(RDConfig::LogPriority prio,const QString &line)
{
  FILE *logfile;

  catch_config->log("rdcatchd",prio,line);

  if(catch_config->catchdLogname().isEmpty()) {
    return;
  }

  QDateTime current=QDateTime::currentDateTime();
  logfile=fopen(catch_config->catchdLogname(),"a");
  if(logfile==NULL) {
    return;
  }
  chmod(catch_config->catchdLogname(),S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
  fprintf(logfile,"%02d/%02d/%4d - %02d:%02d:%02d.%03d : %s\n",
	  current.date().month(),
	  current.date().day(),
	  current.date().year(),
	  current.time().hour(),
	  current.time().minute(),
	  current.time().second(),
	  current.time().msec(),
	  (const char *)line);
  fclose(logfile);
}


void SigHandler(int signum)
{
  pid_t local_pid;

  switch(signum) {
      case SIGINT:
      case SIGTERM:
	RDDeletePid(RD_PID_DIR,"rdcatchd.pid");
	LogLine(RDConfig::LogNotice,"rdcatchd exiting");
	delete catch_config;
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


MainObject::MainObject(QObject *parent,const char *name)
  :QObject(parent,name)
{
  QString sql;
  RDSqlQuery *q;
  bool skip_db_check=false;
  unsigned schema=0;

  //
  // Load the config
  //
  catch_config=new RDConfig();
  catch_config->load();

  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"rdcatchd",RDCATCHD_USAGE);
  for(unsigned i=0;i<cmd->keys();i++) {
    if(cmd->key(i)=="--event-id") {
      RunBatch(cmd);
      return;
    }
    if(cmd->key(i)=="--skip-db-check") {
      skip_db_check=true;
    }
  }

  //
  // Make sure we're the only instance running
  //
  if(RDCheckDaemon(RD_RDCATCHD_PID)) {
    printf("rdcatchd: aborting - multiple instances not allowed");
    exit(1);
  }

  //
  // Initialize Data Structures
  //
  debug=false;
  for(unsigned i=0;i<RDCATCHD_MAX_CONNECTIONS;i++) {
    socket[i]=NULL;
    istate[i]=0;
    argnum[i]=0;
    argptr[i]=0;
    auth[i]=false;
    catch_meter_enabled[i]=false;
  }
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
  catch_temp_dir=RDTempDir();

  //
  // Macro Event Handling
  //
  for(int i=0;i<RDCATCHD_MAX_MACROS;i++) {
    catch_event_pool[i]=NULL;
    catch_event_free[i]=true;
    catch_macro_event_id[i]=-1;
  }
  catch_event_mapper=new QSignalMapper(this,"catch_event_mapper");
  connect(catch_event_mapper,SIGNAL(mapped(int)),
	  this,SLOT(eventFinishedData(int)));
  QTimer *timer=new QTimer(this,"free_events_timer");
  connect(timer,SIGNAL(timeout()),this,SLOT(freeEventsData()));
  timer->start(RDCATCHD_FREE_EVENTS_INTERVAL);

  server=new RDCatchdSocket(RDCATCHD_TCP_PORT,0,this,"socket");
  if(!server->ok()) {
    printf("rdcatchd: aborting - couldn't bind socket");
    exit(1);
  }
  connect(server,SIGNAL(connection(int)),this,SLOT(newConnection(int)));

  //
  // Open Database
  //
  QString err (tr("ERROR rdcatchd aborting - "));

  catch_db=RDInitDb(&schema,&err);
  if(!catch_db) {
    printf(err.ascii());
    exit(1);
  }
  if((schema!=RD_VERSION_DATABASE)&&(!skip_db_check)) {
    fprintf(stderr,
	    "rdcatchd: database version mismatch, should be %u, is %u\n",
	    RD_VERSION_DATABASE,schema);
    exit(256);
  }
  connect (RDDbStatus(),SIGNAL(logText(RDConfig::LogPriority,const QString &)),
	   this,SLOT(log(RDConfig::LogPriority,const QString &)));

  //
  // Create RDCatchConf
  //
  catch_conf=new RDCatchConf(catch_config->stationName());

  //
  // GPI Mappers
  //
  catch_gpi_start_mapper=new QSignalMapper(this,"catch_gpi_start_mapper");
  connect(catch_gpi_start_mapper,SIGNAL(mapped(int)),
	  this,SLOT(startTimerData(int)));
  catch_gpi_offset_mapper=new QSignalMapper(this,"catch_gpi_offset_mapper");
  connect(catch_gpi_offset_mapper,SIGNAL(mapped(int)),
	  this,SLOT(offsetTimerData(int)));

  //
  // Xload Timer
  //
  catch_xload_timer=new QTimer(this,"catch_xload_timer");
  connect(catch_xload_timer,SIGNAL(timeout()),this,SLOT(updateXloadsData()));

  //
  // RIPCD Connection
  //
  catch_ripc=new RDRipc(catch_config->stationName());
  catch_ripc->connectHost("localhost",RIPCD_TCP_PORT,catch_config->password());
  connect(catch_ripc,SIGNAL(rmlReceived(RDMacro *)),
	  this,SLOT(rmlReceivedData(RDMacro *)));
  connect(catch_ripc,SIGNAL(gpiStateChanged(int,int,bool)),
	  this,SLOT(gpiStateChangedData(int,int,bool)));

  //
  // System Configuration
  //
  catch_system=new RDSystem();

  //
  // Station Configuration
  //
  catch_rdstation=new RDStation(catch_config->stationName());

  //
  // CAE Connection
  //
  catch_cae=new RDCae(catch_rdstation,catch_config,this,"catch_cae");
  connect(catch_cae,SIGNAL(isConnected(bool)),
	  this,SLOT(isConnectedData(bool)));
  connect(catch_cae,SIGNAL(recordLoaded(int,int)),
	  this,SLOT(recordLoadedData(int,int)));
  connect(catch_cae,SIGNAL(recording(int,int)),
	  this,SLOT(recordingData(int,int)));
  connect(catch_cae,SIGNAL(recordStopped(int,int)),
	  this,SLOT(recordStoppedData(int,int)));
  connect(catch_cae,SIGNAL(recordUnloaded(int,int,unsigned)),
	  this,SLOT(recordUnloadedData(int,int,unsigned)));
  connect(catch_cae,SIGNAL(playLoaded(int)),
	  this,SLOT(playLoadedData(int)));
  connect(catch_cae,SIGNAL(playing(int)),
	  this,SLOT(playingData(int)));
  connect(catch_cae,SIGNAL(playStopped(int)),
	  this,SLOT(playStoppedData(int)));
  connect(catch_cae,SIGNAL(playUnloaded(int)),
	  this,SLOT(playUnloadedData(int)));
  catch_cae->connectHost();

  //
  // Sound Initialization
  //
  RDSetMixerPorts(catch_config->stationName(),catch_cae);
  sql=QString().sprintf("select CHANNEL,CARD_NUMBER,PORT_NUMBER from DECKS \
                         where (STATION_NAME=\"%s\")&&\
                         (CARD_NUMBER!=-1)&&(CHANNEL>0)&&(CHANNEL<9)",
			(const char *)catch_config->stationName());
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
  sql=QString().sprintf("select CARD_NUMBER,PORT_NUMBER,\
                         MON_PORT_NUMBER,CHANNEL from DECKS\
                         where (STATION_NAME=\"%s\")&&(CHANNEL<=%d)&&\
                         (CARD_NUMBER>=0)&&(MON_PORT_NUMBER>=0)&&\
                         (DEFAULT_MONITOR_ON=\"Y\")",
			(const char *)catch_config->stationName(),
			MAX_DECKS);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    catch_cae->setPassthroughVolume(q->value(0).toInt(),q->value(1).toInt(),
				    q->value(2).toInt(),0);
    catch_monitor_state[q->value(3).toUInt()-1]=true;
  }
  delete q;

  StartDropboxes();

  //
  // Time Engine
  //
  catch_engine=new RDTimeEngine(this,"catch_engine");
  catch_engine->setTimeOffset(catch_rdstation->timeOffset());
  connect(catch_engine,SIGNAL(timeout(int)),this,SLOT(engineData(int)));
  LoadEngine();

  if(qApp->argc()==1) {
    RDDetach(catch_config->logCoreDumpDirectory());
  }
  else {
    debug=true;
  }

  ::signal(SIGINT,SigHandler);
  ::signal(SIGTERM,SigHandler);
  ::signal(SIGCHLD,SigHandler);
  if(!RDWritePid(RD_PID_DIR,"rdcatchd.pid")) {
    printf("rdcatchd: aborting - can't write pid file\n");
    exit(1);
  }

  //
  // Start Heartbeat Timer
  //
  timer=new QTimer(this,"heartbeat_timer");
  connect(timer,SIGNAL(timeout()),this,SLOT(heartbeatData()));
  timer->start(RDCATCHD_HEARTBEAT_INTERVAL);

  //
  // Meter Timer
  //
  timer=new QTimer(this,"meter_timer");
  connect(timer,SIGNAL(timeout()),this,SLOT(meterData()));
  timer->start(RD_METER_UPDATE_INTERVAL);

  //
  // Heartbeat Timer
  //
  catch_heartbeat_timer=new QTimer(this,"catch_heartbeat_timer");
  connect(catch_heartbeat_timer,SIGNAL(timeout()),
	  this,SLOT(sysHeartbeatData()));
  LoadHeartbeat();

  //
  // Mark Interrupted Events
  //
  sql=QString().sprintf("update RECORDINGS set EXIT_CODE=%d\
                         where ((EXIT_CODE=%d)||(EXIT_CODE=%d))||\
                         (EXIT_CODE=%d)&&(STATION_NAME=\"%s\")",
			RDRecording::Interrupted,
			RDRecording::Uploading,
			RDRecording::Downloading,
			RDRecording::RecordActive,
			(const char *)catch_config->stationName());
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("update RECORDINGS set EXIT_CODE=%d\
                         where ((EXIT_CODE=%d)||(EXIT_CODE=%d))&&\
                         (STATION_NAME=\"%s\")",
			RDRecording::Ok,
			RDRecording::Waiting,
			RDRecording::PlayActive,
			(const char *)catch_config->stationName());
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
  if(catch_config->useRealtime()) {
    struct sched_param sp;
    memset(&sp,0,sizeof(sp));
    if(catch_config->realtimePriority()>0) {
      sp.sched_priority=catch_config->realtimePriority()-1;
    }
    if(sched_setscheduler(getpid(),SCHED_FIFO,&sp)!=0) {
      LogLine(RDConfig::LogWarning,
	      QString().sprintf("unable to set realtime permissions, %s",
				strerror(errno)));
    }
    mlockall(MCL_CURRENT|MCL_FUTURE);
  }

  LogLine(RDConfig::LogNotice,"rdcatchd started");
}

void MainObject::log(RDConfig::LogPriority prio,const QString &msg)
{
  LogLine(prio,msg);
}


void MainObject::newConnection(int fd)
{
  unsigned i=0;

  while((i<RDCATCHD_MAX_CONNECTIONS)&&(socket[i]!=NULL)) {
    i++;
  }
  if(i==RDCATCHD_MAX_CONNECTIONS) {      // Table full, drop it on the floor
    LogLine(RDConfig::LogWarning,QString().sprintf(
                "rdcatchd connection table full, dropping connection (%d/%d)", 
                i, RDCATCHD_MAX_CONNECTIONS));
    close(fd);
    return;
  }
  socket[i]=new RDSocket(i,this,"socket_0");
  socket[i]->setSocket(fd);
  connect(socket[i],SIGNAL(readyReadID(int)),this,SLOT(socketData(int)));
  connect(socket[i],SIGNAL(connectionClosedID(int)),
	  this,SLOT(socketKill(int)));
  LogLine(RDConfig::LogDebug,"rdcatchd new connection open");
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
	  catch_cae->
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
  //  bool waiting=false;

  catch_events[event].setStatus(RDDeck::Idle);
  for(unsigned i=0;i<catch_events.size();i++) {
    if((catch_events[i].status()==RDDeck::Waiting)&&
       ((catch_events[i].channel()-1)==deck)) {
      //      waiting=true;
    }
  }
  WriteExitCodeById(id,RDRecording::Ok);
  catch_record_deck_status[deck]=RDDeck::Idle;
  catch_record_id[deck]=0;
  BroadcastCommand(QString().sprintf("RE %d %d %d!",
				     deck+1,catch_record_deck_status[deck],
				     id));
  LogLine(RDConfig::LogInfo,QString().
	  sprintf("gpi start window closes: event: %d, gpi: %d:%d",
		  id,catch_events[event].startMatrix(),
		  catch_events[event].startLine()));
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
    LogLine(RDConfig::LogDebug,QString().
	    sprintf("cannot find event %d, ignoring!",id));
    return;
  }
  if(!catch_events[event].isActive()) {
    LogLine(RDConfig::LogDebug,QString().
	    sprintf("event %d is marked inactive, ignoring",id));
    return;
  }
  if(!catch_events[event].dayOfWeek(date.dayOfWeek())) {
    LogLine(RDConfig::LogDebug,QString().
	    sprintf("event %d is not valid for this DOW, ignoring",
			      id));
    return;
  }

  //
  // Check for Playout Deck Availability
  //
  if(catch_events[event].type()==RDRecording::Playout) {
    if(catch_playout_deck_status[catch_events[event].channel()-129]!=
       RDDeck::Idle) {
      LogLine(RDConfig::LogWarning,QString().
	      sprintf("playout deck P%d is busy for event %d, skipping",
		      catch_events[event].channel()-128,
		      catch_events[event].id()));
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
	  LogLine(RDConfig::LogWarning,QString().
		  sprintf("record aborted: no such cut: %s, id: %d",
			  (const char *)catch_events[event].cutName(),
			  catch_events[event].id()));
	  return;
	}
	catch_record_card[catch_events[event].channel()-1]=-1;
	catch_record_stream[catch_events[event].channel()-1]=-1;
	sql=QString().sprintf("select CARD_NUMBER,PORT_NUMBER,\
                         SWITCH_STATION,SWITCH_MATRIX,SWITCH_OUTPUT,\
                         SWITCH_DELAY from DECKS \
                         where (STATION_NAME=\"%s\")&&(CHANNEL=%d)",
			      (const char *)catch_config->stationName(),
			      catch_events[event].channel());
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
	  LogLine(RDConfig::LogNotice,QString().
		  sprintf("id %d specified non-existent record deck, ignored",
			  catch_events[event].id()));
	  delete q;
	  return;
	}
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
	      LogLine(RDConfig::LogNotice,QString().
		      sprintf("gpi start window opens: event: %d, gpi: %d:%d",
			      id,catch_events[event].startMatrix(),
			      catch_events[event].startLine()));
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
	  LogLine(RDConfig::LogNotice,QString().
		  sprintf("playout aborted: no such cut: %s, id: %d",
			  (const char *)catch_events[event].cutName(),
			  catch_events[event].id()));
	  return;
	}
	catch_playout_card[catch_events[event].channel()-129]=-1;
	catch_playout_stream[catch_events[event].channel()-129]=-1;
	sql=QString().sprintf("select CARD_NUMBER,PORT_NUMBER,PORT_NUMBER \
                         from DECKS where (STATION_NAME=\"%s\")&&(CHANNEL=%d)",
			      (const char *)catch_config->stationName(),
			      catch_events[event].channel());
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
	  LogLine(RDConfig::LogDebug,QString().
		  sprintf("id %d specified non-existent play deck, ignored",
			  catch_events[event].id()));
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
	  LogLine(RDConfig::LogDebug,
		  QString().sprintf("macro aborted: no such cart: %u, id: %d",
				    catch_events[event].macroCart(),
				    catch_events[event].id()));
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
	  LogLine(RDConfig::LogDebug,QString().
		  sprintf("download aborted: no such cut: %s, id: %d",
			  (const char *)catch_events[event].cutName(),
			  catch_events[event].id()));
	  return;
	}

	//
	// Load Import Parameters
	//
	sql=QString().sprintf("select DEFAULT_FORMAT,DEFAULT_CHANNELS,\
                               DEFAULT_SAMPRATE,DEFAULT_LAYER,DEFAULT_BITRATE,\
                               RIPPER_LEVEL\
                               from RDLIBRARY where STATION=\"%s\"",
			      (const char *)catch_config->stationName());
	q=new RDSqlQuery(sql);
	if(q->first())
	{
	  catch_default_format=q->value(0).toInt();
	  catch_default_channels=q->value(1).toInt();
	  catch_default_samplerate=q->value(2).toInt();
	  catch_default_layer=q->value(3).toInt();
	  catch_default_bitrate=q->value(4).toInt();
	  catch_ripper_level=q->value(5).toInt();
	}
	else {
	  LogLine(RDConfig::LogWarning,
		  "unable to load import audio configuration");
	  delete q;
	  return;
	}
	delete q;
	catch_events[event].
	  setResolvedUrl(RDDateTimeDecode(catch_events[event].url(),
	       QDateTime(date.addDays(catch_events[event].eventdateOffset()),
	       current_time)));
	StartDownloadEvent(event);
	break;

      case RDRecording::Upload:
	if(!RDCut::exists(catch_events[event].cutName())) {
	  WriteExitCode(event,RDRecording::NoCut);
	  BroadcastCommand(QString().
			   sprintf("RE 0 0 %d!",catch_events[event].id()));
	  LogLine(RDConfig::LogNotice,QString().
		  sprintf("upload aborted: no such cut: %s, id: %d",
			  (const char *)catch_events[event].cutName(),
			  catch_events[event].id()));
	  return;
	}
	StartUploadEvent(event);
	break;
  }
}


void MainObject::socketData(int ch)
{
  ParseCommand(ch);
}


void MainObject::socketKill(int ch)
{
  KillSocket(ch);
}


void MainObject::isConnectedData(bool state)
{
  if(!state) {
    LogLine(RDConfig::LogErr,
	    "aborting - unable to connect to Core AudioEngine");
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
  LogLine(RDConfig::LogDebug,QString().
	  sprintf("Loaded - Card: %d  Stream: %d\n",card,stream));
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
  catch_cae->unloadRecord(card,stream);
}


void MainObject::recordUnloadedData(int card,int stream,unsigned msecs)
{
  int deck=GetRecordDeck(card,stream);
  if(deck<1) {
    LogLine(RDConfig::LogDebug,QString().
	    sprintf("invalid record deck:  Card: %d  Stream: %d",
		    card,stream));
    return;
  }
  int event=GetEvent(catch_record_id[deck-1]);
  if(event<0) {
    catch_record_deck_status[deck-1]=RDDeck::Idle;
    catch_record_aborting[deck-1]=false;
    LogLine(RDConfig::LogDebug,QString().
	    sprintf("invalid record event:  Id: %d",
		    catch_record_id[deck-1]));
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
    LogLine(RDConfig::LogNotice,QString().
	    sprintf("record aborted: cut %s",
		    (const char *)catch_record_name[deck-1]));
    WriteExitCodeById(catch_record_id[deck-1],RDRecording::Interrupted);
  }
  else {
    LogLine(RDConfig::LogInfo,QString().
	    sprintf("record complete: cut %s",
		    (const char *)catch_record_name[deck-1]));
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
  LogLine(RDConfig::LogNotice,QString().
	  sprintf("playout stopped: cut %s",
		  (const char *)catch_playout_name[deck-129]));
  if(debug) {
    printf("Playout stopped - Card: %d  Stream: %d\n",
	   catch_playout_card[deck-129],
	   catch_playout_stream[deck-129]);
  }
  SendMeterLevel(deck,levels);
  catch_cae->unloadPlay(handle);
}


void MainObject::playUnloadedData(int handle)
{
  int deck=GetPlayoutDeck(handle);

  LogLine(RDConfig::LogInfo,QString().
	  sprintf("play complete: cut %s",
		  (const char *)catch_playout_name[deck-129]));
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


void MainObject::meterData()
{
  short levels[2];

  for(int i=0;i<MAX_DECKS;i++) {
    if(catch_record_deck_status[i]==RDDeck::Recording) {
      catch_cae->inputMeterUpdate(catch_record_card[i],catch_record_stream[i],
				  levels);
      SendMeterLevel(i+1,levels);
    }
    if(catch_playout_deck_status[i]==RDDeck::Recording) {
      catch_cae->
	outputMeterUpdate(catch_playout_card[i],catch_playout_port[i],
				  levels);
      SendMeterLevel(i+129,levels);
    }
  }
}


void MainObject::eventFinishedData(int id)
{
  LogLine(RDConfig::LogDebug,QString().sprintf("eventFinishedData(%d)",id));
  if(catch_macro_event_id[id]>=0) {
    LogLine(RDConfig::LogDebug,QString().
	    sprintf("Clearing event_id: %d",catch_macro_event_id[id]));
    if(catch_macro_event_id[id]<RDCATCHD_ERROR_ID_OFFSET) {
      int event=GetEvent(catch_macro_event_id[id]);
      if(event<0) {
	LogLine(RDConfig::LogDebug,
		"rdcatchd: processed eventFinishedData for unknown ID");
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
  vector<int>::iterator it;
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
  unsigned cartnum=catch_rdstation->startupCart();
  if(cartnum>0) {
    RDCart *cart=new RDCart(cartnum);
    if(cart->exists()) {
      ExecuteMacroCart(cart);
      LogLine(RDConfig::LogInfo,QString().
	      sprintf("ran startup cart %06u",cartnum));
    }
    else {
      LogLine(RDConfig::LogNotice,QString().
	      sprintf("startup cart %06u was invalid",cartnum));
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
    LogLine(RDConfig::LogDebug,"invalid event offset received, ignored");
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
    LogLine(RDConfig::LogWarning,QString().
	    sprintf("invalid audio device for deck: %d, event: %d",
		    deck,catch_events[event].id()));
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
    LogLine(RDConfig::LogNotice,QString().
	    sprintf("device busy for deck: %d, event: %d | in use by event: %d",
		    deck,catch_events[event].id(),catch_record_id[deck-1]));
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
    rml->setArgQuantity(3);
    rml->setCommand(RDMacro::ST);
    rml->setArg(0,catch_swmatrix[deck-1]);
    rml->setArg(1,catch_events[event].switchInput());
    rml->setArg(2,catch_swoutput[deck-1]);
    char str[RD_RML_MAX_LENGTH];
    if(rml->generateString(str,RD_RML_MAX_LENGTH)) {
      catch_ripc->sendRml(rml);
      LogLine(RDConfig::LogDebug,QString().
	      sprintf("sending switcher command: %s",str));
    }
    else {
      LogLine(RDConfig::LogNotice,"switcher command is malformed!");
    }
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
    format=RDCae::Pcm16;
  }    

  //
  // Start the recording
  //
  catch_cae->loadRecord(catch_record_card[deck-1],
			catch_record_stream[deck-1],
			cut_name,
			format,
			catch_events[event].channels(),
			catch_events[event].sampleRate(),
			catch_events[event].bitrate());
  catch_cae->record(catch_record_card[deck-1],catch_record_stream[deck-1],
		    length,0);
  catch_events[event].setStatus(RDDeck::Recording);

  str=QString().sprintf("record started: deck: %d, event: %d",
			deck,catch_events[event].id());
  str+=QString().sprintf(" card: %d, stream: %d, cut: %s length: %d",
			 catch_record_card[deck-1],
			 catch_record_stream[deck-1],
			 (const char *)cut_name,length);
  LogLine(RDConfig::LogInfo,str);

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
  cut->setOriginName(catch_config->stationName());
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
    datetime.setDate(QDate::currentDate().
		     addDays(catch_events[event].startdateOffset()));
    cut->setStartDatetime(datetime,true);
    datetime=cut->endDatetime(&valid);
    datetime.setDate(QDate::currentDate().
		     addDays(catch_events[event].enddateOffset()));
    cut->setEndDatetime(datetime,true);
  }
  delete cut;

  return true;
}


void MainObject::StartPlayout(int event)
{
  unsigned deck=catch_events[event].channel();
  if((catch_playout_card[deck-129]<0)) {
    LogLine(RDConfig::LogWarning,QString().
	    sprintf("invalid audio device for deck: %d, event: %d",
		    deck,catch_events[event].id()));
    return;
  }

  //
  // Get cut parameters
  //
  QString sql=QString().sprintf("select START_POINT,END_POINT from CUTS\
                                 where CUT_NAME=\"%s\"",
				(const char *)catch_events[event].cutName());
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
  catch_cae->loadPlay(catch_playout_card[deck-129],
		      catch_events[event].cutName(),
		      &catch_playout_stream[deck-129],
		      &catch_playout_handle[deck-129]);
  RDSetMixerOutputPort(catch_cae,catch_playout_card[deck-129],
		       catch_playout_stream[deck-129],
		       catch_playout_port[deck-129]);
  catch_cae->positionPlay(catch_playout_handle[deck-129],start);
  catch_cae->
    play(catch_playout_handle[deck-129],end-start,RD_TIMESCALE_DIVISOR,0);
  catch_cae->setPlayPortActive(catch_playout_card[deck-129],
			       catch_playout_port[deck-129],
			       catch_playout_stream[deck-129]);
  catch_events[event].setStatus(RDDeck::Recording);

  LogLine(RDConfig::LogDebug,QString().
	  sprintf("playout started: deck: %d, event %d",
		  deck,catch_events[event].id()));
  LogLine(RDConfig::LogDebug,QString().
	  sprintf("  card %d, stream %d , cut=%s",
		  catch_playout_card[deck-129],
		  catch_playout_stream[deck-129],
		  (const char *)catch_events[event].cutName()));

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
    LogLine(RDConfig::LogWarning,QString().
	    sprintf("cart %u does not exist!",
		    catch_events[event].macroCart()));
    delete cart;
    return;
  }
  if(cart->type()!=RDCart::Macro) {
    LogLine(RDConfig::LogWarning,QString().
	    sprintf("%u is not a macro cart!",
		    catch_events[event].macroCart()));
    delete cart;
    return;
  }
  if(ExecuteMacroCart(cart,catch_events[event].id(),event)) {
    LogLine(RDConfig::LogInfo,QString().
	  sprintf("executing macro cart: %u",catch_events[event].macroCart()));
  }
  delete cart;
}


void MainObject::StartSwitchEvent(int event)
{
  char cmd[RD_RML_MAX_LENGTH];

  RDMacro *rml=new RDMacro();
  rml->setAddress(catch_rdstation->address());
  rml->setRole(RDMacro::Cmd);
  rml->setEchoRequested(false);
  rml->setCommand(RDMacro::ST);
  rml->setArgQuantity(3);
  rml->setArg(0,catch_events[event].channel());
  rml->setArg(1,catch_events[event].switchInput());
  rml->setArg(2,catch_events[event].switchOutput());
  rml->generateString(cmd,RD_RML_MAX_LENGTH);
  LogLine(RDConfig::LogInfo,QString().
	  sprintf("sent switch event, rml: %s",cmd));
  catch_ripc->sendRml(rml);
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
    LogLine(RDConfig::LogWarning,"unable to allocate event context!");
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
    new RDMacroEvent(catch_rdstation->address(),catch_ripc,this,"event");
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
  for(unsigned i=0;i<RDCATCHD_MAX_CONNECTIONS;i++) {
    if(catch_meter_enabled[i]) {
      EchoCommand(i,QString().sprintf("RM %d 0 %d!",deck,(int)levels[0]));
      EchoCommand(i,QString().sprintf("RM %d 1 %d!",deck,(int)levels[1]));
    }
  }
}


void MainObject::ParseCommand(int ch)
{
  char buf[256];
  static int c;

  while((c=socket[ch]->readBlock(buf,256))>0) {
    buf[c]=0;
    for(int i=0;i<c;i++) {
      if(buf[i]==' ') {
	if(argnum[ch]<RDCATCHD_MAX_ARGS) {
	  args[ch][argnum[ch]][argptr[ch]]=0;
	  argnum[ch]++;
	  argptr[ch]=0;
	}
	else {
	  if(debug) {
	    printf("Argument list truncated!\n");
	  }
	}
      }
      if(buf[i]=='!') {
	args[ch][argnum[ch]++][argptr[ch]]=0;
	DispatchCommand(ch);
	argnum[ch]=0;
	argptr[ch]=0;
	if(socket[ch]==NULL) {
	  return;
	}
      }
      if((isgraph(buf[i]))&&(buf[i]!='!')) {
	if(argptr[ch]<RDCATCHD_MAX_LENGTH) {
	  args[ch][argnum[ch]][argptr[ch]]=buf[i];
	  argptr[ch]++;
	}
	else {
	  if(debug) {
	    LogLine(RDConfig::LogWarning,"parser arguments truncated");
	  }
	}
      }
    }
  }
}


void MainObject::DispatchCommand(int ch)
{
  int chan;
  int id;
  int event;
  int code;
  QString str;

  /*
  printf("RDCATCHD Received:");
  for(int i=0;i<argnum[ch];i++) {
    printf(" %s",args[ch][i]);
  }
  printf("\n");
  */

  //
  // Common Commands
  // Authentication not required to execute these!
  //
  if(!strcmp(args[ch][0],"DC")) {  // Drop Connection
    socket[ch]->close();
    KillSocket(ch);
    return;
  }
  if(!strcmp(args[ch][0],"PW")) {  // Password Authenticate
    if(!strcmp(args[ch][1],catch_config->password())) {
      auth[ch]=true;
      EchoCommand(ch,"PW +!");
      return;
    }
    else {
      auth[ch]=false;
      EchoCommand(ch,"PW -!");
      return;
    }
  }

  //
  // Priviledged Commands
  // Authentication required to execute these!
  //
  if(!auth[ch]) {
    EchoArgs(ch,'-');
    return;
  }

  if(!strcmp(args[ch][0],"RS")) {  // Reset
    EchoArgs(ch,'+');
    LoadEngine();
  }

  if(!strcmp(args[ch][0],"RA")) {  // Add Event
    if(AddEvent(QString(args[ch][1]).toInt())) {
      EchoArgs(ch,'+');
      BroadcastCommand(QString().sprintf("RU %s!",args[ch][1]),ch);
    }
    else {
      EchoArgs(ch,'-');
    }
  }

  if(!strcmp(args[ch][0],"RR")) {  // Remove Event
    RemoveEvent(QString(args[ch][1]).toInt());
    EchoArgs(ch,'+');
    BroadcastCommand(QString().sprintf("RU %s!",args[ch][1]),ch);
  }

  if(!strcmp(args[ch][0],"RU")) {  // Update Event
    if(UpdateEvent(QString(args[ch][1]).toInt())) {
      EchoArgs(ch,'+');
    }
    else {
      EchoArgs(ch,'-');
    }
  }

  if(!strcmp(args[ch][0],"RD")) {  // Load Deck List
    EchoArgs(ch,'+');
    LoadDeckList();
  }

  if(!strcmp(args[ch][0],"RO")) {  // Reload Time Offset
    EchoArgs(ch,'+');
    catch_engine->setTimeOffset(catch_rdstation->timeOffset());
  }

  if(!strcmp(args[ch][0],"RE")) {  // Request Status
    if(sscanf(args[ch][1],"%u",&chan)!=1) {
      EchoArgs(ch,'-');
      return;
    }
    if(chan==0) {
      SendFullStatus(ch);
      return;
    }
    chan--;
    if(chan<MAX_DECKS) {
      if(catch_record_deck_status[chan]==RDDeck::Offline) {
	EchoArgs(ch,'-');
	return;
      }
      EchoCommand(ch,QString().sprintf("RE %u %d %d!",
				       chan+1,catch_record_deck_status[chan],
				       catch_record_id[chan]));
      EchoCommand(ch,QString().sprintf("MN %u %d!",chan+1,
				       catch_monitor_state[chan]));
      return;
    }
    if((chan>=128)&&(chan<(MAX_DECKS+128))) {
      if(catch_playout_deck_status[chan-128]==RDDeck::Offline) {
	EchoArgs(ch,'-');
	return;
      }
      EchoCommand(ch,
		  QString().sprintf("RE %u %d %d!",
				    chan+1,catch_playout_deck_status[chan-128],
				    catch_playout_id[chan-128]));
      return;
    }
    EchoArgs(ch,'-');
    return;
  }

  if(!strcmp(args[ch][0],"RM")) {  // Enable/Disable Metering
    if(!strcmp(args[ch][1],"0")) {  // Disable Metering
      catch_meter_enabled[ch]=false;
    }
    if(!strcmp(args[ch][1],"1")) {  // Enable Metering
      catch_meter_enabled[ch]=true;
    }
  }

  if(!strcmp(args[ch][0],"SR")) {  // Stop Recording
    if(sscanf(args[ch][1],"%d",&chan)!=1) {
      return;
    }
    if((chan>0)&&(chan<(MAX_DECKS+1))) {
      switch(catch_record_deck_status[chan-1]) {
	  case RDDeck::Recording:
	    catch_record_aborting[chan-1]=true;
	    catch_cae->stopRecord(catch_record_card[chan-1],
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
	    catch_cae->stopPlay(catch_playout_handle[chan-129]);
	    break;

	  default:
	    break;
      }
    }
  }

  if(!strcmp(args[ch][0],"RH")) {  // Reload Heartbeat Configuration
    LoadHeartbeat();
  }

  if(!strcmp(args[ch][0],"RX")) {  // Restart Dropbox Instances
    StartDropboxes();
  }

  if(!strcmp(args[ch][0],"MN")) {  // Monitor State
    if(sscanf(args[ch][1],"%d",&chan)!=1) {
      return;
    }
    if((chan>0)&&(chan<(MAX_DECKS+1))) {
      if(catch_monitor_port[chan-1]>=0) {
	if(args[ch][2][0]=='1') {
	  catch_cae->setPassthroughVolume(catch_record_card[chan-1],
					  catch_record_stream[chan-1],
					  catch_monitor_port[chan-1],0);
	  catch_monitor_state[chan-1]=true;
	  BroadcastCommand(QString().sprintf("MN %d 1!",chan));
	}
	else {
	  catch_cae->setPassthroughVolume(catch_record_card[chan-1],
					  catch_record_stream[chan-1],
					  catch_monitor_port[chan-1],
					  RD_MUTE_DEPTH);
	  catch_monitor_state[chan-1]=false;
	  BroadcastCommand(QString().sprintf("MN %d 0!",chan));
	}
      }
    }
  }

  if(!strcmp(args[ch][0],"SC")) {  // Set Exit Code
    if(sscanf(args[ch][1],"%d",&id)!=1) {
      return;
    }
    if(sscanf(args[ch][2],"%d",&code)!=1) {
      return;
    }
    str="";
    for(int i=3;i<argnum[ch];i++) {
      str+=QString(args[ch][i])+" ";
      str.stripWhiteSpace();
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


void MainObject::KillSocket(int ch)
{
  istate[ch]=0;
  argnum[ch]=0;
  argptr[ch]=0;
  auth[ch]=false;
  catch_meter_enabled[ch]=false;

  delete socket[ch];
  socket[ch]=NULL;
  LogLine(RDConfig::LogDebug,"rdcatchd dropped connection");
}


void MainObject::EchoCommand(int ch,const char *command)
{
//  LogLine(RDConfig::LogDebug,QString().sprintf("rdcatchd: EchoCommand(%d,%s)",ch,command));
  if(socket[ch]->state()==QSocket::Connection) {
    socket[ch]->writeBlock(command,strlen(command));
  }
}


void MainObject::BroadcastCommand(const char *command,int except_ch)
{
//  LogLine(RDConfig::LogDebug,QString().sprintf("rdcatchd: BroadcastCommand(%s)",command));
  for(unsigned i=0;i<RDCATCHD_MAX_CONNECTIONS;i++) {
    if(socket[i]!=NULL) {
      if((int)i!=except_ch) {
	EchoCommand(i,command);
      }
    }
  }
}


void MainObject::EchoArgs(int ch,const char append)
{
  char command[RDCATCHD_MAX_LENGTH+2];
  int l;

  command[0]=0;
  for(int i=0;i<argnum[ch];i++) {
    strcat(command,args[ch][i]);
    strcat(command," ");
  }
  l=strlen(command);
  command[l]=append;
  command[l+1]='!';
  command[l+2]=0;
  EchoCommand(ch,command);
}


void MainObject::LoadEngine(bool adv_day)
{
  RDSqlQuery *q;
  QString sql;

  catch_events.clear();
  LogLine(RDConfig::LogInfo,"rdcatchd engine load starts...");
  sql=LoadEventSql()+QString().sprintf(" where STATION_NAME=\"%s\"",
				       (const char *)catch_rdstation->name());
  q=new RDSqlQuery(sql);
  while(q->next()) {
    catch_events.push_back(CatchEvent());
    LoadEvent(q,&catch_events.back(),true);
  }
  LogLine(RDConfig::LogInfo,QString().sprintf("loaded %d events",(int)catch_events.size()));
  delete q;
  LogLine(RDConfig::LogInfo,"rdcatchd engine load ends");
}


QString MainObject::LoadEventSql()
{
  return QString("select ID,IS_ACTIVE,TYPE,CHANNEL,CUT_NAME,\
                  SUN,MON,TUE,WED,THU,FRI,SAT,START_TIME,LENGTH,\
                  START_GPI,END_GPI,TRIM_THRESHOLD,STARTDATE_OFFSET,\
                  ENDDATE_OFFSET,FORMAT,CHANNELS,SAMPRATE,BITRATE,\
                  MACRO_CART,SWITCH_INPUT,SWITCH_OUTPUT,ONE_SHOT,\
                  START_TYPE,START_LENGTH,START_MATRIX,START_LINE,\
                  START_OFFSET,END_TYPE,END_TIME,END_LENGTH,\
                  END_MATRIX,END_LINE,URL,URL_USERNAME,URL_PASSWORD,\
                  QUALITY,NORMALIZE_LEVEL,ALLOW_MULT_RECS,\
                  MAX_GPI_REC_LENGTH,DESCRIPTION,FEED_ID,\
                  EVENTDATE_OFFSET,ENABLE_METADATA from RECORDINGS");
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
  e->setFormat((RDCae::AudioCoding)q->value(19).toInt());
  e->setChannels(q->value(20).toInt());
  e->setSampleRate(catch_system->sampleRate());
  //e->setSampleRate(q->value(21).toInt());
  e->setBitrate(q->value(22).toInt());
  e->setMacroCart(q->value(23).toInt());
  e->setSwitchInput(q->value(24).toInt());
  e->setSwitchOutput(q->value(25).toInt());
  e->setStatus(RDDeck::Idle);
  e->setOneShot(RDBool(q->value(26).toString()));
  e->setStartType((RDRecording::StartType)q->value(27).toInt());
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
  QString sql=QString().sprintf("select CHANNEL,CARD_NUMBER,PORT_NUMBER,\
                                 MON_PORT_NUMBER from DECKS \
                                 where (STATION_NAME=\"%s\")&&\
                                 (CARD_NUMBER!=-1)&&(CHANNEL>0)&&(CHANNEL<9)",
				(const char *)catch_config->stationName());
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
	catch_cae->stopRecord(catch_record_card[i],catch_record_stream[i]);
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
  sql=QString().sprintf("select CHANNEL from DECKS \
where (STATION_NAME=\"%s\")&&(CARD_NUMBER!=-1)&&(CHANNEL>128)&&(CHANNEL<137)",
				(const char *)catch_config->stationName());
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
	catch_cae->stopPlay(catch_playout_handle[i]);
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
    QString().sprintf(" where (STATION_NAME=\"%s\")&&(ID=%d)",
		      (const char *)catch_rdstation->name(),id);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    catch_events.push_back(CatchEvent());
    LoadEvent(q,&catch_events.back(),true);
    switch((RDRecording::Type)q->value(2).toInt()) {
	case RDRecording::Recording:
	  LogLine(RDConfig::LogNotice,QString().
		  sprintf("loading event %d, Type: recording, Cut: %s",
			  id,(const char *)q->value(4).toString()));
	  break;

	case RDRecording::Playout:
	  LogLine(RDConfig::LogNotice,QString().
		  sprintf("loading event %d, Type: playout, Cut: %s",
			  id,(const char *)q->value(4).toString()));
	  break;

	case RDRecording::MacroEvent:
	  LogLine(RDConfig::LogNotice,QString().
		  sprintf("loading event %d, Type: macro, Cart: %d",
			  id,q->value(23).toUInt()));
	  break;

	case RDRecording::SwitchEvent:
	  LogLine(RDConfig::LogNotice,QString().sprintf(
	    "loading event %d, Type: switch, Matrix: %d, Source: %d  Dest: %d",
			id,q->value(3).toInt(),q->value(24).toInt(),
			q->value(25).toInt()));
	  break;

	case RDRecording::Download:
	  LogLine(RDConfig::LogNotice,QString().
		  sprintf("loading event %d, Type: download, Cut: %s",
			  id,(const char *)q->value(4).toString()));
	  break;

	case RDRecording::Upload:
	  LogLine(RDConfig::LogNotice,QString().
		  sprintf("loading event %d, Type: upload, Cut: %s",
			  id,(const char *)q->value(4).toString()));
	  break;
    }
    delete q;
    return true;
  }
  LogLine(RDConfig::LogWarning,QString().sprintf("event %d not found, not loaded",id));
  delete q;
  return false;
}


void MainObject::RemoveEvent(int id)
{
  int event=GetEvent(id);
  if(event<0) {
    LogLine(RDConfig::LogNotice,QString().
	    sprintf("event %d not found, not removed",id));
    return;
  }
  switch(catch_events[event].type()) {
      case RDRecording::Recording:
	LogLine(RDConfig::LogDebug,QString().
		sprintf("removed event %d, Type: recording, Cut: %s",
			id,(const char *)catch_events[event].cutName()));
	break;
	
      case RDRecording::Playout:
	LogLine(RDConfig::LogDebug,QString().
		sprintf("removed event %d, Type: playout, Cut: %s",
			id,
			(const char *)catch_events[event].cutName()));
	break;
	
      case RDRecording::MacroEvent:
	LogLine(RDConfig::LogDebug,QString().
		sprintf("removed event %d, Type: macro, Cart: %u",
			id,
			catch_events[event].macroCart()));
	break;
	
      case RDRecording::SwitchEvent:
	LogLine(RDConfig::LogDebug,QString().sprintf(
	   "removed event %d, Type: switch, Matrix: %d, Source: %d  Dest: %d",
		  id,
		  catch_events[event].channel(),
		  catch_events[event].switchInput(),
		  catch_events[event].switchOutput()));
	break;
	
      case RDRecording::Download:
	LogLine(RDConfig::LogDebug,QString().
		sprintf("removed event %d, Type: download, Cut: %s",
			id,(const char *)catch_events[event].cutName()));
	break;
	
      case RDRecording::Upload:
	LogLine(RDConfig::LogDebug,QString().
		sprintf("removed event %d, Type: upload, Cut: %s",
			id,(const char *)catch_events[event].cutName()));
	break;
	
  }
  vector<CatchEvent>::iterator it=catch_events.begin()+event;
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
	  LogLine(RDConfig::LogDebug,QString().
		  sprintf("purged event %d, Type: recording, Cut: %s",
			  catch_events[event].id(),
			  (const char *)catch_events[event].cutName()));
	  break;

	case RDRecording::Playout:
	  LogLine(RDConfig::LogDebug,QString().
		  sprintf("purged event %d, Type: playout, Cut: %s",
			  catch_events[event].id(),
			  (const char *)catch_events[event].cutName()));
	  break;

	case RDRecording::MacroEvent:
	  LogLine(RDConfig::LogDebug,QString().
		  sprintf("purged event %d, Type: macro, Cart: %u",
			  catch_events[event].id(),
			  catch_events[event].macroCart()));
	  break;

	case RDRecording::SwitchEvent:
	  LogLine(RDConfig::LogDebug,QString().sprintf(
	    "purged event %d, Type: switch, Matrix: %d, Source: %d  Dest: %d",
	    catch_events[event].id(),
	    catch_events[event].channel(),
	    catch_events[event].switchInput(),
	    catch_events[event].switchOutput()));
	  break;

	case RDRecording::Download:
	  LogLine(RDConfig::LogDebug,QString().
		  sprintf("purged event %d, Type: download, Cut: %s",
			  catch_events[event].id(),
			  (const char *)catch_events[event].cutName()));
	  break;

	case RDRecording::Upload:
	  LogLine(RDConfig::LogDebug,QString().
		  sprintf("purged event %d, Type: upload, Cut: %s",
			  catch_events[event].id(),
			  (const char *)catch_events[event].cutName()));
	  break;
  }
  catch_engine->removeEvent(catch_events[event].id());
  vector<CatchEvent>::iterator it=catch_events.begin()+event;
  catch_events.erase(it,it+1);
}


void MainObject::LoadHeartbeat()
{
  if(catch_heartbeat_timer->isActive()) {
    catch_heartbeat_timer->stop();
  }
  QString sql=QString().sprintf("select HEARTBEAT_CART,HEARTBEAT_INTERVAL\
                                 from STATIONS where NAME=\"%s\"",
				(const char *)catch_rdstation->name());
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
  cut->checkInRecording(catch_config->stationName(),s,msecs);
  delete s;
  cut->autoTrim(RDCut::AudioBoth,-threshold);
  RDCart *cart=new RDCart(cut->cartNumber());
  cart->updateLength();
  delete cart;
  delete cut;
  chown(RDCut::pathName(cutname),catch_config->uid(),catch_config->gid());
}


void MainObject::CheckInPodcast(CatchEvent *e) const
{
  QString sql;
  RDSqlQuery *q;

  //
  // Purge Stale Casts
  //
  sql=QString().sprintf("delete from PODCASTS where \
                         (FEED_ID=%d)&&(AUDIO_FILENAME=\"%s\")",
			e->feedId(),
			(const char *)RDGetBasePart(e->resolvedUrl()));
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Get Channel Parameters
  //
  sql=QString().sprintf("select ENABLE_AUTOPOST,CHANNEL_TITLE,\
                         CHANNEL_DESCRIPTION,CHANNEL_CATEGORY,\
                         CHANNEL_LINK,MAX_SHELF_LIFE from FEEDS \
                         where ID=%u",e->feedId());
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
  sql=QString().sprintf("insert into PODCASTS set \
                         FEED_ID=%u,\
                         STATUS=%u,\
                         ITEM_TITLE=\"%s\",\
                         ITEM_DESCRIPTION=\"%s\",\
                         ITEM_CATEGORY=\"%s\",\
                         ITEM_LINK=\"%s\",\
                         AUDIO_FILENAME=\"%s\",\
                         AUDIO_LENGTH=%u,\
                         AUDIO_TIME=%u,\
                         SHELF_LIFE=%u,\
                         EFFECTIVE_DATETIME=now(),\
                         ORIGIN_DATETIME=now()",
			  e->feedId(),
			  status,
			  (const char *)RDEscapeString(q->value(1).toString()),
			  (const char *)RDEscapeString(q->value(2).toString()),
			  (const char *)RDEscapeString(q->value(3).toString()),
			  (const char *)RDEscapeString(q->value(4).toString()),
			  (const char *)RDGetBasePart(e->resolvedUrl()),
			  e->podcastLength(),
			  e->podcastTime(),
			  q->value(5).toUInt());
  delete q;
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Update the Build Date
  //
  sql=QString().sprintf("update FEEDS set LAST_BUILD_DATETIME=now() \
                           where ID=%u",e->feedId());
  q=new RDSqlQuery(sql);
  delete q;
}


RDRecording::ExitCode MainObject::ReadExitCode(int event)
{
  RDRecording::ExitCode code=RDRecording::InternalError;
  QString sql=QString().sprintf("select EXIT_CODE from RECORDINGS\
                                 where ID=%d",catch_events[event].id());
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
  QString sql=QString().sprintf("update RECORDINGS set EXIT_CODE=%d,\
                                 EXIT_TEXT=\"%s\"\
                                 where ID=%d",code,
				(const char *)RDEscapeString(err_text),
				catch_events[event].id());
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
  QString sql=QString().sprintf("update RECORDINGS set EXIT_CODE=%d,\
                                 EXIT_TEXT=\"%s\" \
                                 where ID=%d",code,
				(const char *)RDEscapeString(err_text),
				id);
  RDSqlQuery *q=new RDSqlQuery(sql);
  delete q;
}


QString MainObject::BuildTempName(int event,QString str)
{
  return BuildTempName(&catch_events[event],str);
}


QString MainObject::BuildTempName(CatchEvent *evt,QString str)
{
  return QString().sprintf("%s/rdcatchd-%s-%d.%s",(const char *)catch_temp_dir,
	 (const char *)str,evt->id(),
	 (const char *)GetFileExtension(evt->resolvedUrl()));
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
  LogLine(RDConfig::LogDebug,"rdcatchd saveState(), set restart hint to Never");
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
    LogLine(RDConfig::LogWarning,"unable to allocate event context!");
  }
  catch_macro_event_id[event_id]=event->id()+RDCATCHD_ERROR_ID_OFFSET;
  catch_event_pool[event_id]=
    new RDMacroEvent(catch_rdstation->address(),catch_ripc,this,"event");
  catch_event_mapper->setMapping(catch_event_pool[event_id],event_id);
  connect(catch_event_pool[event_id],SIGNAL(finished()),
	  catch_event_mapper,SLOT(map()));
  bool res=catch_event_pool[event_id]->load(rml);
  catch_event_pool[event_id]->exec();
  LogLine(RDConfig::LogNotice,QString().
	  sprintf("executed error rml: id=%d, rml=\"%s\", res=%d",
		  event->id(),(const char *)rml,res));
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
  }
}


void MainObject::StartDropboxes()
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;

  //
  // Kill Old Instances
  //
  system("killall rdimport");

  //
  // Launch Dropbox Configurations
  //
  sql=QString("select ID,GROUP_NAME,PATH,NORMALIZATION_LEVEL,")+
    "AUTOTRIM_LEVEL,TO_CART,USE_CARTCHUNK_ID,TITLE_FROM_CARTCHUNK_ID,"+
    "DELETE_CUTS,METADATA_PATTERN,FIX_BROKEN_FORMATS,LOG_PATH,"+
    "DELETE_SOURCE,STARTDATE_OFFSET,ENDDATE_OFFSET,ID,"+
    "IMPORT_CREATE_DATES,CREATE_STARTDATE_OFFSET,"+
    "CREATE_ENDDATE_OFFSET,SET_USER_DEFINED "+
    "from DROPBOXES where STATION_NAME=\""+
    RDEscapeString(catch_config->stationName())+"\"";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    QString cmd=QString().
      sprintf("nice rdimport --persistent-dropbox-id=%d --drop-box --log-mode",
	      q->value(15).toInt());
    sql=QString("select SCHED_CODE from DROPBOX_SCHED_CODES where ")+
      QString().sprintf("DROPBOX_ID=%d",q->value(0).toInt());
    q1=new RDSqlQuery(sql);
    while(q1->next()) {
      cmd+=QString(" --add-scheduler-code=\"")+q1->value(0).toString()+"\"";
    }
    delete q1;
    cmd+=
      QString().sprintf(" --normalization-level=%d",q->value(3).toInt()/100);
    cmd+=
      QString().sprintf(" --autotrim-level=%d",q->value(4).toInt()/100);
    if(q->value(5).toUInt()>0) {
      cmd+=QString().sprintf(" --to-cart=%u",q->value(5).toUInt());
    }
    if(q->value(6).toString()=="Y") {
      cmd+=" --use-cartchunk-cutid";
    }
    if(q->value(7).toString()=="Y") {
      cmd+=" --title-from-cartchunk-cutid";
    }
    if(q->value(8).toString()=="Y") {
      cmd+=" --delete-cuts";
    }
    if(!q->value(9).toString().isEmpty()) {
      cmd+=QString().sprintf(" \"--metadata-pattern=%s\"",
			     (const char *)q->value(9).toString());
    }
    if(q->value(10).toString()=="Y") {
      cmd+=" --fix-broken-formats";
    }
    if(q->value(12).toString()=="Y") {
      cmd+=" --delete-source";
    }
    if(q->value(16).toString()=="Y") {
      cmd+=QString().sprintf(" --create-startdate-offset=%d",
			     q->value(17).toInt());
      cmd+=QString().sprintf(" --create-enddate-offset=%d",
			     q->value(18).toInt());
    }
    if(!q->value(19).toString().isEmpty()) {
      cmd+=" --set-user-defined="+RDEscapeString(q->value(19).toString());
    }
    cmd+=QString().sprintf(" --startdate-offset=%d",q->value(13).toInt());
    cmd+=QString().sprintf(" --enddate-offset=%d",q->value(14).toInt());
    cmd+=QString().sprintf(" %s \"%s\"",(const char *)q->value(1).toString(),
			   (const char *)q->value(2).toString());
    if(!q->value(11).toString().isEmpty()) {
      cmd+=QString().sprintf(" >> %s 2>> %s",
			     (const char *)q->value(11).toString(),
			     (const char *)q->value(11).toString());
    }
    else {
      cmd+=" > /dev/null 2> /dev/null";
    }
    cmd+=" &";
    LogLine(RDConfig::LogInfo,QString().
	    sprintf("launching dropbox configuration: \"%s\"",
		    (const char *)cmd));
    if(fork()==0) {
      system(cmd);
      exit(0);
    }
  }
  delete q;
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
  RDDeck *deck=new RDDeck(catch_config->stationName(),chan);
  RDCut *cut=new RDCut(cartnum,cutnum);
  QDateTime dt=QDateTime(QDate::currentDate(),QTime::currentTime());
  catch_events.push_back(CatchEvent());
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
  catch_events.back().setSampleRate(catch_system->sampleRate());
  catch_events.back().setBitrate(deck->defaultBitrate());
  catch_events.back().setNormalizeLevel(0);
  StartRecording(catch_events.size()-1);
  delete cut;
  delete deck;
}


void MainObject::StartBatch(int id)
{
  if((fork())==0) {
    QString bin=QString(RD_PREFIX)+"/"+"bin/rdcatchd";
    execl(bin,(const char *)bin,
	  (const char *)QString().sprintf("--event-id=%d",id),
	  (char *)NULL);
    LogLine(RDConfig::LogErr,QString().
	    sprintf("failed to exec %s --event-id=%d: %s",(const char *)bin,
		    id,strerror(errno)));
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
  new MainObject(NULL,"main");
  return a.exec();
}
