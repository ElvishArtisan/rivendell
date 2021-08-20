// caedriver.cpp
//
// Abstract base class for caed(8) audio driver implementations.
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

#include <dlfcn.h>

#include "driver.h"

Driver::Driver(RDStation::AudioDriver type,QObject *parent)
  : QObject(parent)
{
  d_driver_type=type;
  d_system_sample_rate=rda->system()->sampleRate();
  twolame_handle=NULL;
  mad_handle=NULL;
}


RDStation::AudioDriver Driver::driverType() const
{
  return d_driver_type;
}


bool Driver::hasCard(int cardnum) const
{
  return d_cards.contains(cardnum);
}


void Driver::processBuffers()
{
}


void Driver::statePlayUpdate(int card,int stream,int state)
{
  emit playStateChanged(card,stream,state);
}


void Driver::stateRecordUpdate(int card,int stream,int state)
{
  emit recordStateChanged(card,stream,state);
}


void Driver::addCard(unsigned cardnum)
{
  if(d_cards.contains(cardnum)) {
    rda->syslog(LOG_WARNING,
		"%s driver attempted to register card %u multiple times",
		RDStation::audioDriverText(d_driver_type).toUtf8().constData(),
		cardnum);
  }
  else {
    d_cards.push_back(cardnum);
  }
}


unsigned Driver::systemSampleRate() const
{
  return d_system_sample_rate;
}


bool Driver::LoadTwoLame()
{
#ifdef HAVE_TWOLAME
  if((twolame_handle=dlopen("libtwolame.so.0",RTLD_NOW))==NULL) {
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


bool Driver::InitTwoLameEncoder(int card,int stream,int chans,int samprate,
				    int bitrate)
{
  if(twolame_handle==NULL) {
    rda->syslog(LOG_WARNING,"MPEG Layer 2 encode not available");
    return false;
  }
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


void Driver::FreeTwoLameEncoder(int card,int stream)
{
#ifdef HAVE_TWOLAME
  if(twolame_lameopts[card][stream]!=NULL) { 
    twolame_close(&twolame_lameopts[card][stream]);
    twolame_lameopts[card][stream]=NULL;
  }
#endif  // HAVE_TWOLAME
}


bool Driver::LoadMad()
{
#ifdef HAVE_MAD
  if((mad_handle=dlopen("libmad.so.0",RTLD_NOW))==NULL) {
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


bool Driver::InitMadDecoder(int card,int stream,RDWaveFile *wave)
{
  if(mad_handle==NULL) {
    rda->syslog(LOG_WARNING,"MPEG Layer 2 decode not available");
    return false;
  }
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
  return true;
#endif  // HAVE_MAD
  return false;
}


void Driver::FreeMadDecoder(int card,int stream)
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
