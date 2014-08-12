// rdaudioconvert.h
//
// Convert Audio File Formats
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdaudioconvert.h,v 1.4.4.1 2012/09/06 19:47:15 cvs Exp $
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

#ifndef RDAUDIOCONVERT_H
#define RDAUDIOCONVERT_H

#include <sndfile.h>
#ifdef HAVE_TWOLAME
#include <twolame.h>
#endif  // HAVE_TWOLAME
#ifdef HAVE_LAME
#include <lame/lame.h>
#endif  // HAVE_LAME
#ifdef HAVE_MAD
#include <mad.h>
#endif  // HAVE_MAD
#include <qobject.h>

#include <rdwavedata.h>
#include <rdsettings.h>
#include <rdwavefile.h>

class RDAudioConvert : public QObject
{
  Q_OBJECT;
 public:
  enum ErrorCode {ErrorOk=0,ErrorInvalidSettings=1,ErrorNoSource=2,
		  ErrorNoDestination=3,ErrorInvalidSource=4,ErrorInternal=5,
		  ErrorFormatNotSupported=6,ErrorNoDisc=7,ErrorNoTrack=8,
		  ErrorInvalidSpeed=9,ErrorFormatError=10,ErrorNoSpace=11};
  RDAudioConvert(const QString &station_name,
		 QObject *parent=0,const char *name=0);
  ~RDAudioConvert();
  void setSourceFile(const QString &filename);
  void setDestinationFile(const QString &filename);
  void setDestinationSettings(RDSettings *settings);
  RDWaveData *sourceWaveData() const;
  void setDestinationWaveData(RDWaveData *wavedata);
  void setRange(int start_pt,int end_pt);
  void setSpeedRatio(float ratio);
  RDAudioConvert::ErrorCode convert();
  static bool settingsValid(RDSettings *settings);
  static QString errorText(RDAudioConvert::ErrorCode err);

 private:
  RDAudioConvert::ErrorCode Stage1Convert(const QString &srcfile,
					  const QString &dstfile);
  RDAudioConvert::ErrorCode Stage1Flac(const QString &dstfile,
				       RDWaveFile *wave);
  RDAudioConvert::ErrorCode Stage1Vorbis(const QString &dstfile,
					 RDWaveFile *wave);
  RDAudioConvert::ErrorCode Stage1Mpeg(const QString &dstfile,
				       RDWaveFile *wave);
  RDAudioConvert::ErrorCode Stage1SndFile(const QString &dstfile,
					  SNDFILE *sf_src,
					  SF_INFO *sf_src_info);
  RDAudioConvert::ErrorCode Stage2Convert(const QString &srcfile,
					  const QString &dstfile);
  RDAudioConvert::ErrorCode Stage3Convert(const QString &srcfile,
					  const QString &dstfile);
  RDAudioConvert::ErrorCode Stage3Flac(SNDFILE *src_sf,SF_INFO *src_sf_info,
				       const QString &dstfile);
  RDAudioConvert::ErrorCode Stage3Vorbis(SNDFILE *src_sf,SF_INFO *src_sf_info,
					 const QString &dstfile);
  RDAudioConvert::ErrorCode Stage3Layer3(SNDFILE *src_sf,SF_INFO *src_sf_info,
					 const QString &dstfile);
  RDAudioConvert::ErrorCode Stage3Layer2Wav(SNDFILE *src_sf,
					    SF_INFO *src_sf_info,
					    const QString &dstfile);
  RDAudioConvert::ErrorCode Stage3Layer2(SNDFILE *src_sf,SF_INFO *src_sf_info,
					 const QString &dstfile);
  RDAudioConvert::ErrorCode Stage3Pcm16(SNDFILE *src_sf,SF_INFO *src_sf_info,
					const QString &dstfile);
  void ApplyId3Tag(const QString &filename,RDWaveData *wavedata);
  void UpdatePeak(const float data[],ssize_t len);
  void UpdatePeak(const double data[],ssize_t len);
  bool LoadMad();
  bool LoadTwoLame();
  bool LoadLame();
  QString conv_src_filename;
  QString conv_dst_filename;
  int conv_start_point;
  int conv_end_point;
  float conv_speed_ratio;
  RDSettings *conv_settings;
  RDWaveData *conv_src_wavedata;
  RDWaveData *conv_dst_wavedata;
  float conv_peak_sample;
  int conv_src_converter;
  void *conv_mad_handle;
  void *conv_lame_handle;
  void *conv_twolame_handle;
#ifdef HAVE_MAD
  void (*mad_stream_init)(struct mad_stream *);
  void (*mad_frame_init)(struct mad_frame *);
  void (*mad_synth_init)(struct mad_synth *);
  void (*mad_stream_buffer)(struct mad_stream *,unsigned char const *,
			    unsigned long);
  int (*mad_frame_decode)(struct mad_frame *, struct mad_stream *);
  void (*mad_synth_frame)(struct mad_synth *, struct mad_frame const *);
  void (*mad_frame_finish)(struct mad_frame *);
  void (*mad_stream_finish)(struct mad_stream *);
#endif  // HAVE_MAD
#ifdef HAVE_TWOLAME
  twolame_options *(*twolame_init)(void);
  void (*twolame_set_mode)(twolame_options *,TWOLAME_MPEG_mode);
  void (*twolame_set_num_channels)(twolame_options *,int);
  void (*twolame_set_in_samplerate)(twolame_options *,int);
  void (*twolame_set_out_samplerate)(twolame_options *,int);
  void (*twolame_set_bitrate)(twolame_options *,int);
  int (*twolame_init_params)(twolame_options *);
  void (*twolame_close)(twolame_options **);
  int (*twolame_encode_buffer_float32_interleaved)
    (twolame_options *,const float[],int,unsigned char *,int);
  int (*twolame_encode_flush)(twolame_options *,unsigned char *,int);
  int (*twolame_set_energy_levels)(twolame_options *,int);
#endif  // HAVE_TWOLAME
#ifdef HAVE_LAME
  lame_global_flags *(*lame_init)(void);
  void (*lame_set_mode)(lame_global_flags *,int);
  void (*lame_set_num_channels)(lame_global_flags *,int);
  void (*lame_set_in_samplerate)(lame_global_flags *,int);
  void (*lame_set_out_samplerate)(lame_global_flags *,int);
  void (*lame_set_brate)(lame_global_flags *,int);
  int (*lame_init_params)(lame_global_flags *);
  void (*lame_close)(lame_global_flags *);
  int (*lame_encode_buffer_interleaved)
    (lame_global_flags *,short int[],int,unsigned char *,int);
  int (*lame_encode_buffer)
    (lame_global_flags *,short int[],short int[],int,unsigned char *,int);
  int (*lame_encode_flush)(lame_global_flags *,unsigned char *,int);
  int (*lame_set_bWriteVbrTag)(lame_global_flags *, int);
#endif  // HAVE_LAME
};


#endif  // RDAUDIOCONVERT_H
