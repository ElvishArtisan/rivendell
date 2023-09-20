// cae.cpp
//
// The Core Audio Engine component of Rivendell
//
//   (C) Copyright 2002-2023 Fred Gleason <fredg@paravelsystems.com>
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

#include <QCoreApplication>
#include <QDir>

#include <rdapplication.h>
#include <rdaudio_port.h>
#include <rdcmd_switch.h>
#include <rdconf.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdsocket.h>
#include <rdsvc.h>
#include <rdsystem.h>

#include "cae.h"
#include "driver_alsa.h"
#include "driver_hpi.h"
#include "driver_jack.h"

volatile bool exiting=false;

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


MainObject::MainObject(QObject *parent)
  :QObject(parent)
{
  QString err_msg;
  RDCoreApplication::ErrorType err_type=RDCoreApplication::ErrorOk;

  //
  // Open Database
  //
  rda=static_cast<RDApplication *>(new RDCoreApplication("caed","caed",
							 CAED_USAGE,false,this));
  if(!rda->open(&err_msg,&err_type,false,true)) {
    fprintf(stderr,"caed: %s\n",err_msg.toUtf8().constData());
    exit(1);
  }
  rda->config()->setModuleName("caed");
  rda->syslog(LOG_INFO,"cae starting");

  //
  // Initialize Data Structures
  //
  debug=false;
  twolame_handle=NULL;
  mad_handle=NULL;
  if(qApp->arguments().size()>1) {
    for(int i=1;i<qApp->arguments().size();i++) {
      if(qApp->arguments().at(i)=="-d") {
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

  //
  // Server Front End
  //
  cae_server=new CaeServer(this);
  if(!cae_server->bind(QHostAddress::Any,RD_CAED_PORT)) {
    rda->syslog(LOG_ERR,"caed: failed to bind port %d",RD_CAED_PORT);
    exit(1);
  }
  //  connect(cae_server,SIGNAL(connectionDropped(int)),
  //	  this,SLOT(connectionDroppedData(int)));
  connect(cae_server,SIGNAL(connectionClosed(const SessionId &)),
	  this,SLOT(connectionClosedData(const SessionId &)));
  connect(cae_server,SIGNAL(playPositionReq(const SessionId &,int)),
	  this,SLOT(playPositionData(const SessionId &,int)));
  connect(cae_server,SIGNAL(startPlaybackReq(const SessionId &,const QString &,
					     unsigned,unsigned,int,int,int)),
	  this,SLOT(startPlaybackData(const SessionId &,const QString &,
				      unsigned,unsigned,int,int,int)));
  connect(cae_server,SIGNAL(playStopReq(const SessionId &)),
	  this,SLOT(stopPlaybackData(const SessionId &)));
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
	  SIGNAL(setOutputPortReq(int,unsigned,unsigned,unsigned,int)),
	  this,
	  SLOT(setOutputPortData(int,unsigned,unsigned,unsigned,int)));
  connect(cae_server,
	  SIGNAL(setOutputVolumeReq(int,unsigned,unsigned,int,int)),
	  this,
	  SLOT(setOutputVolumeData(int,unsigned,unsigned,int,int)));
  connect(cae_server,SIGNAL(fadeOutputVolumeReq(int,unsigned,unsigned,
						unsigned,int,unsigned)),
	  this,SLOT(fadeOutputVolumeData(int,unsigned,unsigned,
					 unsigned,int,unsigned)));
  /*
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
  connect(cae_server,
	  SIGNAL(getInputStatusReq(const SessionTuple &,unsigned,unsigned)),
	  this,
	  SLOT(getInputStatusData(const SessionTuple &,unsigned,unsigned)));
  */
  connect(cae_server,
	  SIGNAL(setAudioPassthroughLevelReq(const QHostAddress &,
					     unsigned,unsigned,unsigned,int)),
	  this,
	  SLOT(setAudioPassthroughLevelData(const QHostAddress &,
					    unsigned,unsigned,unsigned,int)));
  connect(cae_server,SIGNAL(updateAudioPorts()),
	  this,SLOT(updateAudioPortsData()));
  /*
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
  */
  connect(cae_server,SIGNAL(meterEnableReq(const QHostAddress &,uint16_t,
					   const QList<unsigned> &)),
	  this,SLOT(meterEnableData(const QHostAddress &,uint16_t,
				    const QList<unsigned> &)));
  signal(SIGHUP,SigHandler);
  signal(SIGINT,SigHandler);
  signal(SIGTERM,SigHandler);

  //
  // Meter Socket
  //
  meter_socket=new QUdpSocket(this);

  //
  // Provisioning
  //
  InitProvisioning();


  //
  // Audio Driver Backend
  //
  system_sample_rate=rda->system()->sampleRate();
  ClearDriverEntries();
  unsigned next_card=0;
  MakeDriver(&next_card,RDStation::Hpi);
  MakeDriver(&next_card,RDStation::Alsa);
  MakeDriver(&next_card,RDStation::Jack);

  //
  // Probe Capabilities
  //
  ProbeCaps(rda->station());

  //
  // Close Database Connection
  //
  rda->station()->setScanned(true);

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
  if(rda->config()->useRealtime()) {
    if(!jack_running) {
      sched_params.sched_priority=rda->config()->realtimePriority();
    }
    sched_policy=SCHED_FIFO;
    if(sched_params.sched_priority>sched_get_priority_min(sched_policy)) {
      sched_params.sched_priority--;
    }
    int r=pthread_setschedparam(pthread_self(),sched_policy,&sched_params);
    if(r) {
      result=r;
    }
    mlockall(MCL_CURRENT|MCL_FUTURE);
    if(result) {
      rda->syslog(LOG_WARNING,
			    "unable to set realtime scheduling: %s",
	     strerror(result));
    }
    else {
      rda->syslog(LOG_DEBUG,
			    "using realtime scheduling, priority=%d",
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
  if(rda->config()->enableMixerLogging()) {
    rda->syslog(LOG_INFO,"mixer logging enabled");
  }
  if(rda->config()->testOutputStreams()) {
    rda->syslog(LOG_INFO,"output stream testing enabled");
  }
  rda->syslog(LOG_INFO,"cae started");
}


//
// New Slots
//
void MainObject::startPlaybackData(const SessionId &sid,const QString &cutname,
				   unsigned cardnum,unsigned portnum,
				   int start_pos,int end_pos,int speed)
{
  Driver *dvr=NULL;

  //
  // Find the card
  //
  if((dvr=GetDriver(cardnum))==NULL) {
    rda->syslog(LOG_WARNING,"no such card - session: %s  card: %d",
		sid.dump().toUtf8().constData(),cardnum);
    return;
  }

  //
  // Load
  //
  int streamnum=-1;
  QString wavename=rda->config()->audioFileName(cutname).trimmed();
  if(!dvr->loadPlayback(cardnum,wavename,&streamnum)) {
    rda->syslog(LOG_WARNING,"no available play stream - session: %d  card: %d",
		sid.dump().toUtf8().constData(),cardnum);
    return;
  }

  //
  // Position
  //
  if(!dvr->playbackPosition(cardnum,streamnum,start_pos)) {
    rda->syslog(LOG_WARNING,"invalid start position - session: %s  card: %d  stream: %d  position: %d",
		sid.dump().toUtf8().constData(),cardnum,streamnum,start_pos);
  }

  //
  // Set Mixer
  //
  if(!dvr->setOutputVolume(cardnum,streamnum,portnum,0)) {
    rda->syslog(LOG_WARNING,"set output volume failed - session: %s  card: %d  port: %d  stream: %d  level: 0",
		sid.dump().toUtf8().constData(),cardnum,portnum,streamnum);
  }

  //
  // Start the transport
  //
  if(!dvr->play(cardnum,streamnum,end_pos-start_pos,speed,false,false)) {
    rda->syslog(LOG_WARNING,
		"play start failed - session: %s  card: %d  stream: %d",
		sid.dump().toUtf8().constData(),cardnum,streamnum);
    dvr->unloadPlayback(cardnum,streamnum);
    return;
  }

  //
  // Create session record
  //
  Session *sess=new Session(sid);
  sess->setCardNumber(cardnum);
  sess->setPortNumber(portnum);
  sess->setStreamNumber(streamnum);
  sess->setStartPosition(start_pos);
  sess->setEndPosition(end_pos);
  sess->setSpeed(speed);
  cae_play_sessions[sid]=sess;

  rda->syslog(LOG_DEBUG,"playback started - session: %s  card: %d  port: %d  stream: %d  filename: %s",
	      sid.dump().toUtf8().constData(),cardnum,portnum,streamnum,
	      wavename.toUtf8().constData());
}


void MainObject::playPositionData(const SessionId &sid,int position)
{
  Session *sess=NULL;
  Driver *dvr=NULL;

  //
  // Find the session
  //
  if((sess=cae_play_sessions.value(sid))==NULL) {
    rda->syslog(LOG_WARNING,
		"attempting to position non-existent session - session: %s",
		sid.dump().toUtf8().constData());
    return;
  }

  //
  // Find the card
  //
  if((dvr=GetDriver(sess->cardNumber()))==NULL) {
    rda->syslog(LOG_WARNING,"no such card - session: %s  card: %d",
		sid.dump().toUtf8().constData(),sess->cardNumber());
    return;
  }

  if(!dvr->playbackPosition(sess->cardNumber(),sess->streamNumber(),position)) {
    rda->syslog(LOG_WARNING,"play position failed - session: %s  position: %d",
		sid.dump().toUtf8().constData(),position);
    return;
  }
  cae_server->sendCommand(sid,QString::asprintf("PP %d %d",
						sid.serialNumber(),
						position));
  rda->syslog(LOG_DEBUG,"play position succeeded - session: %s  position: %d",
		sid.dump().toUtf8().constData(),position);
}


void MainObject::stopPlaybackData(const SessionId &sid)
{
  //
  // Maintainer's Note:
  // *Do not* call this from an iterator! Use StopPlayout() instead.
  //
  Session *sess=NULL;

  //
  // Find the session
  //
  if((sess=cae_play_sessions.value(sid))==NULL) {
    rda->syslog(LOG_WARNING,
		"attempting to stop playing non-existent session - session: %s",
		sid.dump().toUtf8().constData());
    return;
  }

  StopPlayout(sess);

  //
  // Delete session
  //
  QString msg=QString::asprintf("stopped playback - session: %s",
				sid.dump().toUtf8().constData());
  delete sess;
  cae_play_sessions.remove(sid);
  rda->syslog(LOG_DEBUG,"%s",msg.toUtf8().constData());
}


//
// Old Slots
//
void MainObject::loadPlaybackData(int id,unsigned card,const QString &name)
{
  /*
  QString wavename;
  int new_stream=-1;
  int handle;
  Driver *dvr=GetDriver(card);

  if(dvr==NULL) {
    cae_server->
      sendCommand(id,QString::asprintf("LP %d %s -1 -1 -!",card,
				       name.toUtf8().constData()));
    return;
  }
  wavename=rda->config()->audioFileName(name);
  if(!dvr->loadPlayback(card,wavename,&new_stream)) {
    cae_server->
      sendCommand(id,QString::asprintf("LP %d %s -1 -1 -!",card,
				       name.toUtf8().constData()));
    rda->syslog(LOG_WARNING,
			  "unable to allocate stream for card %d",card);
    return;
  }
  if((handle=GetHandle(card,new_stream))>=0) {
    rda->syslog(LOG_WARNING,
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
  rda->syslog(LOG_INFO,
		     "LoadPlayback  Card: %d  Stream: %d  Name: %s  Handle: %d",
	 card,new_stream,(const char *)wavename.toUtf8(),handle);
  cae_server->
    sendCommand(id,QString::asprintf("LP %d %s %d %d +!",card,
				     (const char *)name.toUtf8(),
				     new_stream,handle));
  */
}


void MainObject::unloadPlaybackData(int id,unsigned handle)
{
  /*
  int card=play_handle[handle].card;
  int stream=play_handle[handle].stream;
  Driver *dvr=GetDriver(card);

  if(dvr==NULL) {
    cae_server->sendCommand(id,QString::asprintf("UP %d -!",handle));
    return;
  }
  if((play_owner[card][stream]==-1)||(play_owner[card][stream]==id)) {
    if(dvr->unloadPlayback(card,stream)) {
      if(!rda->config()->testOutputStreams()) {
	for(int i=0;i<RD_MAX_PORTS;i++) {
	  dvr->setOutputVolume(card,stream,i,RD_MUTE_DEPTH);  // Clear mixer
	}
      }
      play_owner[card][stream]=-1;
      rda->syslog(LOG_INFO,"UnloadPlayback - Card: %d  Stream: %d  Handle: %d",
		  card,stream,handle);
      cae_server->sendCommand(id,QString::asprintf("UP %d +!",handle));
    }
    else {
      cae_server->sendCommand(id,QString::asprintf("UP %d -!",handle));
    }
    play_handle[handle].card=-1;
    play_handle[handle].stream=-1;
    play_handle[handle].owner=-1;
    return;
  }
  else {
    cae_server->sendCommand(id,QString::asprintf("UP %d -!",handle));
  }
  */
}


void MainObject::playPositionData(int id,unsigned handle,unsigned pos)
{
  /*
  int card=play_handle[handle].card;
  int stream=play_handle[handle].stream;
  Driver *dvr=GetDriver(card);

  if(dvr==NULL) {
    cae_server->sendCommand(id,QString::asprintf("PP %d %d -!",handle,pos));
    return;
  }
  if(play_owner[card][stream]==id) {
    if(dvr->playbackPosition(card,stream,pos)) {
      rda->syslog(LOG_INFO,
		 "PlaybackPosition - Card: %d  Stream: %d  Pos: %d  Handle: %d",
		  card,stream,pos,handle);
      cae_server->sendCommand(id,QString::asprintf("PP %d %d +!",handle,pos));
    }
    else {
      cae_server->sendCommand(id,QString::asprintf("PP %d %d -!",handle,pos));
    }
    return;
  }
  cae_server->sendCommand(id,QString::asprintf("PP %d %d -!",handle,pos));
  */
}


void MainObject::playData(int id,unsigned handle,unsigned length,unsigned speed,
			  unsigned pitch_flag)
{
  /*
  int card=play_handle[handle].card;
  int stream=play_handle[handle].stream;
  Driver *dvr=GetDriver(card);

  if(dvr==NULL) {
    cae_server->
      sendCommand(id,QString::asprintf("PY %u %u %u %u -!",
				       handle,length,speed,pitch_flag));
    return;
  }
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
      sendCommand(id,QString::asprintf("PY %u %u %u %u -!",
				       handle,length,speed,pitch_flag));
    return;
  }
  if(play_owner[card][stream]==id) {
    if(!dvr->play(card,stream,play_length[card][stream],
		  play_speed[card][stream],play_pitch[card][stream],
		  RD_ALLOW_NONSTANDARD_RATES)) {
      cae_server->
	sendCommand(id,QString::asprintf("PY %u %u %u %u -!",
					 handle,length,speed,pitch_flag));
      return;
    }
    rda->syslog(LOG_INFO,
		"Play - Card: %d  Stream: %d  Handle: %d  Length: %d  Speed: %d  Pitch: %d",
		card,stream,handle,play_length[card][stream],
		play_speed[card][stream],pitch_flag);
    // No command echo for success -- statePlayUpdate() sends it!
    return;
  }
  cae_server->
    sendCommand(id,QString::asprintf("PY %u %u %u %u -!",
				     handle,length,speed,pitch_flag));
  */
}


void MainObject::stopPlaybackData(int id,unsigned handle)
{
  /*
  int card=play_handle[handle].card;
  int stream=play_handle[handle].stream;
  Driver *dvr=GetDriver(card);

  if(dvr==NULL) {
    cae_server->sendCommand(id,QString::asprintf("SP %u -!",handle));
    return;
  }
  if(play_owner[card][stream]==id) {
    if(!dvr->stopPlayback(card,stream)) {
      cae_server->sendCommand(id,QString::asprintf("SP %u -!",handle));
      return;
    }
    rda->syslog(LOG_INFO,
			  "StopPlayback - Card: %d  Stream: %d  Handle: %d",
	   card,stream,handle);
    return;
  }
  cae_server->sendCommand(id,QString::asprintf("SP %u -!",handle));
  */
}


void MainObject::timescalingSupportData(int id,unsigned card)
{
  /*
  bool state=false;
  Driver *dvr=GetDriver(card);

  if(dvr==NULL) {
    return;
  }
  state=dvr->timescaleSupported(card);
  if(state) {
    cae_server->sendCommand(id,QString::asprintf("TS %u +!",card));
  }
  else {
    cae_server->sendCommand(id,QString::asprintf("TS %u -!",card));
  }
  */
}


void MainObject::loadRecordingData(int id,unsigned card,unsigned port,
				   unsigned coding,unsigned channels,
				   unsigned samprate,unsigned bitrate,
				   const QString &name)
{
  /*
  QString wavename;
  Driver *dvr=GetDriver(card);

  if(dvr==NULL) {
    cae_server->
      sendCommand(id,QString::asprintf("LR %u %u %u %u %u %u %s -!",
				       card,port,coding,channels,samprate,
				       bitrate,(const char *)name.toUtf8()));
    return;
  }
  if(record_owner[card][port]==-1) {
    wavename=rda->config()->audioFileName(name);
    unlink(wavename.toUtf8());  // So we don't trainwreck any current playouts!
    unlink((wavename+".energy").toUtf8());
    if(!dvr->loadRecord(card,port,coding,channels,samprate,bitrate,wavename)) {
      cae_server->
	sendCommand(id,QString::asprintf("LR %u %u %u %u %u %u %s -!",
					 card,port,coding,channels,samprate,
					 bitrate,(const char *)name.toUtf8()));
      return;
    }
    rda->syslog(LOG_INFO,
	   "LoadRecord - Card: %d  Stream: %d  Coding: %d  Chans: %d  SampRate: %d  BitRate: %d  Name: %s",
	   card,port,coding,channels,samprate,bitrate,
	   (const char *)wavename.toUtf8());
    record_owner[card][port]=id;
    cae_server->
      sendCommand(id,QString::asprintf("LR %u %u %u %u %u %u %s +!",
				       card,port,coding,channels,samprate,
				       bitrate,(const char *)name.toUtf8()));
  }
  else {
    cae_server->
      sendCommand(id,QString::asprintf("LR %u %u %u %u %u %u %s -!",
				       card,port,coding,channels,samprate,
				       bitrate,(const char *)name.toUtf8()));
  }
  */
}


void MainObject::unloadRecordingData(int id,unsigned card,unsigned stream)
{
  /*
  Driver *dvr=GetDriver(card);

  if(dvr==NULL) {
    cae_server->sendCommand(id,QString::asprintf("UR %u %u -!",card,stream));
    return;
  }
  if((record_owner[card][stream]==-1)||(record_owner[card][stream]==id)) {
    unsigned len=0;
    if(!dvr->unloadRecord(card,stream,&len)) {
      cae_server->sendCommand(id,QString::asprintf("UR %u %u -!",card,stream));
      return;
    }
    record_owner[card][stream]=-1;
    rda->syslog(LOG_INFO,
			  "UnloadRecord - Card: %d  Stream: %d, Length: %u",
	   card,stream,len);
    cae_server->
      sendCommand(id,QString::asprintf("UR %u %u %u +!",card,stream,
		   (unsigned)((double)len*1000.0/(double)system_sample_rate)));
    return;
  }
  else {
    cae_server->sendCommand(id,QString::asprintf("UR %u %u -!",card,stream));
    return;
  }
  */
}


void MainObject::recordData(int id,unsigned card,unsigned stream,unsigned len,
			    int threshold_level)
{
  /*
  Driver *dvr=GetDriver(card);

  if(dvr==NULL) {
    cae_server->
      sendCommand(id,QString::asprintf("RD %u %u %u %d -!",
				       card,stream,len,threshold_level));
    return;
  }
  record_length[card][stream]=len;
  record_threshold[card][stream]=threshold_level;
  if(record_owner[card][stream]==id) {
    if(!dvr->record(card,stream,record_length[card][stream],
		    record_threshold[card][stream])) {
      cae_server->
	sendCommand(id,QString::asprintf("RD %u %u %u %d -!",
					 card,stream,len,threshold_level));
      return;
    }
    rda->syslog(LOG_INFO,
			"Record - Card: %d  Stream: %d  Length: %d  Thres: %d",
	   card,stream,record_length[card][stream],
	   record_threshold[card][stream]);
    // No positive echo required here!
    return;
  }
  cae_server->
    sendCommand(id,QString::asprintf("RD %u %u %u %d -!",
				     card,stream,len,threshold_level));
  */
}


void MainObject::stopRecordingData(int id,unsigned card,unsigned stream)
{
  /*
  Driver *dvr=GetDriver(card);

  if(dvr==NULL) {
    cae_server->sendCommand(id,QString::asprintf("SR %u %u -!",card,stream));
    return;
  }
  if(!dvr->stopRecord(card,stream)) {  // No positive echo required here!
    cae_server->sendCommand(id,QString::asprintf("SR %u %u -!",card,stream));
    return;
  }
  rda->syslog(LOG_INFO,"StopRecord - Card: %d  Stream: %d",card,stream);
  */
}


void MainObject::setInputVolumeData(int id,unsigned card,unsigned stream,
				    int level)
{
  /*
  Driver *dvr=GetDriver(card);

  if(dvr==NULL) {
    cae_server->
      sendCommand(id,QString::asprintf("IV %u %u %d -!",card,stream,level));
    return;
  }
  if(!dvr->setInputVolume(card,stream,level)) {
    cae_server->
      sendCommand(id,QString::asprintf("IV %u %u %d -!",card,stream,level));
    return;
  }
  if(rda->config()->enableMixerLogging()) {
    rda->syslog(LOG_INFO,
			  "SetInputVolume - Card: %d  Stream: %d Level: %d",
			  card,stream,level);
  }
  cae_server->
    sendCommand(id,QString::asprintf("IV %u %u %d +!",card,stream,level));
  */
}


void MainObject::setOutputPortData(int id,unsigned card,unsigned stream,
				   unsigned port,int level)
{
  /*
  Driver *dvr=GetDriver(card);

  if(dvr==NULL) {
    cae_server->sendCommand(id,QString::asprintf("OP %u %u %u %d -!",
						 card,stream,port,level));
    return;
  }
  if(!rda->config()->testOutputStreams()) {
    for(unsigned i=0;i<RD_MAX_STREAMS;i++) {
      if(i==port) {
	dvr->setOutputVolume(card,i,port,level);
      }
      else {
	dvr->setOutputVolume(card,i,port,RD_FADE_DEPTH);
      }
    }
    if(rda->config()->enableMixerLogging()) {
      rda->syslog(LOG_INFO,
		  "SetOutputPort - Card: %d  Stream: %d  Port: %d  Level: %d",
		  card,stream,port,level);
    }
  }
  cae_server->sendCommand(id,QString::asprintf("OV %u %u %u %d +!",
					       card,stream,port,level));
  */
}


void MainObject::setOutputVolumeData(int id,unsigned card,unsigned stream,
				     int port,int level)
{
  /*
  Driver *dvr=GetDriver(card);

  if(dvr==NULL) {
    cae_server->sendCommand(id,QString::asprintf("OV %u %u %u %d -!",
						 card,stream,port,level));
    return;
  }
  if(!rda->config()->testOutputStreams()) {
    if(port>=0) {
      if(!dvr->setOutputVolume(card,stream,port,level)) {
	cae_server->sendCommand(id,QString::asprintf("OV %u %u %u %d -!",
						     card,stream,port,level));
	return;
      }
    }
    else {
      for(int i=0;i<RD_MAX_PORTS;i++) {
	dvr->setOutputVolume(card,stream,i,level);
      }
    }
    if(rda->config()->enableMixerLogging()) {
      rda->syslog(LOG_INFO,
		  "SetOutputVolume - Card: %d  Stream: %d  Port: %d  Level: %d",
		  card,stream,port,level);
    }
  }
  cae_server->sendCommand(id,QString::asprintf("OV %u %u %u %d +!",
					       card,stream,port,level));
  */
}


void MainObject::fadeOutputVolumeData(int id,unsigned card,unsigned stream,
				      unsigned port,int level,unsigned length)
{
  /*
  Driver *dvr=GetDriver(card);

  if(dvr==NULL) {
    cae_server->
      sendCommand(id,QString::asprintf("FV %u %u %u %d %u -!",
				       card,stream,port,level,length));
    return;
  }
  if(!rda->config()->testOutputStreams()) {
    if(!dvr->fadeOutputVolume(card,stream,port,level,length)) {
      cae_server->
	sendCommand(id,QString::asprintf("FV %u %u %u %d %u -!",
					 card,stream,port,level,length));
      return;
    }
    if(rda->config()->enableMixerLogging()) {
      rda->syslog(LOG_INFO,
		  "FadeOutputVolume - Card: %d  Stream: %d  Port: %d  Level: %d  Length: %d",
		  card,stream,port,level,length);
    }
  }
  cae_server->
    sendCommand(id,QString::asprintf("FV %u %u %u %d %u +!",
				     card,stream,port,level,length));
  */
}


void MainObject::setInputLevelData(int id,unsigned card,unsigned port,
				   int level)
{
  /*
  Driver *dvr=GetDriver(card);

  if(dvr==NULL) {
    cae_server->sendCommand(id,QString::asprintf("IL %u %u %d -!",
						 card,port,level));
    return;
  }

  if(!dvr->setInputLevel(card,port,level)) {
    cae_server->sendCommand(id,QString::asprintf("IL %u %u %d -!",
						 card,port,level));
    return;
  }
  if(rda->config()->enableMixerLogging()) {
    rda->syslog(LOG_INFO,
			  "SetInputLevel - Card: %d  Port: %d  Level: %d",
			  card,port,level);
  }
  cae_server->sendCommand(id,QString::asprintf("IL %u %u %d +!",
					       card,port,level));
  */
}


void MainObject::setOutputLevelData(int id,unsigned card,unsigned port,
				    int level)
{
  /*
  Driver *dvr=GetDriver(card);

  if(dvr==NULL) {
    cae_server->sendCommand(id,QString::asprintf("OL %u %u %d -!",
						 card,port,level));
    return;
  }
  if(!dvr->setOutputLevel(card,port,level)) {
    cae_server->sendCommand(id,QString::asprintf("OL %u %u %d -!",
						 card,port,level));
    return;
  }
  if(rda->config()->enableMixerLogging()) {
    rda->syslog(LOG_INFO,
			  "SetOutputLevel - Card: %d  Port: %d  Level: %d",
			  card,port,level);
  }
  cae_server->sendCommand(id,QString::asprintf("OL %u %u %d +!",
					       card,port,level));
  */
}


void MainObject::setInputModeData(int id,unsigned card,unsigned stream,
				  unsigned mode)
{
  /*
  Driver *dvr=GetDriver(card);

  if(dvr==NULL) {
    cae_server->sendCommand(id,QString::asprintf("IM %u %u %u -!",
						 card,stream,mode));
    return;
  }
  if(!dvr->setInputMode(card,stream,mode)) {
    cae_server->sendCommand(id,QString::asprintf("IM %u %u %u -!",
						 card,stream,mode));
    return;
  }
  if(rda->config()->enableMixerLogging()) {
    rda->syslog(LOG_INFO,
			  "SetInputMode - Card: %d  Stream: %d  Mode: %d",
	   card,stream,mode);
  }
  cae_server->sendCommand(id,QString::asprintf("IM %u %u %u +!",
					       card,stream,mode));
  */
}


void MainObject::setOutputModeData(int id,unsigned card,unsigned stream,
				   unsigned mode)
{
  /*
  Driver *dvr=GetDriver(card);

  if(dvr==NULL) {
    cae_server->sendCommand(id,QString::asprintf("OM %u %u %u -!",
						 card,stream,mode));
    return;
  }
  if(!dvr->setOutputMode(card,stream,mode)) {
    cae_server->sendCommand(id,QString::asprintf("OM %u %u %u -!",
						 card,stream,mode));
    return;
  }
  if(rda->config()->enableMixerLogging()) {
    rda->syslog(LOG_INFO,
			  "SetOutputMode - Card: %d  Stream: %d  Mode: %d",
			  card,stream,mode);
  }
  cae_server->sendCommand(id,QString::asprintf("OM %u %u %u +!",
					       card,stream,mode));
  */
}


void MainObject::setInputVoxLevelData(int id,unsigned card,unsigned stream,
				      int level)
{
  /*
  Driver *dvr=GetDriver(card);

  if(dvr==NULL) {
    cae_server->sendCommand(id,QString::asprintf("IX %u %u %d -!",
						 card,stream,level));
    return;
  }
  if(!dvr->setInputVoxLevel(card,stream,level)) {
    cae_server->sendCommand(id,QString::asprintf("IX %u %u %d -!",
						 card,stream,level));
    return;
  }
  if(rda->config()->enableMixerLogging()) {
    rda->syslog(LOG_INFO,
			  "SetInputVOXLevel - Card: %d  Stream: %d  Level: %d",
			  card,stream,level);
  }
  cae_server->sendCommand(id,QString::asprintf("IX %u %u %d +!",
					       card,stream,level));
  */
}


void MainObject::setInputTypeData(int id,unsigned card,unsigned port,
				  unsigned type)
{
  /*
  Driver *dvr=GetDriver(card);

  if(dvr==NULL) {
    cae_server->sendCommand(id,QString::asprintf("IT %u %u %u -!",
						 card,port,type));
    return;
  }
  if(!dvr->setInputType(card,port,type)) {
    cae_server->sendCommand(id,QString::asprintf("IT %u %u %u -!",
						 card,port,type));
    return;
  }
  if(rda->config()->enableMixerLogging()) {
    rda->syslog(LOG_INFO,
			  "SetInputType - Card: %d  Port: %d  Type: %d",
			  card,port,type);
  }
  cae_server->sendCommand(id,QString::asprintf("IT %u %u %u +!",
					       card,port,type));
  */
}


void MainObject::getInputStatusData(const SessionId &origin,
				    unsigned card,unsigned port)
{
  Driver *dvr=GetDriver(card);

  if(dvr==NULL) {
    return;
  }
  if(dvr->driverType()==RDStation::Hpi) {
    cae_server->sendCommand(origin,
     QString::asprintf("IS %d %d %d",card,port,dvr->getInputStatus(card,port)));
  }
  else {
    cae_server->
      sendCommand(origin,QString::asprintf("IS %d %d 1 +!",card,port));
  }
}


void MainObject::setAudioPassthroughLevelData(const QHostAddress &src_addr,
					      unsigned card,unsigned input,
					      unsigned output,int level)
{
  Driver *dvr=GetDriver(card);

  if(dvr!=NULL) {
    if(dvr->setPassthroughLevel(card,input,output,level)) {
      if(rda->config()->enableMixerLogging()) {
	rda->syslog(LOG_INFO,
		    "SetPassthroughLevel - Host: %s  Card: %d  InPort: %d  OutPort: %d Level: %d",
		    src_addr.toString().toUtf8().constData(),
		    card,input,output,level);
      }
    }
  }
}


void MainObject::updateAudioPortsData()
{
  for(int i=0;i<RD_MAX_CARDS;i++) {
    RDAudioPort *port=new RDAudioPort(rda->config()->stationName(),i);
    Driver *dvr=GetDriver(i);

    if(dvr!=NULL) {
      dvr->setClockSource(i,port->clockSource());
      for(int j=0;j<RD_MAX_PORTS;j++) {
	if(port->inputPortType(j)==RDAudioPort::Analog) {
	  dvr->setInputType(i,j,RDCae::Analog);
	}
	else {
	  dvr->setInputType(i,j,RDCae::AesEbu);
	}
	dvr->setInputLevel(i,j,RD_BASE_ANALOG+port->inputPortLevel(j));
	dvr->setOutputLevel(i,j,RD_BASE_ANALOG+port->outputPortLevel(j));
	dvr->setInputMode(i,j,port->inputPortMode(j));
      }
    }
    delete port;
  }
}


void MainObject::setClockSourceData(int id,unsigned card,int input)
{
  /*
  if((card<0)||(input<0)) {
    cae_server->sendCommand(id,QString::asprintf("CS %u %u -!",card,input));
    return;
  }
  Driver *dvr=GetDriver(card);

  if(dvr==NULL) {
    cae_server->sendCommand(id,QString::asprintf("CS %u %u +!",card,input));
    return;
  }
  if(dvr->driverType()==RDStation::Hpi) {
    if(!dvr->setClockSource(card,input)) {
      cae_server->sendCommand(id,QString::asprintf("CS %u %u -!",card,input));
      return;
    }
  }
  else {
    cae_server->sendCommand(id,QString::asprintf("CS %u %u +!",card,input));
    return;
  }
  if(rda->config()->enableMixerLogging()) {
    rda->syslog(LOG_INFO,
			  "SetClockSource - Card: %d  Source: %d",card,input);
  }
  cae_server->sendCommand(id,QString::asprintf("CS %u %u +!",card,input));
  */
}


void MainObject::setOutputStatusFlagData(int id,unsigned card,unsigned port,
					 unsigned stream,bool state)
{
  /*
  output_status_flag[card][port][stream]=state;
  SendMeterOutputStatusUpdate(card,port,stream);
  cae_server->sendCommand(id,QString::asprintf("OS %u %u %u %u +!",
					       card,port,stream,state));
  */
}


void MainObject::openRtpCaptureChannelData(int id,unsigned card,unsigned port,
					   uint16_t udp_port,unsigned samprate,
					   unsigned chans)
{
}


void MainObject::meterEnableData(const QHostAddress &addr,uint16_t udp_port,
				 const QList<unsigned> &cards)
{
  /*
  QString cmd=QString::asprintf("ME %u",0xFFFF&udp_port);
  for(int i=0;i<cards.size();i++) {
    cmd+=QString::asprintf(" %u",cards.at(i));
  }
  cae_server->setMeterPort(id,udp_port);
  for(int i=0;i<cards.size();i++) {
    if((cards.at(i)<0)||(cards.at(i)>=RD_MAX_CARDS)) {
      cae_server->sendCommand(id,cmd+" -!");
      return;
    }
    cae_server->setMetersEnabled(id,cards.at(i),true);
  }

  //  cae_server->sendCommand(id,cmd+" +!");
  */
  SendMeterOutputStatusUpdate();
}

/*
void MainObject::connectionDroppedData(int id)
{
  KillSocket(id);
}
*/


void MainObject::connectionClosedData(const SessionId &sid)
{
  rda->syslog(LOG_DEBUG,"cleaning up sessions from %s",
	      sid.dump().toUtf8().constData());

  //
  // Clean up active play sessions
  //
  QMutableMapIterator<SessionId,Session *> it(cae_play_sessions);
  while(it.hasNext()) {
    it.next();
    if(it.key().belongsTo(sid)) {
      StopPlayout(it.value());
      delete it.value();
      it.remove();
    }
  }

  //
  // Clean up active record sessions
  //
  // FIXME: Implement this!
}


void MainObject::statePlayUpdate(int card,int stream,int state)
{
  if(state==0) {  // Stopped
    for(QMap<SessionId,Session *>::iterator it=cae_play_sessions.begin();
	it!=cae_play_sessions.end();it++) {
      if((it.value()->cardNumber()==card)&&
	 (it.value()->streamNumber()==stream)) {
	cae_server->
	  sendCommand(it.key(),QString::asprintf("SP %d",
						 it.key().serialNumber()));
	delete it.value();
	cae_play_sessions.remove(it.key());
	return;
      }
    }
  }

  /*
  printf("statePlayUpdate(%d,%d,%d)\n",card,stream,state);
  int handle=GetHandle(card,stream);

  if(handle<0) {
    return;
  }
  if(play_owner[card][stream]!=-1) {
    switch(state) {
    case 1:   // Playing
      cae_server->sendCommand(play_owner[card][stream],
			      QString::asprintf("PY %d %d %d +!",handle,
				      play_length[card][stream],
				      play_speed[card][stream]).toUtf8());
      break;

    case 2:   // Paused
      cae_server->
	sendCommand(play_owner[card][stream],
		    QString::asprintf("SP %d +!",handle).toUtf8());
      break;

    case 0:   // Stopped
      cae_server->
	sendCommand(play_owner[card][stream],
		    QString::asprintf("SP %d +!",handle).toUtf8());
      break;
    }
  }
  */
}


void MainObject::stateRecordUpdate(int card,int stream,int state)
{
  /*
  if(record_owner[card][stream]!=-1) {
    switch(state) {
    case 0:    // Recording
      cae_server->
	sendCommand(record_owner[card][stream],
		    QString::asprintf("RD %d %d %d %d +!",card,stream,
				      record_length[card][stream],
				      record_threshold[card][stream]).toUtf8());
      break;

    case 4:    // Record Started
      cae_server->
	sendCommand(record_owner[card][stream],
		    QString::asprintf("RS %d %d +!",card,stream).toUtf8());
      break;

    case 2:    // Paused
    case 3:    // Stopped
      cae_server->
	sendCommand(record_owner[card][stream],
		    QString::asprintf("SR %d %d +!",card,stream).toUtf8());
      break;
    }
  }
  */
}


void MainObject::updateMeters()
{
  short levels[2];
  unsigned positions[RD_MAX_STREAMS];

  if(exiting) {
    for(int i=0;i<d_drivers.size();i++) {
      delete d_drivers.at(i);
    }
    rda->syslog(LOG_INFO,"cae exiting");
    exit(0);
  }

  //
  // Service Disk Buffers
  //
  for(int i=0;i<d_drivers.size();i++) {
    d_drivers.at(i)->processBuffers();
  }

  for(int i=0;i<RD_MAX_CARDS;i++) {
    Driver *dvr=GetDriver(i);
    if(dvr!=NULL) {
      for(int j=0;j<RD_MAX_PORTS;j++) {
	if(dvr->getInputStatus(i,j)!=port_status[i][j]) {
	  port_status[i][j]=dvr->getInputStatus(i,j);
	  if(port_status[i][j]) {
	    cae_server->sendCommand(QString::asprintf("IS %d %d 0!",i,j));
	  }
	  else {
	    cae_server->sendCommand(QString::asprintf("IS %d %d 1!",i,j));
	  }
	}
	if(dvr->getInputMeters(i,j,levels)) {
	  SendMeterLevelUpdate("I",i,j,levels);
	}
	if(dvr->getOutputMeters(i,j,levels)) {
	  SendMeterLevelUpdate("O",i,j,levels);
	}      
      }
      dvr->getOutputPosition(i,positions);
      SendMeterPositionUpdate(i,positions);
      for(int j=0;j<RD_MAX_STREAMS;j++) {
	if(dvr->getStreamOutputMeters(i,j,levels)) {
	  SendStreamMeterLevelUpdate(i,j,levels);
	}      
      }
    }
  }
}


void MainObject::StopPlayout(Session *sess)
{
  Driver *dvr=NULL;

  //
  // Find the card
  //
  if((dvr=GetDriver(sess->cardNumber()))==NULL) {
    rda->syslog(LOG_WARNING,"no such card - session: %s  card: %d",
		sess->sessionId().dump().toUtf8().constData(),
		sess->cardNumber());
    return;
  }

  //
  // Stop Transport
  //
  if(!dvr->stopPlayback(sess->cardNumber(),sess->streamNumber())) {
    rda->syslog(LOG_WARNING,
		"stop playback failed - session: %s",
		sess->sessionId().dump().toUtf8().constData());
  }

  //
  // Unload
  //
  if(!dvr->unloadPlayback(sess->cardNumber(),sess->streamNumber())) {
    rda->syslog(LOG_WARNING,
		"unload playback failed - session: %s",
		sess->sessionId().dump().toUtf8().constData());
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
  if(rda->config()->provisioningCreateHost()) {
    if(!rda->config()->provisioningHostTemplate().isEmpty()) {
      sql=QString("select `NAME` from `STATIONS` where ")+
	"`NAME`='"+RDEscapeString(rda->config()->stationName())+"'";
      q=new RDSqlQuery(sql);
      if(!q->first()) {
	if(RDStation::create(rda->config()->stationName(),&err_msg,rda->config()->provisioningHostTemplate(),rda->config()->provisioningHostIpAddress())) {
	  rda->syslog(LOG_INFO,
				"created new host entry \"%s\"",
				rda->config()->stationName().toUtf8().constData());
	  if(!rda->config()->provisioningHostShortName(rda->config()->stationName()).
	     isEmpty()) {
	    RDStation *station=new RDStation(rda->config()->stationName());
	    station->setShortName(rda->config()->
		     provisioningHostShortName(rda->config()->stationName()));
	    delete station;
	  }
	}
	else {
	  fprintf(stderr,"caed: unable to provision host [%s]\n",
		  err_msg.toUtf8().constData());
	  exit(256);
	}
      }
      delete q;
    }
  }

  //
  // Provision a Service
  //
  if(rda->config()->provisioningCreateService()) {
    if(!rda->config()->provisioningServiceTemplate().isEmpty()) {
      QString svcname=
	rda->config()->provisioningServiceName(rda->config()->stationName());
      sql=QString("select `NAME` from `SERVICES` where ")+
	"`NAME`='"+RDEscapeString(svcname)+"'";
      q=new RDSqlQuery(sql);
      if(!q->first()) {
	if(RDSvc::create(svcname,&err_msg,
			 rda->config()->provisioningServiceTemplate(),rda->config())) {
	  rda->syslog(LOG_INFO,
				"created new service entry \"%s\"",
				svcname.toUtf8().constData());
	}
	else {
	  fprintf(stderr,"caed: unable to provision service [%s]\n",
		  err_msg.toUtf8().constData());
	  exit(256);
	}
      }
      delete q;
    }
  }
}


void MainObject::InitMixers()
{
  Driver *dvr=NULL;

  updateAudioPortsData();
  for(int i=0;i<RD_MAX_CARDS;i++) {
    if(dvr!=NULL) {
      for(int j=0;j<RD_MAX_PORTS;j++) {
	for(int k=0;k<RD_MAX_PORTS;k++) {
	  dvr->setPassthroughLevel(i,j,k,RD_MUTE_DEPTH);
	}
      }
    }
  }
}

/*
void MainObject::KillSocket(int ch)
{
  for(int i=0;i<RD_MAX_CARDS;i++) {
    Driver *dvr=GetDriver(i);
    for(int j=0;j<RD_MAX_STREAMS;j++) {
      if(record_owner[i][j]==ch) {
	rda->syslog(LOG_DEBUG,"force unloading record context for connection %s:%u: Card: %d  Stream: %d  Handle: %d",
			      cae_server->peerAddress(ch).toString().toUtf8().constData(),
			      0xFFFF&cae_server->peerPort(ch),
			      i,j,GetHandle(i,j));
	unsigned len=0;
	if(dvr!=NULL) {
	  dvr->unloadRecord(i,j,&len);
	}
	record_length[i][j]=0;
	record_threshold[i][j]=-10000;
	record_owner[i][j]=-1;
      }
      if(play_owner[i][j]==ch) {
	rda->syslog(LOG_DEBUG,"force unloading play context for connection %d [%s:%u]: Card: %d  Stream: %d  Handle: %d",
			      ch,
			      cae_server->peerAddress(ch).toString().toUtf8().constData(),
			      0xFFFF&cae_server->peerPort(ch),
			      i,j,GetHandle(i,j));
	if(dvr!=NULL) {
	  dvr->unloadPlayback(i,j);
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
*/

pid_t MainObject::GetPid(QString pidfile)
{
  FILE *handle;
  pid_t ret;

  if((handle=fopen(pidfile.toUtf8(),"r"))==NULL) {
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
  path=QString(RD_PROC_DIR)+QString("/")+QString::asprintf("%d",GetPid(name));
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
}


void MainObject::ClearDriverEntries() const
{
  for(int i=0;i<RD_MAX_CARDS;i++) {
    rda->station()->setCardDriver(i,RDStation::None);
    rda->station()->setCardName(i,"");
    rda->station()->setCardInputs(i,-1);
    rda->station()->setCardOutputs(i,-1);
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
  if((twolame_handle=dlopen("libtwolame.so.0",RTLD_LAZY))==NULL) {
    rda->syslog(LOG_INFO,
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
  rda->syslog(LOG_INFO,
	 "Found TwoLAME encoder library, MPEG L2 encoding supported");
  return true;
#else
  rda->syslog(LOG_INFO,"MPEG L2 encoding not enabled");

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
    rda->syslog(LOG_WARNING,
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
    rda->syslog(LOG_WARNING,
			  "invalid twolame parameters, card=%d, stream=%d, chans=%d, samprate=%d  bitrate=%d",
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
  if((mad_handle=dlopen("libmad.so.0",RTLD_LAZY))==NULL) {
    rda->syslog(LOG_INFO,
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
  rda->syslog(LOG_INFO,
	 "Found MAD decoder library, MPEG L2 decoding supported");
  return true;
#else
  rda->syslog(LOG_INFO,"MPEG L2 decoding not enabled");
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
  for(QMap<SessionId,Session *>::const_iterator it=cae_play_sessions.begin();
      it!=cae_play_sessions.end();it++) {
    if((it.value()->cardNumber()==cardnum)&&
       (cae_server->meterPort(it.key())>0)) {
      SendMeterUpdate(QString::asprintf("ML %s %d %d %d %d",
					type.toUtf8().constData(),
					cardnum,portnum,levels[0],levels[1]),
		      it.key());
    }
  }
}


void MainObject::SendStreamMeterLevelUpdate(int cardnum,int streamnum,
					    short levels[])
{
  for(QMap<SessionId,Session *>::const_iterator it=cae_play_sessions.begin();
      it!=cae_play_sessions.end();it++) {
    if((it.value()->cardNumber()==cardnum)&&
       (cae_server->meterPort(it.key())>0)) {
      SendMeterUpdate(QString::asprintf("MO %d %d %d %d",
					cardnum,streamnum,levels[0],levels[1]),
		      it.key());
    }
  }
}


void MainObject::SendMeterPositionUpdate(int cardnum,unsigned pos[])
{
  for(unsigned k=0;k<RD_MAX_STREAMS;k++) {
    for(QMap<SessionId,Session *>::const_iterator it=cae_play_sessions.begin();
	it!=cae_play_sessions.end();it++) {
      if((it.value()->cardNumber()==cardnum)&&
	 (cae_server->meterPort(it.key())>0)) {
	SendMeterUpdate(QString::asprintf("MP %d %d %d",cardnum,k,pos[k]),
			it.key());
      }
    }
  }
}


void MainObject::SendMeterOutputStatusUpdate()
{
  for(int i=0;i<RD_MAX_CARDS;i++) {
    if(GetDriver(i)!=NULL) {
      for(unsigned j=0;j<RD_MAX_PORTS;j++) {
	for(unsigned k=0;k<RD_MAX_STREAMS;k++) {
	  for(QMap<SessionId,Session *>::const_iterator it=cae_play_sessions.begin();
	      it!=cae_play_sessions.end();it++) {
	    if((it.value()->cardNumber()==i)&&
	       (cae_server->meterPort(it.key())>0)) {
	      SendMeterUpdate(QString::asprintf("MS %d %d %d %d",i,j,k,
						output_status_flag[i][j][k]),
			      it.key());
	    }
	  }
	}
      }
    }
  }
}


void MainObject::SendMeterOutputStatusUpdate(int card,int port,int stream)
{
  for(QMap<SessionId,Session *>::const_iterator it=cae_play_sessions.begin();
      it!=cae_play_sessions.end();it++) {
    if((it.value()->streamNumber()==stream)&&
       (cae_server->meterPort(it.key())>0)) {
      SendMeterUpdate(QString::asprintf("MS %d %d %d %d",card,port,stream,
					output_status_flag[card][port][stream]),
		      it.key());
    }
  }
}


void MainObject::SendMeterUpdate(const QString &msg,const SessionId &sid)
{
  printf("SendMeterUpdate(\"%s\",%s:%u)\n",msg.toUtf8().constData(),
	 sid.address().toString().toUtf8().constData(),
	 0xffff&cae_server->meterPort(sid));
  meter_socket->
    writeDatagram(msg.toUtf8(),sid.address(),cae_server->meterPort(sid));
}


Driver *MainObject::GetDriver(unsigned card) const
{
  for(int i=0;i<d_drivers.size();i++) {
    if(d_drivers.at(i)->hasCard(card)) {
      return d_drivers.at(i);
    }
  }
  return NULL;
}


void MainObject::MakeDriver(unsigned *next_card,RDStation::AudioDriver type)
{
  unsigned first_card=*next_card;
  int initial_output_volume=RD_MUTE_DEPTH;
  Driver *dvr=NULL;

  switch(type) {
  case RDStation::Hpi:
#ifdef HPI
    dvr=new DriverHpi(this);
    rda->station()->setDriverVersion(RDStation::Hpi,"v"+dvr->version());
#else
    rda->station()->setDriverVersion(RDStation::Hpi,"not enabled");
#endif  // HPI
    break;

  case RDStation::Alsa:
#ifdef ALSA
    dvr=new DriverAlsa(this);
    rda->station()->setDriverVersion(RDStation::Alsa,"v"+dvr->version());
#else
    rda->station()->setDriverVersion(RDStation::Alsa,"not enabled");
#endif  // ALSA
    break;

  case RDStation::Jack:
#ifdef JACK
    dvr=new DriverJack(this);
    rda->station()->setDriverVersion(RDStation::Jack,"v"+dvr->version());
#else
    rda->station()->setDriverVersion(RDStation::Jack,"not enabled");
#endif  // JACK
    break;

  case RDStation::None:
    break;
  }
  if(rda->config()->testOutputStreams()) {
    initial_output_volume=0;
  }
  if(dvr!=NULL) {
    if(dvr->initialize(next_card)) {
      connect(dvr,SIGNAL(playStateChanged(int,int,int)),
	      this,SLOT(statePlayUpdate(int,int,int)));
      connect(dvr,SIGNAL(recordStateChanged(int,int,int)),
	      this,SLOT(stateRecordUpdate(int,int,int)));
      d_drivers.push_back(dvr);
      for(unsigned i=first_card;i<*next_card;i++) {
	for(int j=0;j<RD_MAX_STREAMS;j++) {
	  for(int k=0;k<RD_MAX_PORTS;k++) {
	    dvr->setOutputVolume(i,j,k,initial_output_volume);
	  }
	}
      }
    }
    else {
      delete dvr;
    }
  }
}


int main(int argc,char *argv[])
{
  int rc;
  QCoreApplication a(argc,argv,false);
  new MainObject();
  rc=a.exec();
  rda->syslog(LOG_DEBUG,"cae post a.exec() rc: %d",rc);
  return rc;
}
