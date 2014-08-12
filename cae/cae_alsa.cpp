// cae_alsa.cpp
//
// The ALSA Driver for the Core Audio Engine component of Rivendell
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: cae_alsa.cpp,v 1.48.6.5 2013/06/26 23:18:40 cvs Exp $
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
#include <signal.h>

#include <samplerate.h>

#include <qsignalmapper.h>

#include <rd.h>
#include <rdringbuffer.h>
#include <rdmeteraverage.h>

#include <cae.h>

#ifdef ALSA
//
// Callback Variables
//
volatile int alsa_input_channels[RD_MAX_CARDS][RD_MAX_PORTS];
volatile int alsa_output_channels[RD_MAX_CARDS][RD_MAX_STREAMS];
RDMeterAverage *alsa_input_meter[RD_MAX_CARDS][RD_MAX_PORTS][2];
RDMeterAverage *alsa_output_meter[RD_MAX_CARDS][RD_MAX_PORTS][2];
RDMeterAverage *alsa_stream_output_meter[RD_MAX_CARDS][RD_MAX_STREAMS][2];
volatile double alsa_input_volume[RD_MAX_CARDS][RD_MAX_PORTS];
volatile double alsa_output_volume[RD_MAX_CARDS][RD_MAX_PORTS][RD_MAX_STREAMS];
volatile double 
  alsa_passthrough_volume[RD_MAX_CARDS][RD_MAX_PORTS][RD_MAX_PORTS];
volatile double alsa_input_vox[RD_MAX_CARDS][RD_MAX_PORTS];
RDRingBuffer *alsa_play_ring[RD_MAX_CARDS][RD_MAX_STREAMS];
RDRingBuffer *alsa_record_ring[RD_MAX_CARDS][RD_MAX_PORTS];
RDRingBuffer *alsa_passthrough_ring[RD_MAX_CARDS][RD_MAX_PORTS];
volatile bool alsa_playing[RD_MAX_CARDS][RD_MAX_STREAMS];
volatile bool alsa_stopping[RD_MAX_CARDS][RD_MAX_STREAMS];
volatile bool alsa_eof[RD_MAX_CARDS][RD_MAX_STREAMS];
volatile int alsa_output_pos[RD_MAX_CARDS][RD_MAX_STREAMS];
volatile bool alsa_recording[RD_MAX_CARDS][RD_MAX_PORTS];
volatile bool alsa_ready[RD_MAX_CARDS][RD_MAX_PORTS];
volatile int alsa_channels;


void AlsaCapture1Callback(struct alsa_format *alsa_format)
{
}


void AlsaCapture2Callback(struct alsa_format *alsa_format)
{
  char alsa_buffer[RINGBUFFER_SIZE];
  int modulo;
  short in_meter[RD_MAX_PORTS][2];

  while(!alsa_format->exiting) {
    int s=snd_pcm_readi(alsa_format->pcm,alsa_format->card_buffer,
			rd_config->alsaPeriodSize()/(alsa_format->periods*2));
    if(((snd_pcm_state(alsa_format->pcm)!=SND_PCM_STATE_RUNNING)&&
	(!alsa_format->exiting))||(s<0)) {
      snd_pcm_drop (alsa_format->pcm);
      snd_pcm_prepare(alsa_format->pcm);
      LogLine(RDConfig::LogNotice,QString().
	      sprintf("****** ALSA Capture Xrun - Card: %d ******",
		      alsa_format->card));
    }
    else {
      switch(alsa_format->format) {
	  case SND_PCM_FORMAT_S16_LE:
	    modulo=alsa_format->channels;
	    for(unsigned i=0;i<(alsa_format->channels/2);i++) {
	      if(alsa_recording[alsa_format->card][i]) {
		if(alsa_input_volume[alsa_format->card][i]!=0.0) {
		  switch(alsa_input_channels[alsa_format->card][i]) {
		      case 1:
			for(int k=0;k<(2*s);k++) {
			  ((short *)alsa_buffer)[k]=
			    (short)(alsa_input_volume[alsa_format->card][i]*
				    (double)(((short *)alsa_format->
					      card_buffer)
					     [modulo*k+2*i]))+
			    (short)(alsa_input_volume[alsa_format->card][i]*
				    (double)(((short *)alsa_format->
					      card_buffer)
					     [modulo*k+2*i+1]));
			}
			alsa_record_ring[alsa_format->card][i]->
			  write(alsa_buffer,s*sizeof(short));
			break;
			
		      case 2:
			for(int k=0;k<s;k++) {
			  ((short *)alsa_buffer)[2*k]=
			    (short)(alsa_input_volume[alsa_format->card][i]*
				    (double)(((short *)alsa_format->
					      card_buffer)
					     [modulo*k+2*i]));
			  ((short *)alsa_buffer)[2*k+1]=
			    (short)(alsa_input_volume[alsa_format->card][i]*
				    (double)(((short *)alsa_format->
					      card_buffer)
					     [modulo*k+2*i+1]));
			}
			alsa_record_ring[alsa_format->card][i]->
			  write(alsa_buffer,s*2*sizeof(short));
			break;
		  }
		}
	      }
	    }
	    
	    //
	    // Process Passthroughs
	    //
	    for(unsigned i=0;i<alsa_format->channels;i+=2) {
	      for(unsigned j=0;j<2;j++) {
		for(int k=0;k<s;k++) {
		  ((short *)alsa_format->passthrough_buffer)[2*k+j]=
		    ((short *)alsa_format->
		     card_buffer)[alsa_format->channels*k+2*i+j];
		}
	      }
	      if(alsa_passthrough_ring[alsa_format->card][i/2]==NULL) {
		printf("RING ERROR  Card=%d  Port=%d\n",alsa_format->card,i/2);
	      }
	      else {
	      alsa_passthrough_ring[alsa_format->card][i/2]->
		write(alsa_format->passthrough_buffer,4*s);
	      }
	    }
	    
	    //
	    // Process Input Meters
	    //
	    for(unsigned i=0;i<alsa_format->channels;i+=2) {
	      for(unsigned j=0;j<2;j++) {
		in_meter[i/2][j]=0;
		for(int k=0;k<s;k++) {
		  if(((short *)alsa_format->
		      card_buffer)[alsa_format->channels*k+2*i+j]>
		     in_meter[i][j]) {
		    in_meter[i][j]=
		      ((short *)alsa_format->
		       card_buffer)[alsa_format->channels*k+2*i+j];
		  }
		}
		alsa_input_meter[alsa_format->card][i/2][j]->
		  addValue(((double)in_meter[i/2][j])/32768.0);
	      }
	    }
	    break;
	    
	  case SND_PCM_FORMAT_S32_LE:
	    modulo=alsa_format->channels*2;
	    for(unsigned i=0;i<(alsa_format->channels/2);i++) {
	      if(alsa_recording[alsa_format->card][i]) {
		if(alsa_input_volume[alsa_format->card][i]!=0.0) {
		  switch(alsa_input_channels[alsa_format->card][i]) {
		      case 1:
			for(int k=0;k<(2*s);k++) {
			  ((short *)alsa_buffer)[k]=
			    (short)(alsa_input_volume[alsa_format->card][i]*
				    (double)(((short *)alsa_format->
					      card_buffer)
					     [modulo*k+4*i+1]))+
			    (short)(alsa_input_volume[alsa_format->card][i]*
				    (double)(((short *)alsa_format->
					      card_buffer)
					     [modulo*k+4*i+3]));
			}
			alsa_record_ring[alsa_format->card][i]->
			  write(alsa_buffer,s*sizeof(short));
			break;
			
		      case 2:
			for(int k=0;k<s;k++) {
			  ((short *)alsa_buffer)[2*k]=
			    (short)(alsa_input_volume[alsa_format->card][i]*
				   (double)(((short *)alsa_format->card_buffer)
					     [modulo*k+4*i+1]));
			  ((short *)alsa_buffer)[2*k+1]=
			    (short)(alsa_input_volume[alsa_format->card][i]*
				   (double)(((short *)alsa_format->card_buffer)
					     [modulo*k+4*i+3]));
			}
			alsa_record_ring[alsa_format->card][i]->
			  write(alsa_buffer,s*2*sizeof(short));
			break;
		  }
		}
	      }
	    }
	    
	    //
	    // Process Passthroughs
	    //
	    for(unsigned i=0;i<alsa_format->channels;i+=2) {
	      for(unsigned j=0;j<2;j++) {
		for(int k=0;k<s;k++) {
		  ((short *)alsa_format->passthrough_buffer)[2*k+j]=
		    ((short *)alsa_format->
		     card_buffer)[alsa_format->channels*2*k+2*i+1+2*j];
		}
	      }
	      alsa_passthrough_ring[alsa_format->card][i/2]->
		write(alsa_format->passthrough_buffer,4*s);
	    }
	    
	    //
	    // Process Input Meters
	    //
	    for(unsigned i=0;i<alsa_format->channels;i+=2) {
	      for(unsigned j=0;j<2;j++) {
		in_meter[i/2][j]=0;
		for(int k=0;k<s;k++) {
		  if(((short *)alsa_format->
		      card_buffer)[alsa_format->channels*2*k+2*i+1+2*j]>
		     in_meter[i/2][j]) {
		    in_meter[i/2][j]=
		      ((short *)alsa_format->
		       card_buffer)[alsa_format->channels*2*k+2*i+1+2*j];
		  }
		}
		alsa_input_meter[alsa_format->card][i/2][j]->
		  addValue(((double)in_meter[i/2][j])/32768.0);
	      }
	    }
	    break;

	  default:
	    break;
      }
    }
  }
}


void *AlsaCaptureCallback(void *ptr)
{
  struct alsa_format *alsa_format=(struct alsa_format *)ptr;

  signal(SIGTERM,SigHandler);
  signal(SIGINT,SigHandler);

  switch(alsa_channels) {
      case 1:
	AlsaCapture1Callback(alsa_format);
	break;

      case 2:
        AlsaCapture2Callback(alsa_format);
	break;
  }
  return 0;
}


void AlsaPlay1Callback(struct alsa_format *alsa_format)
{
}


void AlsaPlay2Callback(struct alsa_format *alsa_format)
{
  int n=0;
  int p;
  char alsa_buffer[RINGBUFFER_SIZE];
  int modulo;
  short out_meter[RD_MAX_PORTS][2];
  short stream_out_meter=0;

  while(!alsa_format->exiting) {
    memset(alsa_format->card_buffer,0,alsa_format->card_buffer_size);

    switch(alsa_format->format) {
	case SND_PCM_FORMAT_S16_LE:
	  for(unsigned j=0;j<RD_MAX_STREAMS;j++) {
	    if(alsa_playing[alsa_format->card][j]) {
	      switch(alsa_output_channels[alsa_format->card][j]) {
		  case 1:
		    n=alsa_play_ring[alsa_format->card][j]->
		      read(alsa_buffer,alsa_format->
			   buffer_size/alsa_format->periods)/
		      (2*sizeof(short));
		    stream_out_meter=0;  // Stream Output Meters
		    for(int k=0;k<n;k++) {
		      if(abs(((short *)alsa_buffer)[k])>stream_out_meter) {
			stream_out_meter=abs(((short *)alsa_buffer)[k]);
		      }
		    }
		    alsa_stream_output_meter[alsa_format->card][j][0]->
		      addValue(((double)stream_out_meter)/32768.0);
		    alsa_stream_output_meter[alsa_format->card][j][1]->
		      addValue(((double)stream_out_meter)/32768.0);
		    modulo=alsa_format->channels;
		    for(unsigned i=0;i<(alsa_format->channels/2);i++) {
		      if(alsa_output_volume[alsa_format->card][i][j]!=0.0) {
			for(int k=0;k<(2*n);k++) {
			  ((short *)alsa_format->card_buffer)[modulo*k+2*i]+=
			   (short)(alsa_output_volume[alsa_format->card][i][j]*
				   (double)(((short *)alsa_buffer)[k]));
			  ((short *)alsa_format->card_buffer)[modulo*k+2*i+1]+=
			   (short)(alsa_output_volume[alsa_format->card][i][j]*
				   (double)(((short *)alsa_buffer)[k]));
			}
		      }
		    }
		    n*=2;
		    break;

		  case 2:
		    n=alsa_play_ring[alsa_format->card][j]->
		      read(alsa_buffer,alsa_format->buffer_size*2/
			   alsa_format->periods)/(2*sizeof(short));
		    for(unsigned k=0;k<2;k++) {  // Stream Output Meters
		      stream_out_meter=0;
		      for(int l=0;l<n;l+=2) {
			if(abs(((short *)alsa_buffer)[l+k])>stream_out_meter) {
			  stream_out_meter=abs(((short *)alsa_buffer)[l+k]);
			}
		      }
		      alsa_stream_output_meter[alsa_format->card][j][k]->
			addValue(((double)stream_out_meter)/32768.0);
		    }
		    modulo=alsa_format->channels;
		    for(unsigned i=0;i<(alsa_format->channels/2);i++) {
		      if(alsa_output_volume[alsa_format->card][i][j]!=0.0) {
			for(int k=0;k<n;k++) {
			  ((short *)alsa_format->card_buffer)[modulo*k+2*i]+=
			   (short)(alsa_output_volume[alsa_format->card][i][j]*
				   (double)(((short *)alsa_buffer)[2*k]));
			  ((short *)alsa_format->card_buffer)[modulo*k+2*i+1]+=
			   (short)(alsa_output_volume[alsa_format->card][i][j]*
				   (double)(((short *)alsa_buffer)[2*k+1]));
			}
		      }
		    }
		    break;
	      }
	      alsa_output_pos[alsa_format->card][j]+=n;
	      if((n==0)&&alsa_eof[alsa_format->card][j]) {
		alsa_stopping[alsa_format->card][j]=true;
	      }
	    }
	  }
	  n=alsa_format->buffer_size/(2*alsa_format->periods);

	  //
	  // Process Passthroughs
	  //
	  for(unsigned i=0;i<alsa_format->capture_channels;i+=2) {
	    p=alsa_passthrough_ring[alsa_format->card][i/2]->
	      read(alsa_format->passthrough_buffer,4*n)/4;
	    for(unsigned j=0;j<alsa_format->channels;j+=2) {
	      if(alsa_passthrough_volume[alsa_format->card][i/2][j/2]>0.0) {
		for(unsigned k=0;k<2;k++) {
		  for(int l=0;l<p;l++) {
		    ((short *)alsa_format->
		     card_buffer)[alsa_format->channels*l+4*j+1+k]+=(short)
		      ((double)((short *)alsa_format->
				passthrough_buffer)[2*l+1]*
		       alsa_passthrough_volume[alsa_format->card][i/2][j/2]);
		  }
		}
	      }
	    }
	  }

	  //
	  // Process Output Meters
	  //
	  for(unsigned i=0;i<alsa_format->channels;i+=2) {
	    unsigned port=i/2;
	    for(unsigned j=0;j<2;j++) {
	      out_meter[port][j]=0;
	      for(unsigned k=0;k<alsa_format->buffer_size;k++) {
		if(((short *)alsa_format->
		    card_buffer)[alsa_format->channels*k+2*i+j]>
		   out_meter[i][j]) {
		  out_meter[i][j]=
		    ((short *)alsa_format->
		     card_buffer)[alsa_format->channels*k+2*i+j];
		}
	      }
	      alsa_output_meter[alsa_format->card][i][j]->
		addValue(((double)out_meter[i][j])/32768.0);
	    }
	  }
	  break;

	case SND_PCM_FORMAT_S32_LE:
	  for(unsigned j=0;j<RD_MAX_STREAMS;j++) {
	    if(alsa_playing[alsa_format->card][j]) {
	      switch(alsa_output_channels[alsa_format->card][j]) {
		  case 1:
		    n=alsa_play_ring[alsa_format->card][j]->
		      read(alsa_buffer,alsa_format->buffer_size/
			   alsa_format->periods)/(2*sizeof(short));
		    stream_out_meter=0;
		    for(int k=0;k<n;k++) {  // Stream Output Meters
		      if(abs(((short *)alsa_buffer)[k])>stream_out_meter) {
			stream_out_meter=abs(((short *)alsa_buffer)[k]);
		      }
		    }
		    alsa_stream_output_meter[alsa_format->card][j][0]->
		      addValue(((double)stream_out_meter)/32768.0);
		    alsa_stream_output_meter[alsa_format->card][j][1]->
		      addValue(((double)stream_out_meter)/32768.0);
		    modulo=alsa_format->channels*2;
		    for(unsigned i=0;i<(alsa_format->channels/2);i++) {
		      if(alsa_output_volume[alsa_format->card][i][j]!=0.0) {
			for(int k=0;k<(2*n);k++) {
			  ((short *)alsa_format->card_buffer)[modulo*k+4*i+1]+=
			   (short)(alsa_output_volume[alsa_format->card][i][j]*
				   (double)(((short *)alsa_buffer)[k]));
			  ((short *)alsa_format->card_buffer)[modulo*k+4*i+3]+=
			   (short)(alsa_output_volume[alsa_format->card][i][j]*
				   (double)(((short *)alsa_buffer)[k]));
			}
		      }
		    }
		    n*=2;
		    break;

		  case 2:
		    n=alsa_play_ring[alsa_format->card][j]->
		      read(alsa_buffer,alsa_format->buffer_size*2/
			   alsa_format->periods)/(2*sizeof(short));
		    for(unsigned k=0;k<2;k++) {  // Stream Output Meters
		      stream_out_meter=0;
		      for(int l=0;l<n;l+=2) {
			if(abs(((short *)alsa_buffer)[l+k])>stream_out_meter) {
			  stream_out_meter=abs(((short *)alsa_buffer)[l+k]);
			}
		      }
		      alsa_stream_output_meter[alsa_format->card][j][k]->
			addValue(((double)stream_out_meter)/32768.0);
		    }
		    modulo=alsa_format->channels*2;
		    for(unsigned i=0;i<(alsa_format->channels/2);i++) {
		      if(alsa_output_volume[alsa_format->card][i][j]!=0.0) {
			for(int k=0;k<n;k++) {
			  ((short *)alsa_format->card_buffer)[modulo*k+4*i+1]+=
			   (short)(alsa_output_volume[alsa_format->card][i][j]*
				   (double)(((short *)alsa_buffer)[2*k]));
			  ((short *)alsa_format->card_buffer)[modulo*k+4*i+3]+=
			   (short)(alsa_output_volume[alsa_format->card][i][j]*
				   (double)(((short *)alsa_buffer)[2*k+1]));
			}
		      }
		    }
		    break;
	      }
	      alsa_output_pos[alsa_format->card][j]+=n;
	      if((n==0)&&alsa_eof[alsa_format->card][j]) {
		alsa_stopping[alsa_format->card][j]=true;
		// Empty the ring buffer
		while(alsa_play_ring[alsa_format->card][j]->
		      read(alsa_buffer,alsa_format->buffer_size*2/
			   alsa_format->periods)/(2*sizeof(short))>0);	       
	      }
	    }
	  }
	  n=alsa_format->buffer_size/(2*alsa_format->periods);

	  //
	  // Process Passthroughs
	  //
	  for(unsigned i=0;i<alsa_format->capture_channels;i+=2) {
	    p=alsa_passthrough_ring[alsa_format->card][i/2]->
	      read(alsa_format->passthrough_buffer,4*n)/4;
	    for(unsigned j=0;j<alsa_format->channels;j+=2) {
	      if(alsa_passthrough_volume[alsa_format->card][i/2][j/2]>0.0) {
		for(unsigned k=0;k<2;k++) {
		  for(int l=0;l<p;l++) {
		    ((short *)alsa_format->
		     card_buffer)[alsa_format->channels*2*l+4*j+1+2*k]+=
		      (short)((double)
			 ((short *)alsa_format->passthrough_buffer)[2*l]*
			 alsa_passthrough_volume[alsa_format->card][i/2][j/2]);
		    ((short *)alsa_format->
		     card_buffer)[alsa_format->channels*2*l+4*j+3+2*k]+=
		      ((short *)alsa_format->passthrough_buffer)[2*l+1];
		  }
		}
	      }
	    }
	  }

	  //
	  // Process Output Meters
	  //
	  for(unsigned i=0;i<alsa_format->channels;i+=2) {
	    unsigned port=i/2;
	    for(unsigned j=0;j<2;j++) {
	      out_meter[port][j]=0;
	      for(unsigned k=0;
		  k<(alsa_format->buffer_size*2/alsa_format->periods);
		  k++) {
		if(((short *)alsa_format->
		    card_buffer)[alsa_format->channels*2*k+2*i+1+2*j]>
		   out_meter[port][j]) {
		  out_meter[port][j]=
		    ((short *)alsa_format->
		     card_buffer)[alsa_format->channels*2*k+2*i+1+2*j];
		}
	      }
	      alsa_output_meter[alsa_format->card][port][j]->
		addValue(((double)out_meter[port][j])/32768.0);
	    }
	  }
	  break;

	default:
	  break;
    }
    int s=snd_pcm_writei(alsa_format->pcm,alsa_format->card_buffer,n);
    if(s!=n) {
      if(s<0) {
	LogLine(RDConfig::LogNotice,
		QString().sprintf("*** alsa error %d: %s",-s,snd_strerror(s)));
      }
      else {
	LogLine(RDConfig::LogNotice,
		QString().sprintf("period size mismatch - wrote %d\n",s));
      }
    }
    if((snd_pcm_state(alsa_format->pcm)!=SND_PCM_STATE_RUNNING)&&
       (!alsa_format->exiting)) {
      snd_pcm_drop (alsa_format->pcm);
      snd_pcm_prepare(alsa_format->pcm);
      LogLine(RDConfig::LogNotice,QString().
	      sprintf("****** ALSA Playout Xrun - Card: %d ******",
		      alsa_format->card));
    }
  }
}


void *AlsaPlayCallback(void *ptr)
{
  struct alsa_format *alsa_format=(struct alsa_format *)ptr;

  signal(SIGTERM,SigHandler);
  signal(SIGINT,SigHandler);

  switch(alsa_channels) {
      case 1:
	AlsaPlay1Callback(alsa_format);
	break;

      case 2:
	AlsaPlay2Callback(alsa_format);
	break;
  }
  return 0;
}


void MainObject::AlsaInitCallback()
{
  int avg_periods=
    (330*system_sample_rate)/(1000*rd_config->alsaPeriodSize());
  for(int i=0;i<RD_MAX_CARDS;i++) {
    for(int j=0;j<RD_MAX_PORTS;j++) {
      alsa_recording[i][j]=false;
      alsa_ready[i][j]=false;
      alsa_input_volume[i][j]=1.0;
      alsa_input_vox[i][j]=0.0;
      for(int k=0;k<2;k++) {
	alsa_input_meter[i][j][k]=new RDMeterAverage(avg_periods);
	alsa_output_meter[i][j][k]=new RDMeterAverage(avg_periods);
      }
      for(int k=0;k<RD_MAX_STREAMS;k++) {
	alsa_output_volume[i][j][k]=1.0;
      }
      alsa_passthrough_ring[i][j]=new RDRingBuffer(RINGBUFFER_SIZE);
      alsa_passthrough_ring[i][j]->reset();
      alsa_record_ring[i][j]=NULL;
      for(int k=0;k<RD_MAX_PORTS;k++) {
	alsa_passthrough_volume[i][j][k]=0.0;
      }
    }
    for(int j=0;j<RD_MAX_STREAMS;j++) {
      alsa_play_ring[i][j]=NULL;
      alsa_playing[i][j]=false;
      for(int k=0;k<2;k++) {
	alsa_stream_output_meter[i][j][k]=new RDMeterAverage(avg_periods);
      }
    }
  }
}
#endif  // ALSA


void MainObject::alsaStopTimerData(int cardstream)
{
  LogLine(RDConfig::LogErr,QString().sprintf("alsaStopTimerData(%d)",cardstream));
#ifdef ALSA
  int card=cardstream/RD_MAX_STREAMS;
  int stream=cardstream-card*RD_MAX_STREAMS;

  alsaStopPlayback(card,stream);
  statePlayUpdate(card,stream,2);
#endif  // ALSA
}


void MainObject::alsaFadeTimerData(int cardstream)
{
#ifdef ALSA
  int card=cardstream/RD_MAX_STREAMS;
  int stream=cardstream-card*RD_MAX_STREAMS;
  short level;
  if(alsa_fade_up[card][stream]) {
    level=alsa_output_volume_db[card][alsa_fade_port[card][stream]][stream]+
      alsa_fade_increment[card][stream];
    if(level>=alsa_fade_volume_db[card][stream]) {
      level=alsa_fade_volume_db[card][stream];
      alsa_fade_timer[card][stream]->stop();
    }
  }
  else {
    level=alsa_output_volume_db[card][alsa_fade_port[card][stream]][stream]-
      alsa_fade_increment[card][stream];
    if(level<=alsa_fade_volume_db[card][stream]) {
      level=alsa_fade_volume_db[card][stream];
      alsa_fade_timer[card][stream]->stop();
    }
  }
  LogLine(RDConfig::LogDebug,QString().sprintf("FadeLevel: %d",level));
  alsaSetOutputVolume(card,stream,alsa_fade_port[card][stream],level);
#endif  // ALSA
}


void MainObject::alsaRecordTimerData(int cardport)
{
#ifdef ALSA
  int card=cardport/RD_MAX_PORTS;
  int stream=cardport-card*RD_MAX_PORTS;

  alsaStopRecord(card,stream);
  stateRecordUpdate(card,stream,2);
#endif  // ALSA
}


void MainObject::alsaInit(RDStation *station)
{
#ifdef ALSA
  QString dev;
  int card=0;
  snd_pcm_t *pcm_play_handle;
  snd_pcm_t *pcm_capture_handle;
  snd_ctl_t *snd_ctl;
  snd_ctl_card_info_t *card_info=NULL;
  bool pcm_opened;

  //
  // Initialize Data Structures
  //
  for(int i=0;i<RD_MAX_CARDS;i++) {
    for(int j=0;j<RD_MAX_STREAMS;j++) {
      alsa_input_volume_db[i][j]=0;
      alsa_samples_recorded[i][j]=0;
#ifdef HAVE_MAD
      mad_mpeg[i][j]=new unsigned char[16384];
#endif  // HAVE_MAD
      for(int k=0;k<RD_MAX_PORTS;k++) {
	alsa_output_volume_db[i][k][j]=0;
      }
    }
    for(int j=0;j<RD_MAX_PORTS;j++) {
      for(int k=0;k<RD_MAX_PORTS;k++) {
	alsa_passthrough_volume_db[i][j][k]=RD_MUTE_DEPTH;
      }
    }
  }
  alsa_channels=rd_config->channels();

  //
  // Stop & Fade Timers
  //
  QSignalMapper *stop_mapper=new QSignalMapper(this,"stop_mapper");
  connect(stop_mapper,SIGNAL(mapped(int)),this,SLOT(alsaStopTimerData(int)));
  QSignalMapper *fade_mapper=new QSignalMapper(this,"fade_mapper");
  connect(fade_mapper,SIGNAL(mapped(int)),this,SLOT(alsaFadeTimerData(int)));
  QSignalMapper *record_mapper=new QSignalMapper(this,"record_mapper");
  connect(record_mapper,SIGNAL(mapped(int)),
	  this,SLOT(alsaRecordTimerData(int)));
  for(int i=0;i<RD_MAX_CARDS;i++) {
    for(int j=0;j<RD_MAX_STREAMS;j++) {
      alsa_stop_timer[i][j]=new QTimer(this);
      stop_mapper->setMapping(alsa_stop_timer[i][j],i*RD_MAX_STREAMS+j);
      connect(alsa_stop_timer[i][j],SIGNAL(timeout()),stop_mapper,SLOT(map()));
      alsa_fade_timer[i][j]=new QTimer(this);
      fade_mapper->setMapping(alsa_fade_timer[i][j],i*RD_MAX_STREAMS+j);
      connect(alsa_fade_timer[i][j],SIGNAL(timeout()),fade_mapper,SLOT(map()));
    }
    for(int j=0;j<RD_MAX_PORTS;j++) {
      alsa_record_timer[i][j]=new QTimer(this);
      record_mapper->setMapping(alsa_record_timer[i][j],i*RD_MAX_PORTS+j);
      connect(alsa_record_timer[i][j],SIGNAL(timeout()),
	      record_mapper,SLOT(map()));
    }
  }

  //
  // Allocate Temporary Buffers
  //
  AlsaInitCallback();
  alsa_wave_buffer=new short[RINGBUFFER_SIZE];
  //alsa_resample_buffer=new short[2*RINGBUFFER_SIZE];

  //
  // Start Up Interfaces
  //
  for(int i=0;i<RD_MAX_CARDS;i++) {
    if(cae_driver[i]==RDStation::None) {
      pcm_opened=false;
      // These are used to flag bits of card that are not setup
      // They are cleared just before the pthreads are created.
      alsa_play_format[i].exiting = true;
      alsa_capture_format[i].exiting = true;
      dev=QString().sprintf("rd%d",card);
      if(snd_pcm_open(&pcm_play_handle,(const char *)dev,
		      SND_PCM_STREAM_PLAYBACK,0)==0){
	pcm_opened=true;
	if(AlsaStartPlayDevice(dev,i,pcm_play_handle)) {
	  cae_driver[i]=RDStation::Alsa;
	}
	else {
	  snd_pcm_close(pcm_play_handle);
	}
      }
      if(snd_pcm_open(&pcm_capture_handle,(const char *)dev,
		      SND_PCM_STREAM_CAPTURE,0)==0) {
	pcm_opened=true;
	if(AlsaStartCaptureDevice(dev,i,pcm_capture_handle)) {
	  cae_driver[i]=RDStation::Alsa;
	}
	else {
	  snd_pcm_close(pcm_capture_handle);
	}
      }
      if(cae_driver[i]==RDStation::Alsa) {
	station->setCardDriver(i,RDStation::Alsa);
	if(snd_ctl_open(&snd_ctl,dev,0)<0) {
	  LogLine(RDConfig::LogNotice,QString().
		  sprintf("no control device found for %s",
			  (const char *)dev));
	  station->
	    setCardName(i,
			QString().sprintf("ALSA Device %s",(const char *)dev));
	}
	else {
	  snd_ctl_card_info_malloc(&card_info);
	  snd_ctl_card_info(snd_ctl,card_info);
	  station->
	    setCardName(i,snd_ctl_card_info_get_longname(card_info));
	  snd_ctl_close(snd_ctl);
	}
	station->
	  setCardInputs(i,
			alsa_capture_format[i].channels/rd_config->channels());
	station->
	  setCardOutputs(i,alsa_play_format[i].channels/rd_config->channels());
      }
      else {
	i--;
      }
      card++;
      if(!pcm_opened) {
	return;
      }
    }
  }
#endif  // ALSA
}


void MainObject::alsaFree()
{
#ifdef ALSA
  for(int i=0;i<RD_MAX_CARDS;i++) {
    if(cae_driver[i]==RDStation::Alsa) {
      alsa_play_format[i].exiting=true;
      pthread_join(alsa_play_format[i].thread,NULL);
      snd_pcm_close(alsa_play_format[i].pcm);
      if(alsa_capture_format[i].pcm!=NULL) {
	printf("SHUTDOWN 1\n");
	alsa_capture_format[i].exiting=true;
	printf("SHUTDOWN 2\n");
	pthread_join(alsa_capture_format[i].thread,NULL);
	printf("SHUTDOWN 3\n");
	snd_pcm_close(alsa_capture_format[i].pcm);
	printf("SHUTDOWN 4\n");
      }
    }
  }
#endif  // ALSA
}


bool MainObject::alsaLoadPlayback(int card,QString wavename,int *stream)
{
#ifdef ALSA
  if(alsa_play_format[card].exiting||((*stream=GetAlsaOutputStream(card))<0)) {
    LogLine(RDConfig::LogErr,QString().sprintf(
            "Error: alsaLoadPlayback(%s) GetAlsaOutputStream():%d < 0",
            (const char *) wavename,
            *stream) );
    return false;
  }
  alsa_play_wave[card][*stream]=new RDWaveFile(wavename);
  if(!alsa_play_wave[card][*stream]->openWave()) {
    LogLine(RDConfig::LogErr,QString().sprintf(
            "Error: alsaLoadPlayback(%s)   openWave() failed to open file",
            (const char *) wavename) );
    delete alsa_play_wave[card][*stream];
    alsa_play_wave[card][*stream]=NULL;
    FreeAlsaOutputStream(card,*stream);
    *stream=-1;
    return false;
  }
  switch(alsa_play_wave[card][*stream]->getFormatTag()) {
  case WAVE_FORMAT_PCM:
  case WAVE_FORMAT_VORBIS:
    break;
    
  case WAVE_FORMAT_MPEG:
    InitMadDecoder(card,*stream,alsa_play_wave[card][*stream]);
    break;
    
  default:
    LogLine(RDConfig::LogErr,QString().sprintf(
	   "Error: alsaLoadPlayback(%s)   getFormatTag()%d || getBistsPerSample()%d failed",
	   (const char *) wavename,
	   alsa_play_wave[card][*stream]->getFormatTag(),
	   alsa_play_wave[card][*stream]->getBitsPerSample() ));
    delete alsa_play_wave[card][*stream];
    alsa_play_wave[card][*stream]=NULL;
    FreeAlsaOutputStream(card,*stream);
    *stream=-1;
    return false;
  }
  alsa_output_channels[card][*stream]=
    alsa_play_wave[card][*stream]->getChannels();
  alsa_stopping[card][*stream]=false;
  alsa_offset[card][*stream]=0;
  alsa_output_pos[card][*stream]=0;
  alsa_eof[card][*stream]=false;
  alsa_play_ring[card][*stream]->reset();
  FillAlsaOutputStream(card,*stream);
  return true;
#else
  return false;
#endif  // ALSA
}


bool MainObject::alsaUnloadPlayback(int card,int stream)
{
#ifdef ALSA
  if(alsa_play_ring[card][stream]==NULL) {
    return false;
  }
  alsa_playing[card][stream]=false;
  switch(alsa_play_wave[card][stream]->getFormatTag()) {
  case WAVE_FORMAT_MPEG:
    FreeMadDecoder(card,stream);
    break;
  }
  alsa_play_wave[card][stream]->closeWave();
  delete alsa_play_wave[card][stream];
  alsa_play_wave[card][stream]=NULL;
  FreeAlsaOutputStream(card,stream);
  return true;
#else
  return false;
#endif  // ALSA
}


bool MainObject::alsaPlaybackPosition(int card,int stream,unsigned pos)
{
#ifdef ALSA
  unsigned offset=0;

  if(alsa_play_format[card].exiting){
    return false;
  }
  switch(alsa_play_wave[card][stream]->getFormatTag()) {
  case WAVE_FORMAT_PCM:
    offset=(unsigned)((double)alsa_play_wave[card][stream]->getSamplesPerSec()*
		      (double)alsa_play_wave[card][stream]->getBlockAlign()*
		      (double)pos/1000);
    alsa_offset[card][stream]=
      offset/alsa_play_wave[card][stream]->getBlockAlign();
    offset=
      alsa_offset[card][stream]*alsa_play_wave[card][stream]->getBlockAlign();
    break;

  case WAVE_FORMAT_MPEG:
    offset=(unsigned)((double)alsa_play_wave[card][stream]->getSamplesPerSec()*
		      (double)pos/1000);
    alsa_offset[card][stream]=offset/1152*1152;
    offset=alsa_offset[card][stream]/1152*
      alsa_play_wave[card][stream]->getBlockAlign();
    FreeMadDecoder(card,stream);
    InitMadDecoder(card,stream,alsa_play_wave[card][stream]);
    break;
  }
  if(alsa_offset[card][stream]>
     (int)alsa_play_wave[card][stream]->getSampleLength()) {
    return false;
  }
  alsa_output_pos[card][stream]=0;
  alsa_play_wave[card][stream]->seekWave(offset,SEEK_SET);
  alsa_eof[card][stream]=false;
  alsa_play_ring[card][stream]->reset();
  FillAlsaOutputStream(card,stream);

  if(alsa_playing[card][stream]) {
    alsa_stop_timer[card][stream]->stop();
    alsa_stop_timer[card][stream]->
      start(alsa_play_wave[card][stream]->getExtTimeLength()-pos,true);
  }
  return true;
#else
  return false;
#endif  // ALSA
}


bool MainObject::alsaPlay(int card,int stream,int length,int speed,bool pitch,
			 bool rates)
{
#ifdef ALSA
  if((alsa_play_ring[card][stream]==NULL)||
     alsa_playing[card][stream]||(speed!=RD_TIMESCALE_DIVISOR)) {
    return false;
  }
  alsa_playing[card][stream]=true;
  if(length>0) {
    alsa_stop_timer[card][stream]->start(length,true);
  }
  statePlayUpdate(card,stream,1);
  return true;
#else
  return false;
#endif  // ALSA
}


bool MainObject::alsaTimescaleSupported(int card)
{
#ifdef ALSA
  return false;
#else
  return false;
#endif  // ALSA
}


bool MainObject::alsaStopPlayback(int card,int stream)
{
#ifdef ALSA
  if((alsa_play_ring[card][stream]==NULL)||(!alsa_playing[card][stream])) {
    return false;
  }
  alsa_playing[card][stream]=false;
  alsa_play_ring[card][stream]->reset();
  alsa_stop_timer[card][stream]->stop();
  statePlayUpdate(card,stream,2);
  return true;
#else
  return false;
#endif  // ALSA
}


bool MainObject::alsaLoadRecord(int card,int stream,int coding,int chans,
			       int samprate,int bitrate,QString wavename)
{
#ifdef ALSA
  alsa_record_wave[card][stream]=new RDWaveFile(wavename);
  switch(coding) {
  case 0:  // PCM16
    alsa_record_wave[card][stream]->setFormatTag(WAVE_FORMAT_PCM);
    alsa_record_wave[card][stream]->setChannels(chans);
    alsa_record_wave[card][stream]->setSamplesPerSec(samprate);
    alsa_record_wave[card][stream]->setBitsPerSample(16);
    break;

  case 2:  // MPEG Layer 2
    if(!InitTwoLameEncoder(card,stream,chans,samprate,bitrate)) {
      delete alsa_record_wave[card][stream];
      alsa_record_wave[card][stream]=NULL;
      return false;
    }
    alsa_record_wave[card][stream]->setFormatTag(WAVE_FORMAT_MPEG);
    alsa_record_wave[card][stream]->setChannels(chans);
    alsa_record_wave[card][stream]->setSamplesPerSec(samprate);
    alsa_record_wave[card][stream]->setBitsPerSample(16);
    alsa_record_wave[card][stream]->setHeadLayer(ACM_MPEG_LAYER2);
    switch(chans) {
    case 1:
      alsa_record_wave[card][stream]->setHeadMode(ACM_MPEG_SINGLECHANNEL);
      break;

    case 2:
      alsa_record_wave[card][stream]->setHeadMode(ACM_MPEG_STEREO);
      break;

    default:
      LogLine(RDConfig::LogErr,QString().
	sprintf("requested unsupported channel count %d, card: %d, stream: %d",
		chans,card,stream));
      delete alsa_record_wave[card][stream];
      alsa_record_wave[card][stream]=NULL;
      return false;
    }
    alsa_record_wave[card][stream]->setHeadBitRate(bitrate);
    alsa_record_wave[card][stream]->setMextChunk(true);
    alsa_record_wave[card][stream]->setMextHomogenous(true);
    alsa_record_wave[card][stream]->setMextPaddingUsed(false);
    alsa_record_wave[card][stream]->setMextHackedBitRate(true);
    alsa_record_wave[card][stream]->setMextFreeFormat(false);
    alsa_record_wave[card][stream]->
      setMextFrameSize(144*alsa_record_wave[card][stream]->getHeadBitRate()/
		       alsa_record_wave[card][stream]->getSamplesPerSec());
    alsa_record_wave[card][stream]->setMextAncillaryLength(5);
    alsa_record_wave[card][stream]->setMextLeftEnergyPresent(true);
    if(chans>1) {
      alsa_record_wave[card][stream]->setMextRightEnergyPresent(true);
    }
    else {
      alsa_record_wave[card][stream]->setMextRightEnergyPresent(false);
    }
    alsa_record_wave[card][stream]->setMextPrivateDataPresent(false);
    break;

  default:
    LogLine(RDConfig::LogErr,QString().
	    sprintf("requested invalid audio encoding %d, card: %d, stream: %d",
		    coding,card,stream));
    delete alsa_record_wave[card][stream];
    alsa_record_wave[card][stream]=NULL;
    return false;
  }
  alsa_record_wave[card][stream]->setBextChunk(true);
  alsa_record_wave[card][stream]->setLevlChunk(true);
  if(!alsa_record_wave[card][stream]->createWave()) {
    delete alsa_record_wave[card][stream];
    alsa_record_wave[card][stream]=NULL;
    return false;
  }
  chown((const char *)wavename,rd_config->uid(),rd_config->gid());
  alsa_input_channels[card][stream]=chans;
  alsa_record_ring[card][stream]=new RDRingBuffer(RINGBUFFER_SIZE);
  alsa_record_ring[card][stream]->reset();
  alsa_ready[card][stream]=true;
  return true;
#else
  return false;
#endif  // ALSA
}


bool MainObject::alsaUnloadRecord(int card,int stream,unsigned *len)
{
#ifdef ALSA
  alsa_recording[card][stream]=false;
  alsa_ready[card][stream]=false;
  EmptyAlsaInputStream(card,stream);
  *len=alsa_samples_recorded[card][stream];
  alsa_samples_recorded[card][stream]=0;
  alsa_record_wave[card][stream]->closeWave(*len);
  delete alsa_record_wave[card][stream];
  alsa_record_wave[card][stream]=NULL;
  delete alsa_record_ring[card][stream];
  alsa_record_ring[card][stream]=NULL;
  FreeTwoLameEncoder(card,stream);
  return true;
#else
  return false;
#endif  // ALSA
}


bool MainObject::alsaRecord(int card,int stream,int length,int thres)
{
#ifdef ALSA
  if(!alsa_ready[card][stream]) {
    return false;
  }
  alsa_recording[card][stream]=true;
  if(alsa_input_vox[card][stream]==0.0) {
    if(length>0) {
      alsa_record_timer[card][stream]->start(length,true);
    }
    stateRecordUpdate(card,stream,4);
  }
  return true;
#else
  return false;
#endif  // ALSA
}


bool MainObject::alsaStopRecord(int card,int stream)
{
#ifdef ALSA
  if(!alsa_recording[card][stream]) {
    return false;
  }
  alsa_recording[card][stream]=false;
  return true;
#else
  return false;
#endif  // ALSA
}


bool MainObject::alsaSetInputVolume(int card,int stream,int level)
{
#ifdef ALSA
  if(level>-10000) {
    alsa_input_volume[card][stream]=pow10((double)level/2000.0);
    alsa_input_volume_db[card][stream]=level;
  }
  else {
    alsa_input_volume[card][stream]=0.0;
    alsa_input_volume_db[card][stream]=-10000;
  }
  return true;
#else
  return false;
#endif  // ALSA
}


bool MainObject::alsaSetOutputVolume(int card,int stream,int port,int level)
{
#ifdef ALSA
  if(level>-10000) {
    alsa_output_volume[card][port][stream]=pow10((double)level/2000.0);
    alsa_output_volume_db[card][port][stream]=level;
  }
  else {
    alsa_output_volume[card][port][stream]=0.0;
    alsa_output_volume_db[card][port][stream]=-10000;
  }
  return true;
#else
  return false;
#endif  // ALSA
}


bool MainObject::alsaFadeOutputVolume(int card,int stream,int port,int level,
				     int length)
{
#ifdef ALSA
  int diff;

  if(alsa_fade_timer[card][stream]->isActive()) {
    alsa_fade_timer[card][stream]->stop();
  }
  if(level>alsa_output_volume_db[card][port][stream]) {
    alsa_fade_up[card][stream]=true;
    diff=level-alsa_output_volume_db[card][port][stream];
  }
  else {
    alsa_fade_up[card][stream]=false;
    diff=alsa_output_volume_db[card][port][stream]-level;
  }
  alsa_fade_volume_db[card][stream]=level;
  alsa_fade_port[card][stream]=port;
  alsa_fade_increment[card][stream]=diff*RD_ALSA_FADE_INTERVAL/length;
  alsa_fade_timer[card][stream]->start(RD_ALSA_FADE_INTERVAL);
  return true;
#else
  return false;
#endif  // ALSA
}


bool MainObject::alsaSetInputLevel(int card,int port,int level)
{
#ifdef ALSA
  return true;
#else
  return false;
#endif  // ALSA
}


bool MainObject::alsaSetOutputLevel(int card,int port,int level)
{
#ifdef ALSA
  return true;
#else
  return false;
#endif  // ALSA
}


bool MainObject::alsaSetInputMode(int card,int stream,int mode)
{
#ifdef ALSA
  return true;
#else
  return false;
#endif  // ALSA
}


bool MainObject::alsaSetOutputMode(int card,int stream,int mode)
{
#ifdef ALSA
  return true;
#else
  return false;
#endif  // ALSA
}


bool MainObject::alsaSetInputVoxLevel(int card,int stream,int level)
{
#ifdef ALSA
  return true;
#else
  return false;
#endif  // ALSA
}


bool MainObject::alsaSetInputType(int card,int port,int type)
{
#ifdef ALSA
  return true;
#else
  return false;
#endif  // ALSA
}


bool MainObject::alsaGetInputStatus(int card,int port)
{
#ifdef ALSA
  return true;
#else
  return false;
#endif  // ALSA
}


bool MainObject::alsaGetInputMeters(int card,int port,short levels[2])
{
#ifdef ALSA
  double meter;

  for(int i=0;i<2;i++) {

    meter=alsa_input_meter[card][port][i]->average();
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
#endif  // ALSA
}


bool MainObject::alsaGetOutputMeters(int card,int port,short levels[2])
{
#ifdef ALSA
  double meter;

  for(int i=0;i<2;i++) {
    meter=alsa_output_meter[card][port][i]->average();
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
#endif  // ALSA
}


bool MainObject::alsaGetStreamOutputMeters(int card,int stream,short levels[2])
{
#ifdef ALSA
  double meter;

  for(int i=0;i<2;i++) {
    meter=alsa_stream_output_meter[card][stream][i]->average();
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
#endif  // ALSA
}


void MainObject::alsaGetOutputPosition(int card,unsigned *pos)
{// pos is in miliseconds
#ifdef ALSA
  for(int i=0;i<RD_MAX_STREAMS;i++) {
    if((!alsa_play_format[card].exiting)&&(alsa_play_wave[card][i]!=NULL)) {
      pos[i]=1000*(unsigned long long)(alsa_offset[card][i]+
				       alsa_output_pos[card][i])/
	alsa_play_wave[card][i]->getSamplesPerSec();
    } 
    else {
      pos[i]=0;
    }
  }
#endif  // ALSA
}


bool MainObject::alsaSetPassthroughLevel(int card,int in_port,int out_port,
					 int level)
{
#ifdef ALSA
  if(level>-10000) {
    alsa_passthrough_volume[card][in_port][out_port]=
      pow10((double)level/2000.0);
    alsa_passthrough_volume_db[card][in_port][out_port]=level;
  }
  else {
    alsa_passthrough_volume[card][in_port][out_port]=0.0;
    alsa_passthrough_volume_db[card][in_port][out_port]=-10000;
  }
  return true;
#else
  return false;
#endif  // ALSA
}


#ifdef ALSA
bool MainObject::AlsaStartCaptureDevice(QString &dev,int card,snd_pcm_t *pcm)
{
  snd_pcm_hw_params_t *hwparams;
  snd_pcm_sw_params_t *swparams;
  int dir;
  int err;
  pthread_attr_t pthread_attr;
  unsigned sr;

  memset(&alsa_capture_format[card],0,sizeof(struct alsa_format));

  snd_pcm_hw_params_alloca(&hwparams);
  snd_pcm_hw_params_any(pcm,hwparams);

  LogLine(RDConfig::LogInfo,QString().
	  sprintf("Starting ALSA Capture Device %s:",
		  (const char *)dev));

  //
  // Access Type
  //
  if(snd_pcm_hw_params_test_access(pcm,hwparams,
				   SND_PCM_ACCESS_RW_INTERLEAVED)<0) {
    LogLine(RDConfig::LogErr,"  Interleaved access not supported,");
    LogLine(RDConfig::LogErr,"  aborting initialization of device.");
    return false;
  }
  snd_pcm_hw_params_set_access(pcm,hwparams,SND_PCM_ACCESS_RW_INTERLEAVED);

  //
  // Sample Format
  //
  if(snd_pcm_hw_params_test_format(pcm,hwparams,SND_PCM_FORMAT_S16_LE)==0) {
    alsa_capture_format[card].format=SND_PCM_FORMAT_S16_LE;
    LogLine(RDConfig::LogDebug,"  Format = 16 bit little-endian");
  }
  else {
    if(snd_pcm_hw_params_test_format(pcm,hwparams,SND_PCM_FORMAT_S32_LE)==0) {
      alsa_capture_format[card].format=SND_PCM_FORMAT_S32_LE;
      LogLine(RDConfig::LogDebug,"  Format = 32 bit little-endian");
    }
    else {
      LogLine(RDConfig::LogErr,
	      "  Neither 16 nor 32 bit little-endian formats available,");
      LogLine(RDConfig::LogErr,
	      "  aborting initialization of device.");
      return false;
    }
  }
  snd_pcm_hw_params_set_format(pcm,hwparams,alsa_capture_format[card].format);

  //
  // Sample Rate
  //
  if(alsa_play_format[card].sample_rate>0) {
    sr=alsa_play_format[card].sample_rate;
  }
  else {
    sr=system_sample_rate;
  }
  snd_pcm_hw_params_set_rate_near(pcm,hwparams,&sr,&dir);
  if((sr<(system_sample_rate-RD_ALSA_SAMPLE_RATE_TOLERANCE))||
     (sr>(system_sample_rate+RD_ALSA_SAMPLE_RATE_TOLERANCE))) {
    LogLine(RDConfig::LogErr,
	    QString().sprintf("  Asked for sample rate %u, got %u",
			      system_sample_rate,sr));
    LogLine(RDConfig::LogErr,"  Sample rate unsupported by device");
    return false;
  }
  alsa_capture_format[card].sample_rate=sr;
  LogLine(RDConfig::LogNotice,QString().sprintf("  SampleRate = %u",sr));

  //
  // Channels
  //
  if(rd_config->alsaChannelsPerPcm()<0) {
    alsa_capture_format[card].channels=rd_config->channels()*RD_MAX_PORTS;
  }
  else {
    alsa_capture_format[card].channels=rd_config->alsaChannelsPerPcm();
  }
  snd_pcm_hw_params_set_channels_near(pcm,hwparams,
				      &alsa_capture_format[card].channels);
  alsa_play_format[card].capture_channels=alsa_capture_format[card].channels;
  LogLine(RDConfig::LogDebug,QString().
	  sprintf("  Aggregate Channels = %u",
		  alsa_capture_format[card].channels));

  //
  // Buffer Size
  //
  alsa_capture_format[card].periods=rd_config->alsaPeriodQuantity();
  snd_pcm_hw_params_set_periods_near(pcm,hwparams,
				     &alsa_capture_format[card].periods,&dir);
  LogLine(RDConfig::LogDebug,QString().sprintf("  Periods = %u",
		  alsa_capture_format[card].periods));
  alsa_capture_format[card].buffer_size=
    alsa_capture_format[card].periods*rd_config->alsaPeriodSize();
  snd_pcm_hw_params_set_buffer_size_near(pcm,hwparams,
	       			 &alsa_capture_format[card].buffer_size);
  LogLine(RDConfig::LogDebug,QString().sprintf("  BufferSize = %u frames",
	    (unsigned)alsa_capture_format[card].buffer_size));

  //
  // Fire It Up
  //
  if((err=snd_pcm_hw_params(pcm,hwparams))<0) {
    LogLine(RDConfig::LogErr,QString().sprintf("  Device Error: %s,",
			      (const char *)snd_strerror(err)));
    LogLine(RDConfig::LogErr,"  aborting initialization of device.");
    return false;
  }
  LogLine(RDConfig::LogNotice,"  Device started successfully");
  switch(alsa_capture_format[card].format) {
      case SND_PCM_FORMAT_S16_LE:
	alsa_capture_format[card].card_buffer_size=
	  alsa_capture_format[card].buffer_size*
	  alsa_capture_format[card].channels*2;
	break;

      case SND_PCM_FORMAT_S32_LE:
	alsa_capture_format[card].card_buffer_size=
	  alsa_capture_format[card].buffer_size*
	  alsa_capture_format[card].channels*4;
	break;

      default:
	break;
  }
  alsa_capture_format[card].card_buffer=
    new char[alsa_capture_format[card].card_buffer_size];
  alsa_capture_format[card].passthrough_buffer=
    new char[alsa_capture_format[card].card_buffer_size];
  alsa_capture_format[card].pcm=pcm;
  alsa_capture_format[card].card=card;
  //
  // Set Wake-up Timing
  //
  snd_pcm_sw_params_alloca(&swparams);
  snd_pcm_sw_params_current(pcm,swparams);
  snd_pcm_sw_params_set_avail_min(pcm,swparams,rd_config->alsaPeriodSize());
  if((err=snd_pcm_sw_params(pcm,swparams))<0) {
    LogLine(RDConfig::LogErr,QString().sprintf("ALSA Device %s: %s",
			      (const char *)dev,
			      (const char *)snd_strerror(err)));
    return false;
  }

  //
  // Start the Callback
  //
  pthread_attr_init(&pthread_attr);
/*
  if(use_realtime) {
    pthread_attr_setschedpolicy(&pthread_attr,SCHED_FIFO);
  }
*/
  alsa_capture_format[card].exiting = false;
  pthread_create(&alsa_capture_format[card].thread,&pthread_attr,
		 AlsaCaptureCallback,&alsa_capture_format[card]);
  return true;
}


bool MainObject::AlsaStartPlayDevice(QString &dev,int card,snd_pcm_t *pcm)
{
  snd_pcm_hw_params_t *hwparams;
  snd_pcm_sw_params_t *swparams;
  int dir;
  int err;
  pthread_attr_t pthread_attr;
  unsigned sr;

  memset(&alsa_play_format[card],0,sizeof(struct alsa_format));

  snd_pcm_hw_params_alloca(&hwparams);
  snd_pcm_hw_params_any(pcm,hwparams);

  LogLine(RDConfig::LogNotice,QString().sprintf("Starting ALSA Play Device %s:",
						(const char *)dev));

  //
  // Access Type
  //
  if(snd_pcm_hw_params_test_access(pcm,hwparams,
				   SND_PCM_ACCESS_RW_INTERLEAVED)<0) {
    LogLine(RDConfig::LogErr,"  Interleaved access not supported,");
    LogLine(RDConfig::LogErr,"  aborting initialization of device.");
    return false;
  }
  snd_pcm_hw_params_set_access(pcm,hwparams,SND_PCM_ACCESS_RW_INTERLEAVED);

  //
  // Sample Format
  //
  if(snd_pcm_hw_params_test_format(pcm,hwparams,SND_PCM_FORMAT_S16_LE)==0) {
    alsa_play_format[card].format=SND_PCM_FORMAT_S16_LE;
    LogLine(RDConfig::LogDebug,"  Format = 16 bit little-endian");
  }
  else {
    if(snd_pcm_hw_params_test_format(pcm,hwparams,SND_PCM_FORMAT_S32_LE)==0) {
      alsa_play_format[card].format=SND_PCM_FORMAT_S32_LE;
      LogLine(RDConfig::LogDebug,"  Format = 32 bit little-endian");
    }
    else {
      LogLine(RDConfig::LogErr,
	      "  Neither 16 nor 32 bit little-endian formats available,");
      LogLine(RDConfig::LogErr,"  aborting initialization of device.");
      return false;
    }
  }
  snd_pcm_hw_params_set_format(pcm,hwparams,alsa_play_format[card].format);

  //
  // Sample Rate
  //
  sr=system_sample_rate;
  snd_pcm_hw_params_set_rate_near(pcm,hwparams,&sr,&dir);
  if((sr<(system_sample_rate-RD_ALSA_SAMPLE_RATE_TOLERANCE))||
     (sr>(system_sample_rate+RD_ALSA_SAMPLE_RATE_TOLERANCE))) {
    LogLine(RDConfig::LogErr,
	    QString().sprintf("  Asked for sample rate %u, got %u",
			      system_sample_rate,sr));
    LogLine(RDConfig::LogErr,"  Sample rate unsupported by device");
    return false;
  }
  alsa_play_format[card].sample_rate=sr;
  LogLine(RDConfig::LogDebug,QString().sprintf("  SampleRate = %u",sr));

  //
  // Channels
  //
  if(rd_config->alsaChannelsPerPcm()<0) {
    alsa_play_format[card].channels=rd_config->channels()*RD_MAX_PORTS;
  }
  else {
    alsa_play_format[card].channels=rd_config->alsaChannelsPerPcm();
  }
  snd_pcm_hw_params_set_channels_near(pcm,hwparams,
				      &alsa_play_format[card].channels);
  LogLine(RDConfig::LogDebug,QString().sprintf("  Aggregate Channels = %u",
			    alsa_play_format[card].channels));

  //
  // Buffer Size
  //
  alsa_play_format[card].periods=rd_config->alsaPeriodQuantity();
  snd_pcm_hw_params_set_periods_near(pcm,hwparams,
				     &alsa_play_format[card].periods,&dir);
  LogLine(RDConfig::LogDebug,QString().sprintf("  Periods = %u",
					       alsa_play_format[card].periods));
  alsa_play_format[card].buffer_size=
    alsa_play_format[card].periods*rd_config->alsaPeriodSize();
  snd_pcm_hw_params_set_buffer_size_near(pcm,hwparams,
					 &alsa_play_format[card].buffer_size);
  LogLine(RDConfig::LogDebug,QString().
       sprintf("  BufferSize = %u frames",
	       (unsigned)alsa_play_format[card].buffer_size));

  //
  // Fire It Up
  //
  if((err=snd_pcm_hw_params(pcm,hwparams))<0) {
    LogLine(RDConfig::LogErr,QString().sprintf("  Device Error: %s,",
			      (const char *)snd_strerror(err)));
    LogLine(RDConfig::LogErr,"  aborting initialization of device.");
    return false;
  }
  LogLine(RDConfig::LogNotice,"  Device started successfully");
  switch(alsa_play_format[card].format) {
      case SND_PCM_FORMAT_S16_LE:
	alsa_play_format[card].card_buffer_size=
	  alsa_play_format[card].buffer_size*alsa_play_format[card].channels*2;
	break;

      case SND_PCM_FORMAT_S32_LE:
	alsa_play_format[card].card_buffer_size=
	  alsa_play_format[card].buffer_size*alsa_play_format[card].channels*4;
	break;

      default:
	break;
  }
  alsa_play_format[card].card_buffer=
    new char[alsa_play_format[card].card_buffer_size];
  alsa_play_format[card].passthrough_buffer=
    new char[alsa_play_format[card].card_buffer_size];
  alsa_play_format[card].pcm=pcm;
  alsa_play_format[card].card=card;

  //
  // Set Wake-up Timing
  //
  snd_pcm_sw_params_alloca(&swparams);
  snd_pcm_sw_params_current(pcm,swparams);
  snd_pcm_sw_params_set_avail_min(pcm,swparams,rd_config->alsaPeriodSize());
  if((err=snd_pcm_sw_params(pcm,swparams))<0) {
    LogLine(RDConfig::LogErr,QString().sprintf("ALSA Device %s: %s",
					       (const char *)dev,
					    (const char *)snd_strerror(err)));
    return false;
  }

  //
  // Start the Callback
  //
  pthread_attr_init(&pthread_attr);
  /*
  if(use_realtime) {
    pthread_attr_setschedpolicy(&pthread_attr,SCHED_FIFO);
  }
  */
  alsa_play_format[card].exiting = false;
  pthread_create(&alsa_play_format[card].thread,&pthread_attr,
		 AlsaPlayCallback,&alsa_play_format[card]);
  return true;
}


int MainObject::GetAlsaOutputStream(int card)
{
  for(int i=0;i<RD_MAX_STREAMS;i++) {
    if(alsa_play_ring[card][i]==NULL) {
      alsa_play_ring[card][i]=new RDRingBuffer(RINGBUFFER_SIZE);
      return i;
    }
  }
  return -1;
}


void MainObject::FreeAlsaOutputStream(int card,int stream)
{
  delete alsa_play_ring[card][stream];
  alsa_play_ring[card][stream]=NULL;
}


void MainObject::EmptyAlsaInputStream(int card,int stream)
{
  unsigned n=alsa_record_ring[card][stream]->
    read((char *)alsa_wave_buffer,alsa_record_ring[card][stream]->
	 readSpace());
  WriteAlsaBuffer(card,stream,alsa_wave_buffer,n);
}


void MainObject::WriteAlsaBuffer(int card,int stream,short *buffer,unsigned len)
{
  ssize_t s;
  unsigned char mpeg[2048];
  unsigned frames;
  unsigned n;

  frames=len/(2*alsa_record_wave[card][stream]->getChannels());
  alsa_samples_recorded[card][stream]+=frames;
  switch(alsa_record_wave[card][stream]->getFormatTag()) {
  case WAVE_FORMAT_PCM:
    alsa_record_wave[card][stream]->writeWave(buffer,len);
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
      if((s=twolame_encode_buffer_interleaved(twolame_lameopts[card][stream],
		   buffer+i*alsa_record_wave[card][stream]->getChannels(),
					      n,mpeg,2048))>=0) {
	alsa_record_wave[card][stream]->writeWave(mpeg,s);
      }
      else {
	LogLine(RDConfig::LogErr,QString().
	   sprintf("TwoLAME encode error, card: %d, stream: %d",card,stream));
      }
    }
#endif  // HAVE_TWOLAME
    break;
  }
}


void MainObject::FillAlsaOutputStream(int card,int stream)
{
  unsigned mpeg_frames=0;
  unsigned frame_offset=0;
  int m=0;
  int n=0;
  double ratio=0.0;
  int free=(alsa_play_ring[card][stream]->writeSpace()-1);
  if(free<=0) {
    return;
  }
  ratio=(double)alsa_play_format[card].sample_rate/
    (double)alsa_play_wave[card][stream]->getSamplesPerSec();
  switch(alsa_play_wave[card][stream]->getFormatTag()) {
  case WAVE_FORMAT_PCM:
  case WAVE_FORMAT_VORBIS:
    free=(int)((double)free/ratio)/(2*alsa_output_channels[card][stream])*
      (2*alsa_output_channels[card][stream]);
    n=alsa_play_wave[card][stream]->readWave(alsa_wave_buffer,free);
    if(n!=free) {
      alsa_eof[card][stream]=true;
      alsa_stop_timer[card][stream]->stop();
    }
    break;

  case WAVE_FORMAT_MPEG:
#ifdef HAVE_MAD
    mpeg_frames=free/(2304*alsa_output_channels[card][stream]);
    free=mpeg_frames*2304*alsa_output_channels[card][stream];
    for(unsigned i=0;i<mpeg_frames;i++) {
      m=alsa_play_wave[card][stream]->
	readWave(mad_mpeg[card][stream]+mad_left_over[card][stream],
		 mad_frame_size[card][stream]);
      if(m==mad_frame_size[card][stream]) {
	mad_stream_buffer(&mad_stream[card][stream],mad_mpeg[card][stream],
			  m+mad_left_over[card][stream]);
	while(mad_frame_decode(&mad_frame[card][stream],
			       &mad_stream[card][stream])==0) {
	  mad_synth_frame(&mad_synth[card][stream],&mad_frame[card][stream]);
	  n+=(2*alsa_output_channels[card][stream]*
	      mad_synth[card][stream].pcm.length);
	  for(int j=0;j<mad_synth[card][stream].pcm.length;j++) {
	    for(int k=0;k<mad_synth[card][stream].pcm.channels;k++) {
	      alsa_wave_buffer[frame_offset+
			       j*mad_synth[card][stream].pcm.channels+k]=
		(short)(32768.0*mad_f_todouble(mad_synth[card][stream].
					       pcm.samples[k][j]));
	    }
	  }
	  frame_offset+=(mad_synth[card][stream].pcm.length*
			 mad_synth[card][stream].pcm.channels);
	}
      }
      else {  // End-of-file, read out last samples
	if(!alsa_eof[card][stream]) {
	  memset(mad_mpeg[card][stream]+mad_left_over[card][stream],0,
		 MAD_BUFFER_GUARD);
	  mad_stream_buffer(&mad_stream[card][stream],
			    mad_mpeg[card][stream],
			    MAD_BUFFER_GUARD+mad_left_over[card][stream]);
	  if(mad_frame_decode(&mad_frame[card][stream],
			      &mad_stream[card][stream])==0) {
	    mad_synth_frame(&mad_synth[card][stream],
			    &mad_frame[card][stream]);
	    n+=(alsa_output_channels[card][stream]*
		mad_synth[card][stream].pcm.length);
	    for(int j=0;j<mad_synth[card][stream].pcm.length;j++) {
	      for(int k=0;k<mad_synth[card][stream].pcm.channels;k++) {
		alsa_wave_buffer[frame_offset+
				 j*mad_synth[card][stream].pcm.channels+k]=
		  (short)(32768.0*mad_f_todouble(mad_synth[card][stream].
						 pcm.samples[k][j]));
	      }
	    }
	  }
	}
	alsa_eof[card][stream]=true;
	alsa_stop_timer[card][stream]->stop();
	continue;
      }
      mad_left_over[card][stream]=
	mad_stream[card][stream].bufend-mad_stream[card][stream].next_frame;
      memmove(mad_mpeg[card][stream],mad_stream[card][stream].next_frame,
	      mad_left_over[card][stream]);
    }
#endif  // HAVE_MAD
    break;
  }
  alsa_play_ring[card][stream]->write((char *)alsa_wave_buffer,n);
}
#endif  // ALSA


void MainObject::AlsaClock()
{
#ifdef ALSA
  for(int i=0;i<RD_MAX_CARDS;i++) {
    if(cae_driver[i]==RDStation::Alsa) {
      for(int j=0;j<RD_MAX_STREAMS;j++) {
	if(alsa_stopping[i][j]) {
	  alsa_stopping[i][j]=false;
	  alsa_eof[i][j]=false;
	  alsa_playing[i][j]=false;
	  printf("stop card: %d  stream: %d\n",i,j);
	  statePlayUpdate(i,j,2);
	}
	if(alsa_playing[i][j]) {
	  FillAlsaOutputStream(i,j);
	}
      }
      for(int j=0;j<RD_MAX_PORTS;j++) {
	if(alsa_recording[i][j]) {
	  EmptyAlsaInputStream(i,j);
	}
      }
    }
  }
#endif  // ALSA
}
