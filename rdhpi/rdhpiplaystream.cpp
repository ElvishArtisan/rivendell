//   rdhpiplaystream.cpp
//
//   A class for playing Microsoft WAV file on AudioScience HPI devices.
//
//   (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
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
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <fcntl.h>
#include <qobject.h>
#include <qwidget.h>
#include <qstring.h>
#include <qdatetime.h>

#include <rd.h>
#include <rdapplication.h>
#include "rdhpiplaystream.h"
#include "rdhpisoundcard.h"

#define RDHPIPLAYSTREAM_USE_LOCAL_MUTEX

#ifdef RDHPIPLAYSTREAM_USE_LOCAL_MUTEX
volatile static int stream_mutex[HPI_MAX_ADAPTERS][HPI_MAX_STREAMS]=
  {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
#endif

RDHPIPlayStream::RDHPIPlayStream(RDHPISoundCard *card,QWidget *parent) 
  :QObject(parent),RDWaveFile()
{  
  //  hpi_err_t hpi_err;
  int quan;
  uint16_t type[HPI_MAX_ADAPTERS];
  struct hpi_format fmt;
  uint32_t dma_size=0;

  dma_buffer_size=0;
  sound_card=card;
  card_number=-1;
  stream_number=-1;
  is_open=false;
  playing=false;
  is_paused=false;
  repositioned=false;
  stopping=false;
  samples_skipped=0;
  play_length=0;
  play_speed=1000;
  pitch_can_vary=false;
  rate_can_vary=false;
  stream_state=RDHPIPlayStream::Stopped;
  pdata=NULL;
  restart_transport=false;
  samples_pending=0;
  current_position=0;

  //
  // Get Adapter Indices
  //
#if HPI_VER < 0x00030600
  for(unsigned i=0;i<HPI_MAX_ADAPTERS;i++) {
    card_index[i]=i;
  }
#else
  LogHpi(HPI_SubSysGetNumAdapters(NULL,&quan),__LINE__);
  for(int i=0;i<quan;i++) {
    LogHpi(HPI_SubSysGetAdapter(NULL,i,card_index+i,type+i),__LINE__);
  }
#endif  // HPI_VER

  //
  // Calculate DMA Buffer Size
  //
  memset(&fmt,0,sizeof(fmt));  // Worst case situation
  fmt.dwSampleRate=48000;
  fmt.wChannels=2;
  fmt.wFormat=HPI_FORMAT_PCM32_FLOAT;
  if(LogHpi(HPI_StreamEstimateBufferSize(&fmt,FRAGMENT_TIME,&dma_size),
	    __LINE__)==0) {
    dma_buffer_size=dma_size;
  }

  clock=new QTimer(this);
  connect(clock,SIGNAL(timeout()),this,SLOT(tickClock()));

  play_timer=new QTimer(this);
  connect(play_timer,SIGNAL(timeout()),this,SLOT(pause()));
}


RDHPIPlayStream::~RDHPIPlayStream()
{
  if(is_open) {
    closeWave();
  }
  if(pdata!=NULL) {
    delete pdata;
  }
  delete clock;
  delete play_timer;
}


QString RDHPIPlayStream::errorString(RDHPIPlayStream::Error err)
{
  QString str;

  switch(err) {
  case RDHPIPlayStream::Ok:
    return QString(tr("Ok"));
    break;

  case RDHPIPlayStream::NoFile:
    return QString(tr("No such file or directory"));
    break;

  case RDHPIPlayStream::NoStream:
    return QString(tr("No output stream available"));
    break;

  case RDHPIPlayStream::AlreadyOpen:
    return QString(tr("Stream is already open"));
    break;

  default:
    str=QString(tr("Unknown RDHpiPlayStream Error:"));
    return QString().sprintf("%s %d\n",(const char *)str,err);
    break;
  }
}


bool RDHPIPlayStream::formatSupported(RDWaveFile::Format format)
{
#if HPI_VER < 0x30A00
  HPI_FORMAT hpi_format;
#else
  struct hpi_format hpi_format;
#endif
  hpi_handle_t hostream;
  bool found=false;

  if(card_number<0) {
    return false;
  }
  if(format==RDWaveFile::Vorbis) {
#ifdef HAVE_VORBIS
    return true;
#endif  // HAVE_VORBIS
    return false;
  }
  if(!is_open) {
    for(int i=0;i<sound_card->getCardOutputStreams(card_number);i++) {
      if(LogHpi(HPI_OutStreamOpen(NULL,card_index[card_number],i,&hostream),
		__LINE__)==0) {
	RDApplication::syslog(sound_card->config(),LOG_DEBUG,"buffer_size: %u\n",dma_buffer_size);
	found=true;
	break;
      }
    }
    if(!found) {
      return false;
    }
    if(HPI_OutStreamHostBufferAllocate(NULL,hostream,dma_buffer_size));
  }
  else {
    hostream=hpi_stream;
  }
  switch(format) {
  case RDWaveFile::Pcm16:
    LogHpi(HPI_FormatCreate(&hpi_format,getChannels(),
			    HPI_FORMAT_PCM16_SIGNED,
			    getSamplesPerSec(),getHeadBitRate(),0),__LINE__);
    state=LogHpi(HPI_OutStreamQueryFormat(NULL,hostream,&hpi_format),__LINE__);
    break;

  case RDWaveFile::Pcm24:
    LogHpi(HPI_FormatCreate(&hpi_format,getChannels(),
			    HPI_FORMAT_PCM24_SIGNED,
			    getSamplesPerSec(),getHeadBitRate(),0),__LINE__);
    state=LogHpi(HPI_OutStreamQueryFormat(NULL,hostream,&hpi_format),__LINE__);
    break;

  case RDWaveFile::MpegL1:
    LogHpi(HPI_FormatCreate(&hpi_format,getChannels(),
			    HPI_FORMAT_MPEG_L1,
			    getSamplesPerSec(),getHeadBitRate(),0),__LINE__);
    state=LogHpi(HPI_OutStreamQueryFormat(NULL,hostream,&hpi_format),__LINE__);
    break;

  case RDWaveFile::MpegL2:
    LogHpi(HPI_FormatCreate(&hpi_format,getChannels(),
			    HPI_FORMAT_MPEG_L2,
			    getSamplesPerSec(),getHeadBitRate(),0),__LINE__);
    state=LogHpi(HPI_OutStreamQueryFormat(NULL,hostream,&hpi_format),__LINE__);
    break;

  case RDWaveFile::MpegL3:
    LogHpi(HPI_FormatCreate(&hpi_format,getChannels(),
			    HPI_FORMAT_MPEG_L3,
			    getSamplesPerSec(),getHeadBitRate(),0),__LINE__);
    state=LogHpi(HPI_OutStreamQueryFormat(NULL,hostream,&hpi_format),__LINE__);
    break;

  default:
    state=1;
    break;
  }
  if(!is_open) {
    if(HPI_OutStreamHostBufferFree(NULL,hostream));
    LogHpi(HPI_OutStreamClose(NULL,hostream),__LINE__);
  }
  if(state!=0) {
    return false;
  }
  return true;
}


bool RDHPIPlayStream::formatSupported()
{
  switch(getFormatTag()) {
  case WAVE_FORMAT_PCM:
    switch(getBitsPerSample()) {
    case 8:
      return formatSupported(RDWaveFile::Pcm8);
      break;

    case 16:
      return formatSupported(RDWaveFile::Pcm16);
      break;

    case 24:
      return formatSupported(RDWaveFile::Pcm24);
      break;

    default:
      return false;
    }
    break;

  case WAVE_FORMAT_MPEG:
    switch(getHeadLayer()) {
    case 1:
      return formatSupported(RDWaveFile::MpegL1);
      break;

    case 2:
      return formatSupported(RDWaveFile::MpegL2);
      break;

    case 3:
      return formatSupported(RDWaveFile::MpegL3);
      break;

    default:
      return false;
    }
    break;

  default:
    return false;
  }
}


RDHPIPlayStream::Error RDHPIPlayStream::openWave()
{
  if(is_open) {
    return RDHPIPlayStream::AlreadyOpen;
  }
  nameWave(wave_name);
  samples_skipped=0;
  samples_pending=0;
  if(!RDWaveFile::openWave()) {
    return RDHPIPlayStream::NoFile;
  }
  if(GetStream()<0) {
    RDWaveFile::closeWave();
    return RDHPIPlayStream::NoStream;
  }
  is_open=true;
  return RDHPIPlayStream::Ok;
}


RDHPIPlayStream::Error RDHPIPlayStream::openWave(QString file_name)
{
  if(is_open) {
    return RDHPIPlayStream::AlreadyOpen;
  }
  wave_name=file_name;
  return openWave();
}



void RDHPIPlayStream::closeWave()
{
  if(!is_open) {
    return;
  }
  if(playing||is_paused) {
    stop();
  }
  FreeStream();
  RDWaveFile::closeWave();
  is_open=false;
}


int RDHPIPlayStream::getCard() const
{
  return card_number;
}


void RDHPIPlayStream::setCard(int card)
{
  if(!playing) {
    card_number=card;
  }
}


int RDHPIPlayStream::getStream() const
{
  return stream_number;
}


int RDHPIPlayStream::getSpeed() const
{
  return play_speed;
}


bool RDHPIPlayStream::setSpeed(int speed,bool pitch,bool rate)
{
  if(speed!=RD_TIMESCALE_DIVISOR) {
    if(!pitch) {
      if(!sound_card->haveTimescaling(card_number)) {
	return false;
      }
      if((speed<TIMESCALE_LOW_LIMIT)||(speed>TIMESCALE_HIGH_LIMIT)) {
	return false;
      }
    }
    else {
      if(!rate) {    // Variable speed with resampling not yet supported!
	return false;
      }
      if((speed<96000)||(speed>104000)) {  // Max variation +/- 4%
	return false;
      }
    }
  }
  play_speed=speed;
  pitch_can_vary=pitch;
  rate_can_vary=rate;
  return true;
}


RDHPIPlayStream::State RDHPIPlayStream::getState() const
{
  return stream_state;
}


bool RDHPIPlayStream::play()
{
#ifdef RPLAYSTREAM_SHOW_SLOTS
  printf("play() -- Card: %d  Stream: %d\n",card_number,stream_number);
#endif  // RPLAYSTREAM_SHOW_SLOTS
  if(!is_open) {
    return false;
  }
  if((!playing)&&(!is_paused)) {
    if(sound_card->haveTimescaling(card_number)) {
      LogHpi(HPI_OutStreamSetTimeScale(NULL,hpi_stream,
			(uint16_t)((RD_TIMESCALE_DIVISOR/(double)play_speed)*
				   HPI_OSTREAM_TIMESCALE_UNITS)),__LINE__);
    }
    if(LogHpi(HPI_OutStreamGetInfoEx(NULL,hpi_stream,
				     &state,&buffer_size,&data_to_play,
				     &samples_played,&reserved),__LINE__)!=0) {
      return false;
    }
    fragment_size=buffer_size/4;
    if(fragment_size>MAX_FRAGMENT_SIZE) {
      fragment_size=MAX_FRAGMENT_SIZE;
    }
    if(pdata!=NULL) {
      delete pdata;
    }
    pdata=(uint8_t *)malloc(fragment_size);
    if(pdata==NULL) {
      return false;
    }
    switch(getFormatTag()) {
    case WAVE_FORMAT_PCM:
    case WAVE_FORMAT_VORBIS:
      switch(getBitsPerSample()) {
      case 8:
	LogHpi(HPI_FormatCreate(&format,getChannels(),HPI_FORMAT_PCM8_UNSIGNED,
				getSamplesPerSec(),0,0),__LINE__);
	break;
 
     case 16:
	LogHpi(HPI_FormatCreate(&format,getChannels(),HPI_FORMAT_PCM16_SIGNED,
				getSamplesPerSec(),0,0),__LINE__);
	break;

      case 24:
	LogHpi(HPI_FormatCreate(&format,getChannels(),HPI_FORMAT_PCM24_SIGNED,
				getSamplesPerSec(),0,0),__LINE__);
	break;

      case 32:
	LogHpi(HPI_FormatCreate(&format,getChannels(),HPI_FORMAT_PCM32_SIGNED,
				getSamplesPerSec(),0,0),__LINE__);
	break;

      default:
	LogHpi(HPI_AdapterClose(NULL,card_index[card_number]),__LINE__);
	return false;
      }
      break;
    case WAVE_FORMAT_MPEG:
      switch(getHeadLayer()) {
      case 1:
	LogHpi(HPI_FormatCreate(&format,getChannels(),HPI_FORMAT_MPEG_L1,
				getSamplesPerSec(),getHeadBitRate(),
				getHeadFlags()),__LINE__);
	break;

      case 2:
	LogHpi(HPI_FormatCreate(&format,getChannels(),HPI_FORMAT_MPEG_L2,
				getSamplesPerSec(),getHeadBitRate(),
				getHeadFlags()),__LINE__);
	break;

      case 3:
	LogHpi(HPI_FormatCreate(&format,getChannels(),
				HPI_FORMAT_MPEG_L3,getSamplesPerSec(),
				getHeadBitRate(),getHeadFlags()),__LINE__);
	break;

      default:
	LogHpi(HPI_AdapterClose(NULL,card_index[card_number]),__LINE__);
	return false;
      }
      break;

    default:
      return false;
    }
#if HPI_VER < 0x00030500
    if(LogHpi(HPI_DataCreate(&hpi_data,&format,pdata,fragment_size),__LINE__)!=
       0) {
      return false;
    }
#endif
  }
   if(!is_paused) {
    memset(pdata,0,fragment_size);
    left_to_write=getDataLength()-seekWave(0,SEEK_CUR);
    if(left_to_write<fragment_size) {
      read_bytes = left_to_write;
      left_to_write=0;
      stopping=true;
    }
    else {
      read_bytes=fragment_size;
      left_to_write-=fragment_size;
    }
    readWave(pdata,read_bytes);
#if HPI_VER > 0x00030500
    LogHpi(HPI_OutStreamWriteBuf(NULL,hpi_stream,pdata,read_bytes,&format),
	   __LINE__);
#else
    LogHpi(HPI_DataCreate(&hpi_data,&format,pdata,read_bytes),__LINE__);
    LogHpi(HPI_OutStreamWrite(NULL,hpi_stream,&hpi_data),__LINE__);
#endif
    if(LogHpi(HPI_OutStreamStart(NULL,hpi_stream),__LINE__)!=0) {
      return false;
    }
    clock->start(FRAGMENT_TIME);
    playing=true;
    is_paused=false;
    stopping=false;
    if(play_length>0) {
      play_timer->start(play_length,true);
      start_time=QTime::currentTime();
    }
    stream_state=RDHPIPlayStream::Playing;
    if(!restart_transport) {
      emit isStopped(false);
      emit played();
      emit stateChanged(card_number,stream_number,(int)stream_state);
    }
  }
  if((!playing)&(is_paused|repositioned)) {
    LogHpi(HPI_OutStreamStart(NULL,hpi_stream),__LINE__);
    clock->start(FRAGMENT_TIME);
    playing=true;
    stopping=false;
    is_paused=false;
    stream_state=RDHPIPlayStream::Playing;
    if(!restart_transport) {
      emit isStopped(false);
      emit played();
      emit stateChanged(card_number,stream_number,(int)stream_state);
    }
  }      
  return true;
}


void RDHPIPlayStream::pause()
{
#ifdef RPLAYSTREAM_SHOW_SLOTS
  printf("pause() -- Card: %d  Stream: %d\n",card_number,stream_number);
#endif  // RPLAYSTREAM_SHOW_SLOTS
  uint16_t state;
  uint32_t buffer_size;
  uint32_t data_to_play;
  uint32_t reserved;

  if(!is_open) {
    return;
  }
  if(playing) {
    LogHpi(HPI_OutStreamStop(NULL,hpi_stream),__LINE__);
    clock->stop();
    LogHpi(HPI_OutStreamGetInfoEx(NULL,hpi_stream,&state,&buffer_size,
				  &data_to_play,&samples_played,&reserved),
	   __LINE__);
    switch(getFormatTag()) {
    case WAVE_FORMAT_PCM:
      samples_pending=data_to_play/(getChannels()*getBitsPerSample()/8);
      break;

    case WAVE_FORMAT_MPEG:
      samples_pending=
	1152*data_to_play/(144*getHeadBitRate()/getSamplesPerSec());
      break;
    }
    playing=false;
    is_paused=true;
    stream_state=RDHPIPlayStream::Paused;
    if(!restart_transport) {
      emit paused();
      emit stateChanged(card_number,stream_number,(int)stream_state);
    }
  }
}



void RDHPIPlayStream::stop()
{
#ifdef RPLAYSTREAM_SHOW_SLOTS

  printf("stop() -- Card: %d  Stream: %d\n",card_number,stream_number);
#endif  // RPLAYSTREAM_SHOW_SLOTS

  if(!is_open) {
    return;
  }
  if(playing|is_paused) {
    LogHpi(HPI_OutStreamStop(NULL,hpi_stream),__LINE__);
    clock->stop();
    playing=false;
    is_paused=false;
    seekWave(0,SEEK_SET);
    LogHpi(HPI_OutStreamReset(NULL,hpi_stream),__LINE__);
    samples_pending=0;
    samples_skipped=0;
    stream_state=RDHPIPlayStream::Stopped;
    current_position=0;
    if(pdata!=NULL) {
      delete pdata;
      pdata=NULL;
    }
    if(!restart_transport) {
      emit position(0);
      emit isStopped(true);
      emit stopped();
      emit stateChanged(card_number,stream_number,0);
    }
  }
}


int RDHPIPlayStream::currentPosition()
{
  if(current_position!=samples_played+samples_skipped) {
    current_position=samples_played+samples_skipped;
  }
  return samples_played+samples_skipped;
}


bool RDHPIPlayStream::setPosition(unsigned samples)
{
#ifdef RPLAYSTREAM_SHOW_SLOTS
  printf("setPosition(%d) -- Card: %d  Stream: %d\n",samples,
	 card_number,stream_number);
#endif  // RPLAYSTREAM_SHOW_SLOTS
  if((samples<0)||(samples>getSampleLength())) {
    return false;
  }
  if(playing&&((unsigned)samples!=(samples_skipped+samples_played))) {
    restart_transport=true;
    pause();
  }
  
  if(!playing) {
    if(is_paused) {
      is_paused=false;
      repositioned=true;
    }
    LogHpi(HPI_OutStreamReset(NULL,hpi_stream),__LINE__);
    samples_played=0;
    switch(getFormatTag()) {
    case WAVE_FORMAT_PCM:
    case WAVE_FORMAT_VORBIS:
      samples_skipped=
	(unsigned)((double)getBlockAlign()*
		   ((double)samples/(double)getBlockAlign()));
      seekWave((int)((double)samples_skipped*(double)getBlockAlign()),
	       SEEK_SET);
      break;

    case WAVE_FORMAT_MPEG:
      samples_skipped=
	(unsigned)((double)getBlockAlign()*
		   ((double)samples/(double)getBlockAlign()));
      seekWave((int)((double)(getAvgBytesPerSec())*
		     ((double)(samples_skipped)/
		      ((double)getSamplesPerSec()))),
	       SEEK_SET);
      break;
    }
    emit position(samples);
  }
  if(restart_transport) {
    play();
    restart_transport=false;
  }
  return true;
}


void RDHPIPlayStream::setPlayLength(int length)
{
  int diff;

  if(play_timer->isActive()) {
    QTime current_time=QTime::currentTime();
    if((diff=length-start_time.msecsTo(current_time))<=0) {
      diff=0;
    }
    play_timer->changeInterval(diff);
    start_time=current_time;
  }
  play_length=length;
}


void RDHPIPlayStream::tickClock()
{
  static int count=0;
  hpi_err_t hpi_err;
  char hpi_text[100];
  int n;

  hpi_err=LogHpi(HPI_OutStreamGetInfoEx(NULL,hpi_stream,
					&state,&buffer_size,&data_to_play,
					&samples_played,&reserved),__LINE__);
  if(!stopping) {
    while((buffer_size-data_to_play)>=fragment_size) {
      n=readWave(pdata,fragment_size);
      if((n<=0)||(((uint32_t)n)<fragment_size)) {
	// End of file
#if HPI_VER > 0x00030500
	if((hpi_err=LogHpi(HPI_OutStreamWriteBuf(NULL,hpi_stream,
						 pdata,n,&format),__LINE__))!=
	   0) {
	  HPI_GetErrorText(hpi_err,hpi_text);
	  fprintf(stderr,"*** HPI Error: %s ***\n",hpi_text);
	}
#else
	HPI_DataCreate(&hpi_data,&format,pdata,n);
	if((hpi_err=LogHpi(HPI_OutStreamWrite(NULL,hpi_stream,
					      &hpi_data),__LINE__))!=0) {
	  HPI_GetErrorText(hpi_err,hpi_text);
	  fprintf(stderr,"*** HPI Error: %s ***\n",hpi_text);
	}
#endif
	left_to_write=0;
	stopping=true;
	return;
      }
      left_to_write-=n;
#if HPI_VER > 0x00030500
      hpi_err=LogHpi(HPI_OutStreamWriteBuf(NULL,hpi_stream,pdata,n,&format),
		     __LINE__);
#else
      hpi_err=LogHpi(HPI_DataCreate(&hpi_data,&format,pdata,n),__LINE__);
      hpi_err=LogHpi(HPI_OutStreamWrite(NULL,hpi_stream,&hpi_data),__LINE__);
#endif
      hpi_err=LogHpi(HPI_OutStreamGetInfoEx(NULL,hpi_stream,
					    &state,&buffer_size,&data_to_play,
					    &samples_played,&reserved),
		     __LINE__);
    }
  }
  else {
    if(state==HPI_STATE_DRAINED) {
      LogHpi(HPI_OutStreamStop(NULL,hpi_stream),__LINE__);
      if(HPI_OutStreamHostBufferFree(NULL,hpi_stream));
      LogHpi(HPI_OutStreamClose(NULL,hpi_stream),__LINE__);
      hpi_err=LogHpi(HPI_AdapterClose(NULL,card_index[card_number]),__LINE__);
      clock->stop();
      playing=false;
      seekWave(0,SEEK_SET);
      hpi_err=LogHpi(HPI_OutStreamReset(NULL,hpi_stream),__LINE__);
      samples_pending=0;
      samples_skipped=0;
      stream_state=RDHPIPlayStream::Stopped;
      emit position(0);
      emit isStopped(true);
      emit stopped();
      emit stateChanged(card_number,stream_number,(int)stream_state);
      return;
    }
  }
  if(count++==2) {
    count=0;
    emit position(samples_played+samples_skipped);
  }
}


void RDHPIPlayStream::Drained()
{
}


int RDHPIPlayStream::GetStream()
{
#ifdef RDHPIPLAYSTREAM_USE_LOCAL_MUTEX
  for(int i=0;i<sound_card->getCardOutputStreams(card_number);i++) {
    if(++stream_mutex[card_number][i]==1) {
      if(LogHpi(HPI_OutStreamOpen(NULL,card_index[card_number],i,&hpi_stream),
		__LINE__)==0) {
	if(HPI_OutStreamHostBufferAllocate(NULL,hpi_stream,dma_buffer_size));
	stream_number=i;
	return stream_number;
      }
    }
    stream_mutex[card_number][i]--;
  }
  return -1;
#else
  for(int i=0;i<sound_card->getCardOutputStreams(card_number);i++) {
    if(LogHpi(HPI_OutStreamOpen(NULL,card_index[card_number],i,&hpi_stream),
	      __LINE__)==0) {
      stream_number=i;
//      RDApplication::syslog(sound_card->config(),LOG_ERR,"HPI allocating ostream: %d",stream_number);
      return stream_number;
    }
  }
  return -1;
#endif
}


void RDHPIPlayStream::FreeStream()
{
#ifdef RDHPIPLAYSTREAM_USE_LOCAL_MUTEX
  stream_mutex[card_number][stream_number]--;
  if(HPI_OutStreamHostBufferFree(NULL,hpi_stream));
  LogHpi(HPI_OutStreamClose(NULL,hpi_stream),__LINE__);
  stream_number=-1;
#else
  if(HPI_OutStreamHostBufferFree(NULL,hpi_stream));
  LogHpi(HPI_OutStreamClose(NULL,hpi_stream),__LINE__);
  stream_number=-1;
#endif
}


hpi_err_t RDHPIPlayStream::LogHpi(hpi_err_t err,int lineno)
{
  char err_txt[200];

  if(err!=0) {
    HPI_GetErrorText(err,err_txt);
    RDApplication::syslog(sound_card->config(),LOG_WARNING,
			  "HPI Error: %s, %s line %d",err_txt,__FILE__,lineno);
  }
  return err;
}
