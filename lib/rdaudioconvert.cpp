// rdaudioconvert.cpp
//
// Convert Audio File Formats
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdaudioconvert.cpp,v 1.14.2.3.2.1 2014/05/15 16:30:00 cvs Exp $
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

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <dlfcn.h>
#include <errno.h>

#include <sndfile.h>
#include <samplerate.h>
#include <soundtouch/SoundTouch.h>
#ifdef HAVE_VORBIS
#include <ogg/ogg.h>
#include <vorbis/vorbisenc.h>
#endif  // HAVE_VORBIS
#ifdef HAVE_FLAC
#include <FLAC++/encoder.h>
#include <rdflacdecode.h>
#endif  // HAVE_FLAC
#include <id3/tag.h>
#include <qfile.h>

#include <rd.h>
#include <rdaudioconvert.h>
#include <rdlibrary_conf.h>
#include <rdconf.h>

#define STAGE2_XFER_SIZE 2048
#define STAGE2_BUFFER_SIZE 49152

RDAudioConvert::RDAudioConvert(const QString &station_name,
			       QObject *parent,const char *name)
  : QObject(parent,name)
{
  conv_start_point=-1;
  conv_end_point=-1;
  conv_speed_ratio=1.0;
  conv_peak_sample=0.0;
  conv_settings=NULL;
  conv_src_wavedata=new RDWaveData();
  conv_dst_wavedata=NULL;
  RDLibraryConf *conf=new RDLibraryConf(station_name,0);
  conv_src_converter=conf->srcConverter();
  delete conf;

  //
  // Load MPEG Libraries
  //
  conv_mad_handle=dlopen("libmad.so",RTLD_LAZY);
  conv_lame_handle=dlopen("libmp3lame.so",RTLD_LAZY);
  conv_twolame_handle=dlopen("libtwolame.so",RTLD_LAZY);
}


RDAudioConvert::~RDAudioConvert()
{
  delete conv_src_wavedata;
}


void RDAudioConvert::setSourceFile(const QString &filename)
{
  conv_src_filename=filename;
}


void RDAudioConvert::setDestinationFile(const QString &filename)
{
  conv_dst_filename=filename;
}


void RDAudioConvert::setDestinationSettings(RDSettings *settings)
{
  conv_settings=settings;
}


RDWaveData *RDAudioConvert::sourceWaveData() const
{
  return conv_src_wavedata;  
}


void RDAudioConvert::setDestinationWaveData(RDWaveData *wavedata)
{
  conv_dst_wavedata=wavedata;
}


void RDAudioConvert::setRange(int start_pt,int end_pt)
{
  conv_start_point=start_pt;
  conv_end_point=end_pt;
}


void RDAudioConvert::setSpeedRatio(float ratio)
{
  conv_speed_ratio=ratio;
}


RDAudioConvert::ErrorCode RDAudioConvert::convert()
{
  char tmpdir[PATH_MAX];
  RDAudioConvert::ErrorCode err;
  QString tmpfile1;
  QString tmpfile2;

  //
  // Make sure we're all set to go...
  //
  if(conv_settings==NULL) {
    return RDAudioConvert::ErrorInvalidSettings;
  }
  if(!RDAudioConvert::settingsValid(conv_settings)) {
    return RDAudioConvert::ErrorInvalidSettings;
  }
  if(!QFile::exists(conv_src_filename)) {
    return RDAudioConvert::ErrorNoSource;
  }
  if(conv_dst_filename.isEmpty()) {
    return RDAudioConvert::ErrorNoDestination;
  }
  if((conv_speed_ratio<RD_TIMESCALE_MIN)||(conv_speed_ratio>RD_TIMESCALE_MAX)) {
    return RDAudioConvert::ErrorInvalidSpeed;
  }

  //
  // Generate Temporary Filenames
  //
  strcpy(tmpdir,RDTempDir());
  strcat(tmpdir,"/rdaudioconvertXXXXXX");
  if(mkdtemp(tmpdir)==NULL) {
    return RDAudioConvert::ErrorInternal;
  }
  tmpfile1=QString(tmpdir)+"/signed32_1.wav";
  tmpfile2=QString(tmpdir)+"/signed32_2.wav";

  //
  // Stage One -- Convert Source Format to Signed 32 Bit Integer
  //
  if((err=Stage1Convert(conv_src_filename,tmpfile1))!=
     RDAudioConvert::ErrorOk) {
    unlink(tmpfile1);
    rmdir(tmpdir);
    return err;
  }

  //
  // Stage Two -- Convert Levels, Sample Rate, Channelization, Speed
  //
  if((err=Stage2Convert(tmpfile1,tmpfile2))!=
     RDAudioConvert::ErrorOk) {
    unlink(tmpfile1);
    unlink(tmpfile2);
    rmdir(tmpdir);
    return err;
  }

  //
  // Stage Three -- Write Out Destination Format
  //
  if((err=Stage3Convert(tmpfile2,conv_dst_filename))!=
     RDAudioConvert::ErrorOk) {
    unlink(tmpfile1);
    unlink(tmpfile2);
    rmdir(tmpdir);
    return err;
  }

  //
  // Clean Up
  //
  unlink(tmpfile1);
  unlink(tmpfile2);
  rmdir(tmpdir);

  return RDAudioConvert::ErrorOk;
}


bool RDAudioConvert::settingsValid(RDSettings *settings)
{
  return true;
}


QString RDAudioConvert::errorText(RDAudioConvert::ErrorCode err)
{
  QString ret=QString().sprintf("Unknown Error [%u]",err);

  switch(err) {
  case RDAudioConvert::ErrorOk:
    ret=tr("OK");
    break;

  case RDAudioConvert::ErrorInvalidSettings:
    ret=tr("Invalid/Unsupported Settings");
    break;

  case RDAudioConvert::ErrorNoSource:
    ret=tr("Unable to access source file");
    break;

  case RDAudioConvert::ErrorNoDestination:
    ret=tr("Unable to create destination file");
    break;

  case RDAudioConvert::ErrorInvalidSource:
    ret=tr("Unrecognized source format");
    break;

  case RDAudioConvert::ErrorInternal:
    ret=tr("Internal Error");
    break;

  case RDAudioConvert::ErrorFormatNotSupported:
    ret=tr("Unsupported Format");
    break;

  case RDAudioConvert::ErrorNoDisc:
    ret=tr("No CD found in drive");
    break;

  case RDAudioConvert::ErrorNoTrack:
    ret=tr("No such track on CD");
    break;

  case RDAudioConvert::ErrorInvalidSpeed:
    ret=tr("Invalid speed ratio");
    break;

  case RDAudioConvert::ErrorFormatError:
    ret=tr("Source format error");
    break;

  case RDAudioConvert::ErrorNoSpace:
    ret=tr("No space left on device");
    break;
  }
  return ret;
}


RDAudioConvert::ErrorCode RDAudioConvert::Stage1Convert(const QString &srcfile,
							const QString &dstfile)
{
  SNDFILE *sf_src=NULL;
  SF_INFO sf_src_info;
  RDWaveFile *wave=NULL;
  RDAudioConvert::ErrorCode err=RDAudioConvert::ErrorInvalidSource;

  //
  // Try RDWaveFile
  //
  wave=new RDWaveFile(srcfile);
  if(wave->openWave(conv_src_wavedata)) {
    switch(wave->type()) {
    case RDWaveFile::Wave:
      if(wave->getFormatTag()==WAVE_FORMAT_MPEG) {
	err=Stage1Mpeg(dstfile,wave);
	delete wave;
	return err;
      }
      break;

    case RDWaveFile::Mpeg:
    case RDWaveFile::Atx:
    case RDWaveFile::Tmc:
    case RDWaveFile::Ambos:
      err=Stage1Mpeg(dstfile,wave);
      delete wave;
      return err;

    case RDWaveFile::Ogg:
      err=Stage1Vorbis(dstfile,wave);
      delete wave;
      return err;

    case RDWaveFile::Flac:
      err=Stage1Flac(dstfile,wave);
      delete wave;
      return err;

    case RDWaveFile::Aiff:
    case RDWaveFile::Unknown:
      break;
    }
  }
  delete wave;

  //
  // Try Libsndfile
  //
  memset(&sf_src_info,0,sizeof(sf_src_info));
  if((sf_src=sf_open(srcfile,SFM_READ,&sf_src_info))!=NULL) {
    err=Stage1SndFile(dstfile,sf_src,&sf_src_info);
    sf_close(sf_src);
    return RDAudioConvert::ErrorOk;
  }

  return err;
}


RDAudioConvert::ErrorCode RDAudioConvert::Stage1Flac(const QString &dstfile,
						     RDWaveFile *wave)
{
#ifdef HAVE_FLAC
  SNDFILE *sf_dst=NULL;
  SF_INFO sf_dst_info;
  RDFlacDecode *flac=NULL;

  //
  // Open Destination
  //
  memset(&sf_dst_info,0,sizeof(sf_dst_info));
  sf_dst_info.format=SF_FORMAT_WAV|SF_FORMAT_FLOAT;
  sf_dst_info.channels=wave->getChannels();
  sf_dst_info.samplerate=wave->getSamplesPerSec();
  if((sf_dst=sf_open(dstfile,SFM_WRITE,&sf_dst_info))==NULL) {
    return RDAudioConvert::ErrorNoDestination;
  }

  //
  // Decode
  //
  flac=new RDFlacDecode(sf_dst);
  flac->setRange(conv_start_point,conv_end_point);
  flac->decode(wave,&conv_peak_sample);

  //
  // Clean Up
  //
  delete flac;
  sf_close(sf_dst);
  return RDAudioConvert::ErrorOk;
#else
  return RDAudioConvert::ErrorFormatNotSupported;
#endif  // HAVE_FLAC
}


RDAudioConvert::ErrorCode RDAudioConvert::Stage1Vorbis(const QString &dstfile,
						       RDWaveFile *wave)
{
#ifdef HAVE_VORBIS
  SNDFILE *sf_dst=NULL;
  SF_INFO sf_dst_info;
  ogg_sync_state ogg_sync;
  ogg_stream_state ogg_stream;
  ogg_packet ogg_packet;
  ogg_page ogg_page;
  vorbis_info vorbis_info;
  vorbis_comment vorbis_comment;
  vorbis_dsp_state vorbis_dsp;
  vorbis_block vorbis_block;
  int fd;
  ssize_t n;
  long serialno=-1;
  bool vorbis_ready=false;
  int frames;
  float **pcm;
  float pcmbuf[32768];
  sf_count_t start=0;
  sf_count_t end=wave->getSampleLength();
  sf_count_t total_frames=0;

  //
  // Open Destination
  //
  memset(&sf_dst_info,0,sizeof(sf_dst_info));
  sf_dst_info.format=SF_FORMAT_WAV|SF_FORMAT_FLOAT;
  sf_dst_info.channels=wave->getChannels();
  sf_dst_info.samplerate=wave->getSamplesPerSec();
  if((sf_dst=sf_open(dstfile,SFM_WRITE,&sf_dst_info))==NULL) {
    return RDAudioConvert::ErrorNoDestination;
  }

  //
  // Initialize Decoder
  //
  if((fd=open(wave->getName(),O_RDONLY))<0) {
    sf_close(sf_dst);
    return RDAudioConvert::ErrorNoSource;
  }
  ogg_sync_init(&ogg_sync);
  vorbis_info_init(&vorbis_info);
  vorbis_comment_init(&vorbis_comment);

  //
  // Decode
  //
  if(conv_start_point>0) {
    start=(double)conv_start_point*(double)wave->getSamplesPerSec()/1000.0;
  }
  if(conv_end_point>=0) {
    end=(double)conv_end_point*(double)wave->getSamplesPerSec()/1000.0;
  }
  while((n=read(fd,ogg_sync_buffer(&ogg_sync,4096),4096))>0) {
    ogg_sync_wrote(&ogg_sync,n);
    while(ogg_sync_pageout(&ogg_sync,&ogg_page)==1) {
      if(serialno<0) {
	serialno=ogg_page_serialno(&ogg_page);
	ogg_stream_init(&ogg_stream,serialno);
      }
      if(ogg_stream_pagein(&ogg_stream,&ogg_page)==0) {
	while(ogg_stream_packetout(&ogg_stream,&ogg_packet)==1) {
	  switch(ogg_packet.packetno) {
	  case 0:  // Start Packet
	  case 1:  // Comment Packet
	    vorbis_synthesis_headerin(&vorbis_info,&vorbis_comment,&ogg_packet);
	    break;

	  case 2:  // Codebook Packet
	    vorbis_synthesis_headerin(&vorbis_info,&vorbis_comment,&ogg_packet);
	    vorbis_synthesis_init(&vorbis_dsp,&vorbis_info);
	    vorbis_block_init(&vorbis_dsp,&vorbis_block);
	    vorbis_ready=true;
	    break;

	  default: // Audio Packets
	    if(vorbis_synthesis(&vorbis_block,&ogg_packet)==0) {
	      vorbis_synthesis_blockin(&vorbis_dsp,&vorbis_block);
	    }
	    while((frames=vorbis_synthesis_pcmout(&vorbis_dsp,&pcm))>0) {
	      for(int i=0;i<frames;i++) {
		for(int j=0;j<wave->getChannels();j++) {
		  pcmbuf[wave->getChannels()*i+j]=pcm[j][i];
		}
	      }
	      if(total_frames>=start) {
		if((total_frames+frames)<end) {    // Write entire buffer 
		  UpdatePeak(pcmbuf,frames*wave->getChannels());
		  sf_writef_float(sf_dst,pcmbuf,frames);
		}
		else {
		  if(total_frames<(total_frames+frames)) {  // Write start of buffer
		    UpdatePeak(pcmbuf,
			       (total_frames+frames-end)*wave->getChannels());
		    sf_writef_float(sf_dst,pcmbuf,total_frames+frames-end);
		    //
		    // Done -- no need to decode the rest
		    //
		    if(vorbis_ready) {
		      vorbis_block_clear(&vorbis_block);
		      vorbis_dsp_clear(&vorbis_dsp);
		    }
		    vorbis_info_clear(&vorbis_info);
		    vorbis_comment_clear(&vorbis_comment);
		    ogg_stream_clear(&ogg_stream);
		    ogg_sync_clear(&ogg_sync);
		    ::close(fd);
		    sf_close(sf_dst);

		    return RDAudioConvert::ErrorOk;
		  }
		}
	      }
	      else {
		int diff=total_frames+frames-start;
		if(diff>0) {   // Write end of buffer
		  UpdatePeak(pcmbuf+diff,(frames-diff)*wave->getChannels());
		  sf_writef_float(sf_dst,pcmbuf+diff,frames-diff);
		}
	      }
	      total_frames+=frames;
	      vorbis_synthesis_read(&vorbis_dsp,frames);
	    }
	    break;
	  }
	}
      }
    }
  }

  //
  // Clean Up
  //
  if(vorbis_ready) {
    vorbis_block_clear(&vorbis_block);
    vorbis_dsp_clear(&vorbis_dsp);
  }
  vorbis_info_clear(&vorbis_info);
  vorbis_comment_clear(&vorbis_comment);
  ogg_stream_clear(&ogg_stream);
  ogg_sync_clear(&ogg_sync);
  ::close(fd);
  sf_close(sf_dst);

  return RDAudioConvert::ErrorOk;
#else
  return RDAudioConvert::ErrorFormatNotSupported;
#endif  // HAVE_VORBIS
}

#define STAGE1BUFSIZE 2500

RDAudioConvert::ErrorCode RDAudioConvert::Stage1Mpeg(const QString &dstfile,
						     RDWaveFile *wave)
{
#ifdef HAVE_MAD
  SNDFILE *sf_dst=NULL;
  SF_INFO sf_dst_info;
  struct mad_stream mad_stream;
  struct mad_frame mad_frame;
  struct mad_synth mad_synth;
  int left_over=0;
  int fsize;
  int n;
  unsigned char buffer[STAGE1BUFSIZE];
  float sf_buffer[1152*2];
  sf_count_t start=0;
  sf_count_t end=wave->getSampleLength();
  sf_count_t frames=0;

  //
  // Load MAD
  //
  if(!LoadMad()) {
    return RDAudioConvert::ErrorFormatNotSupported;
  }

  //
  // Open Destination
  //
  memset(&sf_dst_info,0,sizeof(sf_dst_info));
  sf_dst_info.format=SF_FORMAT_WAV|SF_FORMAT_FLOAT;
  sf_dst_info.channels=wave->getChannels();
  sf_dst_info.samplerate=wave->getSamplesPerSec();
  if((sf_dst=sf_open(dstfile,SFM_WRITE,&sf_dst_info))==NULL) {
    return RDAudioConvert::ErrorNoDestination;
  }
  sf_command(sf_dst,SFC_SET_NORM_DOUBLE,NULL,SF_FALSE);

  //
  // Initialize Decoder
  //
  mad_stream_init(&mad_stream);
  mad_frame_init(&mad_frame);
  mad_synth_init(&mad_synth);
  fsize=144*wave->getHeadBitRate()/wave->getSamplesPerSec();

  //
  // Decode
  //
  if(conv_start_point>0) {
    start=(double)conv_start_point*(double)wave->getSamplesPerSec()/1000.0;
  }
  if(conv_end_point>=0) {
    end=(double)conv_end_point*(double)wave->getSamplesPerSec()/1000.0;
  }
  while((n=wave->readWave(buffer+left_over,fsize))>0) {
    if((buffer[left_over]==0xff)&&(buffer[2+left_over]&0x02)!=0) {
       n+=wave->readWave(buffer+left_over+n,1);  // Padding slot
    }
    mad_stream_buffer(&mad_stream,buffer,n+left_over);
    //printf("mad err: %d\n",mad_stream.error);
   while(1) {

      int thiserr=mad_frame_decode(&mad_frame,&mad_stream);
      if(thiserr!=0) {
	if(!MAD_RECOVERABLE(mad_stream.error))
	  break;
	else
	  continue;
      }

      //printf("decoding...\n");
      mad_synth_frame(&mad_synth,&mad_frame);
      for(int i=0;i<mad_synth.pcm.length;i++) {
	for(int j=0;j<mad_synth.pcm.channels;j++) {
	  sf_buffer[i*mad_synth.pcm.channels+j]=
	    (float)mad_f_todouble(mad_synth.pcm.samples[j][i]);
	}
      }
      if(frames>=start) {
	if((frames+mad_synth.pcm.length)<end) {    // Write entire buffer 
	  UpdatePeak(sf_buffer,mad_synth.pcm.length*wave->getChannels());
	  sf_writef_float(sf_dst,sf_buffer,mad_synth.pcm.length);
	}
	else {
	  if(frames<(frames+mad_synth.pcm.length)) {  // Write start of buffer
	    UpdatePeak(sf_buffer,
		       (frames+mad_synth.pcm.length-end)*wave->getChannels());
	    sf_writef_float(sf_dst,sf_buffer,frames+mad_synth.pcm.length-end);
	    //
	    // Done -- no need to decode the rest
	    //
	    mad_synth_finish(&mad_synth);
	    mad_frame_finish(&mad_frame);
	    mad_stream_finish(&mad_stream);
	    wave->closeWave();
	    sf_close(sf_dst);
	    return RDAudioConvert::ErrorOk;
	  }
	}
      }
      else {
	int diff=frames+mad_synth.pcm.length-start;
	if(diff>0) {   // Write end of buffer
	  UpdatePeak(sf_buffer+diff,
		     (mad_synth.pcm.length-diff)*wave->getChannels());
	  sf_writef_float(sf_dst,sf_buffer+diff,mad_synth.pcm.length-diff);
	}
      }
      frames+=mad_synth.pcm.length;

    }
    left_over=mad_stream.bufend-mad_stream.next_frame;

    // Prevent buffer overflow on malformed files.
    // The amount checked for should match the maximum amount that may be read
    // by the next top-of-loop wave->readWave call.
    if(left_over + fsize + 1 > STAGE1BUFSIZE)
      return RDAudioConvert::ErrorFormatError;

    memmove(buffer,mad_stream.next_frame,left_over);

  }
  memset(buffer+left_over,0,MAD_BUFFER_GUARD);
  mad_stream_buffer(&mad_stream,buffer,MAD_BUFFER_GUARD+left_over);
  if(mad_frame_decode(&mad_frame,&mad_stream)==0) {
    mad_synth_frame(&mad_synth,&mad_frame);
    for(int i=0;i<mad_synth.pcm.length;i++) {
      for(int j=0;j<mad_synth.pcm.channels;j++) {
	sf_buffer[i*mad_synth.pcm.channels+j]=
	  (float)mad_f_todouble(mad_synth.pcm.samples[j][i]);
      }
    }
    UpdatePeak(sf_buffer,mad_synth.pcm.length*wave->getChannels());
    sf_writef_float(sf_dst,sf_buffer,mad_synth.pcm.length);
  }

  //
  // Clean Up
  //
  mad_synth_finish(&mad_synth);
  mad_frame_finish(&mad_frame);
  mad_stream_finish(&mad_stream);
  wave->closeWave();

  sf_close(sf_dst);

  return RDAudioConvert::ErrorOk;
#else
  return RDAudioConvert::ErrorFormatNotSupported;
#endif  // HAVE_MAD
}


RDAudioConvert::ErrorCode RDAudioConvert::Stage1SndFile(const QString &dstfile,
							SNDFILE *sf_src,
							SF_INFO *sf_src_info)
{
  SNDFILE *sf_dst=NULL;
  SF_INFO sf_dst_info;
  sf_count_t start=0;
  sf_count_t end=sf_src_info->frames;

  //
  // Open Destination
  //
  sf_dst_info=*sf_src_info;
  sf_dst_info.format=SF_FORMAT_WAV|SF_FORMAT_FLOAT;
  if((sf_dst=sf_open(dstfile,SFM_WRITE,&sf_dst_info))==NULL) {
    return RDAudioConvert::ErrorNoDestination;
  }

  //
  // Transfer Data
  //
  sf_count_t buffer_size=2048/sf_src_info->channels;
  float *buffer=new float[2048];
  sf_count_t n=0;
  if(conv_start_point>0) {
    start=sf_seek(sf_src,(double)conv_start_point*
		  (double)sf_src_info->samplerate/1000.0,SEEK_SET);
  }
  if(conv_end_point>=0) {
    end=(double)conv_end_point*(double)sf_src_info->samplerate/1000.0;
  }
  while((n=sf_readf_float(sf_src,buffer,buffer_size))>0) {
    UpdatePeak(buffer,n*sf_src_info->channels);
    sf_writef_float(sf_dst,buffer,n);
    start+=n;
    if((end-start)<buffer_size) {
      buffer_size=end-start;
    }
  }
  delete buffer;
  sf_close(sf_dst);

  return RDAudioConvert::ErrorOk;
}


RDAudioConvert::ErrorCode RDAudioConvert::Stage2Convert(const QString &srcfile,
							const QString &dstfile)
{
  soundtouch::SoundTouch *st_conv=NULL;
  SNDFILE *src_sf=NULL;
  SNDFILE *dst_sf=NULL;
  SF_INFO src_info;
  SF_INFO dst_info;
  SRC_STATE *src_state=NULL;
  SRC_DATA src_data;
  float *pcm[3]={NULL,NULL,NULL};
  bool free_pcm[3]={false,false,false};
  int err;
  sf_count_t n;
  float ratio=1.0;

  //
  // Open Files
  //
  memset(&src_info,0,sizeof(src_info));
  if((src_sf=sf_open(srcfile,SFM_READ,&src_info))==NULL) {
    return RDAudioConvert::ErrorInternal;
  }
  sf_command(src_sf,SFC_SET_NORM_FLOAT,NULL,SF_FALSE);
  sf_command(dst_sf,SFC_SET_CLIPPING,NULL,SF_TRUE);
  memset(&dst_info,0,sizeof(dst_info));
  dst_info.format=SF_FORMAT_WAV|SF_FORMAT_PCM_32;
  dst_info.channels=conv_settings->channels();
  dst_info.samplerate=conv_settings->sampleRate();
  if((dst_sf=sf_open(dstfile,SFM_WRITE,&dst_info))==NULL) {
    sf_close(src_sf);
    return RDAudioConvert::ErrorInternal;
  }

  //
  // Allocate Buffers
  //
  pcm[0]=new float[STAGE2_BUFFER_SIZE];
  free_pcm[0]=true;
  if(dst_info.samplerate!=src_info.samplerate) {
    pcm[1]=new float[STAGE2_BUFFER_SIZE];
    free_pcm[1]=true;
    if(dst_info.channels!=src_info.channels) {
      pcm[2]=new float[STAGE2_BUFFER_SIZE];
      free_pcm[2]=true;
    }
    else {
      pcm[2]=pcm[1];
    }
  }
  else {
    pcm[1]=pcm[0];
    if(dst_info.channels!=src_info.channels) {
      pcm[2]=new float[STAGE2_BUFFER_SIZE];
      free_pcm[2]=true;
    }
    else {
      pcm[2]=pcm[0];
    }
  }  


  //
  // Initialize Rate Converter
  //
  if(dst_info.samplerate!=src_info.samplerate) {
    if((src_state=src_new(conv_src_converter,src_info.channels,&err))==NULL) {
      sf_close(src_sf);
      sf_close(dst_sf);
      return RDAudioConvert::ErrorInternal;
    }
    memset(&src_data,0,sizeof(src_data));
    src_data.src_ratio=(double)dst_info.samplerate/(double)src_info.samplerate;
    src_data.data_in=pcm[0];
    src_data.data_out=pcm[1];
    src_data.output_frames=STAGE2_XFER_SIZE*dst_info.samplerate/
      src_info.samplerate+src_info.channels;
  }

  //
  // Initialize Speed Converter
  //
  if(conv_speed_ratio!=1.0) {
    st_conv=new soundtouch::SoundTouch();
    st_conv->setTempo(conv_speed_ratio);
    st_conv->setSampleRate(dst_info.samplerate);
    st_conv->setChannels(dst_info.channels);
  }

  //
  // Calculate Gain Ratio
  //
  if(conv_settings->normalizationLevel()!=0) {
    float gain=
      (float)conv_settings->normalizationLevel()-20.0*log10f(conv_peak_sample);
    ratio=exp10f(gain/20.0);
  }

  //
  // Convert
  //
  while((n=sf_readf_float(src_sf,pcm[0],STAGE2_XFER_SIZE))>0) {

    //
    // Levels
    //
    if(ratio!=1.0) {
      for(unsigned i=0;i<(n*src_info.channels);i++) {
	pcm[0][i]=ratio*pcm[0][i];
      }
    }

    //
    // Sample Rate
    //
    if(src_state!=NULL) {
      src_data.input_frames=n;
      if((err=src_process(src_state,&src_data))!=0) {
	fprintf(stderr,"SRC Error: %s\n",src_strerror(err));
	return RDAudioConvert::ErrorInternal;
      }
      n=src_data.output_frames_gen;
    }

    //
    // Channelization
    //
    switch(src_info.channels) {
    case 1:
      switch(dst_info.channels) {
      case 1:  // Nothing to do
	break;

      case 2:
	for(unsigned i=0;i<n;i++) {
	  pcm[2][2*i]=pcm[1][i];
	  pcm[2][2*i+1]=pcm[1][i];
	}
	break;
      }
      break;

    case 2:
      switch(dst_info.channels) {
      case 1:
	for(unsigned i=0;i<n;i++) {
	  pcm[2][i]=(pcm[1][2*i]+pcm[1][2*i+1])/2;
	}
	break;

      case 2:  // Nothing to do
	break;
      }
      break;
    }

    //
    // Speed
    //
    if(st_conv!=NULL) {
      st_conv->putSamples((soundtouch::SAMPLETYPE *)pcm[2],n);
      n=st_conv->receiveSamples((soundtouch::SAMPLETYPE *)pcm[2],STAGE2_BUFFER_SIZE/dst_info.channels);
    }

    //
    // Write Output
    //
    if(sf_writef_float(dst_sf,pcm[2],n)!=n) {
      for(unsigned i=0;i<3;i++) {
	if(free_pcm[i]) {
	  delete pcm[i];
	}
      }
      if(src_state!=NULL) {
	src_delete(src_state);
      }
      sf_close(src_sf);
      sf_close(dst_sf);
      return RDAudioConvert::ErrorNoSpace;
    }
  }

  //
  // Finish Up Speed Conversion
  //
  if(st_conv!=NULL) {
    st_conv->flush();
    while((n=st_conv->
	   receiveSamples((soundtouch::SAMPLETYPE *)pcm[2],
			  STAGE2_BUFFER_SIZE/dst_info.channels))>0) {
      if(sf_writef_float(dst_sf,pcm[2],n)!=n) {
	for(unsigned i=0;i<3;i++) {
	  if(free_pcm[i]) {
	    delete pcm[i];
	  }
	}
	if(src_state!=NULL) {
	  src_delete(src_state);
	}
	sf_close(src_sf);
	sf_close(dst_sf);
	return RDAudioConvert::ErrorNoSpace;
      }
    }
    delete st_conv;
  }

  //
  // Clean Up
  //
  for(unsigned i=0;i<3;i++) {
    if(free_pcm[i]) {
      delete pcm[i];
    }
  }
  if(src_state!=NULL) {
    src_delete(src_state);
  }
  sf_close(src_sf);
  sf_close(dst_sf);

  return RDAudioConvert::ErrorOk;
}


RDAudioConvert::ErrorCode RDAudioConvert::Stage3Convert(const QString &srcfile,
							const QString &dstfile)
{
  SNDFILE *src_sf=NULL;
  SF_INFO src_sf_info;
  RDAudioConvert::ErrorCode ret;

  //
  // Open Source File
  //
  if((src_sf=sf_open(srcfile,SFM_READ,&src_sf_info))==NULL) {
    return RDAudioConvert::ErrorInternal;
  }

  switch(conv_settings->format()) {
  case RDSettings::Pcm16:
    ret=Stage3Pcm16(src_sf,&src_sf_info,dstfile);
    break;

  case RDSettings::MpegL2:
    ret=Stage3Layer2(src_sf,&src_sf_info,dstfile);
    break;

  case RDSettings::MpegL2Wav:
    ret=Stage3Layer2Wav(src_sf,&src_sf_info,dstfile);
    break;

  case RDSettings::MpegL3:
    ret=Stage3Layer3(src_sf,&src_sf_info,dstfile);
    break;

  case RDSettings::Flac:
    ret=Stage3Flac(src_sf,&src_sf_info,dstfile);
    break;

  case RDSettings::OggVorbis:
    ret=Stage3Vorbis(src_sf,&src_sf_info,dstfile);
    break;

  case RDSettings::MpegL1:
  default:
    ret=RDAudioConvert::ErrorInvalidSettings;
  }

  sf_close(src_sf);
  return ret;
}


RDAudioConvert::ErrorCode RDAudioConvert::Stage3Flac(SNDFILE *src_sf,
						     SF_INFO *src_sf_info,
						     const QString &dstfile)
{
#ifdef HAVE_FLAC
  sf_count_t n;
  int32_t *pcm;

  //
  // Initialize Encoder
  //
  FLAC::Encoder::File *flac=new FLAC::Encoder::File();
  flac->set_channels(src_sf_info->channels);
  flac->set_bits_per_sample(16);  // FIXME: Should vary by input file
  flac->set_sample_rate(src_sf_info->samplerate);
  //flac->set_compression_level(8);
  flac->set_blocksize(0);
  unlink(dstfile);
  /*
   * FLAC <1.2.x
   *
  flac->set_filename(dstfile.ascii());
  switch(flac->init()) {
  case 0:
  break;

  default:
  delete flac;
  return RDAudioConvert::ErrorInternal;
  }
  */
  /*
   * FLAC 1.2.x
   */
  switch(flac->init(dstfile.ascii())) {
  case FLAC__STREAM_ENCODER_INIT_STATUS_OK:
    break;

  case FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_NUMBER_OF_CHANNELS:
  case FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_BITS_PER_SAMPLE:
  case FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_SAMPLE_RATE:
    delete flac;
    return RDAudioConvert::ErrorInvalidSettings;

  case FLAC__STREAM_ENCODER_INIT_STATUS_ENCODER_ERROR:
  case FLAC__STREAM_ENCODER_INIT_STATUS_UNSUPPORTED_CONTAINER:
  case FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_MAX_LPC_ORDER:
  case FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_QLP_COEFF_PRECISION:
  case FLAC__STREAM_ENCODER_INIT_STATUS_BLOCK_SIZE_TOO_SMALL_FOR_LPC_ORDER:
  case FLAC__STREAM_ENCODER_INIT_STATUS_NOT_STREAMABLE:
  case FLAC__STREAM_ENCODER_INIT_STATUS_INVALID_METADATA:
  default:
    delete flac;
    return RDAudioConvert::ErrorInternal;
  }

  pcm=new int32_t[2048*src_sf_info->channels];

  //
  // Encode
  //
  while((n=sf_readf_int(src_sf,pcm,2048))>0) {
    for(unsigned i=0;i<(n*src_sf_info->channels);i++) {
      pcm[i]=pcm[i]>>16;
    }
    flac->process_interleaved(pcm,n);
  }
  flac->finish();

  //
  // Clean Up
  //
  delete pcm;
  delete flac;

  return RDAudioConvert::ErrorOk;
#else
  return RDAudioConvert::ErrorFormatNotSupported;
#endif  // HAVE_FLAC
}


RDAudioConvert::ErrorCode RDAudioConvert::Stage3Vorbis(SNDFILE *src_sf,
						       SF_INFO *src_sf_info,
						       const QString &dstfile)
{
#ifdef HAVE_VORBIS
  ogg_stream_state ogg_stream;
  ogg_page ogg_page;
  ogg_packet header;
  ogg_packet comment;
  ogg_packet codebook;
  ogg_packet ogg_packet;
  vorbis_info vorbis_info;
  vorbis_comment vorbis_comment;
  vorbis_dsp_state vorbis_dsp;
  vorbis_block vorbis_block;
  float *pcm=NULL;
  float **vorbis;
  sf_count_t n;
  int dst_fd=-1;

  //
  // Open Destination File
  //
  unlink(dstfile);
  if((dst_fd=open(dstfile,O_WRONLY|O_CREAT|O_TRUNC,
		  S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH))<0) {
    return RDAudioConvert::ErrorNoDestination;
  } 

  //
  // Initialize the Encoder
  //
  vorbis_info_init(&vorbis_info);
  switch(vorbis_encode_init_vbr(&vorbis_info,src_sf_info->channels,
				src_sf_info->samplerate,
				conv_settings->quality())) {
  case OV_EFAULT:
  default:
    return RDAudioConvert::ErrorInternal;

  case OV_EINVAL:
  case OV_EIMPL:
    return RDAudioConvert::ErrorInvalidSettings;

  case 0:
    break;
  }
  vorbis_comment_init(&vorbis_comment);
  // Metadata stuff goes here...
  vorbis_analysis_init(&vorbis_dsp,&vorbis_info);
  vorbis_block_init(&vorbis_dsp,&vorbis_block);
  vorbis_analysis_headerout(&vorbis_dsp,&vorbis_comment,
			    &header,&comment,&codebook);
  ogg_stream_init(&ogg_stream,rand());
  ogg_stream_packetin(&ogg_stream,&header);
  ogg_stream_packetin(&ogg_stream,&comment);
  ogg_stream_packetin(&ogg_stream,&codebook);
  pcm=new float[2048*src_sf_info->channels];

  //
  // Encode
  //
  while((n=sf_readf_float(src_sf,pcm,2048))>0) {
    vorbis=vorbis_analysis_buffer(&vorbis_dsp,n);
    for(unsigned i=0;i<n;i++) {
      for(int j=0;j<src_sf_info->channels;j++) {
	vorbis[j][i]=pcm[src_sf_info->channels*i+j];
      }
    }
    vorbis_analysis_wrote(&vorbis_dsp,n);
    while(vorbis_analysis_blockout(&vorbis_dsp,&vorbis_block)>0) {
      vorbis_analysis(&vorbis_block,&ogg_packet);
      ogg_stream_packetin(&ogg_stream,&ogg_packet);
      while(ogg_stream_pageout(&ogg_stream,&ogg_page)!=0) {
	if(write(dst_fd,ogg_page.header,ogg_page.header_len)!=
	   ogg_page.header_len) {
	  ::close(dst_fd);
	  delete pcm;
	  ogg_stream_clear(&ogg_stream);
	  vorbis_comment_clear(&vorbis_comment);
	  vorbis_info_clear(&vorbis_info);
	  return RDAudioConvert::ErrorNoSpace; 
	}
	if(write(dst_fd,ogg_page.body,ogg_page.body_len)!=
	   ogg_page.body_len) {
	  ::close(dst_fd);
	  delete pcm;
	  ogg_stream_clear(&ogg_stream);
	  vorbis_comment_clear(&vorbis_comment);
	  vorbis_info_clear(&vorbis_info);
	  return RDAudioConvert::ErrorNoSpace; 
	}
      }
    }
    while(ogg_stream_flush(&ogg_stream,&ogg_page)!=0) {
      if(write(dst_fd,ogg_page.header,ogg_page.header_len)!=
	 ogg_page.header_len) {
	  ::close(dst_fd);
	  delete pcm;
	  ogg_stream_clear(&ogg_stream);
	  vorbis_comment_clear(&vorbis_comment);
	  vorbis_info_clear(&vorbis_info);
	  return RDAudioConvert::ErrorNoSpace; 
	}
      }
    if(write(dst_fd,ogg_page.body,ogg_page.body_len)!=
       ogg_page.body_len) {
      ::close(dst_fd);
      delete pcm;
      ogg_stream_clear(&ogg_stream);
      vorbis_comment_clear(&vorbis_comment);
      vorbis_info_clear(&vorbis_info);
      return RDAudioConvert::ErrorNoSpace; 
    }
  }
  vorbis=vorbis_analysis_buffer(&vorbis_dsp,0);
  vorbis_analysis_wrote(&vorbis_dsp,0);
  while(vorbis_analysis_blockout(&vorbis_dsp,&vorbis_block)>0) {
    vorbis_analysis(&vorbis_block,&ogg_packet);
    ogg_stream_packetin(&ogg_stream,&ogg_packet);
    while(ogg_stream_pageout(&ogg_stream,&ogg_page)!=0) {
      if(write(dst_fd,ogg_page.header,ogg_page.header_len)!=
	 ogg_page.header_len) {
	::close(dst_fd);
	delete pcm;
	ogg_stream_clear(&ogg_stream);
	vorbis_comment_clear(&vorbis_comment);
	vorbis_info_clear(&vorbis_info);
	return RDAudioConvert::ErrorNoSpace; 
      }
      if(write(dst_fd,ogg_page.body,ogg_page.body_len)!=
	 ogg_page.body_len) {
	::close(dst_fd);
	delete pcm;
	ogg_stream_clear(&ogg_stream);
	vorbis_comment_clear(&vorbis_comment);
	vorbis_info_clear(&vorbis_info);
	return RDAudioConvert::ErrorNoSpace; 
      }
    }
  }
  while(ogg_stream_flush(&ogg_stream,&ogg_page)!=0) {
    if(write(dst_fd,ogg_page.header,ogg_page.header_len)!=
       ogg_page.header_len) {
      ::close(dst_fd);
      delete pcm;
      ogg_stream_clear(&ogg_stream);
      vorbis_comment_clear(&vorbis_comment);
      vorbis_info_clear(&vorbis_info);
      return RDAudioConvert::ErrorNoSpace; 
    }
    if(write(dst_fd,ogg_page.body,ogg_page.body_len)!=
       ogg_page.body_len) {
      ::close(dst_fd);
      delete pcm;
      ogg_stream_clear(&ogg_stream);
      vorbis_comment_clear(&vorbis_comment);
      vorbis_info_clear(&vorbis_info);
      return RDAudioConvert::ErrorNoSpace; 
    }
  }

  //
  // Clean Up
  //
  ::close(dst_fd);
  delete pcm;
  ogg_stream_clear(&ogg_stream);
  vorbis_comment_clear(&vorbis_comment);
  vorbis_info_clear(&vorbis_info);

  return RDAudioConvert::ErrorOk;
#else
  return RDAudioConvert::ErrorFormatNotSupported;
#endif  // HAVE_VORBIS
}


RDAudioConvert::ErrorCode RDAudioConvert::Stage3Layer3(SNDFILE *src_sf,
						       SF_INFO *src_sf_info,
						       const QString &dstfile)
{
#ifdef HAVE_LAME
  MPEG_mode mpeg_mode=STEREO;
  lame_global_flags *lameopts=NULL;
  int dst_fd=-1;
  int16_t pcm[2304];
  unsigned char mpeg[2048];
  sf_count_t n;
  sf_count_t s;

  //
  // Load LAME
  //
  if(!LoadLame()) {
    return RDAudioConvert::ErrorFormatNotSupported;
  }

  //
  // Determine MPEG Mode
  //
  switch(src_sf_info->channels) {
  case 1:
    mpeg_mode=MONO;
    break;

  case 2:
    mpeg_mode=STEREO;    
    break;

  default:
    return RDAudioConvert::ErrorInvalidSettings;
  }

  //
  // Open Destination File
  //
  unlink(dstfile);
  if((dst_fd=open(dstfile,O_WRONLY|O_CREAT|O_TRUNC,
		  S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH))<0) {
    return RDAudioConvert::ErrorNoDestination;
  } 

  //
  // Initialize Encoder
  //
  if((lameopts=lame_init())==NULL) {
    lame_close(lameopts);
    ::close(dst_fd);
    return RDAudioConvert::ErrorInternal;
  }
  lame_set_mode(lameopts,mpeg_mode);
  lame_set_num_channels(lameopts,src_sf_info->channels);
  lame_set_in_samplerate(lameopts,src_sf_info->samplerate);
  lame_set_out_samplerate(lameopts,src_sf_info->samplerate);
  lame_set_brate(lameopts,conv_settings->bitRate()/1000);
  lame_set_bWriteVbrTag(lameopts,0);
  if(lame_init_params(lameopts)!=0) {
    lame_close(lameopts);
    ::close(dst_fd);
    return RDAudioConvert::ErrorInvalidSettings;
  }

  //
  // Encode
  //
  if(src_sf_info->channels==2) {
    while((n=sf_readf_short(src_sf,pcm,1152))>0) {
      if((s=lame_encode_buffer_interleaved(lameopts,pcm,n,mpeg,2048))>=0) {
	if(write(dst_fd,mpeg,s)!=s) {
	  lame_close(lameopts);
	  ::close(dst_fd);
	  return RDAudioConvert::ErrorNoSpace;
	}
      }
    }
  }
  else {
    while((n=sf_readf_short(src_sf,pcm,1152))>0) {
      if((s=lame_encode_buffer(lameopts,pcm,NULL,n,mpeg,2048))>=0) {
	if(write(dst_fd,mpeg,s)!=s) {
	  lame_close(lameopts);
	  ::close(dst_fd);
	  return RDAudioConvert::ErrorNoSpace;
	}
      }
    }
  }
  if((s=lame_encode_flush(lameopts,mpeg,2048))>=0) {
    if(write(dst_fd,mpeg,s)!=s) {
      lame_close(lameopts);
      ::close(dst_fd);
      return RDAudioConvert::ErrorNoSpace;
    }
  }

  //
  // Clean Up
  //
  lame_close(lameopts);
  ::close(dst_fd);

  //
  // Apply Metadata
  //
  if(conv_dst_wavedata!=NULL) {
    ApplyId3Tag(dstfile,conv_dst_wavedata);
  }

  return RDAudioConvert::ErrorOk;
#else
  return RDAudioConvert::ErrorFormatNotSupported;
#endif  // HAVE_LAME
}


RDAudioConvert::ErrorCode RDAudioConvert::Stage3Layer2Wav(SNDFILE *src_sf,
							  SF_INFO *src_sf_info,
							  const QString &dstfile)
{
#ifdef HAVE_TWOLAME
  sf_count_t n;
  ssize_t s;
  RDWaveFile *wave=NULL;
  TWOLAME_MPEG_mode mpeg_mode=TWOLAME_STEREO;
  twolame_options *lameopts=NULL;
  float pcm[2304];
  unsigned char mpeg[2048];

  //
  // Load TwoLAME
  //
  if(!LoadTwoLame()) {
    return RDAudioConvert::ErrorFormatNotSupported;
  }

  //
  // Determine MPEG Mode
  //
  switch(src_sf_info->channels) {
  case 1:
    mpeg_mode=TWOLAME_MONO;
    break;

  case 2:
    mpeg_mode=TWOLAME_STEREO;    
    break;

  default:
    return RDAudioConvert::ErrorInvalidSettings;
  }

  //
  // Open Destination File
  //
  wave=new RDWaveFile(dstfile);
  wave->setFormatTag(WAVE_FORMAT_MPEG);
  wave->setChannels(src_sf_info->channels);
  switch(src_sf_info->channels) {
  case 1:
    wave->setHeadMode(ACM_MPEG_SINGLECHANNEL);
    break;

  case 2:
    wave->setHeadMode(ACM_MPEG_STEREO);
    break;
  }
  wave->setSamplesPerSec(src_sf_info->samplerate);
  wave->setHeadLayer(2);
  wave->setHeadBitRate(conv_settings->bitRate());
  wave->setBextChunk(true);
  wave->setMextChunk(true);
  wave->setCartChunk(conv_dst_wavedata!=NULL);
  wave->setLevlChunk(true);
  unlink(dstfile);
  if(!wave->createWave(conv_dst_wavedata)) {
    return RDAudioConvert::ErrorNoDestination;
  }

  //
  // Initialize Encoder
  //
  if((lameopts=twolame_init())==NULL) {
    wave->closeWave();
    return RDAudioConvert::ErrorInternal;
  }
  twolame_set_mode(lameopts,mpeg_mode);
  twolame_set_num_channels(lameopts,src_sf_info->channels);
  twolame_set_in_samplerate(lameopts,src_sf_info->samplerate);
  twolame_set_out_samplerate(lameopts,src_sf_info->samplerate);
  twolame_set_bitrate(lameopts,conv_settings->bitRate()/1000);
  twolame_set_energy_levels(lameopts,1);
  if(twolame_init_params(lameopts)!=0) {
    twolame_close(&lameopts);
    wave->closeWave();
    return RDAudioConvert::ErrorInvalidSettings;
  }

  //
  // Encode
  //
  while((n=sf_readf_float(src_sf,pcm,1152))>0) {
    if((s=twolame_encode_buffer_float32_interleaved(lameopts,
						    pcm,n,mpeg,2048))>=0) {
      if(wave->writeWave(mpeg,s)!=s) {
	twolame_close(&lameopts);
	wave->closeWave(src_sf_info->frames);
	return RDAudioConvert::ErrorNoSpace;
      }
    }
    else {
      fprintf(stderr,"TwoLAME encode error\n");
    }
  }
  if((s=twolame_encode_flush(lameopts,mpeg,2048))>=0) {
    if(wave->writeWave(mpeg,s)!=s) {
      twolame_close(&lameopts);
      wave->closeWave(src_sf_info->frames);
      return RDAudioConvert::ErrorNoSpace;
    }
  }
  else {
    fprintf(stderr,"TwoLAME encode error\n");
  }

  //
  // Clean Up
  //
  twolame_close(&lameopts);
  wave->closeWave(src_sf_info->frames);
  return RDAudioConvert::ErrorOk;
#else
  return RDAudioConvert::ErrorFormatNotSupported;
#endif  // HAVE_TWOLAME
}


RDAudioConvert::ErrorCode RDAudioConvert::Stage3Layer2(SNDFILE *src_sf,
						       SF_INFO *src_sf_info,
						       const QString &dstfile)
{
#ifdef HAVE_TWOLAME
  sf_count_t n;
  ssize_t s;
  int dst_fd=-1;
  TWOLAME_MPEG_mode mpeg_mode=TWOLAME_STEREO;
  twolame_options *lameopts=NULL;
  float pcm[2304];
  unsigned char mpeg[2048];

  if(!LoadTwoLame()) {
    return RDAudioConvert::ErrorFormatNotSupported;
  }
  if((conv_settings->bitRate()>192000)&&(src_sf_info->channels<2)) {
    return RDAudioConvert::ErrorInvalidSettings;
  }

  //
  // Determine MPEG Mode
  //
  switch(src_sf_info->channels) {
  case 1:
    mpeg_mode=TWOLAME_MONO;
    break;

  case 2:
    mpeg_mode=TWOLAME_STEREO;    
    break;

  default:
    return RDAudioConvert::ErrorInvalidSettings;
  }

  //
  // Open Destination File
  //
  unlink(dstfile);
  if((dst_fd=open(dstfile,O_WRONLY|O_CREAT|O_TRUNC,
		  S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH))<0) {
    return RDAudioConvert::ErrorNoDestination;
  } 

  //
  // Initialize Encoder
  //
  if((lameopts=twolame_init())==NULL) {
    ::close(dst_fd);
    return RDAudioConvert::ErrorInternal;
  }
  twolame_set_mode(lameopts,mpeg_mode);
  twolame_set_num_channels(lameopts,src_sf_info->channels);
  twolame_set_in_samplerate(lameopts,src_sf_info->samplerate);
  twolame_set_out_samplerate(lameopts,src_sf_info->samplerate);
  twolame_set_bitrate(lameopts,conv_settings->bitRate()/1000);
  if(twolame_init_params(lameopts)!=0) {
    twolame_close(&lameopts);
    ::close(dst_fd);
    return RDAudioConvert::ErrorInvalidSettings;
  }

  //
  // Encode
  //
  while((n=sf_readf_float(src_sf,pcm,1152))>0) {
    if((s=twolame_encode_buffer_float32_interleaved(lameopts,
						    pcm,n,mpeg,2048))>=0) {
      if(write(dst_fd,mpeg,s)!=s) {
	twolame_close(&lameopts);
	::close(dst_fd);
	return RDAudioConvert::ErrorNoSpace;
      }
    }
    else {
      fprintf(stderr,"TwoLAME encode error\n");
    }
  }
  if((s=twolame_encode_flush(lameopts,mpeg,2048))>=0) {
    if(write(dst_fd,mpeg,s)!=s) {
      twolame_close(&lameopts);
      ::close(dst_fd);
      return RDAudioConvert::ErrorNoSpace;
    }
  }
  else {
    fprintf(stderr,"TwoLAME encode error\n");
  }

  //
  // Clean Up
  //
  twolame_close(&lameopts);
  ::close(dst_fd);

  //
  // Apply Metadata
  //
  if(conv_dst_wavedata!=NULL) {
    ApplyId3Tag(dstfile,conv_dst_wavedata);
  }

  return RDAudioConvert::ErrorOk;
#else
  return RDAudioConvert::ErrorFormatNotSupported;
#endif  // HAVE_TWOLAME
}


RDAudioConvert::ErrorCode RDAudioConvert::Stage3Pcm16(SNDFILE *src_sf,
						      SF_INFO *src_sf_info,
						      const QString &dstfile)
{
  short *sf_buffer=NULL;
  ssize_t n;

  RDWaveFile *wave=new RDWaveFile(dstfile);
  wave->setFormatTag(WAVE_FORMAT_PCM);
  wave->setEncoding(RDWaveFile::Signed16Int);
  wave->setChannels(src_sf_info->channels);
  wave->setSamplesPerSec(src_sf_info->samplerate);
  wave->setBitsPerSample(16);
  wave->setBextChunk(true);
  wave->setCartChunk(conv_dst_wavedata!=NULL);
  if((conv_dst_wavedata!=NULL)&&(conv_settings->normalizationLevel()!=0)) {
    wave->setCartLevelRef(32768*
	      exp10((double)conv_settings->normalizationLevel()/20.0));
  }
  wave->setLevlChunk(true);
  sf_buffer=new int16_t[2048*src_sf_info->channels];
  unlink(dstfile);
  if(!wave->createWave(conv_dst_wavedata)) {
    return RDAudioConvert::ErrorNoDestination;
  }
  while((n=sf_readf_short(src_sf,sf_buffer,2048))>0) {
    if((unsigned)wave->
       writeWave(sf_buffer,n*sizeof(short)*src_sf_info->channels)!=
       (n*sizeof(short)*src_sf_info->channels)) {
      delete sf_buffer;
      wave->closeWave();
      delete wave;
      return RDAudioConvert::ErrorNoSpace;
    }
  }
  delete sf_buffer;
  wave->closeWave();
  delete wave;
  return RDAudioConvert::ErrorOk;
}


void RDAudioConvert::ApplyId3Tag(const QString &filename,RDWaveData *wavedata)
{
  ID3_Tag *tag=new ID3_Tag(filename);
  ID3_Frame *frame=new ID3_Frame(ID3FID_TITLE);
  frame->GetField(ID3FN_TEXT)->Set(wavedata->title());
  tag->AddNewFrame(frame);

  if(wavedata->beatsPerMinute()>0) {
    frame=new ID3_Frame(ID3FID_BPM);
    frame->GetField(ID3FN_TEXT)->
      Set(QString().sprintf("%d",wavedata->beatsPerMinute()));
    tag->AddNewFrame(frame);
  }
  if(!wavedata->album().isEmpty()) {
    frame=new ID3_Frame(ID3FID_ALBUM);
    frame->GetField(ID3FN_TEXT)->Set(wavedata->album());
    tag->AddNewFrame(frame);
  }
  if(!wavedata->composer().isEmpty()) {
    frame=new ID3_Frame(ID3FID_COMPOSER);
    frame->GetField(ID3FN_TEXT)->Set(wavedata->composer());
    tag->AddNewFrame(frame);
  }
  if(!wavedata->copyrightNotice().isEmpty()) {
    frame=new ID3_Frame(ID3FID_COPYRIGHT);
    frame->GetField(ID3FN_TEXT)->Set(wavedata->copyrightNotice());
    tag->AddNewFrame(frame);
  }
  if(!wavedata->artist().isEmpty()) {
    frame=new ID3_Frame(ID3FID_LEADARTIST);
    frame->GetField(ID3FN_TEXT)->Set(wavedata->artist());
    tag->AddNewFrame(frame);
  }
  if(!wavedata->publisher().isEmpty()) {
    frame=new ID3_Frame(ID3FID_PUBLISHER);
    frame->GetField(ID3FN_TEXT)->Set(wavedata->publisher());
    tag->AddNewFrame(frame);
  }
  if(!wavedata->isrc().isEmpty()) {
    frame=new ID3_Frame(ID3FID_ISRC);
    frame->GetField(ID3FN_TEXT)->Set(wavedata->isrc());
    tag->AddNewFrame(frame);
  }
  if(wavedata->releaseYear()>0) {
    frame=new ID3_Frame(ID3FID_YEAR);
    frame->GetField(ID3FN_TEXT)->
      Set(QString().sprintf("%d",wavedata->releaseYear()));
    tag->AddNewFrame(frame);
  }
  tag->Update();
  delete tag;
}


void RDAudioConvert::UpdatePeak(const float data[],ssize_t len)
{
  float peak;

  for(ssize_t i=0;i<len;i++) {
    if((peak=fabsf(data[i]))>conv_peak_sample) {
      conv_peak_sample=peak;
    }
  }
}


void RDAudioConvert::UpdatePeak(const double data[],ssize_t len)
{
  float peak;

  for(ssize_t i=0;i<len;i++) {
    if((peak=(float)fabsf(data[i]))>conv_peak_sample) {
      conv_peak_sample=peak;
    }
  }
}


bool RDAudioConvert::LoadMad()
{
#ifdef HAVE_MAD
  if(conv_mad_handle==NULL) {
    return false;
  }
  *(void **)(&mad_stream_init)=
    dlsym(conv_mad_handle,"mad_stream_init");
  *(void **)(&mad_frame_init)=
    dlsym(conv_mad_handle,"mad_frame_init");
  *(void **)(&mad_synth_init)=
    dlsym(conv_mad_handle,"mad_synth_init");
  *(void **)(&mad_stream_buffer)=
    dlsym(conv_mad_handle,"mad_stream_buffer");
  *(void **)(&mad_frame_decode)=
    dlsym(conv_mad_handle,"mad_frame_decode");
  *(void **)(&mad_synth_frame)=
    dlsym(conv_mad_handle,"mad_synth_frame");
  *(void **)(&mad_frame_finish)=
    dlsym(conv_mad_handle,"mad_frame_finish");
  *(void **)(&mad_stream_finish)=
    dlsym(conv_mad_handle,"mad_stream_finish");
  return true;
#else
  return false;
#endif  // HAVE_MAD
}


bool RDAudioConvert::LoadTwoLame()
{
#ifdef HAVE_TWOLAME
  if(conv_twolame_handle==NULL) {
    return false;
  }
  *(void **)(&twolame_init)=dlsym(conv_twolame_handle,"twolame_init");
  *(void **)(&twolame_set_mode)=dlsym(conv_twolame_handle,"twolame_set_mode");
  *(void **)(&twolame_set_num_channels)=
    dlsym(conv_twolame_handle,"twolame_set_num_channels");
  *(void **)(&twolame_set_in_samplerate)=
    dlsym(conv_twolame_handle,"twolame_set_in_samplerate");
  *(void **)(&twolame_set_out_samplerate)=
    dlsym(conv_twolame_handle,"twolame_set_out_samplerate");
  *(void **)(&twolame_set_bitrate)=
    dlsym(conv_twolame_handle,"twolame_set_bitrate");
  *(void **)(&twolame_init_params)=
    dlsym(conv_twolame_handle,"twolame_init_params");
  *(void **)(&twolame_close)=dlsym(conv_twolame_handle,"twolame_close");
  *(void **)(&twolame_encode_buffer_float32_interleaved)=
    dlsym(conv_twolame_handle,"twolame_encode_buffer_float32_interleaved");
  *(void **)(&twolame_encode_flush)=
    dlsym(conv_twolame_handle,"twolame_encode_flush");
  *(void **)(&twolame_set_energy_levels)=
    dlsym(conv_twolame_handle,"twolame_set_energy_levels");
  return true;
#else
  return false;
#endif  // HAVE_TWOLAME
}


bool RDAudioConvert::LoadLame()
{
#ifdef HAVE_LAME
  if(conv_lame_handle==NULL) {
    return false;
  }
  *(void **)(&lame_init)=dlsym(conv_lame_handle,"lame_init");
  *(void **)(&lame_set_mode)=
    dlsym(conv_lame_handle,"lame_set_mode");
  *(void **)(&lame_set_num_channels)=
    dlsym(conv_lame_handle,"lame_set_num_channels");
  *(void **)(&lame_set_in_samplerate)=
    dlsym(conv_lame_handle,"lame_set_in_samplerate");
  *(void **)(&lame_set_out_samplerate)=
    dlsym(conv_lame_handle,"lame_set_out_samplerate");
  *(void **)(&lame_set_brate)=dlsym(conv_lame_handle,"lame_set_brate");
  *(void **)(&lame_init_params)=dlsym(conv_lame_handle,"lame_init_params");
  *(void **)(&lame_close)=dlsym(conv_lame_handle,"lame_close");
  *(void **)(&lame_encode_buffer_interleaved)=
    dlsym(conv_lame_handle,"lame_encode_buffer_interleaved");
  *(void **)(&lame_encode_buffer)=
    dlsym(conv_lame_handle,"lame_encode_buffer");
  *(void **)(&lame_encode_flush)=dlsym(conv_lame_handle,"lame_encode_flush");
  *(void **)(&lame_set_bWriteVbrTag)=
    dlsym(conv_lame_handle,"lame_set_bWriteVbrTag");
  return true;
#else
  return false;
#endif  // HAVE_LAME
}
