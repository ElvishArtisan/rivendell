// cae_jack.cpp
//
// The JACK Driver for the Core Audio Engine component of Rivendell
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: cae_jack.cpp,v 1.59.4.6 2012/11/30 16:14:58 cvs Exp $
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

#include <math.h>

#include <samplerate.h>

#include <qsignalmapper.h>

#include <rd.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdringbuffer.h>
#include <rdprofile.h>
#include <rdmeteraverage.h>

#include <cae.h>

#ifdef JACK
//
// Callback Variables
//
jack_client_t *jack_client;
RDMeterAverage *jack_input_meter[RD_MAX_PORTS][2];
RDMeterAverage *jack_output_meter[RD_MAX_PORTS][2];
RDMeterAverage *jack_stream_output_meter[RD_MAX_STREAMS][2];
//volatile jack_default_audio_sample_t jack_input_meter[RD_MAX_PORTS][2];
//volatile jack_default_audio_sample_t jack_output_meter[RD_MAX_PORTS][2];
volatile jack_default_audio_sample_t 
  jack_input_volume[RD_MAX_PORTS];
volatile jack_default_audio_sample_t 
  jack_output_volume[RD_MAX_PORTS][RD_MAX_STREAMS];
volatile jack_default_audio_sample_t
  jack_passthrough_volume[RD_MAX_PORTS][RD_MAX_PORTS];
volatile jack_default_audio_sample_t jack_input_vox[RD_MAX_PORTS];
jack_port_t *jack_input_port[RD_MAX_PORTS][2];
jack_port_t *jack_output_port[RD_MAX_PORTS][2];
volatile int jack_input_channels[RD_MAX_PORTS];
volatile int jack_output_channels[RD_MAX_STREAMS];
volatile jack_default_audio_sample_t *jack_input_buffer[RD_MAX_PORTS][2];
volatile jack_default_audio_sample_t *jack_output_buffer[RD_MAX_PORTS][2];
RDRingBuffer *jack_play_ring[RD_MAX_STREAMS];
RDRingBuffer *jack_record_ring[RD_MAX_PORTS];
volatile bool jack_playing[RD_MAX_STREAMS];
volatile bool jack_stopping[RD_MAX_STREAMS];
volatile bool jack_eof[RD_MAX_STREAMS];
volatile bool jack_recording[RD_MAX_PORTS];
volatile bool jack_ready[RD_MAX_PORTS];
volatile int jack_output_pos[RD_MAX_STREAMS];
volatile unsigned jack_output_sample_rate[RD_MAX_STREAMS];
volatile unsigned jack_sample_rate;
int jack_input_mode[RD_MAX_CARDS][RD_MAX_PORTS];
int jack_card_process;  // local copy of object member jack_card, for use by the callback process.


//
// Callback Buffers
//
jack_default_audio_sample_t jack_callback_buffer[RINGBUFFER_SIZE];

int JackProcess(jack_nframes_t nframes, void *arg)
{
  unsigned n=0;
  jack_default_audio_sample_t in_meter[2];
  jack_default_audio_sample_t out_meter[2];
  jack_default_audio_sample_t stream_out_meter;

  //
  // Ensure Buffers are Valid
  //
  for(int i=0;i<RD_MAX_PORTS;i++) {
    for(int j=0;j<2;j++) {
      jack_input_buffer[i][j]=(jack_default_audio_sample_t *)
	jack_port_get_buffer(jack_input_port[i][j],nframes);
      jack_output_buffer[i][j]=(jack_default_audio_sample_t *)
	jack_port_get_buffer(jack_output_port[i][j],nframes);
    }
  }

  //
  // Zero Output Ports
  //
  for(int i=0;i<RD_MAX_PORTS;i++) {
    for(int j=0;j<2;j++) {
      for(unsigned k=0;k<nframes;k++) {
	jack_output_buffer[i][j][k]=0.0;
      }
    } 
  }

  //
  // Process Passthroughs
  //
  for(int i=0;i<RD_MAX_PORTS;i++) {
    for(int j=0;j<RD_MAX_PORTS;j++) {
      if(jack_passthrough_volume[i][j]>0.0) {
	for(int k=0;k<2;k++) {
	  for(unsigned l=0;l<nframes;l++) {
	    jack_output_buffer[j][k][l]+=
	      jack_input_buffer[i][k][l]*jack_passthrough_volume[i][j];
	  }
	}
      }
    }
  }

  //
  // Process Input Streams
  //
  for(int i=0;i<RD_MAX_PORTS;i++) {
    if(jack_recording[i]) {
      switch(jack_input_channels[i]) {
      case 1: // mono
	for(unsigned j=0;j<nframes;j++) {
	  switch(jack_input_mode[jack_card_process][i]) {
	  case 3: // R only
	    jack_callback_buffer[j]=jack_input_volume[i]*
	      jack_input_buffer[i][1][j];
	    break;
	  case 2: // L only
	    jack_callback_buffer[j]=jack_input_volume[i]*
	      jack_input_buffer[i][0][j];
	    break;
	  case 1: // swap, sum R+L
	  case 0: // normal, sum L+R
	  default:
	    jack_callback_buffer[j]=jack_input_volume[i]*
	      (jack_input_buffer[i][0][j]+jack_input_buffer[i][1][j]);
	    break;
	  }
	} // for nframes
	n=jack_record_ring[i]->
	  write((char *)jack_callback_buffer,
		nframes*sizeof(jack_default_audio_sample_t))/
	  sizeof(jack_default_audio_sample_t);
	break;

      case 2: // stereo
	for(unsigned j=0;j<nframes;j++) {
	  switch(jack_input_mode[jack_card_process][i]) {
	  case 3: // R only
	    memset(&jack_callback_buffer[2*j],0,
		   sizeof(jack_input_buffer[i][0][j]));
	    jack_callback_buffer[2*j+1]=jack_input_volume[i]*
	      jack_input_buffer[i][1][j];
	    break;
	  case 2: // L only
	    jack_callback_buffer[2*j]=jack_input_volume[i]*
	      jack_input_buffer[i][0][j];
	    memset(&jack_callback_buffer[2*j+1],0,
		   sizeof(jack_input_buffer[i][1][j]));
	    break;
	  case 1: // swap
	    jack_callback_buffer[2*j]=jack_input_volume[i]*
	      jack_input_buffer[i][1][j];
	    jack_callback_buffer[2*j+1]=jack_input_volume[i]*
	      jack_input_buffer[i][0][j];
	    break;
	  case 0: // normal
	  default:
	    jack_callback_buffer[2*j]=jack_input_volume[i]*
	      jack_input_buffer[i][0][j];
	    jack_callback_buffer[2*j+1]=jack_input_volume[i]*
	      jack_input_buffer[i][1][j];
	    break;
	  }
	} // for nframes
	n=jack_record_ring[i]->
	  write((char *)jack_callback_buffer,
		2*nframes*sizeof(jack_default_audio_sample_t))/
	  (2*sizeof(jack_default_audio_sample_t));
	break;
      }
    }
  }

  //
  // Process Output Streams
  //
  for(int i=0;i<RD_MAX_STREAMS;i++) {
    if(jack_playing[i]) {
      switch(jack_output_channels[i]) {
      case 1:
	n=jack_play_ring[i]->
	  read((char *)jack_callback_buffer,
	       nframes*sizeof(jack_default_audio_sample_t))/
	  sizeof(jack_default_audio_sample_t);
	stream_out_meter=0.0;
	for(unsigned j=0;j<n;j++) {  // Stream Output Meters
	  if(fabsf(jack_callback_buffer[j])>stream_out_meter) {
	    stream_out_meter=fabsf(jack_callback_buffer[j]);
	  }
	}
	jack_stream_output_meter[i][0]->addValue(stream_out_meter);
	jack_stream_output_meter[i][1]->addValue(stream_out_meter);
	break;

      case 2:
	n=jack_play_ring[i]->
	  read((char *)jack_callback_buffer,
	       2*nframes*sizeof(jack_default_audio_sample_t))/
	  (2*sizeof(jack_default_audio_sample_t));
	for(unsigned j=0;j<2;j++) {  // Stream Output Meters
	  stream_out_meter=0.0;
	  for(unsigned k=0;k<n;k+=2) {
	    if(fabsf(jack_callback_buffer[k+j])>stream_out_meter) {
	      stream_out_meter=fabsf(jack_callback_buffer[k+j]);
	    }
	  }
	  jack_stream_output_meter[i][j]->addValue(stream_out_meter);
	}
	break;
      }
      for(int j=0;j<RD_MAX_PORTS;j++) {
	if(jack_output_volume[j][i]>0.0) {
	  switch(jack_output_channels[i]) {
	  case 1:
	    for(unsigned k=0;k<n;k++) {
	      jack_output_buffer[j][0][k]=
		jack_output_buffer[j][0][k]+jack_output_volume[j][i]*
		jack_callback_buffer[k];
	      jack_output_buffer[j][1][k]=
		jack_output_buffer[j][1][k]+jack_output_volume[j][i]*
		jack_callback_buffer[k];
	    }
	    if(n!=nframes && jack_eof[i]) {
	      jack_stopping[i]=true;
	      jack_playing[i]=false;
	    }
	    break;

	  case 2:
	    for(unsigned k=0;k<n;k++) {
	      jack_output_buffer[j][0][k]=
		jack_output_buffer[j][0][k]+jack_output_volume[j][i]*
		jack_callback_buffer[k*2];
	      jack_output_buffer[j][1][k]=
		jack_output_buffer[j][1][k]+jack_output_volume[j][i]*
		jack_callback_buffer[k*2+1];
	    }
	    if(n!=nframes && jack_eof[i]) {
	      jack_stopping[i]=true;
	      jack_playing[i]=false;
	    }
	    break;
	  }
	}
      }
      double ratio=(double)jack_output_sample_rate[i]/(double)jack_sample_rate;
      jack_output_pos[i]+=(int)(((double)n*ratio)+0.5);
    }
  }

  //
  // Process Meters
  //
  for(int i=0;i<RD_MAX_PORTS;i++) {
    // input meters (taking input mode into account)
    in_meter[0]=0.0;
    in_meter[1]=0.0;
    for(unsigned k=0;k<nframes;k++) {
      switch(jack_input_mode[jack_card_process][i]) {
      case 3: // R only
	if(jack_input_buffer[i][1][k]>in_meter[1]) 
	  in_meter[1]=jack_input_buffer[i][1][k];
	break;
      case 2: // L only
	if(jack_input_buffer[i][0][k]>in_meter[0]) 
	  in_meter[0]=jack_input_buffer[i][0][k];
	break;
      case 1: // swap
	if(jack_input_buffer[i][0][k]>in_meter[1]) 
	  in_meter[1]=jack_input_buffer[i][0][k];
	if(jack_input_buffer[i][1][k]>in_meter[0]) 
	  in_meter[0]=jack_input_buffer[i][1][k];
	break;
      case 0: // normal
      default:
	if(jack_input_buffer[i][0][k]>in_meter[0]) 
	  in_meter[0]=jack_input_buffer[i][0][k];
	if(jack_input_buffer[i][1][k]>in_meter[1]) 
	  in_meter[1]=jack_input_buffer[i][1][k];
	break;
      }
    } // for nframes
    jack_input_meter[i][0]->addValue(in_meter[0]);
    jack_input_meter[i][1]->addValue(in_meter[1]);

    // output meters
    for(int j=0;j<2;j++) {
      out_meter[j]=0.0;
      for(unsigned k=0;k<nframes;k++) {
	if(jack_output_buffer[i][j][k]>out_meter[j]) 
	  out_meter[j]=jack_output_buffer[i][j][k];
      }
      jack_output_meter[i][j]->addValue(out_meter[j]);
    }

  } // for RD_MAX_PORTS
  return 0;
}


int JackSampleRate(jack_nframes_t nframes, void *arg)
{
  jack_sample_rate=nframes;

  return 0;
}


void JackError(const char *desc)
{
  fprintf(stderr,"caed: Jack error: %s\n",desc);
}


void JackShutdown(void *arg)
{
}


void JackInitCallback()
{
  int avg_periods=(int)(330.0*jack_get_sample_rate(jack_client)/
			(1000.0*jack_get_buffer_size(jack_client)));
  for(int i=0;i<RD_MAX_PORTS;i++) {
    jack_recording[i]=false;
    jack_ready[i]=false;
    jack_input_volume[i]=1.0;
    jack_input_vox[i]=0.0;
    for(int j=0;j<2;j++) {
      jack_input_meter[i][j]=new RDMeterAverage(avg_periods);
      jack_output_meter[i][j]=new RDMeterAverage(avg_periods);
      jack_input_buffer[i][j]=NULL;
      jack_output_buffer[i][j]=NULL;
    }
    for(int j=0;j<RD_MAX_STREAMS;j++) {
      jack_output_volume[i][j]=1.0;
    }
    for(int j=0;j<RD_MAX_PORTS;j++) {
      jack_passthrough_volume[i][j]=0.0;
    }
    jack_record_ring[i]=NULL;
  }
  for(int i=0;i<RD_MAX_STREAMS;i++) {
    jack_play_ring[i]=NULL;
    jack_playing[i]=false;
    for(int j=0;j<2;j++) {
      jack_stream_output_meter[i][j]=new RDMeterAverage(avg_periods);
    }
  }
}
#endif  // JACK


void MainObject::jackStopTimerData(int stream)
{
#ifdef JACK
  jackStopPlayback(jack_card,stream);
  statePlayUpdate(jack_card,stream,2);
#endif  // JACK
}


void MainObject::jackFadeTimerData(int stream)
{
#ifdef JACK
  int level;
  if(!jack_fade_up[stream]) {
    level=jack_output_volume_db[jack_fade_port[stream]][stream]-
      jack_fade_increment[stream];
    if(level<=jack_fade_volume_db[stream]) {
      level=jack_fade_volume_db[stream];
      jack_fade_timer[stream]->stop();
    }
  }
  else {
    level=jack_output_volume_db[jack_fade_port[stream]][stream]+
      jack_fade_increment[stream];
    if(level>=jack_fade_volume_db[stream]) {
      level=jack_fade_volume_db[stream];
      jack_fade_timer[stream]->stop();
    }
  }
  jackSetOutputVolume(jack_card,stream,jack_fade_port[stream],level);
#endif  // JACK
}


void MainObject::jackRecordTimerData(int stream)
{
#ifdef JACK
  jackStopRecord(jack_card,stream);
  stateRecordUpdate(jack_card,stream,2);
#endif  // JACK
}


void MainObject::jackInit(RDStation *station)
{
#ifdef JACK
  QString sql;
  RDSqlQuery *q;
  jack_options_t jackopts=JackNullOption;
  jack_status_t jackstat=JackFailure;
  RDConfig::LogPriority prio=RDConfig::LogDebug;

  jack_connected=false;
  jack_activated=false;

  //
  // Get Next Available Card Number
  //
  for(jack_card=0;jack_card<RD_MAX_CARDS;jack_card++) {
    if(cae_driver[jack_card]==RDStation::None) {
      break;
    }
  }
  if(jack_card==RD_MAX_CARDS) {
    LogLine(RDConfig::LogInfo,"no more RD cards available");
    return;
  }
  QString name=QString().sprintf("rivendell_%d",jack_card);

  //
  // Start Jack Server
  //
  if(station->startJack()) {
    QStringList fields=QStringList().split(" ",station->jackCommandLine());
    QProcess *proc=new QProcess(fields,this);
    if(proc->start()) {
      LogLine(RDConfig::LogDebug,"JACK server started");
    }
    else {
      LogLine(RDConfig::LogErr,"failed to start JACK server");
    }
    sleep(1);
  }

  //
  // Attempt to Connect to Jack Server
  //
  jackopts=JackNoStartServer;
  if(station->jackServerName().isEmpty()) {
    jack_client=jack_client_open(name,jackopts,&jackstat);
  }
  else {
    jack_client=jack_client_open(name,jackopts,&jackstat,
				 (const char *)station->jackServerName());
  }
  if(jack_client==NULL) {
    if((jackstat&JackInvalidOption)!=0) {
      fprintf (stderr, "invalid or unsupported JACK option\n");
      LogLine(prio,"invalid or unsupported JACK option");
    }

    if((jackstat&JackServerError)!=0) {
      fprintf (stderr, "communication error with the JACK server\n");
      LogLine(prio,"communication error with the JACK server");
    }

    if((jackstat&JackNoSuchClient)!=0) {
      fprintf (stderr, "requested JACK client does not exist\n");
      LogLine(prio,"requested JACK client does not exist");
    }

    if((jackstat&JackLoadFailure)!=0) {
      fprintf (stderr, "unable to load internal JACK client\n");
      LogLine(prio,"unable to load internal JACK client");
    }

    if((jackstat&JackInitFailure)!=0) {
      fprintf (stderr, "unable to initialize JACK client\n");
      LogLine(prio,"unable to initialize JACK client");
    }

    if((jackstat&JackShmFailure)!=0) {
      fprintf (stderr, "unable to access JACK shared memory\n");
      LogLine(prio,"unable to access JACK shared memory");
    }

    if((jackstat&JackVersionError)!=0) {
      fprintf (stderr, "JACK protocol version mismatch\n");
      LogLine(prio,"JACK protocol version mismatch");
    }

    if((jackstat&JackServerStarted)!=0) {
      fprintf (stderr, "JACK server started\n");
      LogLine(prio,"JACK server started");
    }

    if((jackstat&JackServerFailed)!=0) {
      fprintf (stderr, "unable to communication with JACK server\n");
      LogLine(prio,"unable to communicate with JACK server");
    }

    if((jackstat&JackNameNotUnique)!=0) {
      fprintf (stderr, "JACK client name not unique\n");
      LogLine(prio,"JACK client name not unique");
    }

    if((jackstat&JackFailure)!=0) {
      fprintf (stderr, "JACK general failure\n");
      LogLine(prio,"JACK general failure");
    }
    jack_card=-1;
    fprintf (stderr, "no connection to JACK server\n");
    LogLine(prio,"no connection to JACK server");
    return;
  }
  jack_connected=true;
  jack_set_process_callback(jack_client,JackProcess,0);
  jack_set_sample_rate_callback(jack_client,JackSampleRate,0);
  //jack_set_port_connect_callback(jack_client,JackPortConnectCB,this);
#ifdef HAVE_JACK_INFO_SHUTDOWN
  jack_on_info_shutdown(jack_client,JackInfoShutdown,0);
#else
  jack_on_shutdown(jack_client,JackShutdown,0);
#endif  // HAVE_JACK_INFO_SHUTDOWN
  LogLine(RDConfig::LogDebug,"connected to JACK server");

  //
  // Start JACK Clients
  //
  sql=QString("select DESCRIPTION,COMMAND_LINE from JACK_CLIENTS where ")+
    "STATION_NAME=\""+RDEscapeString(station->name())+"\"";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    QStringList fields=QStringList().split(" ",q->value(1).toString());
    jack_clients.push_back(new QProcess(fields,this));
    if(jack_clients.back()->start()) {
      LogLine(RDConfig::LogDebug,"started JACK Client \""+
	      q->value(0).toString()+"\"");
    }
    else {
      LogLine(RDConfig::LogWarning,"failed to start JACK Client \""+
	      q->value(0).toString()+"\" ["+
	      q->value(1).toString()+"]");
    }
    sleep(1);
  }
  delete q;

  //
  // Tell the database about us
  //
  if(jack_connected) {
    station->setCardDriver(jack_card,RDStation::Jack);
    station->setCardName(jack_card,"JACK Audio Connection Kit");
    station->setCardInputs(jack_card,RD_MAX_PORTS);
    station->setCardOutputs(jack_card,RD_MAX_PORTS);
  }

  //
  // Initialize Data Structures
  //
  for(int i=0;i<RD_MAX_STREAMS;i++) {
    for(int j=0;j<RD_MAX_PORTS;j++) {
      jack_output_volume_db[j][i]=0; 
      jack_samples_recorded[i]=0;
    }
    jack_st_conv[i]=NULL;
  }
  for(int i=0;i<RD_MAX_PORTS;i++) {
    jack_input_volume_db[i]=0;
    for(int j=0;j<RD_MAX_PORTS;j++) {
      jack_passthrough_volume_db[j][i]=-10000;
    }
  }
  for(int i=0;i<RD_MAX_CARDS;i++) {
    for(int j=0;j<RD_MAX_PORTS;j++) {
      jack_input_mode[i][j]=0;
    }
  }
  jack_card_process = jack_card; // populate variable used by callback process

  //
  // Stop & Fade Timers
  //
  QSignalMapper *stop_mapper=new QSignalMapper(this);
  connect(stop_mapper,SIGNAL(mapped(int)),this,SLOT(jackStopTimerData(int)));
  QSignalMapper *fade_mapper=new QSignalMapper(this);
  connect(fade_mapper,SIGNAL(mapped(int)),this,SLOT(jackFadeTimerData(int)));
  QSignalMapper *record_mapper=new QSignalMapper(this);
  connect(record_mapper,SIGNAL(mapped(int)),
	  this,SLOT(jackRecordTimerData(int)));
  for(int i=0;i<RD_MAX_STREAMS;i++) {
    jack_stop_timer[i]=new QTimer(this);
    stop_mapper->setMapping(jack_stop_timer[i],i);
    connect(jack_stop_timer[i],SIGNAL(timeout()),stop_mapper,SLOT(map()));
    jack_fade_timer[i]=new QTimer(this);
    fade_mapper->setMapping(jack_fade_timer[i],i);
    connect(jack_fade_timer[i],SIGNAL(timeout()),fade_mapper,SLOT(map()));
  }
  for(int i=0;i<RD_MAX_PORTS;i++) {
    jack_record_timer[i]=new QTimer(this);
    record_mapper->setMapping(jack_record_timer[i],i);
    connect(jack_record_timer[i],SIGNAL(timeout()),record_mapper,SLOT(map()));
  }

  //
  // Register Ports
  //
  for(int i=0;i<RD_MAX_PORTS;i++) {
    name=QString().sprintf("playout_%dL",i);
    jack_output_port[i][0]=
      jack_port_register(jack_client,(const char *)name,
			 JACK_DEFAULT_AUDIO_TYPE,
			 JackPortIsOutput|JackPortIsTerminal,0);
    name=QString().sprintf("playout_%dR",i);
    jack_output_port[i][1]=
      jack_port_register(jack_client,(const char *)name,
			 JACK_DEFAULT_AUDIO_TYPE,
			 JackPortIsOutput|JackPortIsTerminal,0);
    name=QString().sprintf("record_%dL",i);
    jack_input_port[i][0]=
      jack_port_register(jack_client,(const char *)name,
			 JACK_DEFAULT_AUDIO_TYPE,
			 JackPortIsInput|JackPortIsTerminal,0);
    name=QString().sprintf("record_%dR",i);
    jack_input_port[i][1]=
      jack_port_register(jack_client,(const char *)name,
			 JACK_DEFAULT_AUDIO_TYPE,
			 JackPortIsInput|JackPortIsTerminal,0);
  }

  //
  // Allocate Temporary Buffers
  //
  JackInitCallback();
  jack_wave_buffer=new short[RINGBUFFER_SIZE];
  jack_sample_buffer=new jack_default_audio_sample_t[RINGBUFFER_SIZE];

  //
  // Join the Graph
  //
  if(jack_activate(jack_client)) {
    return;
  }
  jack_sample_rate=jack_get_sample_rate(jack_client);
  if(jack_sample_rate!=system_sample_rate) {
    fprintf (stderr,"JACK sample rate mismatch!\n");
    LogLine(RDConfig::LogWarning,"JACK sample rate mismatch!");
  }
  jack_activated=true;
  cae_driver[jack_card]=RDStation::Jack;
  JackSessionSetup();

#endif  // JACK
}


void MainObject::jackFree()
{
#ifdef JACK
  for(unsigned i=0;i<jack_clients.size();i++) {
    jack_clients[i]->kill();
    delete jack_clients[i];
  }
  jack_clients.clear();
  if(jack_activated) {
    jack_deactivate(jack_client);
  }
#endif  // JACK
}


bool MainObject::jackLoadPlayback(int card,QString wavename,int *stream)
{
#ifdef JACK
  if((*stream=GetJackOutputStream())<0) {
    LogLine(RDConfig::LogErr,QString().sprintf(
            "Error: jackLoadPlayback(%s)   GetJackOutputStream():%d <0",
            (const char *) wavename,
            *stream) );
    return false;
  }
  jack_play_wave[*stream]=new RDWaveFile(wavename);
  if(!jack_play_wave[*stream]->openWave()) {
    LogLine(RDConfig::LogNotice,QString().sprintf(
            "Error: jackLoadPlayback(%s)   openWave() failed to open file",
            (const char *) wavename) );
    delete jack_play_wave[*stream];
    jack_play_wave[*stream]=NULL;
    FreeJackOutputStream(*stream);
    *stream=-1;
    return false;
  }
  switch(jack_play_wave[*stream]->getFormatTag()) {
  case WAVE_FORMAT_PCM:
  case WAVE_FORMAT_VORBIS:
    break;

  case WAVE_FORMAT_MPEG:
    InitMadDecoder(card,*stream,jack_play_wave[*stream]);
    break;

  default:
    LogLine(RDConfig::LogNotice,
	    QString().sprintf(
	    "Error: jackLoadPlayback(%s)   getFormatTag()%d || getBistsPerSample()%d failed",
	    (const char *) wavename,
	    jack_play_wave[*stream]->getFormatTag(),
	    jack_play_wave[*stream]->getBitsPerSample() ));
    delete jack_play_wave[*stream];
    jack_play_wave[*stream]=NULL;
    FreeJackOutputStream(*stream);
    *stream=-1;
    return false;
  }
  jack_output_channels[*stream]=jack_play_wave[*stream]->getChannels();
  jack_output_sample_rate[*stream]=jack_play_wave[*stream]->getSamplesPerSec();
  jack_stopping[*stream]=false;
  jack_offset[*stream]=0;
  jack_output_pos[*stream]=0;
  jack_eof[*stream]=false;
  FillJackOutputStream(*stream);
  return true;
#else
  return false;
#endif  // JACK
}


bool MainObject::jackUnloadPlayback(int card,int stream)
{
#ifdef JACK
  if ((stream <0) || (stream >= RD_MAX_STREAMS)){
    return false;
  }
  if(jack_play_ring[stream]==NULL) {
    return false;
  }
  jack_playing[stream]=false;
  switch(jack_play_wave[stream]->getFormatTag()) {
  case WAVE_FORMAT_MPEG:
    FreeMadDecoder(card,stream);
    break;
  }
  jack_play_wave[stream]->closeWave();
  delete jack_play_wave[stream];
  jack_play_wave[stream]=NULL;
  FreeJackOutputStream(stream);
  return true;
#else
  return false;
#endif  // JACK
}


bool MainObject::jackPlaybackPosition(int card,int stream,unsigned pos)
{
#ifdef JACK
  unsigned offset=0;

  if ((stream <0) || (stream >= RD_MAX_STREAMS)){
    return false;
  }
  jack_eof[stream]=false;
  jack_play_ring[stream]->reset();


  switch(jack_play_wave[stream]->getFormatTag()) {
  case WAVE_FORMAT_PCM:
  case WAVE_FORMAT_VORBIS:
    offset=(unsigned)((double)jack_play_wave[stream]->getSamplesPerSec()*
		      (double)jack_play_wave[stream]->getBlockAlign()*
		      (double)pos/1000);
    jack_offset[stream]=offset/jack_play_wave[stream]->getBlockAlign();
    offset=jack_offset[stream]*jack_play_wave[stream]->getBlockAlign();
    break;

  case WAVE_FORMAT_MPEG:
    offset=(unsigned)((double)jack_play_wave[stream]->getSamplesPerSec()*
		      (double)pos/1000);
    jack_offset[stream]=offset/1152*1152;
    offset=jack_offset[stream]/1152*jack_play_wave[stream]->getBlockAlign();
    FreeMadDecoder(jack_card,stream);
    InitMadDecoder(jack_card,stream,jack_play_wave[stream]);
    break;
  }
  if(jack_offset[stream]>(int)jack_play_wave[stream]->getSampleLength()) {
    return false;
  }
  jack_output_pos[stream]=0;
  jack_play_wave[stream]->seekWave(offset,SEEK_SET);
  FillJackOutputStream(stream);

  if(jack_playing[stream]) {
    jack_stop_timer[stream]->stop();
    jack_stop_timer[stream]->
      start(jack_play_wave[stream]->getExtTimeLength()-pos,true);
  }
  return true;
#else
  return false;
#endif  // JACK
}


bool MainObject::jackPlay(int card,int stream,int length,int speed,bool pitch,
			 bool rates)
{
#ifdef JACK
  if((stream <0) || (stream >= RD_MAX_STREAMS) || 
     (jack_play_ring[stream]==NULL)||jack_playing[stream]) {
    return false;
  }
  if(speed!=RD_TIMESCALE_DIVISOR) {
    jack_st_conv[stream]=new soundtouch::SoundTouch();
    jack_st_conv[stream]->setTempo((float)speed/RD_TIMESCALE_DIVISOR);
    jack_st_conv[stream]->setSampleRate(jack_output_sample_rate[stream]);
    jack_st_conv[stream]->setChannels(jack_output_channels[stream]);
  }
  jack_playing[stream]=true;
  if(length>0) {
    jack_stop_timer[stream]->start(length,true);
  }
  statePlayUpdate(card,stream,1);
  return true;
#else
  return false;
#endif  // JACK
}


bool MainObject::jackStopPlayback(int card,int stream)
{
#ifdef JACK
  if((stream <0) || (stream>=RD_MAX_STREAMS) || 
     (jack_play_ring[stream]==NULL)||(!jack_playing[stream])) {
    return false;
  }
  jack_playing[stream]=false;
  jack_stop_timer[stream]->stop();
  statePlayUpdate(card,stream,2);
  return true;
#else
  return false;
#endif  // JACK
}


bool MainObject::jackTimescaleSupported(int card)
{
#ifdef JACK
  return true;
#else
  return false;
#endif  // JACK
}


bool MainObject::jackLoadRecord(int card,int stream,int coding,int chans,
			       int samprate,int bitrate,QString wavename)
{
#ifdef JACK
  jack_record_wave[stream]=new RDWaveFile(wavename);
  switch(coding) {
  case 0:  // PCM16
    jack_record_wave[stream]->setFormatTag(WAVE_FORMAT_PCM);
    jack_record_wave[stream]->setChannels(chans);
    jack_record_wave[stream]->setSamplesPerSec(samprate);
    jack_record_wave[stream]->setBitsPerSample(16);
    break;

  case 2:  // MPEG Layer 2
    if(!InitTwoLameEncoder(card,stream,chans,samprate,bitrate)) {
      delete jack_record_wave[stream];
      jack_record_wave[stream]=NULL;
      return false;
    }
    jack_record_wave[stream]->setFormatTag(WAVE_FORMAT_MPEG);
    jack_record_wave[stream]->setChannels(chans);
    jack_record_wave[stream]->setSamplesPerSec(samprate);
    jack_record_wave[stream]->setBitsPerSample(16);
    jack_record_wave[stream]->setHeadLayer(ACM_MPEG_LAYER2);
    switch(chans) {
    case 1:
      jack_record_wave[stream]->setHeadMode(ACM_MPEG_SINGLECHANNEL);
      break;

    case 2:
      jack_record_wave[stream]->setHeadMode(ACM_MPEG_STEREO);
      break;

    default:
      LogLine(RDConfig::LogErr,QString().
	sprintf("requested unsupported channel count %d, card: %d, stream: %d",
		chans,card,stream));
      delete jack_record_wave[stream];
      jack_record_wave[stream]=NULL;
      return false;
    }
    jack_record_wave[stream]->setHeadBitRate(bitrate);
    jack_record_wave[stream]->setMextChunk(true);
    jack_record_wave[stream]->setMextHomogenous(true);
    jack_record_wave[stream]->setMextPaddingUsed(false);
    jack_record_wave[stream]->setMextHackedBitRate(true);
    jack_record_wave[stream]->setMextFreeFormat(false);
    jack_record_wave[stream]->
      setMextFrameSize(144*jack_record_wave[stream]->getHeadBitRate()/
		       jack_record_wave[stream]->getSamplesPerSec());
    jack_record_wave[stream]->setMextAncillaryLength(5);
    jack_record_wave[stream]->setMextLeftEnergyPresent(true);
    if(chans>1) {
      jack_record_wave[stream]->setMextRightEnergyPresent(true);
    }
    else {
      jack_record_wave[stream]->setMextRightEnergyPresent(false);
    }
    jack_record_wave[stream]->setMextPrivateDataPresent(false);
    break;

  default:
    LogLine(RDConfig::LogErr,QString().
	    sprintf("requested invalid audio encoding %d, card: %d, stream: %d",
		    coding,card,stream));
    delete jack_record_wave[stream];
    jack_record_wave[stream]=NULL;
    return false;
  }
  jack_record_wave[stream]->setBextChunk(true);
  jack_record_wave[stream]->setLevlChunk(true);
  if(!jack_record_wave[stream]->createWave()) {
    delete jack_record_wave[stream];
    jack_record_wave[stream]=NULL;
    return false;
  }
  chown((const char *)wavename,rd_config->uid(),rd_config->gid());
  jack_input_channels[stream]=chans;
  jack_record_ring[stream]=new RDRingBuffer(RINGBUFFER_SIZE);
  jack_record_ring[stream]->reset();
  jack_ready[stream]=true;
  return true;

  /*
  if ((stream <0) || (stream >=RD_MAX_PORTS)){
    return false;
  }
    jack_record_wave[stream]=new RDWaveFile(wavename);
  jack_record_wave[stream]->setFormatTag(WAVE_FORMAT_PCM);
  jack_record_wave[stream]->setChannels(chans);
  jack_record_wave[stream]->setSamplesPerSec(samprate);
  jack_record_wave[stream]->setBitsPerSample(16);
  jack_record_wave[stream]->setBextChunk(true);
  jack_record_wave[stream]->setLevlChunk(true);
  if(!jack_record_wave[stream]->createWave()) {
    delete jack_record_wave[stream];
    jack_record_wave[stream]=NULL;
    return false;
  }
  chown((const char *)wavename,rd_config->uid(),rd_config->gid());
  jack_input_channels[stream]=chans; 
  jack_record_ring[stream]=new RDRingBuffer(RINGBUFFER_SIZE);
  jack_record_ring[stream]->reset();
  jack_ready[stream]=true;
  return true;
  */
#else
  return false;
#endif  // JACK
}


bool MainObject::jackUnloadRecord(int card,int stream,unsigned *len)
{
#ifdef JACK
  if ((stream <0) || (stream >= RD_MAX_PORTS)){
    return false;
  }
  jack_recording[stream]=false;
  jack_ready[stream]=false;
  EmptyJackInputStream(stream,true);
  *len=jack_samples_recorded[stream];
  jack_samples_recorded[stream]=0;
  jack_record_wave[stream]->closeWave(*len);
  delete jack_record_wave[stream];
  jack_record_wave[stream]=NULL;
  delete jack_record_ring[stream];
  jack_record_ring[stream]=NULL;
  FreeTwoLameEncoder(card,stream);
  return true;
#else
  return false;
#endif  // JACK
}


bool MainObject::jackRecord(int card,int stream,int length,int thres)
{
#ifdef JACK
  if ((stream <0) || (stream >= RD_MAX_PORTS)){
    return false;
  }
  if(!jack_ready[stream]) {
    return false;
  }
  jack_recording[stream]=true;
  if(jack_input_vox[stream]==0.0) {
    if(length>0) {
      jack_record_timer[stream]->start(length,true);
    }
    stateRecordUpdate(card,stream,4);
  }
  return true;
#else
  return false;
#endif  // JACK
}


bool MainObject::jackStopRecord(int card,int stream)
{
#ifdef JACK
  if ((stream <0) || (stream >= RD_MAX_PORTS)){
    return false;
  }
  if(!jack_recording[stream]) {
    return false;
  }
  jack_recording[stream]=false;
  return true;
#else
  return false;
#endif  // JACK
}


bool MainObject::jackSetInputVolume(int card,int stream,int level)
{
#ifdef JACK
  if ((stream <0) || (stream >= RD_MAX_STREAMS)){
    return false;
  }
  if(level>-10000) {
    jack_input_volume[stream]=
      (jack_default_audio_sample_t)pow10((double)level/2000.0);
    jack_input_volume_db[stream]=level;
  }
  else {
    jack_input_volume[stream]=0.0;
    jack_input_volume_db[stream]=-10000;
  }
  return true;
#else
  return false;
#endif  // JACK
}


bool MainObject::jackSetOutputVolume(int card,int stream,int port,int level)
{
#ifdef JACK
  if ((stream <0) ||(stream >= RD_MAX_STREAMS) || 
      (port <0) || (port >= RD_MAX_PORTS)){
    return false;
  }
  if(level>-10000) {
    jack_output_volume[port][stream]=
      (jack_default_audio_sample_t)pow10((double)level/2000.0);
    jack_output_volume_db[port][stream]=level;
  }
  else {
    jack_output_volume[port][stream]=0.0;
    jack_output_volume_db[port][stream]=-10000;
  }
  return true;
#else
  return false;
#endif  // JACK
}


bool MainObject::jackFadeOutputVolume(int card,int stream,int port,int level,
				     int length)
{
#ifdef JACK
  int diff;
  if ((stream <0) ||(stream >= RD_MAX_STREAMS) || 
      (port <0) || (port >= RD_MAX_PORTS)){
    return false;
  }
  if(jack_fade_timer[stream]->isActive()) {
    jack_fade_timer[stream]->stop();
  }
  if(level>jack_output_volume_db[port][stream]) {
    jack_fade_up[stream]=true;
    diff=level-jack_output_volume_db[port][stream];
  }
  else {
    jack_fade_up[stream]=false;
    diff=jack_output_volume_db[port][stream]-level;
  }
  jack_fade_volume_db[stream]=level;
  jack_fade_port[stream]=port;
  jack_fade_increment[stream]=diff*RD_JACK_FADE_INTERVAL/length;
  jack_fade_timer[stream]->start(RD_JACK_FADE_INTERVAL);
  return true;
#else
  return false;
#endif  // JACK
}


bool MainObject::jackSetInputLevel(int card,int port,int level)
{
#ifdef JACK
  return true;
#else
  return false;
#endif  // JACK
}


bool MainObject::jackSetOutputLevel(int card,int port,int level)
{
#ifdef JACK
  return true;
#else
  return false;
#endif  // JACK
}


bool MainObject::jackSetInputMode(int card,int stream,int mode)
{
#ifdef JACK
  jack_input_mode[card][stream]=mode;
  return true;
#else
  return false;
#endif  // JACK
}


bool MainObject::jackSetOutputMode(int card,int stream,int mode)
{
#ifdef JACK
  return true;
#else
  return false;
#endif  // JACK
}


bool MainObject::jackSetInputVoxLevel(int card,int stream,int level)
{
#ifdef JACK
  return true;
#else
  return false;
#endif  // JACK
}


bool MainObject::jackSetInputType(int card,int port,int type)
{
#ifdef JACK
  return true;
#else
  return false;
#endif  // JACK
}


bool MainObject::jackGetInputStatus(int card,int port)
{
#ifdef JACK
  return true;
#else
  return false;
#endif  // JACK
}


bool MainObject::jackGetInputMeters(int card,int port,short levels[2])
{
#ifdef JACK
  jack_default_audio_sample_t meter;
  if ((port <0) || (port >= RD_MAX_PORTS)){
    return false;
  }
  for(int i=0;i<2;i++) {
    meter=jack_input_meter[port][i]->average();
    if(meter==0.0) {
      levels[i]=-10000;
    }
    else {
      levels[i]=(short)(2000.0*log10(meter));
      if(levels[i]<-10000) {
	levels[i]=-10000;
      }
    }
  }
  return true;
#else
  return false;
#endif  // JACK
}


bool MainObject::jackGetOutputMeters(int card,int port,short levels[2])
{
#ifdef JACK
  jack_default_audio_sample_t meter;
  if ((port <0) || (port >= RD_MAX_PORTS)){
    return false;
  }

  for(int i=0;i<2;i++) {
    meter=jack_output_meter[port][i]->average();
    if(meter==0.0) {
      levels[i]=-10000;
    }
    else {
      levels[i]=(short)(2000.0*log10(meter));
      if(levels[i]<-10000) {
	levels[i]=-10000;
      }
    }
  }
  return true;
#else
  return false;
#endif  // JACK
}


bool MainObject::jackGetStreamOutputMeters(int card,int stream,short levels[2])
{
#ifdef JACK
  jack_default_audio_sample_t meter;
  if ((stream<0) || (stream>=RD_MAX_STREAMS)){
    return false;
  }

  for(int i=0;i<2;i++) {
    meter=jack_stream_output_meter[stream][i]->average();
    if(meter==0.0) {
      levels[i]=-10000;
    }
    else {
      levels[i]=(short)(2000.0*log10(meter));
      if(levels[i]<-10000) {
	levels[i]=-10000;
      }
    }
  }
  return true;
#else
  return false;
#endif  // JACK
}


void MainObject::jackGetOutputPosition(int card,unsigned *pos)
{
#ifdef JACK
  for(int i=0;i<RD_MAX_STREAMS;i++) {
    if(jack_play_wave[i]!=NULL) {
      pos[i]=1000*((unsigned long long)jack_offset[i]+jack_output_pos[i])/
	jack_play_wave[i]->getSamplesPerSec();
    }
    else {
      pos[i]=0;
    }
  }
#endif  // JACK
}

bool MainObject::jackSetPassthroughLevel(int card,int in_port,int out_port,
					int level)
{
#ifdef JACK
  if ((in_port <0) || (in_port >= RD_MAX_PORTS) || 
      (out_port <0) || (out_port >= RD_MAX_PORTS)){
    return false;
  }
  if(level>-10000) {
    jack_passthrough_volume[in_port][out_port]=
      (jack_default_audio_sample_t)pow10((double)level/2000.0);
    jack_passthrough_volume_db[in_port][out_port]=level;
  }
  else {
    jack_passthrough_volume[in_port][out_port]=0.0;
    jack_passthrough_volume_db[in_port][out_port]=-10000;
  }
  return true;
#else
  return false;
#endif  // JACK
}


void MainObject::jackConnectPorts(const QString &out,const QString &in)
{
#ifdef JACK
  if(jack_card<0) {
    return;
  }
  jack_connect(jack_client,(const char *)in,(const char *)out);  
#endif  // JACK
}


void MainObject::jackDisconnectPorts(const QString &out,const QString &in)
{
#ifdef JACK
  if(jack_card<0) {
    return;
  }
  jack_disconnect(jack_client,(const char *)in,(const char *)out);  
#endif  // JACK
}


int MainObject::GetJackOutputStream()
{
#ifdef JACK
  for(int i=0;i<RD_MAX_STREAMS;i++) {
    if(jack_play_ring[i]==NULL) {
      jack_play_ring[i]=new RDRingBuffer(RINGBUFFER_SIZE);
      return i;
    }
  }
  return -1;
#else
  return -1;
#endif
}


void MainObject::FreeJackOutputStream(int stream)
{
#ifdef JACK
  if ((stream <0) || (stream >= RD_MAX_STREAMS)){
    return;
  }
  delete jack_play_ring[stream];
  jack_play_ring[stream]=NULL;
  if(jack_st_conv[stream]!=NULL) {
    delete jack_st_conv[stream];
    jack_st_conv[stream]=NULL;
  }
#else
  return;
#endif
}


void MainObject::EmptyJackInputStream(int stream,bool done)
{
#ifdef JACK
  if ((stream <0) || (stream >= RD_MAX_STREAMS)){
    return;
  }
  unsigned n=jack_record_ring[stream]->
    read((char *)jack_sample_buffer,jack_record_ring[stream]->readSpace());
  WriteJackBuffer(stream,jack_sample_buffer,n,done);
#endif  // JACK
}

#ifdef JACK
void MainObject::WriteJackBuffer(int stream,jack_default_audio_sample_t *buffer,
				 unsigned len,bool done)
{
  ssize_t s;
  unsigned char mpeg[2048];
  unsigned frames;
  unsigned n;

  frames=len/(sizeof(jack_default_audio_sample_t)*
	      jack_record_wave[stream]->getChannels());
  jack_samples_recorded[stream]+=frames;
  switch(jack_record_wave[stream]->getFormatTag()) {
  case WAVE_FORMAT_PCM:
    n=len/sizeof(jack_default_audio_sample_t);
    src_float_to_short_array(buffer,jack_wave_buffer,n);
    jack_record_wave[stream]->writeWave(jack_wave_buffer,n*sizeof(short));
    break;

  case WAVE_FORMAT_MPEG:
#ifdef HAVE_TWOLAME
    for(unsigned i=0;i<frames;i+=1152) {
      if((i+1152)>frames) {
	n=frames-i;
      }
      else {
	n=1152;
      }
      if((s=twolame_encode_buffer_float32_interleaved(
		 twolame_lameopts[jack_card][stream],
		 buffer+i*jack_record_wave[stream]->getChannels(),
		 n,mpeg,2048))>=0) {
	jack_record_wave[stream]->writeWave(mpeg,s);
      }
      else {
	LogLine(RDConfig::LogErr,QString().
	   sprintf("TwoLAME encode error, card: %d, stream: %d",jack_card,
		   stream));
      }
    }
    if(done) {
      if((s=twolame_encode_flush(twolame_lameopts[jack_card][stream],
				 mpeg,2048))>=0) {
	jack_record_wave[stream]->writeWave(mpeg,s);
      }
    }
#endif  // HAVE_TWOLAME
    break;
  }
}
#endif  // JACK

void MainObject::FillJackOutputStream(int stream)
{
#ifdef JACK
  int n=0;
  unsigned mpeg_frames=0;
  unsigned frame_offset=0;
  int m=0;

  if ((stream <0) || (stream >= RD_MAX_STREAMS)){
    return;
  }
  int free=
    jack_play_ring[stream]->writeSpace()/sizeof(jack_default_audio_sample_t)-1;
  if((free<=0)||(jack_eof[stream]==true)) {
    return;
  }
  switch(jack_play_wave[stream]->getFormatTag()) {
  case WAVE_FORMAT_PCM:
  case WAVE_FORMAT_VORBIS:
    free=(int)free/jack_output_channels[stream]*jack_output_channels[stream];
    n=jack_play_wave[stream]->readWave(jack_wave_buffer,sizeof(short)*free)/
      sizeof(short);
    if((n!=free)&&(jack_st_conv[stream]==NULL)) {
      jack_eof[stream]=true;
      jack_stop_timer[stream]->stop();
    }
    src_short_to_float_array(jack_wave_buffer,jack_sample_buffer,n);
    break;

  case WAVE_FORMAT_MPEG:
#ifdef HAVE_MAD
    mpeg_frames=free/(1152*jack_output_channels[stream]);
    free=mpeg_frames*1152*jack_output_channels[stream];
    for(unsigned i=0;i<mpeg_frames;i++) {
      m=jack_play_wave[stream]->
	readWave(mad_mpeg[jack_card][stream]+mad_left_over[jack_card][stream],
		 mad_frame_size[jack_card][stream]);
      if(m==mad_frame_size[jack_card][stream]) {
	mad_stream_buffer(&mad_stream[jack_card][stream],
			  mad_mpeg[jack_card][stream],
			  m+mad_left_over[jack_card][stream]);
	while(mad_frame_decode(&mad_frame[jack_card][stream],
			    &mad_stream[jack_card][stream])==0) {
	  mad_synth_frame(&mad_synth[jack_card][stream],
			  &mad_frame[jack_card][stream]);
	  n+=(jack_output_channels[stream]*
	      mad_synth[jack_card][stream].pcm.length);
	  for(int j=0;j<mad_synth[jack_card][stream].pcm.length;j++) {
	    for(int k=0;k<mad_synth[jack_card][stream].pcm.channels;k++) {
	      jack_sample_buffer[frame_offset+
				 j*mad_synth[jack_card][stream].pcm.channels+k]=
		(jack_default_audio_sample_t)
		mad_f_todouble(mad_synth[jack_card][stream].pcm.samples[k][j]);
	      
	    }
	  }
	  frame_offset+=(mad_synth[jack_card][stream].pcm.length*
			 mad_synth[jack_card][stream].pcm.channels);
	}
      }
      else {  // End-of-file, read out last samples
	memset(mad_mpeg[jack_card][stream]+mad_left_over[jack_card][stream],0,
	       MAD_BUFFER_GUARD);
	mad_stream_buffer(&mad_stream[jack_card][stream],
			  mad_mpeg[jack_card][stream],
			  MAD_BUFFER_GUARD+mad_left_over[jack_card][stream]);
	if(mad_frame_decode(&mad_frame[jack_card][stream],
			    &mad_stream[jack_card][stream])==0) {
	  mad_synth_frame(&mad_synth[jack_card][stream],
			  &mad_frame[jack_card][stream]);
	  n+=(jack_output_channels[stream]*
	      mad_synth[jack_card][stream].pcm.length);
	  for(int j=0;j<mad_synth[jack_card][stream].pcm.length;j++) {
	    for(int k=0;k<mad_synth[jack_card][stream].pcm.channels;k++) {
	      jack_sample_buffer[frame_offset+
				 j*mad_synth[jack_card][stream].pcm.channels+k]=
		(jack_default_audio_sample_t)
		mad_f_todouble(mad_synth[jack_card][stream].pcm.samples[k][j]);
	    }
	  }
	}
	jack_eof[stream]=true;
	jack_stop_timer[stream]->stop();
	continue;
      }
      mad_left_over[jack_card][stream]=
	mad_stream[jack_card][stream].bufend-
	mad_stream[jack_card][stream].next_frame;
      memmove(mad_mpeg[jack_card][stream],
	      mad_stream[jack_card][stream].next_frame,
	      mad_left_over[jack_card][stream]);
    }
#endif  // HAVE_MAD
    break;
  }
  if(jack_st_conv[stream]==NULL) {
    jack_play_ring[stream]->
      write((char *)jack_sample_buffer,n*sizeof(jack_default_audio_sample_t));
  }
  else {
    jack_st_conv[stream]->
      putSamples(jack_sample_buffer,n/jack_output_channels[stream]);
    free=jack_play_ring[stream]->writeSpace()/
      (sizeof(jack_default_audio_sample_t)*jack_output_channels[stream])-1;
    while((n=jack_st_conv[stream]->
	   receiveSamples(jack_sample_buffer,free))>0) {
      jack_play_ring[stream]->
	write((char *)jack_sample_buffer,n*
	      sizeof(jack_default_audio_sample_t)*
	      jack_output_channels[stream]);
      free=jack_play_ring[stream]->writeSpace()/
	(sizeof(jack_default_audio_sample_t)*jack_output_channels[stream])-1;
    }
    if((jack_st_conv[stream]->numSamples()==0)&&
       (jack_st_conv[stream]->numUnprocessedSamples()==0)) {
      jack_eof[stream]=true;
      jack_stop_timer[stream]->stop();
    }
  }
#endif  // JACK
}


void MainObject::JackClock()
{
#ifdef JACK
  for(int i=0;i<RD_MAX_STREAMS;i++) {
    if(jack_stopping[i]) {
      jack_stopping[i]=false;
      statePlayUpdate(jack_card,i,2);
    }
    if(jack_playing[i]&&((jack_clock_phase%4)==0)) {
      FillJackOutputStream(i);
    }
  }
  jack_clock_phase++;
  for(int i=0;i<RD_MAX_PORTS;i++) {
    if(jack_recording[i]) {
      EmptyJackInputStream(i,false);
    }
  }
#endif  // JACK
}


void MainObject::JackSessionSetup()
{
#ifdef JACK
  int count=0;
  RDProfile *profile=new RDProfile();
  profile->setSource(RD_CONF_FILE);
  bool src_ok=false;
  bool dest_ok=false;
  QString src_tag="Source1";
  QString dest_tag="Destination1";
  QString src=profile->stringValue("JackSession",src_tag,"",&src_ok);
  QString dest=profile->stringValue("JackSession",dest_tag,"",&dest_ok);
  while(src_ok&&dest_ok) {
    if(jack_connect(jack_client,(const char *)src,(const char *)dest)!=0) {
      LogLine(RDConfig::LogNotice,QString().
	      sprintf("unable to connect %s to %s",
		      (const char *)src,(const char *)dest));
    }
    count++;
    src_tag=QString().sprintf("Source%d",count+1);
    dest_tag=QString().sprintf("Destination%d",count+1);
    src=profile->stringValue("JackSession",src_tag,"",&src_ok);
    dest=profile->stringValue("JackSession",dest_tag,"",&dest_ok);
  }
  delete profile;
#endif  // JACK
}
