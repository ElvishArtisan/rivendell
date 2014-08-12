// rdflacdecode.h
//
// Decode FLAC Files using libFLAC++
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdflacdecode.h,v 1.4 2010/11/24 18:03:30 cvs Exp $
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

#ifndef RDFLACDECODE_H
#define RDFLACDECODE_H

#include <sndfile.h>
#ifdef HAVE_FLAC
#include <FLAC++/decoder.h>

#include <rdwavefile.h>

class RDFlacDecode : public FLAC::Decoder::File
{
 public:
  RDFlacDecode(SNDFILE *dst_sf);
  void setRange(int start_pt,int end_pt);
  void decode(RDWaveFile *src_wave,float *peak);

 protected:
  FLAC__StreamDecoderWriteStatus 
    write_callback(const ::FLAC__Frame *frame, 
		    const FLAC__int32 *const buffer[]);
  void error_callback(::FLAC__StreamDecoderErrorStatus status);
  void metadata_callback(const FLAC__StreamMetadata*);

 private:
  void UpdatePeak(const float data[],ssize_t len);
  SNDFILE *flac_sf_dst;
  int flac_start_point;
  int flac_end_point;
  int flac_start_sample;
  int flac_end_sample;
  float *flac_peak_sample;
  int flac_total_frames;
  RDWaveFile *flac_wavefile;
  bool flac_active;
};
#endif  // HAVE_FLAC


#endif  // RDFLACDECODER_H
