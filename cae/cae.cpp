// cae.cpp
//
// The Core Audio Engine component of Rivendell
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


#include <qcoreapplication.h>
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

#include <rdsocket.h>
#include <rdconf.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdcmd_switch.h>
#include <rdsvc.h>
#include <rdsystem.h>
#include <rdaudio_port.h>

#include <cae.h>

volatile bool exiting=false;
RDConfig *rd_config;
#ifdef JACK
extern jack_client_t *jack_client;
#endif  // JACK

#define PRINT_COMMANDS

#ifndef HAVE_SRC_CONV
void src_int_to_float_array (const int *in, float *out, int len)
{
  for(int i=0;i<len;i++) {
    out[i]=(double)in[i]/2147483648.0;
  }
}


void src_float_to_int_array (const float *in, int *out, int len)
{
  for(int i=0;i<len;i++) {
    out[i]=(int)(in[i]*2147483648.0);
  }
}
#endif  // HAVE_SRC_CONV


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
  new RDCmdSwitch(qApp->argc(),qApp->argv(),"caed",CAED_USAGE);

  //
  // LogLine references rd_config
  // 
  rd_config=new RDConfig(RD_CONF_FILE);
  rd_config->load();
  rd_config->setModuleName("caed");

  syslog(LOG_INFO,"cae starting");

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

  cae_server=new CaeServer(rd_config,this);
  if(!cae_server->listen(QHostAddress::Any,CAED_TCP_PORT)) {
    syslog(LOG_ERR,"caed: failed to bind port %d",CAED_TCP_PORT);
    exit(1);
  }
  connect(cae_server,SIGNAL(connectionDropped(int)),
	  this,SLOT(connectionDroppedData(int)));
  connect(cae_server,SIGNAL(loadPlaybackReq(int,unsigned,const QString &)),
	  this,SLOT(loadPlaybackData(int,unsigned,const QString &)));
  connect(cae_server,SIGNAL(unloadPlaybackReq(int,unsigned)),
	  this,SLOT(unloadPlaybackData(int,unsigned)));
  connect(cae_server,SIGNAL(playPositionReq(int,unsigned,unsigned)),
	  this,SLOT(playPositionData(int,unsigned,unsigned)));
  connect(cae_server,SIGNAL(playReq(int,unsigned,unsigned,unsigned,unsigned)),
	  this,SLOT(playData(int,unsigned,unsigned,unsigned,unsigned)));
  connect(cae_server,SIGNAL(stopPlaybackReq(int,unsigned)),
	  this,SLOT(stopPlaybackData(int,unsigned)));
  connect(cae_server,SIGNAL(timescalingSupportReq(int,unsigned)),
	  this,SLOT(timescalingSupportData(int,unsigned)));
  connect(cae_server,
	  SIGNAL(loadRecordingReq(int,unsigned,unsigned,unsigned,unsigned,
				  unsigned,unsigned,const QString &)),
	  this,
	  SLOT(loadRecordingData(int,unsigned,unsigned,unsigned,unsigned,
				unsigned,unsigned,const QString &)));
  connect(cae_server,SIGNAL(unloadRecordingReq(int,unsigned,unsigned)),
	  this,SLOT(unloadRecordingData(int,unsigned,unsigned)));
  connect(cae_server,SIGNAL(recordReq(int,unsigned,unsigned,unsigned,int)),
	  this,SLOT(recordData(int,unsigned,unsigned,unsigned,int)));
  connect(cae_server,SIGNAL(stopRecordingReq(int,unsigned,unsigned)),
	  this,SLOT(stopRecordingData(int,unsigned,unsigned)));
  connect(cae_server,SIGNAL(setInputVolumeReq(int,unsigned,unsigned,int)),
	  this,SLOT(setInputVolumeData(int,unsigned,unsigned,int)));
  connect(cae_server,
	  SIGNAL(setOutputVolumeReq(int,unsigned,unsigned,unsigned,int)),
	  this,
	  SLOT(setOutputVolumeData(int,unsigned,unsigned,unsigned,int)));
  connect(cae_server,SIGNAL(fadeOutputVolumeReq(int,unsigned,unsigned,
						unsigned,int,unsigned)),
	  this,SLOT(fadeOutputVolumeData(int,unsigned,unsigned,
					 unsigned,int,unsigned)));
  connect(cae_server,SIGNAL(setInputLevelReq(int,unsigned,unsigned,int)),
	  this,SLOT(setInputLevelData(int,unsigned,unsigned,int)));
  connect(cae_server,SIGNAL(setOutputLevelReq(int,unsigned,unsigned,int)),
	  this,SLOT(setOutputLevelData(int,unsigned,unsigned,int)));
  connect(cae_server,SIGNAL(setInputModeReq(int,unsigned,unsigned,unsigned)),
	  this,SLOT(setInputModeData(int,unsigned,unsigned,unsigned)));
  connect(cae_server,SIGNAL(setOutputModeReq(int,unsigned,unsigned,unsigned)),
	  this,SLOT(setOutputModeData(int,unsigned,unsigned,unsigned)));
  connect(cae_server,SIGNAL(setInputVoxLevelReq(int,unsigned,unsigned,int)),
	  this,SLOT(setInputVoxLevelData(int,unsigned,unsigned,int)));
  connect(cae_server,SIGNAL(setInputTypeReq(int,unsigned,unsigned,unsigned)),
	  this,SLOT(setInputTypeData(int,unsigned,unsigned,unsigned)));
  connect(cae_server,SIGNAL(getInputStatusReq(int,unsigned,unsigned)),
	  this,SLOT(getInputStatusData(int,unsigned,unsigned)));
  connect(cae_server,
	SIGNAL(setAudioPassthroughLevelReq(int,unsigned,unsigned,unsigned,int)),
	  this,
	SLOT(setAudioPassthroughLevelData(int,unsigned,unsigned,unsigned,int)));
  connect(cae_server,SIGNAL(setClockSourceReq(int,unsigned,int)),
	  this,SLOT(setClockSourceData(int,unsigned,int)));
  connect(cae_server,
	  SIGNAL(setOutputStatusFlagReq(int,unsigned,unsigned,unsigned,bool)),
	  this,
	  SLOT(setOutputStatusFlagData(int,unsigned,unsigned,unsigned,bool)));
  connect(cae_server,
	  SIGNAL(openRtpCaptureChannelReq(int,unsigned,unsigned,uint16_t,
					  unsigned,unsigned)),
	  this,
	  SLOT(openRtpCaptureChannelData(int,unsigned,unsigned,uint16_t,
					unsigned,unsigned)));
  connect(cae_server,
	  SIGNAL(jackConnectPortsReq(int,const QString &,const QString &)),
	  this,
	  SLOT(jackConnectPortsData(int,const QString &,const QString &)));
  connect(cae_server,
	  SIGNAL(jackDisconnectPortsReq(int,const QString &,const QString &)),
	  this,
	  SLOT(jackDisconnectPortsData(int,const QString &,const QString &)));
  connect(cae_server,
	  SIGNAL(meterEnableReq(int,uint16_t,const QList<unsigned> &)),
	  this,
	  SLOT(meterEnableData(int,uint16_t,const QList<unsigned> &)));

  signal(SIGHUP,SigHandler);
  signal(SIGINT,SigHandler);
  signal(SIGTERM,SigHandler);

  //
  // Allocate Meter Socket
  //
  meter_socket=new Q3SocketDevice(Q3SocketDevice::Datagram);

  //
  // Open Database
  //
  QSqlDatabase db=QSqlDatabase::addDatabase(rd_config->mysqlDriver());
  db.setDatabaseName(rd_config->mysqlDbname());
  db.setUserName(rd_config->mysqlUsername());
  db.setPassword(rd_config->mysqlPassword());
  db.setHostName(rd_config->mysqlHostname());
  if(!db.open()) {
    syslog(LOG_ERR,"unable to connect to mySQL Server");
    printf("caed: unable to connect to mySQL Server");
    exit(1);
  }

  //
  // Provisioning
  //
  InitProvisioning();

  //
  // Start Up the Drivers
  //
  cae_station=new RDStation(rd_config->stationName());
  RDSystem *sys=new RDSystem();
  system_sample_rate=sys->sampleRate();
  delete sys;
  hpiInit(cae_station);
  alsaInit(cae_station);
  jackInit(cae_station);
  ClearDriverEntries(cae_station);

  //
  // Probe Capabilities
  //
  ProbeCaps(cae_station);

  //
  // Close Database Connection
  //
  cae_station->setScanned(true);

  //
  // Initialize Mixers
  //
  InitMixers();

  //
  // Meter Update Timer
  //
  QTimer *timer=new QTimer(this);
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
    int r=pthread_setschedparam(pthread_self(),sched_policy,&sched_params);
    if(r) {
      result=r;
    }
    mlockall(MCL_CURRENT|MCL_FUTURE);
    if(result) {
      syslog(LOG_WARNING,"unable to set realtime scheduling: %s",
	     strerror(result));
    }
    else {
      syslog(LOG_DEBUG,"using realtime scheduling, priority=%d",
	     sched_params.sched_priority);
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
    syslog(LOG_NOTICE,"mixer logging enabled");
  }
  syslog(LOG_INFO,"cae started");
}


void MainObject::loadPlaybackData(int id,unsigned card,const QString &name)
{
  QString wavename;
  int new_stream=-1;
  int handle;

  wavename=rd_config->audioFileName(name);
  switch(cae_driver[card]) {
  case RDStation::Hpi:
    if(!hpiLoadPlayback(card,wavename,&new_stream)) {
      cae_server->
	sendCommand(id,QString().sprintf("LP %d %s -1 -1 -!",card,
					 (const char *)name.toUtf8()));
      syslog(LOG_WARNING,"unable to allocate stream for card %d",card);
      return;
    }
    break;

  case RDStation::Alsa:
    if(!alsaLoadPlayback(card,wavename,&new_stream)) {
      cae_server->
	sendCommand(id,QString().sprintf("LP %d %s -1 -1 -!",card,
					 (const char *)name.toUtf8()));
      syslog(LOG_WARNING,"unable to allocate stream for card %d",card);
      return;
    }
    break;

  case RDStation::Jack:
    if(!jackLoadPlayback(card,wavename,&new_stream)) {
      cae_server->
	sendCommand(id,QString().sprintf("LP %d %s -1 -1 -!",card,
					 (const char *)name.toUtf8()));
      syslog(LOG_WARNING,"unable to allocate stream for card %d",card);
      return;
    }
    break;

  default:
    cae_server->
      sendCommand(id,QString().sprintf("LP %d %s -1 -1 -!",card,
				       (const char *)name.toUtf8()));
    return;
  }
  if((handle=GetHandle(card,new_stream))>=0) {
    syslog(LOG_WARNING,
	   "*** clearing stale stream assignment, card=%d  stream=%d ***",
	   card,new_stream);
    play_handle[handle].card=-1;
    play_handle[handle].stream=-1;
    play_handle[handle].owner=-1;
  }
  handle=GetNextHandle();
  play_handle[handle].card=card;
  play_handle[handle].stream=new_stream;
  play_handle[handle].owner=id;
  play_owner[card][new_stream]=id;
  syslog(LOG_DEBUG,"LoadPlayback  Card: %d  Stream: %d  Name: %s  Handle: %d",
	 card,new_stream,(const char *)wavename.toUtf8(),handle);
  cae_server->
    sendCommand(id,QString().sprintf("LP %d %s %d %d +!",card,
				     (const char *)name.toUtf8(),
				     new_stream,handle));
}


void MainObject::unloadPlaybackData(int id,unsigned handle)
{
  int card=play_handle[handle].card;
  int stream=play_handle[handle].stream;

  if((play_owner[card][stream]==-1)||(play_owner[card][stream]==id)) {
    switch(cae_driver[card]) {
    case RDStation::Hpi:
      if(hpiUnloadPlayback(card,stream)) {
	play_owner[card][stream]=-1;
	syslog(LOG_DEBUG,"UnloadPlayback - Card: %d  Stream: %d  Handle: %d",
	       card,stream,handle);
	cae_server->sendCommand(id,QString().sprintf("UP %d+!",handle));
      }
      else {
	cae_server->sendCommand(id,QString().sprintf("UP %d-!",handle));
      }
      break;

    case RDStation::Alsa:
      if(alsaUnloadPlayback(card,stream)) {
	play_owner[card][stream]=-1;
	syslog(LOG_DEBUG,"UnloadPlayback - Card: %d  Stream: %d  Handle: %d",
	       card,stream,handle);
	cae_server->sendCommand(id,QString().sprintf("UP %d+!",handle));
      }
      else {
	cae_server->sendCommand(id,QString().sprintf("UP %d-!",handle));
      }
      break;

    case RDStation::Jack:
      if(jackUnloadPlayback(card,stream)) {
	play_owner[card][stream]=-1;
	syslog(LOG_DEBUG,"UnloadPlayback - Card: %d  Stream: %d  Handle: %d",
	       card,stream,handle);
	cae_server->sendCommand(id,QString().sprintf("UP %d+!",handle));
      }
      else {
	cae_server->sendCommand(id,QString().sprintf("UP %d-!",handle));
      }
      break;

    default:
      cae_server->sendCommand(id,QString().sprintf("UP %d-!",handle));
      return;
    }
    play_handle[handle].card=-1;
    play_handle[handle].stream=-1;
    play_handle[handle].owner=-1;
    return;
  }
  else {
    cae_server->sendCommand(id,QString().sprintf("UP %d-!",handle));
  }
}


void MainObject::playPositionData(int id,unsigned handle,unsigned pos)
{
  int card=play_handle[handle].card;
  int stream=play_handle[handle].stream;

  if(play_owner[card][stream]==id) {
    switch(cae_driver[card]) {
    case RDStation::Hpi:
      if(hpiPlaybackPosition(card,stream,pos)) {
	syslog(LOG_DEBUG,
	       "PlaybackPosition - Card: %d  Stream: %d  Pos: %d  Handle: %d",
	       card,stream,pos,handle);
	cae_server->sendCommand(id,QString().sprintf("PP %d %d +!",handle,pos));
      }
      else {
	cae_server->sendCommand(id,QString().sprintf("PP %d %d -!",handle,pos));
      }
      break;
      
    case RDStation::Alsa:
      if(alsaPlaybackPosition(card,stream,pos)) {
	syslog(LOG_DEBUG,
	       "PlaybackPosition - Card: %d  Stream: %d  Pos: %d  Handle: %d",
	       card,stream,pos,handle);
	cae_server->sendCommand(id,QString().sprintf("PP %d %d +!",handle,pos));
      }
      else {
	cae_server->sendCommand(id,QString().sprintf("PP %d %d -!",handle,pos));
      }
      break;
      
    case RDStation::Jack:
      if(jackPlaybackPosition(card,stream,pos)) {
	syslog(LOG_DEBUG,
	       "PlaybackPosition - Card: %d  Stream: %d  Pos: %d  Handle: %d",
	       card,stream,pos,handle);
	cae_server->sendCommand(id,QString().sprintf("PP %d %d +!",handle,pos));
      }
      else {
	cae_server->sendCommand(id,QString().sprintf("PP %d %d -!",handle,pos));
      }
      break;
      
    default:
      cae_server->sendCommand(id,QString().sprintf("PP %d %d -!",handle,pos));
      return;
    }
    return;
  }
  cae_server->sendCommand(id,QString().sprintf("PP %d %d -!",handle,pos));
}


void MainObject::playData(int id,unsigned handle,unsigned length,unsigned speed,
			  unsigned pitch_flag)
{
  int card=play_handle[handle].card;
  int stream=play_handle[handle].stream;

  play_length[card][stream]=length;
  play_speed[card][stream]=speed;
  switch(pitch_flag) {
  case 0:
    play_pitch[card][stream]=false;
    break;

  case 1:
    play_pitch[card][stream]=true;
    break;

  default:
    cae_server->
      sendCommand(id,QString().sprintf("PY %u %u %u %u-!",
				       handle,length,speed,pitch_flag));
    return;
  }
  if(play_owner[card][stream]==id) {
    switch(cae_driver[card]) {
    case RDStation::Hpi:
      if(!hpiPlay(card,stream,play_length[card][stream],
		  play_speed[card][stream],play_pitch[card][stream],
		  RD_ALLOW_NONSTANDARD_RATES)) {
	cae_server->
	  sendCommand(id,QString().sprintf("PY %u %u %u %u-!",
					   handle,length,speed,pitch_flag));
	return;
      }
      break;

    case RDStation::Alsa:
      if(!alsaPlay(card,stream,play_length[card][stream],
		   play_speed[card][stream],play_pitch[card][stream],
		   RD_ALLOW_NONSTANDARD_RATES)) {
	cae_server->
	  sendCommand(id,QString().sprintf("PY %u %u %u %u-!",
					   handle,length,speed,pitch_flag));
	return;
      }
      break;

    case RDStation::Jack:
      if(!jackPlay(card,stream,play_length[card][stream],
		   play_speed[card][stream],play_pitch[card][stream],
		   RD_ALLOW_NONSTANDARD_RATES)) {
	cae_server->
	  sendCommand(id,QString().sprintf("PY %u %u %u %u-!",
					   handle,length,speed,pitch_flag));
	return;
      }
      break;

    default:
      cae_server->
	sendCommand(id,QString().sprintf("PY %u %u %u %u-!",
					 handle,length,speed,pitch_flag));
      return;
    }
    syslog(LOG_DEBUG,
	   "Play - Card: %d  Stream: %d  Handle: %d  Length: %d  Speed: %d  Pitch: %d",
	   card,stream,handle,play_length[card][stream],
	   play_speed[card][stream],pitch_flag);
    // No command echo for success -- statePlayUpdate() sends it!
    return;
  }
  cae_server->
    sendCommand(id,QString().sprintf("PY %u %u %u %u-!",
				     handle,length,speed,pitch_flag));
}


void MainObject::stopPlaybackData(int id,unsigned handle)
{
  int card=play_handle[handle].card;
  int stream=play_handle[handle].stream;
  if(play_owner[card][stream]==id) {
    switch(cae_driver[card]) {
    case RDStation::Hpi:
      if(!hpiStopPlayback(card,stream)) {
	cae_server->sendCommand(id,QString().sprintf("SP %u-!",handle));
	return;
      }
      break;

    case RDStation::Alsa:
      if(!alsaStopPlayback(card,stream)) {
	cae_server->sendCommand(id,QString().sprintf("SP %u-!",handle));
	return;
      }
      break;

    case RDStation::Jack:
      if(!jackStopPlayback(card,stream)) {
	cae_server->sendCommand(id,QString().sprintf("SP %u-!",handle));
	return;
      }
      break;

    default:
      cae_server->sendCommand(id,QString().sprintf("SP %u-!",handle));
      return;
    }
    syslog(LOG_DEBUG,"StopPlayback - Card: %d  Stream: %d  Handle: %d",
	   card,stream,handle);
    return;
  }
  cae_server->sendCommand(id,QString().sprintf("SP %u-!",handle));
}


void MainObject::timescalingSupportData(int id,unsigned card)
{
  bool state=false;

  switch(cae_driver[card]) {
  case RDStation::Hpi:
    state=hpiTimescaleSupported(card);
    break;

  case RDStation::Jack:
    state=jackTimescaleSupported(card);
    break;

  case RDStation::Alsa:
    state=alsaTimescaleSupported(card);
    break;

  case RDStation::None:
    break;
  }
  if(state) {
    cae_server->sendCommand(id,"TS+!");
  }
  else {
    cae_server->sendCommand(id,"TS-!");
  }
}


void MainObject::loadRecordingData(int id,unsigned card,unsigned port,
				   unsigned coding,unsigned channels,
				   unsigned samprate,unsigned bitrate,
				   const QString &name)
{
  QString wavename;

  if(record_owner[card][port]==-1) {
    wavename=rd_config->audioFileName(name);
    unlink(wavename);  // So we don't trainwreck any current playouts!
    unlink(wavename+".energy");
    switch(cae_driver[card]) {
    case RDStation::Hpi:
      if(!hpiLoadRecord(card,port,coding,channels,samprate,bitrate,
			wavename)) {
	cae_server->
	  sendCommand(id,QString().sprintf("LR %u %u %u %u %u %u %s-!",
					   card,port,coding,channels,samprate,
				       bitrate,(const char *)name.toUtf8()));
	return;
      }
      break;

    case RDStation::Alsa:
      if(!alsaLoadRecord(card,port,coding,channels,samprate,
			 bitrate,wavename)) {
	cae_server->
	  sendCommand(id,QString().sprintf("LR %u %u %u %u %u %u %s-!",
					   card,port,coding,channels,samprate,
				       bitrate,(const char *)name.toUtf8()));
	return;
            }
      break;
      
    case RDStation::Jack:
      if(!jackLoadRecord(card,port,coding,channels,samprate,
			 bitrate,wavename)) {
	cae_server->
	  sendCommand(id,QString().sprintf("LR %u %u %u %u %u %u %s-!",
					   card,port,coding,channels,samprate,
					  bitrate,(const char *)name.toUtf8()));
	return;
      }
      break;

    default:
      cae_server->
	sendCommand(id,QString().sprintf("LR %u %u %u %u %u %u %s-!",
					 card,port,coding,channels,samprate,
					 bitrate,(const char *)name.toUtf8()));
      return;
    }
    syslog(LOG_DEBUG,
	   "LoadRecord - Card: %d  Stream: %d  Coding: %d  Chans: %d  SampRate: %d  BitRate: %d  Name: %s",
	   card,port,coding,channels,samprate,bitrate,
	   (const char *)wavename.toUtf8());
    record_owner[card][port]=id;
    cae_server->
      sendCommand(id,QString().sprintf("LR %u %u %u %u %u %u %s+!",
				       card,port,coding,channels,samprate,
				       bitrate,(const char *)name.toUtf8()));
  }
  else {
    cae_server->
      sendCommand(id,QString().sprintf("LR %u %u %u %u %u %u %s-!",
				       card,port,coding,channels,samprate,
				       bitrate,(const char *)name.toUtf8()));
  }
}


void MainObject::unloadRecordingData(int id,unsigned card,unsigned stream)
{
  if((record_owner[card][stream]==-1)||(record_owner[card][stream]==id)) {
    unsigned len=0;
    switch(cae_driver[card]) {
    case RDStation::Hpi:
      if(!hpiUnloadRecord(card,stream,&len)) {
	cae_server->sendCommand(id,QString().sprintf("UR %u %u-!",card,stream));
	return;
      }
      break;

    case RDStation::Alsa:
      if(!alsaUnloadRecord(card,stream,&len)) {
	cae_server->sendCommand(id,QString().sprintf("UR %u %u-!",card,stream));
	return;
      }
      break;

    case RDStation::Jack:
      if(!jackUnloadRecord(card,stream,&len)) {
	cae_server->sendCommand(id,QString().sprintf("UR %u %u-!",card,stream));
	return;
      }
      break;

    default:
      cae_server->sendCommand(id,QString().sprintf("UR %u %u-!",card,stream));
      return;
    }
    record_owner[card][stream]=-1;
    syslog(LOG_DEBUG,"UnloadRecord - Card: %d  Stream: %d, Length: %u",
	   card,stream,len);
    cae_server->
      sendCommand(id,QString().sprintf("UR %u %u %u+!",card,stream,
		   (unsigned)((double)len*1000.0/(double)system_sample_rate)));
    return;
  }
  else {
    cae_server->sendCommand(id,QString().sprintf("UR %u %u-!",card,stream));
    return;
  }
}


void MainObject::recordData(int id,unsigned card,unsigned stream,unsigned len,
			    int threshold_level)
{
  record_length[card][stream]=len;
  record_threshold[card][stream]=threshold_level;
  if(record_owner[card][stream]==id) {
    switch(cae_driver[card]) {
    case RDStation::Hpi:
      if(!hpiRecord(card,stream,record_length[card][stream],
		    record_threshold[card][stream])) {
	cae_server->
	  sendCommand(id,QString().sprintf("RD %u %u %u %d-!",
					   card,stream,len,threshold_level));
	return;
      }
      break;

    case RDStation::Alsa:
      if(!alsaRecord(card,stream,record_length[card][stream],
		     record_threshold[card][stream])) {
	cae_server->
	  sendCommand(id,QString().sprintf("RD %u %u %u %d-!",
					   card,stream,len,threshold_level));
	return;
      }
      break;

    case RDStation::Jack:
      if(!jackRecord(card,stream,record_length[card][stream],
		     record_threshold[card][stream])) {
	cae_server->
	  sendCommand(id,QString().sprintf("RD %u %u %u %d-!",
					   card,stream,len,threshold_level));
	return;
      }
      break;

    default:
      cae_server->
	sendCommand(id,QString().sprintf("RD %u %u %u %d-!",
					 card,stream,len,threshold_level));
      return;
    }
    syslog(LOG_DEBUG,"Record - Card: %d  Stream: %d  Length: %d  Thres: %d",
	   card,stream,record_length[card][stream],
	   record_threshold[card][stream]);
    // No positive echo required here!
    return;
  }
  cae_server->
    sendCommand(id,QString().sprintf("RD %u %u %u %d-!",
				     card,stream,len,threshold_level));
}


void MainObject::stopRecordingData(int id,unsigned card,unsigned stream)
{
  switch(cae_driver[card]) {
  case RDStation::Hpi:
    if(!hpiStopRecord(card,stream)) {
      cae_server->sendCommand(id,QString().sprintf("SR %u %u-!",card,stream));
      return;
    }
    break;

  case RDStation::Alsa:
    if(!alsaStopRecord(card,stream)) {
      cae_server->sendCommand(id,QString().sprintf("SR %u %u-!",card,stream));
      return;
    }
    cae_server->sendCommand(id,QString().sprintf("SR %u %u+!",card,stream));
    break;

  case RDStation::Jack:
    if(!jackStopRecord(card,stream)) {
      cae_server->sendCommand(id,QString().sprintf("SR %u %u-!",card,stream));
      return;
    }
    cae_server->sendCommand(id,QString().sprintf("SR %u %u+!",card,stream));
    break;

  default:
    cae_server->sendCommand(id,QString().sprintf("SR %u %u-!",card,stream));
    return;
  }
  syslog(LOG_DEBUG,"StopRecord - Card: %d  Stream: %d",card,stream);
}


void MainObject::setInputVolumeData(int id,unsigned card,unsigned stream,
				    int level)
{
  switch(cae_driver[card]) {
  case RDStation::Hpi:
    if(!hpiSetInputVolume(card,stream,level)) {
      cae_server->
	sendCommand(id,QString().sprintf("IV %u %u %d-!",card,stream,level));
      return;
    }

  case RDStation::Alsa:
    if(!alsaSetInputVolume(card,stream,level)) {
      cae_server->
	sendCommand(id,QString().sprintf("IV %u %u %d-!",card,stream,level));
      return;
    }

  case RDStation::Jack:
    if(!jackSetInputVolume(card,stream,level)) {
      cae_server->
	sendCommand(id,QString().sprintf("IV %u %u %d-!",card,stream,level));
      return;
    }

  default:
    cae_server->
      sendCommand(id,QString().sprintf("IV %u %u %d-!",card,stream,level));
    return;
  }
  if(rd_config->enableMixerLogging()) {
    syslog(LOG_DEBUG,"SetInputVolume - Card: %d  Stream: %d Level: %d",
	   card,stream,level);
  }
  cae_server->
    sendCommand(id,QString().sprintf("IV %u %u %d+!",card,stream,level));
}


void MainObject::setOutputVolumeData(int id,unsigned card,unsigned stream,
				     unsigned port,int level)
{
  switch(cae_driver[card]) {
  case RDStation::Hpi:
    if(!hpiSetOutputVolume(card,stream,port,level)) {
      cae_server->sendCommand(id,QString().sprintf("OV %u %u %u %d-!",
						   card,stream,port,level));
      return;
    }
    break;

  case RDStation::Alsa:
    if(!alsaSetOutputVolume(card,stream,port,level)) {
      cae_server->sendCommand(id,QString().sprintf("OV %u %u %u %d-!",
						   card,stream,port,level));
      return;
    }
    break;

  case RDStation::Jack:
    if(!jackSetOutputVolume(card,stream,port,level)) {
      cae_server->sendCommand(id,QString().sprintf("OV %u %u %u %d-!",
						   card,stream,port,level));
      return;
    }
    break;

  default:
    cae_server->sendCommand(id,QString().sprintf("OV %u %u %u %d-!",
						 card,stream,port,level));
    return;
  }
  if(rd_config->enableMixerLogging()) {
    syslog(LOG_DEBUG,
	   "SetOutputVolume - Card: %d  Stream: %d  Port: %d  Level: %d",
	   card,stream,port,level);
  }
  cae_server->sendCommand(id,QString().sprintf("OV %u %u %u %d+!",
					       card,stream,port,level));
}


void MainObject::fadeOutputVolumeData(int id,unsigned card,unsigned stream,
				      unsigned port,int level,unsigned length)
{
  switch(cae_driver[card]) {
  case RDStation::Hpi:
    if(!hpiFadeOutputVolume(card,stream,port,level,length)) {
      cae_server->
	sendCommand(id,QString().sprintf("FV %u %u %u %d %u-!",
					 card,stream,port,level,length));
      return;
    }
    break;

  case RDStation::Alsa:
    if(!alsaFadeOutputVolume(card,stream,port,level,length)) {
      cae_server->
	sendCommand(id,QString().sprintf("FV %u %u %u %d %u-!",
					 card,stream,port,level,length));
      return;
    }
    break;

  case RDStation::Jack:
    if(!jackFadeOutputVolume(card,stream,port,level,length)) {
      cae_server->
	sendCommand(id,QString().sprintf("FV %u %u %u %d %u-!",
					 card,stream,port,level,length));
      return;
    }
    break;

  default:
    cae_server->
      sendCommand(id,QString().sprintf("FV %u %u %u %d %u-!",
				       card,stream,port,level,length));
    return;
  }
  if(rd_config->enableMixerLogging()) {
    syslog(LOG_DEBUG,
     "FadeOutputVolume - Card: %d  Stream: %d  Port: %d  Level: %d  Length: %d",
	   card,stream,port,level,length);
  }
  cae_server->
    sendCommand(id,QString().sprintf("FV %u %u %u %d %u+!",
				     card,stream,port,level,length));
}


void MainObject::setInputLevelData(int id,unsigned card,unsigned port,
				   int level)
{
  switch(cae_driver[card]) {
  case RDStation::Hpi:
    if(!hpiSetInputLevel(card,port,level)) {
      cae_server->sendCommand(id,QString().sprintf("IL %u %u %d-!",
						   card,port,level));
      return;
    }
    break;

  case RDStation::Alsa:
    if(!alsaSetInputLevel(card,port,level)) {
      cae_server->sendCommand(id,QString().sprintf("IL %u %u %d-!",
						   card,port,level));
      return;
    }
    break;

  case RDStation::Jack:
    if(!jackSetInputLevel(card,port,level)) {
      cae_server->sendCommand(id,QString().sprintf("IL %u %u %d-!",
						   card,port,level));
      return;
    }
    break;

  default:
    cae_server->sendCommand(id,QString().sprintf("IL %u %u %d-!",
						 card,port,level));
    return;
  }
  if(rd_config->enableMixerLogging()) {
    syslog(LOG_DEBUG,"SetInputLevel - Card: %d  Port: %d  Level: %d",
	   card,port,level);
  }
  cae_server->sendCommand(id,QString().sprintf("IL %u %u %d+!",
					       card,port,level));
}


void MainObject::setOutputLevelData(int id,unsigned card,unsigned port,
				    int level)
{
  switch(cae_driver[card]) {
  case RDStation::Hpi:
    if(!hpiSetOutputLevel(card,port,level)) {
      cae_server->sendCommand(id,QString().sprintf("OL %u %u %d-!",
						   card,port,level));
      return;
    }
    break;

  case RDStation::Alsa:
    if(!alsaSetOutputLevel(card,port,level)) {
      cae_server->sendCommand(id,QString().sprintf("OL %u %u %d-!",
						   card,port,level));
      return;
    }
    break;

  case RDStation::Jack:
    if(!jackSetOutputLevel(card,port,level)) {
      cae_server->sendCommand(id,QString().sprintf("OL %u %u %d-!",
						   card,port,level));
      return;
    }
    break;

  default:
    cae_server->sendCommand(id,QString().sprintf("OL %u %u %d-!",
						 card,port,level));
    return;
  }
  if(rd_config->enableMixerLogging()) {
    syslog(LOG_DEBUG,"SetOutputLevel - Card: %d  Port: %d  Level: %d",
	   card,port,level);
  }
  cae_server->sendCommand(id,QString().sprintf("OL %u %u %d+!",
					       card,port,level));
}


void MainObject::setInputModeData(int id,unsigned card,unsigned stream,
				  unsigned mode)
{
  switch(cae_driver[card]) {
  case RDStation::Hpi:
    if(!hpiSetInputMode(card,stream,mode)) {
      cae_server->sendCommand(id,QString().sprintf("IM %u %u %u-!",
						   card,stream,mode));
      return;
    }
    break;

  case RDStation::Alsa:
    if(!alsaSetInputMode(card,stream,mode)) {
      cae_server->sendCommand(id,QString().sprintf("IM %u %u %u-!",
						   card,stream,mode));
      return;
    }
    break;

  case RDStation::Jack:
    if(!jackSetInputMode(card,stream,mode)) {
      cae_server->sendCommand(id,QString().sprintf("IM %u %u %u-!",
						   card,stream,mode));
      return;
    }
    break;

  default:
    cae_server->sendCommand(id,QString().sprintf("IM %u %u %u-!",
						 card,stream,mode));
    return;
  }
  if(rd_config->enableMixerLogging()) {
    syslog(LOG_DEBUG,"SetInputMode - Card: %d  Stream: %d  Mode: %d",
	   card,stream,mode);
  }
  cae_server->sendCommand(id,QString().sprintf("IM %u %u %u+!",
					       card,stream,mode));
}


void MainObject::setOutputModeData(int id,unsigned card,unsigned stream,
				   unsigned mode)
{
  switch(cae_driver[card]) {
  case RDStation::Hpi:
    if(!hpiSetOutputMode(card,stream,mode)) {
      cae_server->sendCommand(id,QString().sprintf("OM %u %u %u-!",
						   card,stream,mode));
      return;
    }
    break;

  case RDStation::Alsa:
    if(!alsaSetOutputMode(card,stream,mode)) {
      cae_server->sendCommand(id,QString().sprintf("OM %u %u %u-!",
						   card,stream,mode));
      return;
    }
    break;

  case RDStation::Jack:
    if(!jackSetOutputMode(card,stream,mode)) {
      cae_server->sendCommand(id,QString().sprintf("OM %u %u %u-!",
						   card,stream,mode));
      return;
    }
    break;

  default:
    cae_server->sendCommand(id,QString().sprintf("OM %u %u %u-!",
						 card,stream,mode));
    return;
  }
  if(rd_config->enableMixerLogging()) {
    syslog(LOG_DEBUG,"SetOutputMode - Card: %d  Stream: %d  Mode: %d",
	   card,stream,mode);
  }
  cae_server->sendCommand(id,QString().sprintf("OM %u %u %u+!",
					       card,stream,mode));
}


void MainObject::setInputVoxLevelData(int id,unsigned card,unsigned stream,
				      int level)
{
  switch(cae_driver[card]) {
  case RDStation::Hpi:
    if(!hpiSetInputVoxLevel(card,stream,level)) {
      cae_server->sendCommand(id,QString().sprintf("IX %u %u %d-!",
						   card,stream,level));
      return;
    }
    break;

  case RDStation::Alsa:
    if(!alsaSetInputVoxLevel(card,stream,level)) {
      cae_server->sendCommand(id,QString().sprintf("IX %u %u %d-!",
						   card,stream,level));
      return;
    }
    break;

  case RDStation::Jack:
    if(!jackSetInputVoxLevel(card,stream,level)) {
      cae_server->sendCommand(id,QString().sprintf("IX %u %u %d-!",
						   card,stream,level));
      return;
    }
    break;

  default:
    cae_server->sendCommand(id,QString().sprintf("IX %u %u %d-!",
						 card,stream,level));
    return;
  }
  if(rd_config->enableMixerLogging()) {
    syslog(LOG_DEBUG,"SetInputVOXLevel - Card: %d  Stream: %d  Level: %d",
	   card,stream,level);
  }
  cae_server->sendCommand(id,QString().sprintf("IX %u %u %d+!",
					       card,stream,level));
}


void MainObject::setInputTypeData(int id,unsigned card,unsigned port,
				  unsigned type)
{
  switch(cae_driver[card]) {
  case RDStation::Hpi:
    if(!hpiSetInputType(card,port,type)) {
      cae_server->sendCommand(id,QString().sprintf("IT %u %u %u-!",
						   card,port,type));
      return;
    }
    break;

  case RDStation::Alsa:
    if(!alsaSetInputType(card,port,type)) {
      cae_server->sendCommand(id,QString().sprintf("IT %u %u %u-!",
						   card,port,type));
      return;
    }
    break;

  case RDStation::Jack:
    if(!jackSetInputType(card,port,type)) {
      cae_server->sendCommand(id,QString().sprintf("IT %u %u %u-!",
						   card,port,type));
      return;
    }
    break;

  default:
    cae_server->sendCommand(id,QString().sprintf("IT %u %u %u-!",
						 card,port,type));
    return;
  }
  if(rd_config->enableMixerLogging()) {
    syslog(LOG_DEBUG,"SetInputType - Card: %d  Port: %d  Type: %d",
	   card,port,type);
  }
  cae_server->sendCommand(id,QString().sprintf("IT %u %u %u+!",
					       card,port,type));
}


void MainObject::getInputStatusData(int id,unsigned card,unsigned port)
{
  if(hpiGetInputStatus(card,port)) {
    cae_server->sendCommand(id,QString().sprintf("IS %d %d 0 +!",card,port));
  }
  else {
    cae_server->sendCommand(id,QString().sprintf("IS %d %d 1 +!",card,port));
  }
}


void MainObject::setAudioPassthroughLevelData(int id,unsigned card,
					      unsigned input,unsigned output,
					      int level)
{
  switch(cae_driver[card]) {
  case RDStation::Hpi:
    if(!hpiSetPassthroughLevel(card,input,output,level)) {
      cae_server->sendCommand(id,QString().sprintf("AL %u %u %u %d-!",
						   card,input,output,level));
      return;
    }
    break;

  case RDStation::Alsa:
    if(!alsaSetPassthroughLevel(card,input,output,level)) {
      cae_server->sendCommand(id,QString().sprintf("AL %u %u %u %d-!",
						   card,input,output,level));
      return;
    }
    break;

  case RDStation::Jack:
    if(!jackSetPassthroughLevel(card,input,output,level)) {
      cae_server->sendCommand(id,QString().sprintf("AL %u %u %u %d-!",
						   card,input,output,level));
      return;
    }
    break;

  default:
    cae_server->sendCommand(id,QString().sprintf("AL %u %u %u %d-!",
						 card,input,output,level));
    return;
  }
  if(rd_config->enableMixerLogging()) {
    syslog(LOG_DEBUG,
	   "SetPassthroughLevel - Card: %d  InPort: %d  OutPort: %d Level: %d",
	   card,input,output,level);
  }
  cae_server->sendCommand(id,QString().sprintf("AL %u %u %u %d+!",
					       card,input,output,level));
}


void MainObject::setClockSourceData(int id,unsigned card,int input)
{
  if((card<0)||(input<0)) {
    cae_server->sendCommand(id,QString().sprintf("CS %u %u-!",card,input));
    return;
  }
  switch(cae_driver[card]) {
  case RDStation::Hpi:
    if(!hpiSetClockSource(card,input)) {
      cae_server->sendCommand(id,QString().sprintf("CS %u %u-!",card,input));
      return;
    }

  default:
    cae_server->sendCommand(id,QString().sprintf("CS %u %u+!",card,input));
    return;
  }
  if(rd_config->enableMixerLogging()) {
    syslog(LOG_DEBUG,"SetClockSource - Card: %d  Source: %d",card,input);
  }
  cae_server->sendCommand(id,QString().sprintf("CS %u %u+!",card,input));
}


void MainObject::setOutputStatusFlagData(int id,unsigned card,unsigned port,
					 unsigned stream,bool state)
{
  output_status_flag[card][port][stream]=state;
  SendMeterOutputStatusUpdate(card,port,stream);
  cae_server->sendCommand(id,QString().sprintf("OS %u %u %u %u+!",
					       card,port,stream,state));
}


void MainObject::openRtpCaptureChannelData(int id,unsigned card,unsigned port,
					   uint16_t udp_port,unsigned samprate,
					   unsigned chans)
{
}


void MainObject::jackConnectPortsData(int id,const QString &out_name,
				      const QString &in_name)
{
  jackConnectPorts(out_name,in_name);
}


void MainObject::jackDisconnectPortsData(int id,const QString &out_name,
					 const QString &in_name)
{
  jackDisconnectPorts(out_name,in_name);
}


void MainObject::meterEnableData(int id,uint16_t udp_port,
				 const QList<unsigned> &cards)
{
  QString cmd=QString().sprintf("ME %u",0xFFFF&udp_port);
  for(int i=0;i<cards.size();i++) {
    cmd+=QString().sprintf(" %u",cards.at(i));
  }
  if((udp_port<0)||(udp_port>0xFFFF)) {
    cae_server->sendCommand(id,cmd+"-!");
    return;
  }
  cae_server->setMeterPort(id,udp_port);
  for(int i=0;i<cards.size();i++) {
    if((cards.at(i)<0)||(cards.at(i)>=RD_MAX_CARDS)) {
      cae_server->sendCommand(id,cmd+"-!");
      return;
    }
    cae_server->setMetersEnabled(id,cards.at(i),true);
  }

  cae_server->sendCommand(id,cmd+"+!");
  SendMeterOutputStatusUpdate();
}


void MainObject::connectionDroppedData(int id)
{
  KillSocket(id);
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
	  cae_server->sendCommand(play_owner[card][stream],
				  (const char *)QString().
				  sprintf("PY %d %d %d +!",handle,
					  play_length[card][stream],
					  play_speed[card][stream]));
	  break;
	case 2:   // Paused
	  cae_server->
	    sendCommand(play_owner[card][stream],(const char *)QString().
			sprintf("SP %d +!",handle));
	  break;
	case 0:   // Stopped
	  cae_server->
	    sendCommand(play_owner[card][stream],(const char *)QString().
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
	  cae_server->
	    sendCommand(record_owner[card][stream],(const char *)QString().
		      sprintf("RD %d %d %d %d +!",card,stream,
			      record_length[card][stream],
			      record_threshold[card][stream]));
	  break;

	case 4:    // Record Started
	  cae_server->
	    sendCommand(record_owner[card][stream],(const char *)QString().
		      sprintf("RS %d %d +!",card,stream));
	  break;

	case 2:    // Paused
	case 3:    // Stopped
	  cae_server->
	    sendCommand(record_owner[card][stream],(const char *)QString().
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
    syslog(LOG_INFO,"cae exiting");
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
		cae_server->sendCommand(QString().sprintf("IS %d %d 0!",i,j));
	      }
	      else {
		cae_server->sendCommand(QString().sprintf("IS %d %d 1!",i,j));
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
	      cae_server->sendCommand(QString().sprintf("IS %d %d %d",i,j,
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
	      cae_server->sendCommand(QString().sprintf("IS %d %d %d",i,j,
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
  }
}


void MainObject::InitProvisioning() const
{
  QString sql;
  RDSqlQuery *q;
  QString err_msg;

  //
  // Provision a Host
  //
  if(rd_config->provisioningCreateHost()) {
    if(!rd_config->provisioningHostTemplate().isEmpty()) {
      sql=QString("select NAME from STATIONS where ")+
	"NAME=\""+RDEscapeString(rd_config->stationName())+"\"";
      q=new RDSqlQuery(sql);
      if(!q->first()) {
	if(RDStation::create(rd_config->stationName(),&err_msg,rd_config->provisioningHostTemplate(),rd_config->provisioningHostIpAddress())) {
	  syslog(LOG_INFO,"created new host entry \"%s\"",
		 (const char *)rd_config->stationName());
	  if(!rd_config->provisioningHostShortName(rd_config->stationName()).
	     isEmpty()) {
	    RDStation *station=new RDStation(rd_config->stationName());
	    station->setShortName(rd_config->
		     provisioningHostShortName(rd_config->stationName()));
	    delete station;
	  }
	}
	else {
	  fprintf(stderr,"caed: unable to provision host [%s]\n",
		  (const char *)err_msg);
	  exit(256);
	}
      }
      delete q;
    }
  }

  //
  // Provision a Service
  //
  if(rd_config->provisioningCreateService()) {
    if(!rd_config->provisioningServiceTemplate().isEmpty()) {
      QString svcname=
	rd_config->provisioningServiceName(rd_config->stationName());
      sql=QString("select NAME from SERVICES where ")+
	"NAME=\""+RDEscapeString(svcname)+"\"";
      q=new RDSqlQuery(sql);
      if(!q->first()) {
	if(RDSvc::create(svcname,&err_msg,
			 rd_config->provisioningServiceTemplate(),rd_config)) {
	  syslog(LOG_INFO,"created new service entry \"%s\"",
		 (const char *)svcname);
	}
	else {
	  fprintf(stderr,"caed: unable to provision service [%s]\n",
		  (const char *)err_msg);
	  exit(256);
	}
      }
      delete q;
    }
  }
}


void MainObject::InitMixers()
{
  for(int i=0;i<RD_MAX_CARDS;i++) {
    RDAudioPort *port=new RDAudioPort(rd_config->stationName(),i);
    switch(cae_driver[i]) {
      case RDStation::Hpi:
        hpiSetClockSource(i,port->clockSource());
        for(int j=0;j<RD_MAX_PORTS;j++) {
          for(int k=0;k<RD_MAX_PORTS;k++) {
            hpiSetPassthroughLevel(i,j,k,RD_MUTE_DEPTH);
          }
          if(port->inputPortType(j)==RDAudioPort::Analog) {
            hpiSetInputType(i,j,RDCae::Analog);
          }
          else {
            hpiSetInputType(i,j,RDCae::AesEbu);
          }
          hpiSetInputLevel(i,j,RD_BASE_ANALOG+port->inputPortLevel(j));
          hpiSetOutputLevel(i,j,RD_BASE_ANALOG+port->outputPortLevel(j));
          hpiSetInputMode(i,j,port->inputPortMode(j));
        }
        break;

      case RDStation::Jack:
        for(int j=0;j<RD_MAX_PORTS;j++) {
          for(int k=0;k<RD_MAX_PORTS;k++) {
            jackSetPassthroughLevel(i,j,k,RD_MUTE_DEPTH);
          }
          if(port->inputPortType(j)==RDAudioPort::Analog) {
            jackSetInputType(i,j,RDCae::Analog);
          }
          else {
            jackSetInputType(i,j,RDCae::AesEbu);
          }
          jackSetInputLevel(i,j,RD_BASE_ANALOG+port->inputPortLevel(j));
          jackSetOutputLevel(i,j,RD_BASE_ANALOG+port->outputPortLevel(j));
          jackSetInputMode(i,j,port->inputPortMode(j));
        }
        break;

      case RDStation::Alsa:
        for(int j=0;j<RD_MAX_PORTS;j++) {
          for(int k=0;k<RD_MAX_PORTS;k++) {
            alsaSetPassthroughLevel(i,j,k,RD_MUTE_DEPTH);
          }
          if(port->inputPortType(j)==RDAudioPort::Analog) {
            alsaSetInputType(i,j,RDCae::Analog);
          }
          else {
            alsaSetInputType(i,j,RDCae::AesEbu);
          }
          alsaSetInputLevel(i,j,RD_BASE_ANALOG+port->inputPortLevel(j));
          alsaSetOutputLevel(i,j,RD_BASE_ANALOG+port->outputPortLevel(j));
          alsaSetInputMode(i,j,port->inputPortMode(j));
        }
        break;

      case RDStation::None:
        break;
    }
    delete port;
  }
}


void MainObject::KillSocket(int ch)
{
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
	      syslog(LOG_NOTICE,"tried to kill unowned socket!");
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

  //
  // MP4 Decoder
  //
  station->setHaveCapability(RDStation::HaveMp4Decode,CheckMp4Decode());

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


bool MainObject::CheckLame()
{
#ifdef HAVE_LAME
  return dlopen("libmp3lame.so.0",RTLD_LAZY)!=NULL;
#else
  return false;
#endif  // HAVE_LAME
}


bool MainObject::CheckMp4Decode()
{
#ifdef HAVE_MP4_LIBS
  return (dlopen("libfaad.so.2",RTLD_LAZY)!=NULL)&&
    (dlopen("libmp4v2.so.2",RTLD_LAZY)!=NULL);
#else
  return false;
#endif  // HAVE_MP4_LIBS
}


bool MainObject::LoadTwoLame()
{
#ifdef HAVE_TWOLAME
  if((twolame_handle=dlopen("libtwolame.so.0",RTLD_NOW))==NULL) {
    syslog(LOG_INFO,
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
  syslog(LOG_INFO,
	 "Found TwoLAME encoder library, MPEG L2 encoding supported");
  return true;
#else
  syslog(LOG_INFO,"MPEG L2 encoding not enabled");

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
    syslog(LOG_WARNING,
	   "unable to initialize twolame instance, card=%d, stream=%d",
	   card,stream);
    return false;
  }
  twolame_set_mode(twolame_lameopts[card][stream],mpeg_mode);
  twolame_set_num_channels(twolame_lameopts[card][stream],chans);
  twolame_set_in_samplerate(twolame_lameopts[card][stream],samprate);
  twolame_set_out_samplerate(twolame_lameopts[card][stream],samprate);
  twolame_set_bitrate(twolame_lameopts[card][stream],bitrate/1000);
  twolame_set_energy_levels(twolame_lameopts[card][stream],1);
  if(twolame_init_params(twolame_lameopts[card][stream])!=0) {
    syslog(LOG_WARNING,"invalid twolame parameters, card=%d, stream=%d, chans=%d, samprate=%d  bitrate=%d",
	   card,stream,chans,samprate,bitrate);
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
  if((mad_handle=dlopen("libmad.so.0",RTLD_NOW))==NULL) {
    syslog(LOG_INFO,
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
  syslog(LOG_INFO,
	 "Found MAD decoder library, MPEG L2 decoding supported");
  return true;
#else
  syslog(LOG_INFO,"MPEG L2 decoding not enabled");
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
  QList<int> ids=cae_server->connectionIds();

  for(int l=0;l<ids.size();l++) {
    if((cae_server->meterPort(ids.at(l))>0)&&
       cae_server->metersEnabled(ids.at(l),cardnum)) {
      SendMeterUpdate(QString().sprintf("ML %s %d %d %d %d",
		      (const char *)type,cardnum,portnum,levels[0],levels[1]),
		      ids.at(l));
    }
  }
}


void MainObject::SendStreamMeterLevelUpdate(int cardnum,int streamnum,
					    short levels[])
{
  QList<int> ids=cae_server->connectionIds();

  for(int l=0;l<ids.size();l++) {
    if((cae_server->meterPort(ids.at(l))>0)&&
       cae_server->metersEnabled(ids.at(l),cardnum)) {
      SendMeterUpdate(QString().sprintf("MO %d %d %d %d",
		  cardnum,streamnum,levels[0],levels[1]),ids.at(l));
    }
  }
}


void MainObject::SendMeterPositionUpdate(int cardnum,unsigned pos[])
{
  QList<int> ids=cae_server->connectionIds();

  for(unsigned k=0;k<RD_MAX_STREAMS;k++) {
    for(int l=0;l<ids.size();l++) {
      if((cae_server->meterPort(ids.at(l))>0)&&
	 cae_server->metersEnabled(ids.at(l),cardnum)) {
	SendMeterUpdate(QString().sprintf("MP %d %d %d",cardnum,k,pos[k]),
			ids.at(l));
      }
    }
  }
}


void MainObject::SendMeterOutputStatusUpdate()
{
  QList<int> ids=cae_server->connectionIds();

  for(unsigned i=0;i<RD_MAX_CARDS;i++) {
    if(cae_driver[i]!=RDStation::None) {
      for(unsigned j=0;j<RD_MAX_PORTS;j++) {
	for(unsigned k=0;k<RD_MAX_STREAMS;k++) {
	  for(int l=0;l<ids.size();l++) {
	    if((cae_server->meterPort(ids.at(l))>0)&&
	       cae_server->metersEnabled(ids.at(l),i)) {
	      SendMeterUpdate(QString().sprintf("MS %d %d %d %d",i,j,k,
				      output_status_flag[i][j][k]),ids.at(l));
	    }
	  }
	}
      }
    }
  }
}


void MainObject::SendMeterOutputStatusUpdate(int card,int port,int stream)
{
  QList<int> ids=cae_server->connectionIds();

  for(int l=0;l<ids.size();l++) {
    if((cae_server->meterPort(ids.at(l))>0)&&
       cae_server->metersEnabled(ids.at(l),card)) {
      SendMeterUpdate(QString().sprintf("MS %d %d %d %d",card,port,stream,
			    output_status_flag[card][port][stream]),ids.at(l));
    }
  }
}

void MainObject::SendMeterUpdate(const QString &msg,int conn_id)
{
  meter_socket->writeBlock(msg,msg.length(),cae_server->peerAddress(conn_id),
			   cae_server->meterPort(conn_id));
}


int main(int argc,char *argv[])
{
  int rc;
  QCoreApplication a(argc,argv,false);
  new MainObject(NULL,"main");
  rc=a.exec();
  syslog(LOG_DEBUG,"cae post a.exec() rc: %d",rc);
  return rc;
}
