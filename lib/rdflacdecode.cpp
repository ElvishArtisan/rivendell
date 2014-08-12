// rdflacdecode.cpp
//
// Decode FLAC Files using libFLAC++
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdflacdecode.cpp,v 1.5.6.1 2012/12/13 22:33:44 cvs Exp $
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

#include <stdio.h>
#include <math.h>

#include <rdflacdecode.h>


#ifdef HAVE_FLAC
RDFlacDecode::RDFlacDecode(SNDFILE *dst_sf)
  : FLAC::Decoder::File()
{
  flac_sf_dst=dst_sf;
  flac_start_point=-1;
  flac_end_point=-1;
}


void RDFlacDecode::setRange(int start_pt,int end_pt)
{
  flac_start_point=start_pt;
  flac_end_point=end_pt;
}


void RDFlacDecode::decode(RDWaveFile *wave,float *peak)
{
  flac_active=true;
  flac_wavefile=wave;
  flac_peak_sample=peak;
  if(flac_start_point<0) {
    flac_start_sample=0;
  }
  else {
    flac_start_sample=
      (double)flac_start_point*(double)wave->getSamplesPerSec()/1000.0;
  }
  if(flac_end_point<0) {
    flac_end_sample=wave->getSampleLength();
  }
  else {
    flac_end_sample=
      (double)flac_end_point*(double)wave->getSamplesPerSec()/1000.0;
  }
  flac_total_frames=0;
  init(wave->getName().ascii());
  //set_filename(wave->getName().ascii());
  //init();

  while(flac_active&&process_single());
}


FLAC__StreamDecoderWriteStatus 
RDFlacDecode::write_callback(const ::FLAC__Frame *frame, 
			     const FLAC__int32 *const buffer[])
{
  float *pcm=new float[frame->header.blocksize*frame->header.channels];
  float divider=(float)((1<<frame->header.bits_per_sample)/2.0);

  for(unsigned i=0;i<frame->header.channels;i++) {
    for(unsigned j=0;j<frame->header.blocksize;j++) {
      pcm[j*frame->header.channels+i]=(float)(buffer[i][j])/divider;
    }
  }
  if(flac_total_frames>=flac_start_sample) {
    if((flac_total_frames+frame->header.blocksize)<(unsigned)flac_end_sample) {    // Write entire buffer 
      UpdatePeak(pcm,frame->header.blocksize*flac_wavefile->getChannels());
      sf_writef_float(flac_sf_dst,pcm,frame->header.blocksize);
    }
    else {
      if((unsigned)flac_total_frames<(flac_total_frames+frame->header.blocksize)) {  // Write start of buffer
	UpdatePeak(pcm,
		   (flac_total_frames+frame->header.blocksize-flac_end_sample)*flac_wavefile->getChannels());
	sf_writef_float(flac_sf_dst,pcm,flac_total_frames+frame->header.blocksize-flac_end_sample);
	//
	// Done
	//
	sf_close(flac_sf_dst);
	flac_active=false;
	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
      }
    }
  }
  else {
    int diff=flac_total_frames+frame->header.blocksize-flac_start_sample;
    if(diff>0) {   // Write end of buffer
      UpdatePeak(pcm+diff,(frame->header.blocksize-diff)*flac_wavefile->getChannels());
      sf_writef_float(flac_sf_dst,pcm+diff,frame->header.blocksize-diff);
    }
  }
  flac_total_frames+=frame->header.blocksize;

  delete pcm;
  return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}


void RDFlacDecode::error_callback(::FLAC__StreamDecoderErrorStatus status)
{
  printf("FLAC error: %d\n",status);
}


void RDFlacDecode::metadata_callback(const FLAC__StreamMetadata*)
{
}


void RDFlacDecode::UpdatePeak(const float data[],ssize_t len)
{
  float peak;

  if(!flac_active) {
    return;
  }
  for(ssize_t i=0;i<len;i++) {
    if((peak=fabsf(data[i]))>*flac_peak_sample) {
      *flac_peak_sample=peak;
    }
  }
}

#endif  // HAVE_FLAC
