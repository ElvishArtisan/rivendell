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
#include <rd.h>
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
  for(int i=0;i<RD_MAX_CARDS;i++) {
    for(int j=0;j<RD_MAX_STREAMS;j++) {
      record_length[i][j]=0;
      record_threshold[i][j]=-10000;
      record_owner[i][j]=-1;
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
  cae_server=new CaeServer(rda->config(),this);
  if(!cae_server->listen(QHostAddress::Any,CAED_TCP_PORT)) {
    rda->syslog(LOG_ERR,"caed: failed to bind port %d",CAED_TCP_PORT);
    exit(1);
  }
  connect(cae_server,SIGNAL(connectionDropped(int)),
	  this,SLOT(connectionDroppedData(int)));
  connect(cae_server,
	  SIGNAL(loadPlaybackReq(uint64_t,unsigned,unsigned,const QString &)),
	  this,
	  SLOT(loadPlaybackData(uint64_t,unsigned,unsigned,const QString &)));
  connect(cae_server,SIGNAL(unloadPlaybackReq(uint64_t)),
	  this,SLOT(unloadPlaybackData(uint64_t)));
  connect(cae_server,SIGNAL(playPositionReq(uint64_t,unsigned)),
	  this,SLOT(playPositionData(uint64_t,unsigned)));
  connect(cae_server,SIGNAL(playReq(uint64_t,unsigned,unsigned,unsigned)),
	  this,SLOT(playData(uint64_t,unsigned,unsigned,unsigned)));
  connect(cae_server,SIGNAL(stopPlaybackReq(uint64_t)),
	  this,SLOT(stopPlaybackData(uint64_t)));
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
  connect(cae_server,SIGNAL(setOutputVolumeReq(uint64_t,int)),
	  this,SLOT(setOutputVolumeData(uint64_t,int)));
  connect(cae_server,SIGNAL(fadeOutputVolumeReq(uint64_t,int,unsigned)),
	  this,SLOT(fadeOutputVolumeData(uint64_t,int,unsigned)));
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
	  SIGNAL(openRtpCaptureChannelReq(int,unsigned,unsigned,uint16_t,
					  unsigned,unsigned)),
	  this,
	  SLOT(openRtpCaptureChannelData(int,unsigned,unsigned,uint16_t,
					unsigned,unsigned)));
  connect(cae_server,
	  SIGNAL(meterEnableReq(int,uint16_t,const QList<unsigned> &)),
	  this,
	  SLOT(meterEnableData(int,uint16_t,const QList<unsigned> &)));

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
    rda->syslog(LOG_INFO,"[mixer] logging enabled");
  }
  if(rda->config()->testOutputStreams()) {
    rda->syslog(LOG_INFO,"output stream testing enabled");
  }
  rda->syslog(LOG_INFO,"cae started");
}


void MainObject::loadPlaybackData(uint64_t phandle,unsigned cardnum,
				  unsigned portnum,const QString &name)
{
  QString wavename;
  int new_stream=-1;
  unsigned serial=PlaySession::serialNumber(phandle);
  Driver *dvr=GetDriver(cardnum);

  if(dvr==NULL) {
    cae_server->
      sendCommand(phandle,QString::asprintf("LP %u %u %u %s -!",
					    serial,cardnum,portnum,
					    name.toUtf8().constData()));
    return;
  }
  wavename=rda->config()->audioFileName(name);
  if(dvr->loadPlayback(cardnum,wavename,&new_stream)) {
    play_sessions[phandle]=new PlaySession(phandle,cardnum,portnum,new_stream);
  }
  else {
    cae_server->
      sendCommand(phandle,QString::asprintf("LP %u %u %u %s -!",
					    serial,cardnum,portnum,
					    name.toUtf8().constData()));
    rda->syslog(LOG_WARNING,
			  "unable to allocate stream for card %d",cardnum);
    return;
  }

  //
  // Mute all volume controls for the stream
  //
  for(int i=0;i<dvr->outputPortQuantity(cardnum);i++) {
    dvr->setOutputVolume(cardnum,new_stream,i,RD_MUTE_DEPTH);
  }

  rda->
    syslog(LOG_INFO,"LoadPlayback  Card: %d  Stream: %d  Name: %s  Serial: %u",
	   cardnum,new_stream,wavename.toUtf8().constData(),serial);
  cae_server->
    sendCommand(phandle,QString::asprintf("LP %u %u %u %s +!",
					  serial,cardnum,portnum,
					  name.toUtf8().constData()));
}


void MainObject::unloadPlaybackData(uint64_t phandle)
{
  PlaySession *psess=play_sessions.value(phandle);
  unsigned serial=PlaySession::serialNumber(phandle);

  if(psess==NULL) {
    cae_server->sendCommand(phandle,QString::asprintf("UP %u -!",serial));
    rda->syslog(LOG_WARNING,
		"attempted to unload non-existent session, serial:%u",serial);
  }
  else {
    Driver *dvr=GetDriver(psess->cardNumber());
    if(dvr==NULL) {
      cae_server->sendCommand(phandle,QString::asprintf("UP %u -!",serial));
      rda->syslog(LOG_WARNING,
		  "attempted to access non-existent card, serial: %u card: %u",
		  serial,psess->cardNumber());
    }
    else {
      if(dvr->unloadPlayback(psess->cardNumber(),psess->streamNumber())) {
	if(!rda->config()->testOutputStreams()) {
	  for(int i=0;i<RD_MAX_PORTS;i++) {
	    dvr->setOutputVolume(psess->cardNumber(),psess->streamNumber(),i,
				 RD_MUTE_DEPTH);  // Clear mixer
	  }
	}
	rda->syslog(LOG_INFO,
		    "UnloadPlayback - Card: %d  Stream: %d  Serial: %u",
		    psess->cardNumber(),psess->streamNumber(),
		    psess->serialNumber());
	cae_server->sendCommand(phandle,QString::asprintf("UP %u +!",serial));
      }
      else {
	cae_server->sendCommand(phandle,QString::asprintf("UP %d -!",serial));
	rda->syslog(LOG_WARNING,
	      "failed to unload play session, serial: %u, card: %u  stream: %d",
		    serial,psess->cardNumber(),psess->streamNumber());
      }
      play_sessions.remove(phandle);
    }
  }
}


void MainObject::playPositionData(uint64_t phandle,unsigned pos)
{
  PlaySession *psess=play_sessions.value(phandle);
  unsigned serial=PlaySession::serialNumber(phandle);

  if(psess==NULL) {
    cae_server->sendCommand(phandle,QString::asprintf("PP %u %u -!",
						      serial,pos));
    rda->syslog(LOG_WARNING,
		"attempted to unload non-existent session, serial: %u",serial);
  }
  else {
    Driver *dvr=GetDriver(psess->cardNumber());
    if(dvr==NULL) {
    cae_server->sendCommand(phandle,QString::asprintf("PP %u %u -!",
						      serial,pos));
      rda->syslog(LOG_WARNING,
		  "attempted to access non-existent card, serial: %u pos: %u",
		  serial,pos);
    }
    else {
      if(dvr->playbackPosition(psess->cardNumber(),psess->streamNumber(),pos)) {
	rda->
	  syslog(LOG_DEBUG,
		 "PlaybackPosition - Card: %d  Stream: %d  Pos: %d  Serial: %d",
		 psess->cardNumber(),psess->streamNumber(),pos,serial);
	cae_server->
	  sendCommand(phandle,QString::asprintf("PP %u %u +!",serial,pos));
      }
      else {
	cae_server->
	  sendCommand(phandle,QString::asprintf("PP %u %u -!",serial,pos));
	rda->syslog(LOG_WARNING,"PlaybackPosition failed, serial: %u, pos: %u",
		    serial,pos);
      }
    }
  }
}


void MainObject::playData(uint64_t phandle,unsigned length,unsigned speed,
			  unsigned pitch_flag)
{
  PlaySession *psess=play_sessions.value(phandle);
  unsigned serial=PlaySession::serialNumber(phandle);

  if(psess==NULL) {
    cae_server->
      sendCommand(phandle,QString::asprintf("PY %u %u %u %u -!",
					 serial,length,speed,pitch_flag));
    rda->syslog(LOG_WARNING,
		"attempted to play non-existent session, serial:%u",serial);
  }
  else {
    Driver *dvr=GetDriver(psess->cardNumber());
    if(dvr==NULL) {
    cae_server->
      sendCommand(phandle,QString::asprintf("PY %u %u %u %u -!",
					 serial,length,speed,pitch_flag));
      rda->syslog(LOG_WARNING,
		  "attempted to access non-existent card, serial: %u card: %u",
		  serial,psess->cardNumber());
    }
    else {
      psess->setLength(length);
      psess->setSpeed(speed);
      if(!dvr->play(psess->cardNumber(),psess->streamNumber(),psess->length(),
		    psess->speed(),false,RD_ALLOW_NONSTANDARD_RATES)) {
	cae_server->
	  sendCommand(phandle,QString::asprintf("PY %u %u %u %u -!",
						serial,length,speed,
						pitch_flag));
      }
      else {
	rda->syslog(LOG_INFO,
		    "Play - Card: %d  Stream: %d  Serial: %d  Length: %d  Speed: %d  Pitch: %d",
		    psess->cardNumber(),psess->streamNumber(),serial,
		    psess->length(),psess->speed(),pitch_flag);
	// No command echo for success -- statePlayUpdate() sends it!
      }
    }
  }
}


void MainObject::stopPlaybackData(uint64_t phandle)
{
  PlaySession *psess=play_sessions.value(phandle);
  unsigned serial=PlaySession::serialNumber(phandle);

  if(psess==NULL) {
    cae_server->sendCommand(phandle,QString::asprintf("SP %u -!",serial));
    rda->syslog(LOG_WARNING,
		"attempted to stop non-existent session, serial: %u",serial);
  }
  else {
    Driver *dvr=GetDriver(psess->cardNumber());
    if(dvr==NULL) {
      cae_server->sendCommand(phandle,QString::asprintf("SP %u -!",serial));
      rda->syslog(LOG_WARNING,
		  "attempted to access non-existent card, serial: %u card: %u",
		  serial,psess->cardNumber());
    }
    else {
      if(!dvr->stopPlayback(psess->cardNumber(),psess->streamNumber())) {
	cae_server->sendCommand(phandle,QString::asprintf("SP %u -!",serial));
	return;
      }
      rda->syslog(LOG_INFO,"StopPlayback - Card: %d  Stream: %d  Serial: %d",
		  psess->cardNumber(),psess->streamNumber(),serial);
      return;
    }
  }
  cae_server->sendCommand(phandle,QString::asprintf("SP %u -!",serial));
}


void MainObject::timescalingSupportData(int id,unsigned card)
{
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
}


void MainObject::loadRecordingData(int id,unsigned card,unsigned port,
				   unsigned coding,unsigned channels,
				   unsigned samprate,unsigned bitrate,
				   const QString &name)
{
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
}


void MainObject::unloadRecordingData(int id,unsigned card,unsigned stream)
{
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
}


void MainObject::recordData(int id,unsigned card,unsigned stream,unsigned len,
			    int threshold_level)
{
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
}


void MainObject::stopRecordingData(int id,unsigned card,unsigned stream)
{
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
}


void MainObject::setInputVolumeData(int id,unsigned card,unsigned stream,
				    int level)
{
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
			  "[mixer] SetInputVolume - Card: %d  Stream: %d Level: %d",
			  card,stream,level);
  }
  cae_server->
    sendCommand(id,QString::asprintf("IV %u %u %d +!",card,stream,level));
}


void MainObject::setOutputVolumeData(uint64_t phandle,int level)
{
  PlaySession *psess=play_sessions.value(phandle);
  unsigned serial=PlaySession::serialNumber(phandle);

  if(psess==NULL) {
    cae_server->sendCommand(phandle,QString::asprintf("SP %u -!",serial));
    rda->syslog(LOG_WARNING,
		"attempted to operate non-existent session, serial: %u",serial);
  }
  else {
    Driver *dvr=GetDriver(psess->cardNumber());
    if(dvr==NULL) {
      cae_server->sendCommand(phandle,QString::asprintf("SP %u -!",serial));
      rda->syslog(LOG_WARNING,
		  "attempted to access non-existent card, serial: %u card: %u",
		  serial,psess->cardNumber());
    }
    else {
      if(!rda->config()->testOutputStreams()) {
	if(psess->portNumber()>=0) {
	  if(!dvr->setOutputVolume(psess->cardNumber(),psess->streamNumber(),
				   psess->portNumber(),level)) {
	    cae_server->sendCommand(phandle,QString::asprintf("OV %u %d -!",
							      serial,level));
	    return;
	  }
	}
	/*  RESET SECTION?
	else {
	  for(int i=0;i<RD_MAX_PORTS;i++) {
	    dvr->setOutputVolume(card,stream,i,level);
	  }
	}
	*/
	if(rda->config()->enableMixerLogging()) {
	  rda->syslog(LOG_INFO,
		      "[mixer] SetOutputVolume - Serial: %u  Card: %d  Stream: %d  Port: %d  Level: %d",
		      serial,psess->cardNumber(),psess->streamNumber(),
		      psess->portNumber(),level);
	}
      }
      cae_server->sendCommand(phandle,QString::asprintf("OV %u %d +!",
						   serial,level));
    }
  }
}


void MainObject::fadeOutputVolumeData(uint64_t phandle,int level,
				      unsigned length)
{
  PlaySession *psess=play_sessions.value(phandle);
  unsigned serial=PlaySession::serialNumber(phandle);

  if(psess==NULL) {
    cae_server->sendCommand(phandle,QString::asprintf("FV %u %d %u -!",
						      serial,level,length));
    rda->syslog(LOG_WARNING,
		"attempted to operate non-existent session, serial: %u",serial);
  }
  else {
    Driver *dvr=GetDriver(psess->cardNumber());
    if(dvr==NULL) {
      cae_server->sendCommand(phandle,QString::asprintf("FV %u %d %u -!",
							serial,level,length));
      rda->syslog(LOG_WARNING,
		  "attempted to access non-existent card, serial: %u card: %u",
		  serial,psess->cardNumber());
    }
    else {
      if(!rda->config()->testOutputStreams()) {
	if(!dvr->fadeOutputVolume(psess->cardNumber(),psess->streamNumber(),
				  psess->portNumber(),level,length)) {
	  cae_server->
	    sendCommand(phandle,QString::asprintf("FV %u %d %u -!",
						  serial,level,length));
	  return;
	}
	if(rda->config()->enableMixerLogging()) {
	  rda->syslog(LOG_INFO,
		      "[mixer] FadeOutputVolume - Serial: %u  Card: %d  Stream: %d  Port: %d  Level: %d  Length: %d",
		      serial,psess->cardNumber(),psess->streamNumber(),
		      psess->portNumber(),level,length);
	}
      }
      cae_server->
	sendCommand(phandle,QString::asprintf("FV %u %d %u +!",
					      serial,level,length));
    }
  }
}


void MainObject::setInputLevelData(int id,unsigned card,unsigned port,
				   int level)
{
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
		"[mixer] SetInputLevel - Card: %d  Port: %d  Level: %d",
		card,port,level);
  }
  cae_server->sendCommand(id,QString::asprintf("IL %u %u %d +!",
					       card,port,level));
}


void MainObject::setOutputLevelData(int id,unsigned card,unsigned port,
				    int level)
{
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
		"[mixer] SetOutputLevel - Card: %d  Port: %d  Level: %d",
		card,port,level);
  }
  cae_server->sendCommand(id,QString::asprintf("OL %u %u %d +!",
					       card,port,level));
}


void MainObject::setInputModeData(int id,unsigned card,unsigned stream,
				  unsigned mode)
{
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
		"[mixer] SetInputMode - Card: %d  Stream: %d  Mode: %d",
		card,stream,mode);
  }
  cae_server->sendCommand(id,QString::asprintf("IM %u %u %u +!",
					       card,stream,mode));
}


void MainObject::setOutputModeData(int id,unsigned card,unsigned stream,
				   unsigned mode)
{
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
		"[mixer] SetOutputMode - Card: %d  Stream: %d  Mode: %d",
		card,stream,mode);
  }
  cae_server->sendCommand(id,QString::asprintf("OM %u %u %u +!",
					       card,stream,mode));
}


void MainObject::setInputVoxLevelData(int id,unsigned card,unsigned stream,
				      int level)
{
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
		"[mixer] SetInputVOXLevel - Card: %d  Stream: %d  Level: %d",
		card,stream,level);
  }
  cae_server->sendCommand(id,QString::asprintf("IX %u %u %d +!",
					       card,stream,level));
}


void MainObject::setInputTypeData(int id,unsigned card,unsigned port,
				  unsigned type)
{
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
		"[mixer] SetInputType - Card: %d  Port: %d  Type: %d",
		card,port,type);
  }
  cae_server->sendCommand(id,QString::asprintf("IT %u %u %u +!",
					       card,port,type));
}


void MainObject::getInputStatusData(int id,unsigned card,unsigned port)
{
  Driver *dvr=GetDriver(card);

  if(dvr==NULL) {
    return;
  }
  if(dvr->driverType()==RDStation::Hpi) {
    if(dvr->getInputStatus(card,port)) {
      cae_server->sendCommand(id,QString::asprintf("IS %d %d 0 +!",card,port));
    }
    else {
      cae_server->sendCommand(id,QString::asprintf("IS %d %d 1 +!",card,port));
    }
  }
}


void MainObject::setAudioPassthroughLevelData(int id,unsigned card,
					      unsigned input,unsigned output,
					      int level)
{
  Driver *dvr=GetDriver(card);

  if(dvr==NULL) {
    cae_server->sendCommand(id,QString::asprintf("AL %u %u %u %d -!",
						 card,input,output,level));
    return;
  }
  if(!dvr->setPassthroughLevel(card,input,output,level)) {
    cae_server->sendCommand(id,QString::asprintf("AL %u %u %u %d -!",
						 card,input,output,level));
    return;
  }
  if(rda->config()->enableMixerLogging()) {
    rda->syslog(LOG_INFO,
		"[mixer] SetPassthroughLevel - Card: %d  InPort: %d  OutPort: %d Level: %d",
		card,input,output,level);
  }
  cae_server->sendCommand(id,QString::asprintf("AL %u %u %u %d +!",
					       card,input,output,level));
}


void MainObject::setClockSourceData(int id,unsigned card,int input)
{
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
		"[mixer] SetClockSource - Card: %d  Source: %d",card,input);
  }
  cae_server->sendCommand(id,QString::asprintf("CS %u %u +!",card,input));
}


void MainObject::meterEnableData(int id,uint16_t udp_port,
				 const QList<unsigned> &cards)
{
  QString cmd=QString::asprintf("ME %u",0xFFFF&udp_port);
  for(int i=0;i<cards.size();i++) {
    cmd+=QString::asprintf(" %u",cards.at(i));
  }
  if((udp_port<0)||(udp_port>0xFFFF)) {
    cae_server->sendCommand(id,cmd+" -!");
    return;
  }
  cae_server->setMeterPort(id,udp_port);
  for(int i=0;i<cards.size();i++) {
    if((cards.at(i)<0)||(cards.at(i)>=RD_MAX_CARDS)) {
      cae_server->sendCommand(id,cmd+" -!");
      return;
    }
    cae_server->setMetersEnabled(id,cards.at(i),true);
  }

  cae_server->sendCommand(id,cmd+" +!");
}


void MainObject::connectionDroppedData(int id)
{
  KillSocket(id);
}


void MainObject::statePlayUpdate(int card,int stream,int state)
{
  uint64_t phandle=GetPlayHandle(card,stream);
  unsigned serial=PlaySession::serialNumber(phandle);
  PlaySession *psess=play_sessions.value(phandle);

  if(psess==NULL) {
    return;
  }
  switch(state) {
  case 1:   // Playing
    cae_server->
      sendCommand(phandle,QString::asprintf("PY %u %d %d 0 +!",
					    serial,
					    psess->length(),
					    psess->speed()));
    break;

  case 2:   // Paused
    cae_server->sendCommand(phandle,QString::asprintf("SP %d +!",serial));
    break;

  case 0:   // Stopped
    cae_server->
      sendCommand(phandle,QString::asprintf("SP %d +!",serial).toUtf8());
    break;
  }
}


void MainObject::stateRecordUpdate(int card,int stream,int state)
{
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

	//
	// Input Port Statuses
	//
	if(dvr->getInputStatus(i,j)!=port_status[i][j]) {
	  port_status[i][j]=dvr->getInputStatus(i,j);
	  if(port_status[i][j]) {
	    cae_server->sendCommand(QString::asprintf("IS %d %d 0!",i,j));
	  }
	  else {
	    cae_server->sendCommand(QString::asprintf("IS %d %d 1!",i,j));
	  }
	}

	//
	// Port Meters
	//
	if(dvr->getInputMeters(i,j,levels)) {
	  SendMeterLevelUpdate("I",i,j,levels);
	}
	if(dvr->getOutputMeters(i,j,levels)) {
	  SendMeterLevelUpdate("O",i,j,levels);
	}      
      }

      //
      // Output Positions
      //
      dvr->getOutputPosition(i,positions);
      SendMeterPositionUpdate(i,positions);

      //
      // Output Stream Meters
      //
      for(QMap<uint64_t,PlaySession *>::const_iterator it=play_sessions.begin();
	  it!=play_sessions.end();it++) {
	if((int)it.value()->cardNumber()==i) {
	  if(dvr->getStreamOutputMeters(it.value()->cardNumber(),
					it.value()->streamNumber(),
					levels)) {
	    SendStreamMeterLevelUpdate(it.value(),levels);
	  }
	}
      }
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
  for(int i=0;i<RD_MAX_CARDS;i++) {
    RDAudioPort *port=new RDAudioPort(rda->config()->stationName(),i);
    Driver *dvr=GetDriver(i);

    if(dvr!=NULL) {
      dvr->setClockSource(i,port->clockSource());
      for(int j=0;j<RD_MAX_PORTS;j++) {
	for(int k=0;k<RD_MAX_PORTS;k++) {
	  dvr->setPassthroughLevel(i,j,k,RD_MUTE_DEPTH);
	}
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


void MainObject::KillSocket(int sock)
{
  for(int i=0;i<RD_MAX_CARDS;i++) {
    Driver *dvr=GetDriver(i);
    for(int j=0;j<RD_MAX_STREAMS;j++) {
      //
      // Clear Active Record Events
      //
      if(record_owner[i][j]==sock) {
	rda->syslog(LOG_DEBUG,"force unloading record context for connection %s:%u: Card: %d  Stream: %d",
		    cae_server->peerAddress(sock).toString().toUtf8().constData(),
		    0xFFFF&cae_server->peerPort(sock),
		    i,j);
	unsigned len=0;
	if(dvr!=NULL) {
	  dvr->unloadRecord(i,j,&len);
	}
	record_length[i][j]=0;
	record_threshold[i][j]=-10000;
	record_owner[i][j]=-1;
      }

      //
      // Clear Active Playout Events
      //
      QMap<uint64_t,PlaySession *>::iterator it=play_sessions.begin();
      while(it!=play_sessions.end()) {
	if((it.value()!=NULL)&&(it.value()->socketDescriptor()==sock)) {
	  rda->syslog(LOG_DEBUG,"force unloading play context for connection [%s:%u]: Serial: %u  Card: %d  Stream: %d",
		      cae_server->peerAddress(sock).toString().toUtf8().
		      constData(),
		      0xFFFF&cae_server->peerPort(sock),
		      it.value()->serialNumber(),i,j);
	  dvr->unloadPlayback(it.value()->cardNumber(),
			      it.value()->streamNumber());
	  it=play_sessions.erase(it);
	}
	else {
	  ++it;
	}
      }
    }
  }
}


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


uint64_t MainObject::GetPlayHandle(unsigned cardnum,unsigned streamnum) const
{
  for(QMap<uint64_t,PlaySession *>::const_iterator it=play_sessions.begin();
      it!=play_sessions.end();it++) {
    if((it.value()->cardNumber()==cardnum)&&
       (it.value()->streamNumber()==streamnum)) {
      return it.key();
    }
  }
  return 0;
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
  QList<int> ids=cae_server->connectionIds();

  for(int l=0;l<ids.size();l++) {
    if((cae_server->meterPort(ids.at(l))>0)&&
       cae_server->metersEnabled(ids.at(l),cardnum)) {
      SendMeterUpdate(QString::asprintf("ML %s %d %d %d %d",
					type.toUtf8().constData(),
					cardnum,portnum,levels[0],levels[1]),
		      ids.at(l));
    }
  }
}


void MainObject::SendStreamMeterLevelUpdate(PlaySession *psess,short levels[])
{
  if((cae_server->meterPort(psess->socketDescriptor())>0)&&
     cae_server->metersEnabled(psess->socketDescriptor(),psess->cardNumber())) {
    SendMeterUpdate(QString::asprintf("MO %u %d %d",psess->serialNumber(),
				      levels[0],levels[1]),
		    psess->socketDescriptor());
  }

}


void MainObject::SendMeterPositionUpdate(int cardnum,unsigned pos[])
{
  PlaySession *psess=NULL;
  QList<int> ids=cae_server->connectionIds();

  for(unsigned k=0;k<RD_MAX_STREAMS;k++) {
    if((psess=GetPlaySession(cardnum,k))!=NULL) {
      for(int l=0;l<ids.size();l++) {
	if((cae_server->meterPort(ids.at(l))>0)&&
	   cae_server->metersEnabled(ids.at(l),cardnum)) {
	  SendMeterUpdate(QString::asprintf("MP %u %d",
					    psess->serialNumber(),pos[k]),
			  psess->socketDescriptor());
	}
      }
    }
  }
}


void MainObject::SendMeterUpdate(const QString &msg,int conn_id)
{
  /*
  rda->syslog(LOG_NOTICE,"writing %s to %s:%u",
	      msg.toUtf8().constData(),
	      cae_server->peerAddress(conn_id).toString().toUtf8().constData(),
	      0xFFFF&cae_server->meterPort(conn_id));
  */
  meter_socket->writeDatagram(msg.toUtf8(),cae_server->peerAddress(conn_id),
			      cae_server->meterPort(conn_id));
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


PlaySession *MainObject::GetPlaySession(unsigned card,unsigned stream) const
{
  for(QMap<uint64_t,PlaySession *>::const_iterator it=play_sessions.begin();
      it!=play_sessions.end();it++) {
    if((it.value()->cardNumber()==card)&&
       (it.value()->streamNumber()==stream)) {
      return it.value();
    }
  }
  return NULL;
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
