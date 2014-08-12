// cae.cpp
//
// The Core Audio Engine component of Rivendell
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: cae.cpp,v 1.115.4.3 2012/11/30 16:14:57 cvs Exp $
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


#include <qapplication.h>
#include <qobject.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <errno.h>
#include <dlfcn.h>

#include <qdir.h>
#include <qsqldatabase.h>
#include <qsessionmanager.h>

#include <rdsocket.h>
#include <rdconf.h>
#include <rdcheck_daemons.h>
#include <rddebug.h>
#include <rdcmd_switch.h>
#include <rdsystem.h>

#include <cae_socket.h>
#include <cae.h>

volatile bool exiting=false;
RDConfig *rd_config;
#ifdef JACK
extern jack_client_t *jack_client;
#endif  // JACK

#define PRINT_COMMANDS

void LogLine(RDConfig::LogPriority prio,const QString &line)
{
  FILE *file;

  rd_config->log("caed",prio,line);

  if(rd_config->caeLogfile().isEmpty()) {
    return;
  }

  QDateTime current=QDateTime::currentDateTime();

  file=fopen(rd_config->caeLogfile(),"a");
  if(file==NULL) {
    return;
  }
  chmod(rd_config->caeLogfile(),S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
  fprintf(file,"%02d/%02d/%4d - %02d:%02d:%02d.%03d : %s\n",
	  current.date().month(),
	  current.date().day(),
	  current.date().year(),
	  current.time().hour(),
	  current.time().minute(),
	  current.time().second(),
	  current.time().msec(),
	  (const char *)line);
  fclose(file);
}


void SigHandler(int signum)
{
  switch(signum) {
      case SIGINT:
      case SIGTERM:
      case SIGHUP:
	exiting=true;
	break;
  }
}


MainObject::MainObject(QObject *parent,const char *name)
  :QObject(parent,name)
{
  //
  // Read Command Options
  //
  RDCmdSwitch *cmd=
    new RDCmdSwitch(qApp->argc(),qApp->argv(),"caed",CAED_USAGE);
  delete cmd;

  //
  // LogLine references rd_config
  // 
  rd_config=new RDConfig(RD_CONF_FILE);
  rd_config->load();

  //
  // Make sure we're the only instance running
  //
  if(CheckDaemon(RD_CAED_PID)) {
    LogLine(RDConfig::LogErr,
	    "ERROR caed aborting - multiple instances not allowed");
    exit(1);
  }

  //
  // Initialize Data Structures
  //
  debug=false;
  twolame_handle=NULL;
  mad_handle=NULL;
  if(qApp->argc()>1) {
    for(int i=1;i<qApp->argc();i++) {
      if(!strcmp(qApp->argv()[i],"-d")) {
	debug=true;
      }
    }
  }
  for(int i=0;i<256;i++) {
    play_handle[i].card=-1;
    play_handle[i].stream=-1;
    play_handle[i].owner=-1;
  }
  next_play_handle=0;
  for(int i=0;i<CAE_MAX_CONNECTIONS;i++) {
    socket[i]=NULL;
    meter_port[i]=0;
    istate[i]=0;
    argnum[i]=0;
    argptr[i]=0;
    auth[i]=false;
  }
  for(int i=0;i<RD_MAX_CARDS;i++) {
    cae_driver[i]=RDStation::None;
    for(int j=0;j<RD_MAX_STREAMS;j++) {
      record_length[i][j]=0;
      record_threshold[i][j]=-10000;
      record_owner[i][j]=-1;
      play_owner[i][j]=-1;
      play_length[i][j]=0;
      play_speed[i][j]=100;
      play_pitch[i][j]=false;
      for(int k=0;k<RD_MAX_PORTS;k++) {
	output_status_flag[i][k][j]=false;
      }
#ifdef HAVE_TWOLAME
      twolame_lameopts[i][j]=NULL;
#endif  // HAVE_TWOLAME
#ifdef HAVE_MAD
      mad_active[i][j]=false;
#endif  // HAVE_MAD
    }
  }
#ifdef JACK
  jack_client=NULL;
#endif  // JACK

  server=new CaeSocket(CAED_TCP_PORT,0,this,"socket");
  if(!server->ok()) {
    LogLine(RDConfig::LogErr,"ERROR caed aborting - CaeSocket() server not ok");
    exit(1);
  }
  connect(server,SIGNAL(connection(int)),this,SLOT(newConnection(int)));

  if(!debug) {
    RDDetach(rd_config->logCoreDumpDirectory());
  }

  signal(SIGHUP,SigHandler);
  signal(SIGINT,SigHandler);
  signal(SIGTERM,SigHandler);

  if(!RDWritePid(RD_PID_DIR,"caed.pid",rd_config->uid())) {
    LogLine(RDConfig::LogErr,"can't write pid file");
    fprintf(stderr,"caed: can't write pid file\n");
    exit(1);
  }

  //
  // Allowcate Meter Socket
  //
  meter_socket=new QSocketDevice(QSocketDevice::Datagram);

  //
  // Open Database
  //
  QSqlDatabase *db=QSqlDatabase::addDatabase(rd_config->mysqlDriver());
  if(!db) {
    LogLine(RDConfig::LogErr,"can't open mySQL database");
    fprintf(stderr,"caed: can't open mySQL database");
    exit(1);
  }
  db->setDatabaseName(rd_config->mysqlDbname());
  db->setUserName(rd_config->mysqlUsername());
  db->setPassword(rd_config->mysqlPassword());
  db->setHostName(rd_config->mysqlHostname());
  if(!db->open()) {
    LogLine(RDConfig::LogErr,"unable to connect to mySQL Server");
    printf("caed: unable to connect to mySQL Server");
    db->removeDatabase(rd_config->mysqlDbname());
    exit(1);
  }

  //
  // Start Up the Drivers
  //
  RDStation *station=new RDStation(rd_config->stationName());
  RDSystem *sys=new RDSystem();
  system_sample_rate=sys->sampleRate();
  delete sys;
  hpiInit(station);
  alsaInit(station);
  jackInit(station);
  ClearDriverEntries(station);

  //
  // Probe Capabilities
  //
  ProbeCaps(station);

  //
  // Close Database Connection
  //
  station->setScanned(true);
  delete station;
  db->removeDatabase(rd_config->mysqlDbname());

  //
  // Initialize Mixers
  //
  InitMixers();

  //
  // Meter Update Timer
  //
  QTimer *timer=new QTimer(this,"meter_update_timer");
  connect(timer,SIGNAL(timeout()),this,SLOT(updateMeters()));
  timer->start(RD_METER_UPDATE_INTERVAL);

  //
  // Initialize Thread Priorities
  //
  bool jack_running=false;
  int sched_policy=SCHED_OTHER;
  struct sched_param sched_params;
  int result = 0;
  memset(&sched_params,0,sizeof(struct sched_param));
#ifdef JACK
  if(jack_client!=NULL) {
    pthread_getschedparam(jack_client_thread_id(jack_client),&sched_policy,
			  &sched_params);
#ifdef ALSA
    for(int i=0;i<RD_MAX_CARDS;i++) {
      if(cae_driver[i]==RDStation::Alsa) {
	if (!alsa_play_format[i].exiting) {
	  int r = pthread_setschedparam(alsa_play_format[i].thread,sched_policy,
					&sched_params);
	  if (r) {
	    result = r;
	  }	
	}
	if (!alsa_capture_format[i].exiting) {
	  int r = pthread_setschedparam(alsa_capture_format[i].thread,sched_policy,
					&sched_params);
	  if (r) {
	    result = r;
	  }
	}
      }
    }
#endif  // ALSA
    jack_running=true;
  }
#endif  // JACK
  if(rd_config->useRealtime()) {
    if(!jack_running) {
      sched_params.sched_priority=rd_config->realtimePriority();
    }
    sched_policy=SCHED_FIFO;
#ifdef ALSA
    for(int i=0;i<RD_MAX_CARDS;i++) {
      if(cae_driver[i]==RDStation::Alsa) {
	if (!alsa_play_format[i].exiting) {
	  int r = pthread_setschedparam(alsa_play_format[i].thread,sched_policy,
					&sched_params);
	  if (r) {
	    result = r;
	  }
	}
	if (!alsa_capture_format[i].exiting) {
	  int r = pthread_setschedparam(alsa_capture_format[i].thread,sched_policy,
					&sched_params);
	  if (r) {
	    result = r;
	  }
	}
      }
    }
#endif  // ALSA
    if(sched_params.sched_priority>sched_get_priority_min(sched_policy)) {
      sched_params.sched_priority--;
    }
    int r = pthread_setschedparam(pthread_self(),sched_policy,&sched_params);
    if (r) {
      result = r;
    }
    mlockall(MCL_CURRENT|MCL_FUTURE);
    if (result){
          LogLine(RDConfig::LogWarning,QString().
		  sprintf("Unable to set realtime scheduling: %s",
			  strerror (result)));
    } else {
    LogLine(RDConfig::LogInfo,QString().
	    sprintf("using realtime scheduling, priority=%d",
		    sched_params.sched_priority));
    }
  }

  //
  // Relinquish Root Permissions (if present)
  //
/*
  if(getuid()==0) {
    if(setuid(rd_config->uid())<0) {
      perror("cae");
      exit(1);
    }
//  if(setegid(rd_config->gid())<0) {
//    perror("cae");
//    exit(1);
//  }
  }
*/
  if(rd_config->enableMixerLogging()) {
    LogLine(RDConfig::LogNotice,"mixer logging enabled");
  }
  LogLine(RDConfig::LogInfo,"cae started");
}


MainObject::~MainObject() {
  delete server;
}

void MainObject::newConnection(int fd)
{
  int i=0;

  while((i<CAE_MAX_CONNECTIONS)&&(socket[i]!=NULL)) {
    i++;
  }
  if(i==CAE_MAX_CONNECTIONS) {      // Table full, drop it on the floor
    LogLine(RDConfig::LogErr,"connection refused due to full socket table");
    close(fd);
    return;
  }
  socket[i]=new RDSocket(i,this,"socket_0");
  socket[i]->setSocket(fd);
  connect(socket[i],SIGNAL(readyReadID(int)),this,SLOT(socketData(int)));
  connect(socket[i],SIGNAL(connectionClosedID(int)),
	  this,SLOT(socketKill(int)));
}


void MainObject::socketData(int ch)
{
  ParseCommand(ch);
}


void MainObject::socketKill(int ch)
{
  KillSocket(ch);
}


void MainObject::statePlayUpdate(int card,int stream,int state)
{
  int handle=GetHandle(card,stream);

  if(handle<0) {
    return;
  }
  if(play_owner[card][stream]!=-1) {
    switch(state) {
	case 1:   // Playing
	  EchoCommand(play_owner[card][stream],(const char *)QString().
		      sprintf("PY %d %d %d +!",handle,
			      play_length[card][stream],
			      play_speed[card][stream]));
	  break;
	case 2:   // Paused
	  EchoCommand(play_owner[card][stream],(const char *)QString().
		      sprintf("SP %d +!",handle));
	  break;
	case 0:   // Stopped
	  EchoCommand(play_owner[card][stream],(const char *)QString().
		      sprintf("SP %d +!",handle));
	  break;
    }
  }
}


void MainObject::stateRecordUpdate(int card,int stream,int state)
{
  if(record_owner[card][stream]!=-1) {
    switch(state) {
	case 0:    // Recording
	  EchoCommand(record_owner[card][stream],(const char *)QString().
		      sprintf("RD %d %d %d %d +!",card,stream,
			      record_length[card][stream],
			      record_threshold[card][stream]));
	  break;

	case 4:    // Record Started
	  EchoCommand(record_owner[card][stream],(const char *)QString().
		      sprintf("RS %d %d +!",card,stream));
	  break;

	case 2:    // Paused
	case 3:    // Stopped
	  EchoCommand(record_owner[card][stream],(const char *)QString().
		      sprintf("SR %d %d +!",card,stream));
	  break;
    }
  }
}


void MainObject::updateMeters()
{
  short levels[2];
  unsigned positions[RD_MAX_STREAMS];

  if(exiting) {
    jackFree();
    alsaFree();
    hpiFree();
    RDDeletePid(RD_PID_DIR,"caed.pid");
    LogLine(RDConfig::LogInfo,"cae exiting");
    exit(0);
  }

  AlsaClock();
  JackClock();

  for(int i=0;i<RD_MAX_CARDS;i++) {
    switch(cae_driver[i]) {
	case RDStation::Hpi:
	  for(int j=0;j<RD_MAX_PORTS;j++) {
	    if(hpiGetInputStatus(i,j)!=port_status[i][j]) {
	      port_status[i][j]=hpiGetInputStatus(i,j);
	      if(port_status[i][j]) {
		BroadcastCommand(QString().sprintf("IS %d %d 0!",i,j));
	      }
	      else {
		BroadcastCommand(QString().sprintf("IS %d %d 1!",i,j));
	      }
	    }
	    if(hpiGetInputMeters(i,j,levels)) {
	      SendMeterLevelUpdate("I",i,j,levels);
	    }
	    if(hpiGetOutputMeters(i,j,levels)) {
	      SendMeterLevelUpdate("O",i,j,levels);
	    }      
	  }
	  hpiGetOutputPosition(i,positions);
	  SendMeterPositionUpdate(i,positions);
	  for(int j=0;j<RD_MAX_STREAMS;j++) {
	    if(hpiGetStreamOutputMeters(i,j,levels)) {
	      SendStreamMeterLevelUpdate(i,j,levels);
	    }      
	  }
	  break;

	case RDStation::Jack:
	  for(int j=0;j<RD_MAX_PORTS;j++) {
	    if(jackGetInputStatus(i,j)!=port_status[i][j]) {
	      port_status[i][j]=!port_status[i][j];
	      BroadcastCommand(QString().sprintf("IS %d %d %d",i,j,
						 port_status[i][j]));
	    }
	    if(jackGetInputMeters(i,j,levels)) {
	      SendMeterLevelUpdate("I",i,j,levels);
	    }
	    if(jackGetOutputMeters(i,j,levels)) {
	      SendMeterLevelUpdate("O",i,j,levels);
	    }
	  }
	  jackGetOutputPosition(i,positions);
	  SendMeterPositionUpdate(i,positions);
	  for(int j=0;j<RD_MAX_STREAMS;j++) {
	    if(jackGetStreamOutputMeters(i,j,levels)) {
	      SendStreamMeterLevelUpdate(i,j,levels);
	    }      
	  }
	  break;

	case RDStation::Alsa:
	  for(int j=0;j<RD_MAX_PORTS;j++) {
	    if(alsaGetInputStatus(i,j)!=port_status[i][j]) {
	      port_status[i][j]=!port_status[i][j];
	      BroadcastCommand(QString().sprintf("IS %d %d %d",i,j,
						 port_status[i][j]));
	    }
	    if(alsaGetInputMeters(i,j,levels)) {
	      SendMeterLevelUpdate("I",i,j,levels);
	    }
	    if(alsaGetOutputMeters(i,j,levels)) {
	      SendMeterLevelUpdate("O",i,j,levels);
	    }
	  }
	  alsaGetOutputPosition(i,positions);
	  SendMeterPositionUpdate(i,positions);
	  for(int j=0;j<RD_MAX_STREAMS;j++) {
	    if(alsaGetStreamOutputMeters(i,j,levels)) {
	      SendStreamMeterLevelUpdate(i,j,levels);
	    }      
	  }
	  break;

	case RDStation::None:
	  break;
    }
    //
  }
  //  SendMeterOutputStatusUpdate();
}


void MainObject::InitMixers()
{
  for(int i=0;i<RD_MAX_CARDS;i++) {
    switch(cae_driver[i]) {
	case RDStation::Hpi:
	  for(int j=0;j<RD_MAX_PORTS;j++) {
	    for(int k=0;k<RD_MAX_PORTS;k++) {
	      hpiSetPassthroughLevel(i,j,k,RD_MUTE_DEPTH);
	    }
	  }
	  break;

	case RDStation::Jack:
	  for(int j=0;j<RD_MAX_PORTS;j++) {
	    for(int k=0;k<RD_MAX_PORTS;k++) {
	      jackSetPassthroughLevel(i,j,k,RD_MUTE_DEPTH);
	    }
	  }
	  break;

	case RDStation::Alsa:
	  for(int j=0;j<RD_MAX_PORTS;j++) {
	    for(int k=0;k<RD_MAX_PORTS;k++) {
	      alsaSetPassthroughLevel(i,j,k,RD_MUTE_DEPTH);
	    }
	  }
	  break;

	case RDStation::None:
	  break;
    }
  }
}


void MainObject::ParseCommand(int ch)
{
  char buf[256];
  int c;

  while((c=socket[ch]->readBlock(buf,256))>0) {
    buf[c]=0;
    // LogLine(QString().sprintf("RCVD: %s",buf));
    for(int i=0;i<c;i++) {
      if(buf[i]==' ') {
	if(argnum[ch]<CAE_MAX_ARGS) {
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
	if(argptr[ch]<CAE_MAX_LENGTH) {
	  args[ch][argnum[ch]][argptr[ch]]=buf[i];
	  argptr[ch]++;
	}
	else {
	  if(debug) {
	    printf("WARNING: argument truncated!\n");
	  }
	}
      }
    }
  }
}


void MainObject::DispatchCommand(int ch)
{
  int card=-1;
  int stream=-1;
  int port=0;
  int in_port=0;
  int out_port=0;
  int new_stream=-1;
  int pos=0;
  int coding=0;
  int sample_rate=0;
  int channels=0;
  int bit_rate=0;
  int flag=0;
  int level=0;
  int length=0;
  int mode=0;
  int type=0;
  QString wavename;
  char temp[256];
  int handle;
  QString in_jport;
  QString out_jport;

#ifdef PRINT_COMMANDS
  printf("CAE: connection %d receiving ",ch);
  for(int i=0;i<argnum[ch];i++) {
    printf("%s ",args[ch][i]);
  }
  printf("\n");
#endif  // PRINT_COMMANDS

  //
  // Calculate Card and Stream Numbers
  //
  sscanf(args[ch][1],"%d",&card);
  sscanf(args[ch][2],"%d",&stream);

/*
  printf("RECEIVED:");
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
    if(!strcmp(args[ch][1],rd_config->password())) {
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

  if(!strcmp(args[ch][0],"LP")) {  // Load Playback
    if(card<0) {
      sprintf(temp,"LP %d %s -1 -1 -!",card,args[ch][2]);
      EchoCommand(ch,temp);
      return;
    }
    wavename = rd_config->audioFileName (QString(args[ch][2]));
    switch(cae_driver[card]) {
	case RDStation::Hpi:
	  if(!hpiLoadPlayback(card,wavename,&new_stream)) {
	    sprintf(temp,"LP %d %s -1 -1 -!",card,args[ch][2]);
	    EchoCommand(ch,temp);
	    LogLine(RDConfig::LogErr,
		    QString().sprintf("unable to allocate stream for card %d",
				      card));
	    return;
	  }
	  break;

	case RDStation::Alsa:
	  if(!alsaLoadPlayback(card,wavename,&new_stream)) {
	    sprintf(temp,"LP %d %s -1 -1 -!",card,args[ch][2]);
	    EchoCommand(ch,temp);
	    LogLine(RDConfig::LogErr,QString().
		    sprintf("unable to allocate stream for card %d",
			    card));
	    return;
	  }
	  break;

	case RDStation::Jack:
	  if(!jackLoadPlayback(card,wavename,&new_stream)) {
	    sprintf(temp,"LP %d %s -1 -1 -!",card,args[ch][2]);
	    EchoCommand(ch,temp);
	    LogLine(RDConfig::LogErr,QString().
		    sprintf("unable to allocate stream for card %d",
			    card));
	    return;
	  }
	  break;

	default:
	  sprintf(temp,"LP %d %s -1 -1 -!",card,args[ch][2]);
	  EchoCommand(ch,temp);
	  return;
    }
    if((handle=GetHandle(card,new_stream))>=0) {
      LogLine(RDConfig::LogErr,QString().
	      sprintf("*** clearing stale stream assignment, card=%d  stream=%d ***",card,new_stream));
      play_handle[handle].card=-1;
      play_handle[handle].stream=-1;
      play_handle[handle].owner=-1;
    }
    handle=GetNextHandle();
    play_handle[handle].card=card;
    play_handle[handle].stream=new_stream;
    play_handle[handle].owner=ch;
    play_owner[card][new_stream]=ch;
    LogLine(RDConfig::LogInfo,QString().
	    sprintf("LoadPlayback  Card: %d  Stream: %d  Name: %s  Handle: %d",
		    card,new_stream,(const char *)wavename,handle));
    sprintf(temp,"LP %d %s %d %d +!",card,args[ch][2],new_stream,handle);
    EchoCommand(ch,temp);
    return;
  }

  if(!strcmp(args[ch][0],"UP")) {  // Unload Playback
    if((handle=GetHandle(ch,&card,&stream))<0) {
      EchoArgs(ch,'-');
      return;
    }
    card=play_handle[handle].card;
    stream=play_handle[handle].stream;
    if((play_owner[card][stream]==-1)||(play_owner[card][stream]==ch)) {
      switch(cae_driver[card]) {
	  case RDStation::Hpi:
	    if(hpiUnloadPlayback(card,stream)) {
	      play_owner[card][stream]=-1;
	      LogLine(RDConfig::LogInfo,QString().
		      sprintf("UnloadPlayback - Card: %d  Stream: %d  Handle: %d",
			      card,stream,handle));
	      EchoArgs(ch,'+');
	    }
	    else {
	      EchoArgs(ch,'-');
	    }
	    break;
	    
	  case RDStation::Alsa:
	    if(alsaUnloadPlayback(card,stream)) {
	      play_owner[card][stream]=-1;
	      LogLine(RDConfig::LogInfo,QString().
		      sprintf("UnloadPlayback - Card: %d  Stream: %d  Handle: %d",
			      card,stream,handle));
	      EchoArgs(ch,'+');
	    }
	    else {
	      EchoArgs(ch,'-');
	    }
	    break;
	    
	  case RDStation::Jack:
	    if(jackUnloadPlayback(card,stream)) {
	      play_owner[card][stream]=-1;
	      LogLine(RDConfig::LogInfo,QString().
		      sprintf("UnloadPlayback - Card: %d  Stream: %d  Handle: %d",
			      card,stream,handle));
	      EchoArgs(ch,'+');
	    }
	    else {
	      EchoArgs(ch,'-');
	    }
	    break;
	    
	  default:
	    EchoArgs(ch,'-');
	    return;
      }
      play_handle[handle].card=-1;
      play_handle[handle].stream=-1;
      play_handle[handle].owner=-1;
      return;
    }
    else {
      EchoArgs(ch,'-');
    }
  }

  if(!strcmp(args[ch][0],"PP")) {  // Playback Position
    if((handle=GetHandle(ch,&card,&stream))<0) {
      EchoArgs(ch,'-');
      return;
    }
    card=play_handle[handle].card;
    stream=play_handle[handle].stream;
    if(play_owner[card][stream]==ch) {
      if(sscanf(args[ch][2],"%d",&pos)!=1) {
	EchoArgs(ch,'-');
	return;
      }
      switch(cae_driver[card]) {
	  case RDStation::Hpi:
	    if(hpiPlaybackPosition(card,stream,pos)) {
	      LogLine(RDConfig::LogInfo,QString().
		   sprintf("PlaybackPosition - Card: %d  Stream: %d  Pos: %d  Handle: %d",
			   card,stream,pos,handle));
	      EchoArgs(ch,'+');
	    }
	    else {
	      LogLine(RDConfig::LogNotice,QString().
		   sprintf("*** PlaybackPosition out of bounds - Card: %d  Stream: %d  Pos: %d   Handle: %d***",
			   card,stream,pos,handle));
	      EchoArgs(ch,'-');
	    }
	    break;

	  case RDStation::Alsa:
	    if(alsaPlaybackPosition(card,stream,pos)) {
	      LogLine(RDConfig::LogInfo,QString().
		   sprintf("PlaybackPosition - Card: %d  Stream: %d  Pos: %d  Handle: %d",
			   card,stream,pos,handle));
	      EchoArgs(ch,'+');
	    }
	    else {
	      EchoArgs(ch,'-');
	    }
	    break;

	  case RDStation::Jack:
	    if(jackPlaybackPosition(card,stream,pos)) {
	      LogLine(RDConfig::LogInfo,QString().
		   sprintf("PlaybackPosition - Card: %d  Stream: %d  Pos: %d  Handle: %d",
			   card,stream,pos,handle));
	      EchoArgs(ch,'+');
	    }
	    else {
	      EchoArgs(ch,'-');
	    }
	    break;

	  default:
	    EchoArgs(ch,'-');
	    return;
      }
      return;
    }
    EchoArgs(ch,'-');
    return;
  }

  if(!strcmp(args[ch][0],"PY")) {  // Play
    if((handle=GetHandle(ch,&card,&stream))<0) {
      EchoArgs(ch,'-');
      return;
    }
    card=play_handle[handle].card;
    stream=play_handle[handle].stream;
    if(sscanf(args[ch][2],"%d",&play_length[card][stream])!=1) {
      EchoArgs(ch,'-');
      return;
    }
    if(sscanf(args[ch][3],"%d",&play_speed[card][stream])!=1) {
      EchoArgs(ch,'-');
      return;
    }
    if(sscanf(args[ch][4],"%d",&flag)!=1) {
      EchoArgs(ch,'-');
      return;
    }
    switch(flag) {
	case 0:
	  play_pitch[card][stream]=false;
	  break;
	case 1:
	  play_pitch[card][stream]=true;
	  break;
	default:
	  EchoArgs(ch,'-');
	  return;
    }
    if(play_owner[card][stream]==ch) {
      switch(cae_driver[card]) {
	  case RDStation::Hpi:
	    if(!hpiPlay(card,stream,play_length[card][stream],
			play_speed[card][stream],play_pitch[card][stream],
			RD_ALLOW_NONSTANDARD_RATES)) {
	      EchoArgs(ch,'-');
	      return;
	    }
	    break;

	  case RDStation::Alsa:
	    if(!alsaPlay(card,stream,play_length[card][stream],
			play_speed[card][stream],play_pitch[card][stream],
			RD_ALLOW_NONSTANDARD_RATES)) {
	      EchoArgs(ch,'-');
	      return;
	    }
	    break;

	  case RDStation::Jack:
	    if(!jackPlay(card,stream,play_length[card][stream],
			play_speed[card][stream],play_pitch[card][stream],
			RD_ALLOW_NONSTANDARD_RATES)) {
	      EchoArgs(ch,'-');
	      return;
	    }
	    break;

	  default:
	      EchoArgs(ch,'-');
	      return;
      }
      LogLine(RDConfig::LogInfo,QString().
       sprintf("Play - Card: %d  Stream: %d  Handle: %d  Length: %d  Speed: %d  Pitch: %d",
	       card,stream,handle,play_length[card][stream],
	       play_speed[card][stream],flag));
      // No command echo for success -- statePlayUpdate() sends it!
      return;
    }
    EchoArgs(ch,'-');
    return;
  }

  if(!strcmp(args[ch][0],"SP")) {  // Stop Playback
    if((handle=GetHandle(ch,&card,&stream))<0) {
      EchoArgs(ch,'-');
      return;
    }
    card=play_handle[handle].card;
    stream=play_handle[handle].stream;
    if(play_owner[card][stream]==ch) {
      switch(cae_driver[card]) {
	  case RDStation::Hpi:
	    if(!hpiStopPlayback(card,stream)) {
	      EchoArgs(ch,'-');
	      return;
	    }
	    break;

	  case RDStation::Alsa:
	    if(!alsaStopPlayback(card,stream)) {
	      EchoArgs(ch,'-');
	      return;
	    }
	    break;

	  case RDStation::Jack:
	    if(!jackStopPlayback(card,stream)) {
	      EchoArgs(ch,'-');
	      return;
	    }
	    break;

	  default:
	    EchoArgs(ch,'-');
	    return;
      }
      LogLine(RDConfig::LogInfo,QString().
	      sprintf("StopPlayback - Card: %d  Stream: %d  Handle: %d",
		      card,stream,handle));
      //      EchoArgs(ch,'+');
      return;
    }
    EchoArgs(ch,'-');
    return;
  }

  if(!strcmp(args[ch][0],"TS")) {  // Timescale Support
    switch(cae_driver[card]) {
	case RDStation::Hpi:
	  if(!hpiTimescaleSupported(card)) {
	    EchoArgs(ch,'-');
	    return;
	  }
	  break;

	case RDStation::Jack:
	  if(!jackTimescaleSupported(card)) {
	    EchoArgs(ch,'-');
	    return;
	  }
	  break;

	case RDStation::Alsa:
	  if(!alsaTimescaleSupported(card)) {
	    EchoArgs(ch,'-');
	    return;
	  }
	  break;

	default:
	  EchoArgs(ch,'-');
	  return;
    }
    EchoArgs(ch,'+');
    return;
  }

  if(!strcmp(args[ch][0],"LR")) {  // Load Record
    if((card<0)||(stream<0)) {
      EchoArgs(ch,'-');
      return;
    }
    if(record_owner[card][stream]==-1) {
      if(sscanf(args[ch][3],"%d",&coding)!=1) {
	EchoArgs(ch,'-');
	return;
      }
      if(sscanf(args[ch][4],"%d",&channels)!=1) {
	EchoArgs(ch,'-');
	return;
      }
      if(sscanf(args[ch][5],"%d",&sample_rate)!=1) {
	EchoArgs(ch,'-');
	return;
      }
      if(sscanf(args[ch][6],"%d",&bit_rate)!=1) {
	EchoArgs(ch,'-');
 	return;
      }
      wavename = rd_config->audioFileName(QString(args[ch][7]));
      unlink(wavename);  // So we don't trainwreck any current playouts!
      unlink(wavename+".energy");
      switch(cae_driver[card]) {
	  case RDStation::Hpi:
	    if(!hpiLoadRecord(card,stream,coding,channels,sample_rate,bit_rate,
			      wavename)) {
	      EchoArgs(ch,'-');
	      return;
	    }
	    break;

	  case RDStation::Alsa:
	    if(!alsaLoadRecord(card,stream,coding,channels,sample_rate,
			       bit_rate,wavename)) {
	      EchoArgs(ch,'-');
	      return;
	    }
	    break;

	  case RDStation::Jack:
	    if(!jackLoadRecord(card,stream,coding,channels,sample_rate,
			       bit_rate,wavename)) {
	      EchoArgs(ch,'-');
	      return;
	    }
	    break;

	  default:
	    EchoArgs(ch,'-');
	    return;
      }
      LogLine(RDConfig::LogInfo,QString().
	      sprintf("LoadRecord - Card: %d  Stream: %d  Coding: %d  Chans: %d  SampRate: %d  BitRate: %d  Name: %s",
		      card,stream,coding,channels,sample_rate,bit_rate,
		      (const char *)wavename));
      record_owner[card][stream]=ch;
      EchoArgs(ch,'+');
    }
    else {
      EchoArgs(ch,'-');
    }
    return;
  }

  if(!strcmp(args[ch][0],"UR")) {  // Unload Record
    if((card<0)||(stream<0)) {
      EchoArgs(ch,'-');
      return;
    }
    if((record_owner[card][stream]==-1)||(record_owner[card][stream]==ch)) {
      unsigned len=0;
      switch(cae_driver[card]) {
	  case RDStation::Hpi:
	    if(!hpiUnloadRecord(card,stream,&len)) {
	      EchoArgs(ch,'-');
	      return;
	    }
	    break;

	  case RDStation::Alsa:
	    if(!alsaUnloadRecord(card,stream,&len)) {
	      EchoArgs(ch,'-');
	      return;
	    }
	    break;

	  case RDStation::Jack:
	    if(!jackUnloadRecord(card,stream,&len)) {
	      EchoArgs(ch,'-');
	      return;
	    }
	    break;

	  default:
	    EchoArgs(ch,'-');
	    return;
      }
      record_owner[card][stream]=-1;
      LogLine(RDConfig::LogInfo,QString().
	      sprintf("UnloadRecord - Card: %d  Stream: %d, Length: %u",
		      card,stream,len));
      EchoCommand(ch,(const char *)QString().sprintf("UR %d %d %u +!",
						     card,stream,
	  (unsigned)((double)len*1000.0/(double)system_sample_rate)));
      return;
    }
    else {
      EchoArgs(ch,'-');
      return;
    }
  }

  if(!strcmp(args[ch][0],"RD")) {  // Record
    if((card<0)||(stream<0)) {
      EchoArgs(ch,'-');
      return;
    }
    if(sscanf(args[ch][3],"%d",&record_length[card][stream])!=1) {
      EchoArgs(ch,'-');
      return;
    }
    if(sscanf(args[ch][4],"%d",&record_threshold[card][stream])!=1) {
      EchoArgs(ch,'-');
      return;
    }
    if(record_owner[card][stream]==ch) {
      switch(cae_driver[card]) {
	  case RDStation::Hpi:
	    if(!hpiRecord(card,stream,record_length[card][stream],
			  record_threshold[card][stream])) {
	      EchoArgs(ch,'-');
	      return;
	    }
	    break;

	  case RDStation::Alsa:
	    if(!alsaRecord(card,stream,record_length[card][stream],
			  record_threshold[card][stream])) {
	      EchoArgs(ch,'-');
	      return;
	    }
	    break;

	  case RDStation::Jack:
	    if(!jackRecord(card,stream,record_length[card][stream],
			  record_threshold[card][stream])) {
	      EchoArgs(ch,'-');
	      return;
	    }
	    break;

	  default:
	    EchoArgs(ch,'-');
	    return;
      }
      LogLine(RDConfig::LogInfo,QString().
	      sprintf("Record - Card: %d  Stream: %d  Length: %d  Thres: %d",
		      card,stream,record_length[card][stream],
		      record_threshold[card][stream]));
//      EchoArgs(ch,'+');
      return;
    }
    EchoArgs(ch,'-');
    return;
  }

  if(!strcmp(args[ch][0],"SR")) {  // Stop Record
    if((card<0)||(stream<0)) {
      EchoArgs(ch,'-');
      return;
    }
    switch(cae_driver[card]) {
	case RDStation::Hpi:
	  if(!hpiStopRecord(card,stream)) {
	    EchoArgs(ch,'-');
	    return;
	  }
	  break;

	case RDStation::Alsa:
	  if(!alsaStopRecord(card,stream)) {
	    EchoArgs(ch,'-');
	    return;
	  }
	  EchoArgs(ch,'+');
	  break;

	case RDStation::Jack:
	  if(!jackStopRecord(card,stream)) {
	    EchoArgs(ch,'-');
	    return;
	  }
	  EchoArgs(ch,'+');
	  break;

	default:
	  EchoArgs(ch,'-');
	  return;
    }
    LogLine(RDConfig::LogInfo,QString().
	    sprintf("StopRecord - Card: %d  Stream: %d",
		    card,stream));
    return;
  }

  if(!strcmp(args[ch][0],"CS")) {  // Set Clock Source
    if((card<0)||(stream<0)) {
      EchoArgs(ch,'-');
      return;
    }
    switch(cae_driver[card]) {
	case RDStation::Hpi:
	  if(!hpiSetClockSource(card,stream)) {
	    EchoArgs(ch,'-');
	    return;
	  }

	default:
	  EchoArgs(ch,'+');
	  return;
    }
    if(rd_config->enableMixerLogging()) {
      LogLine(RDConfig::LogInfo,QString().
	      sprintf("SetClockSource - Card: %d  Source: %d",card,stream));
    }
    EchoArgs(ch,'+');
    return;
  }

  if(!strcmp(args[ch][0],"IV")) {  // Set Input Volume
    if((card<0)||(stream<0)) {
      EchoArgs(ch,'-');
      return;
    }
    sscanf(args[ch][3],"%d",&level);
    switch(cae_driver[card]) {
	case RDStation::Hpi:
	  if(!hpiSetInputVolume(card,stream,level)) {
	    EchoArgs(ch,'-');
	    return;
	  }

	case RDStation::Alsa:
	  if(!alsaSetInputVolume(card,stream,level)) {
	    EchoArgs(ch,'-');
	    return;
	  }

	case RDStation::Jack:
	  if(!jackSetInputVolume(card,stream,level)) {
	    EchoArgs(ch,'-');
	    return;
	  }

	default:
	  EchoArgs(ch,'-');
	  return;
    }
    if(rd_config->enableMixerLogging()) {
      LogLine(RDConfig::LogInfo,QString().
	      sprintf("SetInputVolume - Card: %d  Stream: %d Level: %d",
		      card,stream,level));
    }
    EchoArgs(ch,'+');
    return;
  }

  if(!strcmp(args[ch][0],"OV")) {  // Set Output Volume
    if((card<0)||(stream<0)) {
      EchoArgs(ch,'-');
      return;
    }
    sscanf(args[ch][3],"%d",&port);
    sscanf(args[ch][4],"%d",&level);
    switch(cae_driver[card]) {
	case RDStation::Hpi:
	  if(!hpiSetOutputVolume(card,stream,port,level)) {
	    EchoArgs(ch,'-');
	    return;
	  }
	  break;
	  
	case RDStation::Alsa:
	  if(!alsaSetOutputVolume(card,stream,port,level)) {
	    EchoArgs(ch,'-');
	    return;
	  }
	  break;
	  
	case RDStation::Jack:
	  if(!jackSetOutputVolume(card,stream,port,level)) {
	    EchoArgs(ch,'-');
	    return;
	  }
	  break;
	  
	default:
	  EchoArgs(ch,'-');
	  return;
    }
    if(rd_config->enableMixerLogging()) {
      LogLine(RDConfig::LogInfo,QString().
	      sprintf("SetOutputVolume - Card: %d  Stream: %d  Port: %d  Level: %d",
		      card,stream,port,level));
    }
    EchoArgs(ch,'+');
    return;
  }

  if(!strcmp(args[ch][0],"FV")) {  // Fade Output Volume
    if((card<0)||(stream<0)) {
      EchoArgs(ch,'-');
      return;
    }
    sscanf(args[ch][3],"%d",&port);
    sscanf(args[ch][4],"%d",&level);
    sscanf(args[ch][5],"%d",&length);
    switch(cae_driver[card]) {
	case RDStation::Hpi:
	  if(!hpiFadeOutputVolume(card,stream,port,level,length)) {
	    EchoArgs(ch,'-');
	    return;
	  }
	  break;

	case RDStation::Alsa:
	  if(!alsaFadeOutputVolume(card,stream,port,level,length)) {
	    EchoArgs(ch,'-');
	    return;
	  }
	  break;

	case RDStation::Jack:
	  if(!jackFadeOutputVolume(card,stream,port,level,length)) {
	    EchoArgs(ch,'-');
	    return;
	  }
	  break;

	default:
	  EchoArgs(ch,'-');
	  return;
    }
    if(rd_config->enableMixerLogging()) {
      LogLine(RDConfig::LogInfo,QString().
	      sprintf("FadeOutputVolume - Card: %d  Stream: %d  Port: %d  Level: %d  Length: %d",
		      card,stream,port,level,length));
    }
    EchoArgs(ch,'+');
    return;
  }

  if(!strcmp(args[ch][0],"IL")) {  // Set Input Level
    sscanf(args[ch][2],"%d",&port);
    sscanf(args[ch][3],"%d",&level);
    if((card<0)||(port<0)) {
      EchoArgs(ch,'-');
      return;
    }
    switch(cae_driver[card]) {
	case RDStation::Hpi:
	  if(!hpiSetInputLevel(card,port,level)) {
	    EchoArgs(ch,'-');
	    return;
	  }
	  break;
	  
	case RDStation::Alsa:
	  if(!alsaSetInputLevel(card,port,level)) {
	    EchoArgs(ch,'-');
	    return;
	  }
	  break;
	  
	case RDStation::Jack:
	  if(!jackSetInputLevel(card,port,level)) {
	    EchoArgs(ch,'-');
	    return;
	  }
	  break;
	  
	default:
	  EchoArgs(ch,'-');
	  return;
    }
    if(rd_config->enableMixerLogging()) {
      LogLine(RDConfig::LogInfo,QString().
	      sprintf("SetInputLevel - Card: %d  Port: %d  Level: %d",
		      card,port,level));
    }
    EchoArgs(ch,'+');
    return;
  }

  if(!strcmp(args[ch][0],"OL")) {  // Set Output Level
    sscanf(args[ch][2],"%d",&port);
    sscanf(args[ch][3],"%d",&level);
    if((card<0)||(port<0)) {
      EchoArgs(ch,'-');
      return;
    }
    switch(cae_driver[card]) {
	case RDStation::Hpi:
	  if(!hpiSetOutputLevel(card,port,level)) {
	    EchoArgs(ch,'-');
	    return;
	  }
	  break;

	case RDStation::Alsa:
	  if(!alsaSetOutputLevel(card,port,level)) {
	    EchoArgs(ch,'-');
	    return;
	  }
	  break;

	case RDStation::Jack:
	  if(!jackSetOutputLevel(card,port,level)) {
	    EchoArgs(ch,'-');
	    return;
	  }
	  break;

	default:
	  EchoArgs(ch,'-');
	  return;
    }
    if(rd_config->enableMixerLogging()) {
      LogLine(RDConfig::LogInfo,QString().
	      sprintf("SetOutputLevel - Card: %d  Port: %d  Level: %d",
		      card,port,level));
    }
    EchoArgs(ch,'+');
    return;
  }

  if(!strcmp(args[ch][0],"IM")) {  // Set Input Mode
    sscanf(args[ch][2],"%d",&port);
    sscanf(args[ch][3],"%d",&mode);
    if((card<0)||(port<0)) {
      EchoArgs(ch,'-');
      return;
    }
    switch(cae_driver[card]) {
	case RDStation::Hpi:
	  if(!hpiSetInputMode(card,port,mode)) {
	    EchoArgs(ch,'-');
	    return;
	  }
	  break;

	case RDStation::Alsa:
	  if(!alsaSetInputMode(card,port,mode)) {
	    EchoArgs(ch,'-');
	    return;
	  }
	  break;

	case RDStation::Jack:
	  if(!jackSetInputMode(card,port,mode)) {
	    EchoArgs(ch,'-');
	    return;
	  }
	  break;

	default:
	  EchoArgs(ch,'-');
	  return;
    }
    if(rd_config->enableMixerLogging()) {
      LogLine(RDConfig::LogInfo,QString().
	      sprintf("SetInputMode - Card: %d  Port: %d  Mode: %d",
		      card,port,mode));
    }
    EchoArgs(ch,'+');
    return;
  }

  if(!strcmp(args[ch][0],"OM")) {  // Set Output Mode
    sscanf(args[ch][2],"%d",&port);
    sscanf(args[ch][3],"%d",&mode);
    if((card<0)||(port<0)) {
      EchoArgs(ch,'-');
      return;
    }
    switch(cae_driver[card]) {
	case RDStation::Hpi:
	  if(!hpiSetOutputMode(card,port,mode)) {
	    EchoArgs(ch,'-');
	    return;
	  }
	  break;

	case RDStation::Alsa:
	  if(!alsaSetOutputMode(card,port,mode)) {
	    EchoArgs(ch,'-');
	    return;
	  }
	  break;

	case RDStation::Jack:
	  if(!jackSetOutputMode(card,port,mode)) {
	    EchoArgs(ch,'-');
	    return;
	  }
	  break;

	default:
	  EchoArgs(ch,'-');
	  return;
    }
    if(rd_config->enableMixerLogging()) {
      LogLine(RDConfig::LogInfo,QString().
	      sprintf("SetOutputMode - Card: %d  Port: %d  Mode: %d",
		      card,port,mode));
    }
    EchoArgs(ch,'+');
    return;
  }

  if(!strcmp(args[ch][0],"IX")) {  // Set Input VOX Level
    if((card<0)||(stream<0)) {
      EchoArgs(ch,'-');
      return;
    }
    sscanf(args[ch][3],"%d",&level);
    switch(cae_driver[card]) {
	case RDStation::Hpi:
	  if(!hpiSetInputVoxLevel(card,stream,level)) {
	    EchoArgs(ch,'-');
	    return;
	  }
	  break;

	case RDStation::Alsa:
	  if(!alsaSetInputVoxLevel(card,stream,level)) {
	    EchoArgs(ch,'-');
	    return;
	  }
	  break;

	case RDStation::Jack:
	  if(!jackSetInputVoxLevel(card,stream,level)) {
	    EchoArgs(ch,'-');
	    return;
	  }
	  break;

	default:
	  EchoArgs(ch,'-');
	  return;
    }
    if(rd_config->enableMixerLogging()) {
      LogLine(RDConfig::LogInfo,QString().
	      sprintf("SetInputVOXLevel - Card: %d  Stream: %d  Level: %d",
		      card,stream,level));
    }
    EchoArgs(ch,'+');
    return;
  }

  if(!strcmp(args[ch][0],"IT")) {  // Set Input Type
    sscanf(args[ch][2],"%d",&port);
    sscanf(args[ch][3],"%d",&type);
    if((card<0)||(port<0)) {
      EchoArgs(ch,'-');
      return;
    }
    switch(cae_driver[card]) {
	case RDStation::Hpi:
	  if(!hpiSetInputType(card,port,type)) {
	    EchoArgs(ch,'-');
	    return;
	  }
	  break;

	case RDStation::Alsa:
	  if(!alsaSetInputType(card,port,type)) {
	    EchoArgs(ch,'-');
	    return;
	  }
	  break;

	case RDStation::Jack:
	  if(!jackSetInputType(card,port,type)) {
	    EchoArgs(ch,'-');
	    return;
	  }
	  break;

	default:
	  EchoArgs(ch,'-');
	  return;
    }
    if(rd_config->enableMixerLogging()) {
      LogLine(RDConfig::LogInfo,QString().
	      sprintf("SetInputType - Card: %d  Port: %d  Type: %d",
		      card,port,type));
    }
    EchoArgs(ch,'+');
    return;
  }

  if(!strcmp(args[ch][0],"AL")) {  // Set Passthrough Level
    sscanf(args[ch][2],"%d",&in_port);
    sscanf(args[ch][3],"%d",&out_port);
    sscanf(args[ch][4],"%d",&level);
    if((card<0)||(in_port<0)||(out_port<0)) {
      EchoArgs(ch,'-');
      return;
    }
    switch(cae_driver[card]) {
	case RDStation::Hpi:
	  if(!hpiSetPassthroughLevel(card,in_port,out_port,level)) {
	    EchoArgs(ch,'-');
	    return;
	  }
	  break;
	  
	case RDStation::Alsa:
	  if(!alsaSetPassthroughLevel(card,in_port,out_port,level)) {
	    EchoArgs(ch,'-');
	    return;
	  }
	  break;
	  
	case RDStation::Jack:
	  if(!jackSetPassthroughLevel(card,in_port,out_port,level)) {
	    EchoArgs(ch,'-');
	    return;
	  }
	  break;
	  
	default:
	  EchoArgs(ch,'-');
	  return;
    }
    if(rd_config->enableMixerLogging()) {
      LogLine(RDConfig::LogInfo,QString().
	      sprintf("SetPassthroughLevel - Card: %d  InPort: %d  OutPort: %d Level: %d",
		      card,in_port,out_port,level));
    }
    EchoArgs(ch,'+');
    return;
  }

  if(!strcmp(args[ch][0],"IS")) {  // Input Status
    sscanf(args[ch][2],"%d",&port);
    if((card<0)||(port<0)) {
      EchoArgs(ch,'-');
      return;
    }
    if(hpiGetInputStatus(card,port)) {
      EchoCommand(ch,QString().sprintf("IS %d %d 0 +!",card,port));
    }
    else {
      EchoCommand(ch,QString().sprintf("IS %d %d 1 +!",card,port));
    }
    return;
  }

  if(!strcmp(args[ch][0],"ME")) {  // Meter Enable
    sscanf(args[ch][1],"%d",&port);
    if((port<0)||(port>0xFFFF)) {
      EchoArgs(ch,'-');
      return;
    }
    meter_port[ch]=port;
    EchoArgs(ch,'+');
    SendMeterOutputStatusUpdate();
    return;
  }

  if(!strcmp(args[ch][0],"OS")) {  // Set Output Status Flag
    sscanf(args[ch][1],"%d",&card);
    sscanf(args[ch][2],"%d",&port);
    sscanf(args[ch][3],"%d",&stream);
    if((card<0)||(card>=RD_MAX_CARDS)||
       (port<0)||(port>=RD_MAX_PORTS)||
       (stream<0)||(stream>=RD_MAX_STREAMS)) {
      EchoArgs(ch,'-');
      return;
    }
    output_status_flag[card][port][stream]=args[ch][4][0]=='1';
    SendMeterOutputStatusUpdate(card,port,stream);
    EchoArgs(ch,'+');
    return;
  }

  if(!strcmp(args[ch][0],"JC")) {  // Connect JACK Ports
    pos=-1;
    for(int i=0;i<argnum[ch];i++) {
      if(args[ch][i][0]=='|') {
	pos=i;
      }
    }
    if(pos<0) {
      return;
    }
    for(int i=1;i<pos;i++) {
      out_jport+=args[ch][i];
      out_jport+=" ";
    }
    out_jport=out_jport.left(out_jport.length()-1);
    for(int i=pos+1;i<argnum[ch];i++) {
      in_jport+=args[ch][i];
      in_jport+=" ";
    }
    in_jport=in_jport.left(in_jport.length()-1);
    jackConnectPorts(out_jport,in_jport);
  }

  if(!strcmp(args[ch][0],"JD")) {  // Disconnect JACK Ports
    pos=-1;
    for(int i=0;i<argnum[ch];i++) {
      if(args[ch][i][0]=='|') {
	pos=i;
      }
    }
    if(pos<0) {
      return;
    }
    for(int i=1;i<pos;i++) {
      out_jport+=args[ch][i];
      out_jport+=" ";
    }
    out_jport=out_jport.left(out_jport.length()-1);
    for(int i=pos+1;i<argnum[ch];i++) {
      in_jport+=args[ch][i];
      in_jport+=" ";
    }
    in_jport=in_jport.left(in_jport.length()-1);
    jackDisconnectPorts(out_jport,in_jport);
  }
}


void MainObject::KillSocket(int ch)
{
  istate[ch]=0;
  argnum[ch]=0;
  argptr[ch]=0;
  auth[ch]=false;
  meter_port[ch]=0;
  for(int i=0;i<RD_MAX_CARDS;i++) {
    for(int j=0;j<RD_MAX_STREAMS;j++) {
      if(record_owner[i][j]==ch) {
	unsigned len=0;
	switch(cae_driver[i]) {
	    case RDStation::Hpi:
	      hpiUnloadRecord(i,j,&len);
	      break;

	    case RDStation::Jack:
	      jackUnloadRecord(i,j,&len);
	      break;

	    case RDStation::Alsa:
	      alsaUnloadRecord(i,j,&len);
	      break;

	    default:
	      LogLine(RDConfig::LogNotice,"tried to kill unowned socket!");
	      break;
	}
	record_length[i][j]=0;
	record_threshold[i][j]=-10000;
	record_owner[i][j]=-1;
      }
      if(play_owner[i][j]==ch) {
	switch(cae_driver[i]) {
	    case RDStation::Hpi:
	      hpiUnloadPlayback(i,j);
	      break;

	    case RDStation::Jack:
	      jackUnloadPlayback(i,j);
	      break;

	    case RDStation::Alsa:
	      alsaUnloadPlayback(i,j);
	      break;

	    case RDStation::None:
	      break;
	}
	play_owner[i][j]=-1;
	play_length[i][j]=0;
	play_speed[i][j]=100;
	play_pitch[i][j]=false;
      }
    }
    for(int i=0;i<256;i++) {
      if(play_handle[i].owner==ch) {
	play_handle[i].card=-1;
	play_handle[i].stream=-1;
	play_handle[i].owner=-1;
      }
    }
  }
  delete socket[ch];
  socket[ch]=NULL;
}


void MainObject::BroadcastCommand(const char *command)
{
  for(int i=0;i<CAE_MAX_CONNECTIONS;i++) {
    if(auth[i]) {
      EchoCommand(i,command);
    }
  }
}


void MainObject::EchoCommand(int ch,const char *command)
{
  if(socket[ch]->state()==QSocket::Connection) {
#ifdef PRINT_COMMANDS
    printf("CAE: Connection %d sending %s\n",ch,command);
#endif  // PRINT_COMMANDS
    socket[ch]->writeBlock(command,strlen(command));
  }
}


void MainObject::EchoArgs(int ch,const char append)
{
  char command[CAE_MAX_LENGTH+2];
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


pid_t MainObject::GetPid(QString pidfile)
{
  FILE *handle;
  pid_t ret;

  if((handle=fopen((const char *)pidfile,"r"))==NULL) {
    return -1;
  }
  if(fscanf(handle,"%d",&ret)!=1) {
    ret=-1;
  }
  fclose(handle);
  return ret;
}


bool MainObject::CheckDaemon(QString name)
{
  QDir dir;
  QString path;
  path=QString(RD_PROC_DIR)+QString("/")+QString().sprintf("%d",GetPid(name));
  dir.setPath(path);
  return dir.exists();
}


int MainObject::GetNextHandle()
{
  while(play_handle[next_play_handle].card>=0) {
    next_play_handle++;
    next_play_handle&=0xFF;
  }
  int handle=next_play_handle;
  next_play_handle++;
  next_play_handle&=0xFF;
  return handle;
}


int MainObject::GetHandle(int ch,int *card,int *stream)
{
  int handle;

  if(sscanf(args[ch][1],"%d",&handle)!=1) {
    return -1;
  }
  if((handle<0)||(handle>=256)) {
    return -1;
  }
  if((*card=play_handle[handle].card)<0) {
    return -1;
  }
  if((*stream=play_handle[handle].stream<0)) {
    return -1;
  }
  return handle;
}


int MainObject::GetHandle(int card,int stream)
{
  for(int i=0;i<256;i++) {
    if((play_handle[i].card==card)&&(play_handle[i].stream==stream)) {
      return i;
    }
  }
  return -1;
}


void MainObject::ProbeCaps(RDStation *station)
{
  //
  // Patent-clear codecs
  //
#ifdef HAVE_VORBIS
  station->setHaveCapability(RDStation::HaveOggenc,true);
  station->setHaveCapability(RDStation::HaveOgg123,true);
#else
  station->setHaveCapability(RDStation::HaveOggenc,false);
  station->setHaveCapability(RDStation::HaveOgg123,false);
#endif  // HAVE_VORBIS
#ifdef HAVE_FLAC
  station->setHaveCapability(RDStation::HaveFlac,true);
#else
  station->setHaveCapability(RDStation::HaveFlac,false);
#endif  // HAVE_FLAC

  //
  // MPEG Codecs
  //
  station->setHaveCapability(RDStation::HaveLame,CheckLame());
  station->setHaveCapability(RDStation::HaveTwoLame,LoadTwoLame());
  station->setHaveCapability(RDStation::HaveMpg321,LoadMad());

#ifdef HPI
  station->setDriverVersion(RDStation::Hpi,hpiVersion());
#else
  station->setDriverVersion(RDStation::Hpi,"[not enabled]");
#endif  // HPI
#ifdef JACK
  //
  // FIXME: How can we detect the current JACK version?
  //
  station->setDriverVersion(RDStation::Jack,"Generic");
#else
  station->setDriverVersion(RDStation::Jack,"");
#endif  // JACK
#ifdef ALSA
  station->setDriverVersion(RDStation::Alsa,
			    QString().sprintf("%d.%d.%d",
					      SND_LIB_MAJOR,
					      SND_LIB_MINOR,
					      SND_LIB_SUBMINOR));
#else
  station->setDriverVersion(RDStation::Alsa,"");
#endif  // ALSA
}


void MainObject::ClearDriverEntries(RDStation *station)
{
  for(int i=0;i<RD_MAX_CARDS;i++) {
    if(cae_driver[i]==RDStation::None) {
      station->setCardDriver(i,RDStation::None);
      station->setCardName(i,"");
      station->setCardInputs(i,-1);
      station->setCardOutputs(i,-1);
    }
  }
}


/* This is an overloaded virtual function to tell a session manager not to restart this daemon. */
void QApplication::saveState(QSessionManager &sm) {
  sm.setRestartHint(QSessionManager::RestartNever);
  LogLine(RDConfig::LogDebug,"cae saveState(), set restart hint to Never");
  return;
};


bool MainObject::CheckLame()
{
#ifdef HAVE_LAME
  return dlopen("libmp3lame.so",RTLD_LAZY)!=NULL;
#else
  return false;
#endif  // HAVE_LAME
}


bool MainObject::LoadTwoLame()
{
#ifdef HAVE_TWOLAME
  if((twolame_handle=dlopen("libtwolame.so",RTLD_NOW))==NULL) {
    LogLine(RDConfig::LogInfo,
	 "TwoLAME encoder library not found, MPEG L2 encoding not supported");
    return false;
  }
  *(void **)(&twolame_init)=dlsym(twolame_handle,"twolame_init");
  *(void **)(&twolame_set_mode)=dlsym(twolame_handle,"twolame_set_mode");
  *(void **)(&twolame_set_num_channels)=
    dlsym(twolame_handle,"twolame_set_num_channels");
  *(void **)(&twolame_set_in_samplerate)=
    dlsym(twolame_handle,"twolame_set_in_samplerate");
  *(void **)(&twolame_set_out_samplerate)=
    dlsym(twolame_handle,"twolame_set_out_samplerate");
  *(void **)(&twolame_set_bitrate)=
    dlsym(twolame_handle,"twolame_set_bitrate");
  *(void **)(&twolame_init_params)=
    dlsym(twolame_handle,"twolame_init_params");
  *(void **)(&twolame_close)=dlsym(twolame_handle,"twolame_close");
  *(void **)(&twolame_encode_buffer_interleaved)=
    dlsym(twolame_handle,"twolame_encode_buffer_interleaved");
  *(void **)(&twolame_encode_buffer_float32_interleaved)=
    dlsym(twolame_handle,"twolame_encode_buffer_float32_interleaved");
  *(void **)(&twolame_encode_flush)=
    dlsym(twolame_handle,"twolame_encode_flush");
  *(void **)(&twolame_set_energy_levels)=
    dlsym(twolame_handle,"twolame_set_energy_levels");
  LogLine(RDConfig::LogInfo,
	  "Found TwoLAME encoder library, MPEG L2 encoding supported");
  return true;
#else
  LogLine(RDConfig::LogInfo,"MPEG L2 encoding not enabled");
  return false;
#endif  // HAVE_TWOLAME
}


bool MainObject::InitTwoLameEncoder(int card,int stream,int chans,int samprate,
				    int bitrate)
{
#ifdef HAVE_TWOLAME
  TWOLAME_MPEG_mode mpeg_mode=TWOLAME_STEREO;

  switch(chans) {
  case 1:
    mpeg_mode=TWOLAME_MONO;
    break;

  case 2:
    mpeg_mode=TWOLAME_STEREO;    
    break;
  }
  if((twolame_lameopts[card][stream]=twolame_init())==NULL) {
    LogLine(RDConfig::LogErr,QString().sprintf("unable to initialize twolame instance, card=%d, stream=%d",card,stream));
    return false;
  }
  twolame_set_mode(twolame_lameopts[card][stream],mpeg_mode);
  twolame_set_num_channels(twolame_lameopts[card][stream],chans);
  twolame_set_in_samplerate(twolame_lameopts[card][stream],samprate);
  twolame_set_out_samplerate(twolame_lameopts[card][stream],samprate);
  twolame_set_bitrate(twolame_lameopts[card][stream],bitrate/1000);
  twolame_set_energy_levels(twolame_lameopts[card][stream],1);
  if(twolame_init_params(twolame_lameopts[card][stream])!=0) {
    LogLine(RDConfig::LogErr,QString().sprintf("invalid twolame parameters, card=%d, stream=%d, chans=%d, samprate=%d  bitrate=%d",
					       card,stream,chans,samprate,bitrate));
    return false;
  }
  return true;
#else
  return false;
#endif  // HAVE_TWOLAME
}


void MainObject::FreeTwoLameEncoder(int card,int stream)
{
#ifdef HAVE_TWOLAME
  if(twolame_lameopts[card][stream]!=NULL) { 
    twolame_close(&twolame_lameopts[card][stream]);
    twolame_lameopts[card][stream]=NULL;
  }
#endif  // HAVE_TWOLAME
}


bool MainObject::LoadMad()
{
#ifdef HAVE_MAD
  if((mad_handle=dlopen("libmad.so",RTLD_NOW))==NULL) {
    LogLine(RDConfig::LogInfo,
	 "MAD decoder library not found, MPEG L2 decoding not supported");
    return false;
  }
  *(void **)(&mad_stream_init)=
    dlsym(mad_handle,"mad_stream_init");
  *(void **)(&mad_frame_init)=
    dlsym(mad_handle,"mad_frame_init");
  *(void **)(&mad_synth_init)=
    dlsym(mad_handle,"mad_synth_init");
  *(void **)(&mad_stream_buffer)=
    dlsym(mad_handle,"mad_stream_buffer");
  *(void **)(&mad_frame_decode)=
    dlsym(mad_handle,"mad_frame_decode");
  *(void **)(&mad_synth_frame)=
    dlsym(mad_handle,"mad_synth_frame");
  *(void **)(&mad_frame_finish)=
    dlsym(mad_handle,"mad_frame_finish");
  *(void **)(&mad_stream_finish)=
    dlsym(mad_handle,"mad_stream_finish");
  LogLine(RDConfig::LogInfo,
	  "Found MAD decoder library, MPEG L2 decoding supported");
  return true;
#else
  LogLine(RDConfig::LogInfo,"MPEG L2 decoding not enabled");
  return false;
#endif  // HAVE_MAD
}


void MainObject::InitMadDecoder(int card,int stream,RDWaveFile *wave)
{
#ifdef HAVE_MAD
  if(mad_active[card][stream]) {
    FreeMadDecoder(card,stream);
  }
  mad_stream_init(&mad_stream[card][stream]);
  mad_frame_init(&mad_frame[card][stream]);
  mad_synth_init(&mad_synth[card][stream]);
  mad_frame_size[card][stream]=
    144*wave->getHeadBitRate()/wave->getSamplesPerSec();
  mad_left_over[card][stream]=0;
  mad_active[card][stream]=true;
#endif  // HAVE_MAD
}


void MainObject::FreeMadDecoder(int card,int stream)
{
#ifdef HAVE_MAD
  if(mad_active[card][stream]) {
    mad_synth_finish(&mad_synth[card][stream]);
    mad_frame_finish(&mad_frame[card][stream]);
    mad_stream_finish(&mad_stream[card][stream]);
    mad_frame_size[card][stream]=0;
    mad_left_over[card][stream]=0;
    mad_active[card][stream]=false;
  }
#endif  // HAVE_MAD
}


void MainObject::SendMeterLevelUpdate(const QString &type,int cardnum,
				      int portnum,short levels[])
{
  char msg[1500];
  int n=snprintf(msg,1500,"ML %s %d %d %d %d",
		 (const char *)type,cardnum,portnum,levels[0],levels[1]);
  SendMeterUpdate(msg,n);
}


void MainObject::SendStreamMeterLevelUpdate(int cardnum,int streamnum,
					    short levels[])
{
  char msg[1500];
  int n=snprintf(msg,1500,"MO %d %d %d %d",
		 cardnum,streamnum,levels[0],levels[1]);
  SendMeterUpdate(msg,n);
}


void MainObject::SendMeterPositionUpdate(int cardnum,unsigned pos[])
{
  char msg[1500];
  int n;
  for(unsigned i=0;i<RD_MAX_STREAMS;i++) {
    n=snprintf(msg,1500,"MP %d %d %d",cardnum,i,pos[i]);
    SendMeterUpdate(msg,n);
  }
}


void MainObject::SendMeterOutputStatusUpdate()
{
  for(unsigned i=0;i<RD_MAX_CARDS;i++) {
    if(cae_driver[i]!=RDStation::None) {
      for(unsigned j=0;j<RD_MAX_PORTS;j++) {
	for(unsigned k=0;k<RD_MAX_STREAMS;k++) {
	  SendMeterOutputStatusUpdate(i,j,k);
	}
      }
    }
  }
}


void MainObject::SendMeterOutputStatusUpdate(int card,int port,int stream)
{
  char msg[1500];
  int n;
  n=snprintf(msg,1500,"MS %d %d %d %d",card,port,stream,
	     output_status_flag[card][port][stream]);
  SendMeterUpdate(msg,n);
}


void MainObject::SendMeterUpdate(const char *msg,unsigned len)
{
  for(unsigned i=0;i<CAE_MAX_CONNECTIONS;i++) {
    if(meter_port[i]>0) {
      meter_socket->writeBlock(msg,len,socket[i]->peerAddress(),meter_port[i]);
    }
  }
}


int main(int argc,char *argv[])
{
  int rc;
  QApplication a(argc,argv,false);
  new MainObject(NULL,"main");
  rc=a.exec();
  LogLine(RDConfig::LogDebug,QString().sprintf("cae post a.exec() rc:%d", rc));
  return rc;
}
