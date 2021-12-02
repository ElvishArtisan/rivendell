// driver_jack.cpp
//
// caed(8) driver for Advanced Linux Audio Architecture devices
//
//   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <QProcessEnvironment>

#include <rdconf.h>
#include <rddatedecode.h>
#include <rdescape_string.h>
#include <rdprofile.h>

#include "driver_jack.h"

#ifdef JACK
#endif  // JACK

#ifdef JACK
//
// Callback Variables
//
jack_client_t *jack_client;
RDMeterAverage *jack_input_meter[RD_MAX_PORTS][2];
RDMeterAverage *jack_output_meter[RD_MAX_PORTS][2];
RDMeterAverage *jack_stream_output_meter[RD_MAX_STREAMS][2];
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
      if(jack_input_port[i][j]!=NULL) {
	jack_input_buffer[i][j]=(jack_default_audio_sample_t *)
	  jack_port_get_buffer(jack_input_port[i][j],nframes);
      }
      if(jack_output_port[i][j]!=NULL) {
	jack_output_buffer[i][j]=(jack_default_audio_sample_t *)
	  jack_port_get_buffer(jack_output_port[i][j],nframes);
      }
    }
  }

  //
  // Zero Output Ports
  //
  for(int i=0;i<RD_MAX_PORTS;i++) {
    for(int j=0;j<2;j++) {
      if(jack_output_port[i][j]!=NULL) {
	for(unsigned k=0;k<nframes;k++) {
	  jack_output_buffer[i][j][k]=0.0;
	}
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
	  if((jack_output_port[j][k]!=NULL)&&(jack_input_port[i][k]!=NULL)) {
	    for(unsigned l=0;l<nframes;l++) {
	      jack_output_buffer[j][k][l]+=
		jack_input_buffer[i][k][l]*jack_passthrough_volume[i][j];
	    }
	  }
	}
      }
    }
  }

  //
  // Process Input Streams
  //
  for(int i=0;i<RD_MAX_PORTS;i++) {
    if(jack_input_port[i][0]!=NULL) {
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
	if(jack_output_port[j][0]!=NULL) {
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
      }
      double ratio=(double)jack_output_sample_rate[i]/(double)jack_sample_rate;
      jack_output_pos[i]+=(int)(((double)n*ratio)+0.5);
    }
  }

  //
  // Process Meters
  //
  for(int i=0;i<RD_MAX_PORTS;i++) {
    if(jack_input_port[i][0]!=NULL) {
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
    }
    if(jack_output_port[i][0]!=NULL) {
      // output meters
      for(int j=0;j<2;j++) {
	out_meter[j]=0.0;
	for(unsigned k=0;k<nframes;k++) {
	  if(jack_output_buffer[i][j][k]>out_meter[j]) 
	    out_meter[j]=jack_output_buffer[i][j][k];
	}
	jack_output_meter[i][j]->addValue(out_meter[j]);
      }
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




DriverJack::DriverJack(QObject *parent)
  : Driver(RDStation::Jack,parent)
{
#ifdef JACK
  jack_connected=false;
  jack_activated=false;

  //
  // Allocate Temporary Buffers
  //
  jack_wave_buffer=new short[RINGBUFFER_SIZE];
  jack_wave32_buffer=new int[RINGBUFFER_SIZE];
  jack_wave24_buffer=new uint8_t[RINGBUFFER_SIZE];
  jack_sample_buffer=new jack_default_audio_sample_t[RINGBUFFER_SIZE];
#ifdef HAVE_MAD
  for(int i=0;i<RD_MAX_CARDS;i++) {
    for(int j=0;j<RD_MAX_STREAMS;j++) {
      mad_mpeg[i][j]=new unsigned char[16384];
    }
  }
#endif  // HAVE_MAD

  LoadTwoLame();
  LoadMad();
#endif  // JACK
}


DriverJack::~DriverJack()
{
#ifdef JACK
  for(int i=0;i<jack_clients.size();i++) {
    jack_clients[i]->kill();
    delete jack_clients[i];
  }
  jack_clients.clear();
  if(jack_activated) {
    jack_deactivate(jack_client);
  }
  delete jack_wave_buffer;
  delete jack_wave32_buffer;
  delete jack_wave24_buffer;
  delete jack_sample_buffer;
#ifdef HAVE_MAD
  for(int i=0;i<RD_MAX_CARDS;i++) {
    for(int j=0;j<RD_MAX_STREAMS;j++) {
      delete mad_mpeg[i][j];
    }
  }
#endif  // HAVE_MAD
#endif  // JACK
}


QString DriverJack::version() const
{
#ifdef JACK
  return QString(jack_get_version_string());
#else
  return QString();
#endif  // JACK
}


bool DriverJack::initialize(unsigned *next_cardnum)
{
#ifdef JACK
  jack_options_t jackopts=JackNullOption;
  jack_status_t jackstat=JackFailure;

  if((*next_cardnum)==RD_MAX_CARDS) {
    rda->syslog(LOG_INFO,"no more RD cards available");
    return false;
  }
  jack_card=*next_cardnum;
  QString name=QString::asprintf("rivendell_%d",jack_card);

  //
  // Start Jack Server
  //
  if(rda->station()->startJack()) {
    QStringList args=
      rda->station()->jackCommandLine().split(" ",QString::SkipEmptyParts);
    if(args.size()) {
      QString program=args.at(0);
      args.removeFirst();
      QProcessEnvironment penv=QProcessEnvironment::systemEnvironment();
      QProcess *proc=new QProcess(this);
      penv.insert("JACK_PROMISCUOUS_SERVER","audio");
      proc->setProcessEnvironment(penv);
      proc->start(program,args);
      if(proc->waitForStarted()) {
        rda->syslog(LOG_INFO,"JACK server started");
      }
      else {
        rda->syslog(LOG_WARNING,
			    "failed to start JACK server");
      }
      sleep(1);
    }
    else {
        rda->syslog(LOG_WARNING,
			    "could not start JACK server: no command line specified");
    }
  }

  //
  // Attempt to Connect to Jack Server
  //
  jackopts=JackNoStartServer;
  if(rda->station()->jackServerName().isEmpty()) {
    jack_client=jack_client_open(name.toUtf8(),jackopts,&jackstat);
  }
  else {
    jack_client=
      jack_client_open(name.toUtf8(),jackopts,&jackstat,
		       rda->station()->jackServerName().toUtf8().constData());
  }
  if(jack_client==NULL) {
    if((jackstat&JackInvalidOption)!=0) {
      fprintf (stderr, "invalid or unsupported JACK option\n");
      rda->syslog(LOG_WARNING,
			    "invalid or unsupported JACK option");
    }

    if((jackstat&JackServerError)!=0) {
      fprintf (stderr, "communication error with the JACK server\n");
      rda->syslog(LOG_WARNING,
			    "communication error with the JACK server");
    }

    if((jackstat&JackNoSuchClient)!=0) {
      fprintf (stderr, "requested JACK client does not exist\n");
      rda->syslog(LOG_WARNING,
			    "requested JACK client does not exist");
    }

    if((jackstat&JackLoadFailure)!=0) {
      fprintf (stderr, "unable to load internal JACK client\n");
      rda->syslog(LOG_WARNING,
			    "unable to load internal JACK client");
    }

    if((jackstat&JackInitFailure)!=0) {
      fprintf (stderr, "unable to initialize JACK client\n");
      rda->syslog(LOG_WARNING,
			    "unable to initialize JACK client");
    }

    if((jackstat&JackShmFailure)!=0) {
      fprintf (stderr, "unable to access JACK shared memory\n");
      rda->syslog(LOG_WARNING,
			    "unable to access JACK shared memory");
    }

    if((jackstat&JackVersionError)!=0) {
      fprintf (stderr, "JACK protocol version mismatch\n");
      rda->syslog(LOG_WARNING,
			    "JACK protocol version mismatch");
    }

    if((jackstat&JackServerStarted)!=0) {
      fprintf (stderr, "JACK server started\n");
      rda->syslog(LOG_WARNING,"JACK server started");
    }

    if((jackstat&JackServerFailed)!=0) {
      fprintf (stderr, "unable to communication with JACK server\n");
      rda->syslog(LOG_WARNING,
			    "unable to communicate with JACK server");
    }

    if((jackstat&JackNameNotUnique)!=0) {
      fprintf (stderr, "JACK client name not unique\n");
      rda->syslog(LOG_WARNING,
			    "JACK client name not unique");
    }

    if((jackstat&JackFailure)!=0) {
      fprintf (stderr, "JACK general failure\n");
      rda->syslog(LOG_WARNING,"JACK general failure");
    }
    jack_card=-1;
    fprintf (stderr, "no connection to JACK server\n");
    rda->syslog(LOG_WARNING,"no connection to JACK server");
    return false;
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
  rda->syslog(LOG_INFO,"connected to JACK server");

  //
  // Start JACK Clients
  //
  jack_client_start_timer=new QTimer(this);
  jack_client_start_timer->setSingleShot(true);
  connect(jack_client_start_timer,SIGNAL(timeout()),
	  this,SLOT(clientStartData()));
  jack_client_start_timer->start(6000);

  //
  // Tell the database about us
  //
  if(jack_connected) {
    rda->station()->setCardDriver(jack_card,RDStation::Jack);
    rda->station()->setCardName(jack_card,"JACK Audio Connection Kit");
    rda->station()->setCardInputs(jack_card,RD_MAX_PORTS);
    rda->station()->setCardOutputs(jack_card,RD_MAX_PORTS);
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
  connect(stop_mapper,SIGNAL(mapped(int)),this,SLOT(stopTimerData(int)));
  QSignalMapper *fade_mapper=new QSignalMapper(this);
  connect(fade_mapper,SIGNAL(mapped(int)),this,SLOT(fadeTimerData(int)));
  QSignalMapper *record_mapper=new QSignalMapper(this);
  connect(record_mapper,SIGNAL(mapped(int)),this,SLOT(recordTimerData(int)));
  for(int i=0;i<RD_MAX_STREAMS;i++) {
    jack_stop_timer[i]=new QTimer(this);
    jack_stop_timer[i]->setSingleShot(true);
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
    for(int j=0;j<2;j++) {
      jack_output_port[i][j]=NULL;
      jack_input_port[i][j]=NULL;
    }
  }
  for(int i=0;i<rda->station()->jackPorts();i++) {
    name=QString::asprintf("playout_%dL",i);
    jack_output_port[i][0]=
      jack_port_register(jack_client,name.toUtf8(),
			 JACK_DEFAULT_AUDIO_TYPE,
			 JackPortIsOutput|JackPortIsTerminal,0);
    name=QString::asprintf("playout_%dR",i);
    jack_output_port[i][1]=
      jack_port_register(jack_client,name.toUtf8(),
			 JACK_DEFAULT_AUDIO_TYPE,
			 JackPortIsOutput|JackPortIsTerminal,0);
    name=QString::asprintf("record_%dL",i);
    jack_input_port[i][0]=
      jack_port_register(jack_client,name.toUtf8(),
			 JACK_DEFAULT_AUDIO_TYPE,
			 JackPortIsInput|JackPortIsTerminal,0);
    name=QString::asprintf("record_%dR",i);
    jack_input_port[i][1]=
      jack_port_register(jack_client,name.toUtf8(),
			 JACK_DEFAULT_AUDIO_TYPE,
			 JackPortIsInput|JackPortIsTerminal,0);
  }

  //
  // Allocate Temporary Buffers
  //
  JackInitCallback();

  //
  // Join the Graph
  //
  if(jack_activate(jack_client)) {
    return false;
  }
  jack_sample_rate=jack_get_sample_rate(jack_client);
  if(jack_sample_rate!=systemSampleRate()) {
    fprintf (stderr,"JACK sample rate mismatch!\n");
    rda->syslog(LOG_WARNING,"JACK sample rate mismatch!");
  }
  jack_activated=true;
  (*next_cardnum)++;
  addCard(jack_card);
  JackSessionSetup();
  return true;
#else
  return false;
#endif  // JACK
}


bool DriverJack::loadPlayback(int card,QString wavename,int *stream)
{
#ifdef JACK
  if((*stream=GetJackOutputStream())<0) {
    rda->syslog(LOG_DEBUG,"jackLoadPlayback(%s)   GetJackOutputStream():%d <0",
		wavename.toUtf8().constData(),*stream);
    return false;
  }
  jack_play_wave[*stream]=new RDWaveFile(wavename);
  if(!jack_play_wave[*stream]->openWave()) {
    rda->syslog(LOG_DEBUG,"jackLoadPlayback(%s) openWave() failed to open file",
		wavename.toUtf8().constData());
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
    if(!InitMadDecoder(card,*stream,jack_play_wave[*stream])) {
      delete jack_play_wave[*stream];
      jack_play_wave[*stream]=NULL;
      FreeJackOutputStream(*stream);
      *stream=-1;
      return false;
    }
    break;

  default:
    rda->syslog(LOG_DEBUG,
	"jackLoadPlayback(%s) getFormatTag()%d || getBistsPerSample()%d failed",
		wavename.toUtf8().constData(),
	   jack_play_wave[*stream]->getFormatTag(),
	   jack_play_wave[*stream]->getBitsPerSample());
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


bool DriverJack::unloadPlayback(int card,int stream)
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


bool DriverJack::playbackPosition(int card,int stream,unsigned pos)
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
      start(jack_play_wave[stream]->getExtTimeLength()-pos);
  }
  return true;
#else
  return false;
#endif  // JACK
}


bool DriverJack::play(int card,int stream,int length,int speed,bool pitch,
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
    jack_stop_timer[stream]->start(length);
  }
  statePlayUpdate(card,stream,1);
  return true;
#else
  return false;
#endif  // JACK
}


bool DriverJack::stopPlayback(int card,int stream)
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


bool DriverJack::timescaleSupported(int card)
{
#ifdef JACK
  return true;
#else
  return false;
#endif  // JACK
}


bool DriverJack::loadRecord(int card,int port,int coding,int chans,int samprate,
			    int bitrate,QString wavename)
{
#ifdef JACK
  jack_record_wave[port]=new RDWaveFile(wavename);
  switch(coding) {
  case 0:  // PCM16
    jack_record_wave[port]->setFormatTag(WAVE_FORMAT_PCM);
    jack_record_wave[port]->setChannels(chans);
    jack_record_wave[port]->setSamplesPerSec(samprate);
    jack_record_wave[port]->setBitsPerSample(16);
    break;

  case 4:  // PCM24
    jack_record_wave[port]->setFormatTag(WAVE_FORMAT_PCM);
    jack_record_wave[port]->setChannels(chans);
    jack_record_wave[port]->setSamplesPerSec(samprate);
    jack_record_wave[port]->setBitsPerSample(24);
    break;

  case 2:  // MPEG Layer 2
    if(!InitTwoLameEncoder(card,port,chans,samprate,bitrate)) {
      delete jack_record_wave[port];
      jack_record_wave[port]=NULL;
      return false;
    }
    jack_record_wave[port]->setFormatTag(WAVE_FORMAT_MPEG);
    jack_record_wave[port]->setChannels(chans);
    jack_record_wave[port]->setSamplesPerSec(samprate);
    jack_record_wave[port]->setBitsPerSample(16);
    jack_record_wave[port]->setHeadLayer(ACM_MPEG_LAYER2);
    switch(chans) {
    case 1:
      jack_record_wave[port]->setHeadMode(ACM_MPEG_SINGLECHANNEL);
      break;

    case 2:
      jack_record_wave[port]->setHeadMode(ACM_MPEG_STEREO);
      break;

    default:
      rda->syslog(LOG_WARNING,
	     "requested unsupported channel count %d, card: %d, port: %d",
	     chans,card,port);
      delete jack_record_wave[port];
      jack_record_wave[port]=NULL;
      return false;
    }
    jack_record_wave[port]->setHeadBitRate(bitrate);
    jack_record_wave[port]->setMextChunk(true);
    jack_record_wave[port]->setMextHomogenous(true);
    jack_record_wave[port]->setMextPaddingUsed(false);
    jack_record_wave[port]->setMextHackedBitRate(true);
    jack_record_wave[port]->setMextFreeFormat(false);
    jack_record_wave[port]->
      setMextFrameSize(144*jack_record_wave[port]->getHeadBitRate()/
		       jack_record_wave[port]->getSamplesPerSec());
    jack_record_wave[port]->setMextAncillaryLength(5);
    jack_record_wave[port]->setMextLeftEnergyPresent(true);
    if(chans>1) {
      jack_record_wave[port]->setMextRightEnergyPresent(true);
    }
    else {
      jack_record_wave[port]->setMextRightEnergyPresent(false);
    }
    jack_record_wave[port]->setMextPrivateDataPresent(false);
    break;

  default:
    rda->syslog(LOG_WARNING,
	   "requested invalid audio encoding %d, card: %d, port: %d",
	   coding,card,port);
    delete jack_record_wave[port];
    jack_record_wave[port]=NULL;
    return false;
  }
  jack_record_wave[port]->setBextChunk(true);
  jack_record_wave[port]->setLevlChunk(true);
  if(!jack_record_wave[port]->createWave()) {
    delete jack_record_wave[port];
    jack_record_wave[port]=NULL;
    return false;
  }
  RDCheckExitCode(rda->config(),"jackLoadRecord() chown",
		  chown(wavename.toUtf8(),rda->config()->uid(),rda->config()->gid()));
  jack_input_channels[port]=chans;
  jack_record_ring[port]=new RDRingBuffer(RINGBUFFER_SIZE);
  jack_record_ring[port]->reset();
  jack_ready[port]=true;
  return true;
#else
  return false;
#endif  // JACK
}


bool DriverJack::unloadRecord(int card,int port,unsigned *len)
{
#ifdef JACK
  if((port <0)||(port>=RD_MAX_PORTS)) {
    return false;
  }
  jack_recording[port]=false;
  jack_ready[port]=false;
  EmptyJackInputStream(port,true);
  *len=jack_samples_recorded[port];
  jack_samples_recorded[port]=0;
  jack_record_wave[port]->closeWave(*len);
  delete jack_record_wave[port];
  jack_record_wave[port]=NULL;
  delete jack_record_ring[port];
  jack_record_ring[port]=NULL;
  FreeTwoLameEncoder(card,port);
  return true;
#else
  return false;
#endif  // JACK
}


bool DriverJack::record(int card,int port,int length,int thres)
{
#ifdef JACK
  if((port<0)||(port>=RD_MAX_PORTS)) {
    return false;
  }
  if(!jack_ready[port]) {
    return false;
  }
  jack_recording[port]=true;
  if(jack_input_vox[port]==0.0) {
    if(length>0) {
      jack_record_timer[port]->start(length);
    }
    stateRecordUpdate(card,port,4);
  }
  return true;
#else
  return false;
#endif  // JACK
}


bool DriverJack::stopRecord(int card,int port)
{
#ifdef JACK
  if((port<0)||(port>=RD_MAX_PORTS)) {
    return false;
  }
  if(!jack_recording[port]) {
    return false;
  }
  jack_recording[port]=false;
  return true;
#else
  return false;
#endif  // JACK
}


bool DriverJack::setClockSource(int card,int src)
{
#ifdef JACK
  return true;
#else
  return false;
#endif  // JACK
}


bool DriverJack::setInputVolume(int card,int stream,int level)
{
#ifdef JACK
  if((stream<0)||(stream>=RD_MAX_STREAMS)) {
    return false;
  }
  if(level>-10000) {
    jack_input_volume[stream]=
      (jack_default_audio_sample_t)pow(10.0,(double)level/2000.0);
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


bool DriverJack::setOutputVolume(int card,int stream,int port,int level)
{
#ifdef JACK
  if((stream<0)||(stream>=RD_MAX_STREAMS)||(port<0)||(port>=RD_MAX_PORTS)) {
    return false;
  }
  if(level>-10000) {
    jack_output_volume[port][stream]=
      (jack_default_audio_sample_t)pow(10.0,(double)level/2000.0);
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


bool DriverJack::fadeOutputVolume(int card,int stream,int port,int level,
				  int length)
{
#ifdef JACK
  int diff;

  if((stream<0)||(stream>=RD_MAX_STREAMS)||(port<0)||(port>=RD_MAX_PORTS)) {
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


bool DriverJack::setInputLevel(int card,int port,int level)
{
#ifdef JACK
  return true;
#else
  return false;
#endif  // JACK
}


bool DriverJack::setOutputLevel(int card,int port,int level)
{
#ifdef JACK
  return true;
#else
  return false;
#endif  // JACK
}


bool DriverJack::setInputMode(int card,int stream,int mode)
{
#ifdef JACK
  jack_input_mode[card][stream]=mode;
  return true;
#else
  return false;
#endif  // JACK
}


bool DriverJack::setOutputMode(int card,int stream,int mode)
{
#ifdef JACK
  return true;
#else
  return false;
#endif  // JACK
}


bool DriverJack::setInputVoxLevel(int card,int stream,int level)
{
#ifdef JACK
  return true;
#else
  return false;
#endif  // JACK
}


bool DriverJack::setInputType(int card,int port,int type)
{
#ifdef JACK
  return true;
#else
  return false;
#endif  // JACK
}


bool DriverJack::getInputStatus(int card,int port)
{
#ifdef JACK
  return true;
#else
  return false;
#endif  // JACK
}


bool DriverJack::getInputMeters(int card,int port,short levels[2])
{
#ifdef JACK
  jack_default_audio_sample_t meter;
  if((port<0)||(port>=RD_MAX_PORTS)) {
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


bool DriverJack::getOutputMeters(int card,int port,short levels[2])
{
#ifdef JACK
  jack_default_audio_sample_t meter;
  if((port<0)||(port>=RD_MAX_PORTS)) {
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


bool DriverJack::getStreamOutputMeters(int card,int stream,short levels[2])
{
#ifdef JACK
  jack_default_audio_sample_t meter;
  if((stream<0)||(stream>=RD_MAX_STREAMS)) {
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


bool DriverJack::setPassthroughLevel(int card,int in_port,int out_port,
				     int level)
{
#ifdef JACK
  if((in_port <0)||(in_port>=RD_MAX_PORTS)|| 
     (out_port<0)||(out_port>=RD_MAX_PORTS)) {
    return false;
  }
  if(level>-10000) {
    jack_passthrough_volume[in_port][out_port]=
      (jack_default_audio_sample_t)pow(10.0,(double)level/2000.0);
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


void DriverJack::getOutputPosition(int card,unsigned *pos)
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


void DriverJack::processBuffers()
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


void DriverJack::stopTimerData(int stream)
{
#ifdef JACK
  stopPlayback(jack_card,stream);
  //  statePlayUpdate(jack_card,stream,2);
#endif  // JACK
}


void DriverJack::fadeTimerData(int stream)
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
  setOutputVolume(jack_card,stream,jack_fade_port[stream],level);
#endif  // JACK
}


void DriverJack::recordTimerData(int stream)
{
#ifdef JACK
  stopRecord(jack_card,stream);
  //  stateRecordUpdate(jack_card,stream,2);
#endif  // JACK
}


void DriverJack::clientStartData()
{
#ifdef JACK
  QString sql=QString("select ")+
    "`DESCRIPTION`,"+   // 00
    "`COMMAND_LINE` "+  // 01
    "from `JACK_CLIENTS` where "+
    "`STATION_NAME`='"+RDEscapeString(rda->config()->stationName())+"'";
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    QString cmd=RDDateDecode(q->value(1).toString(),QDate::currentDate(),
			     rda->station(),rda->config(),
			     rda->config()->provisioningServiceName(rda->config()->stationName()));
    QStringList args=cmd.split(" ",QString::SkipEmptyParts);
    QString program=args.at(0);
    args.removeFirst();
    jack_clients.push_back(new QProcess(this));
    jack_clients.back()->start(program,args);
    if(jack_clients.back()->waitForStarted()) {
      rda->syslog(LOG_INFO,"started JACK Client \"%s\"",
	     (const char *)q->value(0).toString().toUtf8());
    }
    else {
      rda->syslog(LOG_WARNING,
			    "failed to start JACK Client \"%s\" [%s]",
			    (const char *)q->value(0).toString().toUtf8(),
			    (const char *)q->value(1).toString().toUtf8());
    }
    sleep(1);
  }
  delete q;
#endif  // JACK
}


int DriverJack::GetJackOutputStream()
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


void DriverJack::FreeJackOutputStream(int stream)
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


void DriverJack::EmptyJackInputStream(int stream,bool done)
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
void DriverJack::WriteJackBuffer(int stream,jack_default_audio_sample_t *buffer,
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
    switch(jack_record_wave[stream]->getBitsPerSample()) {
    case 16:  // PCM16
      n=len/sizeof(jack_default_audio_sample_t);
      src_float_to_short_array(buffer,jack_wave_buffer,n);
      jack_record_wave[stream]->writeWave(jack_wave_buffer,n*sizeof(short));
      break;

    case 24:  // PCM24
      n=len/sizeof(jack_default_audio_sample_t);
      src_float_to_int_array(buffer,jack_wave32_buffer,n);
      for(unsigned i=0;i<n;i++) {
	for(unsigned j=0;j<3;j++) {
	  jack_wave24_buffer[3*i+j]=((uint8_t *)jack_wave32_buffer)[4*i+j+1];
	}
      }
      jack_record_wave[stream]->writeWave(jack_wave24_buffer,n*3);
      break;
    }
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
	rda->syslog(LOG_WARNING,
	       "TwoLAME encode error, card: %d, stream: %d",jack_card,stream);
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

void DriverJack::FillJackOutputStream(int stream)
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
    switch(jack_play_wave[stream]->getBitsPerSample()) {
    case 16:  // PMC16
      free=(int)free/jack_output_channels[stream]*jack_output_channels[stream];
      n=jack_play_wave[stream]->readWave(jack_wave_buffer,sizeof(short)*free)/
	sizeof(short);
      if((n!=free)&&(jack_st_conv[stream]==NULL)) {
	jack_eof[stream]=true;
	jack_stop_timer[stream]->stop();
      }
      src_short_to_float_array(jack_wave_buffer,jack_sample_buffer,n);
      break;

    case 24:  // PMC24
      free=(int)free/jack_output_channels[stream]*jack_output_channels[stream];
      n=jack_play_wave[stream]->readWave(jack_wave24_buffer,3*free)/3;
      if((n!=free)&&(jack_st_conv[stream]==NULL)) {
	jack_eof[stream]=true;
	jack_stop_timer[stream]->stop();
      }
      for(int i=0;i<n;i++) {
	for(unsigned j=0;j<3;j++) {
	  ((uint8_t *)jack_wave32_buffer)[4*i+j+1]=jack_wave24_buffer[3*i+j];
	}
      }
      src_int_to_float_array(jack_wave32_buffer,jack_sample_buffer,n);
      break;
    }
    break;

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


void DriverJack::JackClock()
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


void DriverJack::JackSessionSetup()
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
    if(jack_connect(jack_client,src.toUtf8(),dest.toUtf8())!=0) {
      rda->syslog(LOG_WARNING,"unable to connect %s to %s",
	     (const char *)src.toUtf8(),(const char *)dest.toUtf8());
    }
    count++;
    src_tag=QString::asprintf("Source%d",count+1);
    dest_tag=QString::asprintf("Destination%d",count+1);
    src=profile->stringValue("JackSession",src_tag,"",&src_ok);
    dest=profile->stringValue("JackSession",dest_tag,"",&dest_ok);
  }
  delete profile;
#endif  // JACK
}
