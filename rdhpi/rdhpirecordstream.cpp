//   rdhpirecordstream.cpp
//
//   A class for recording Microsoft WAV files.
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdhpirecordstream.cpp,v 1.7 2011/05/19 22:16:54 cvs Exp $
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
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <qobject.h>
#include <qwidget.h>
#include <qstring.h>
#include <qdatetime.h>

#include <rdhpirecordstream.h>


RDHPIRecordStream::RDHPIRecordStream(RDHPISoundCard *card,
			     QWidget *parent,const char *name) 
  :QObject(parent,name),RDWaveFile()
{ 
  hpi_err_t hpi_err;
  int quan;
  uint16_t type[HPI_MAX_ADAPTERS];

  if(getenv(DEBUG_VAR)==NULL) {
    debug=false;
  }
  else {
    debug=true;
    printf("RDHPIRecordStream: debugging enabled\n");
  }
  if(getenv(XRUN_VAR)==NULL) {
    xrun=false;
  }
  else {
    xrun=true;
    printf("RDHPIRecordStream: xrun notification enabled\n");
  }

  sound_card=card;

  card_number=-1;
  stream_number=-1;
  is_ready=false;
  is_recording=false;
  is_paused=false;
  stopping=false;
  record_started=false;
  record_length=0;
  is_open=false;
  pdata=NULL;

  //
  // Get Adapter Indices
  //
#if HPI_VER < 0x00030600
  for(unsigned i=0;i<HPI_MAX_ADAPTERS;i++) {
    card_index[i]=i;
  }
#else
  hpi_err=HPI_SubSysGetNumAdapters(NULL,&quan);
  for(int i=0;i<quan;i++) {
    hpi_err=HPI_SubSysGetAdapter(NULL,i,card_index+i,type+i);
  }
#endif  // HPI_VER

  clock=new QTimer(this,"clock");
  connect(clock,SIGNAL(timeout()),this,SLOT(tickClock()));

  length_timer=new QTimer(this,"length_timer");
  connect(length_timer,SIGNAL(timeout()),this,SLOT(pause()));
}


RDHPIRecordStream::~RDHPIRecordStream()
{
  if(pdata!=NULL) {
    delete pdata;
  }
} 


QString RDHPIRecordStream::errorString(RDHPIRecordStream::Error err)
{
  QString str;

  switch(err) {
      case RDHPIRecordStream::Ok:
	return QString(tr("Ok"));
	break;

      case RDHPIRecordStream::NoFile:
	return QString(tr("Unable to create/open file"));
	break;

      case RDHPIRecordStream::NoStream:
	return QString(tr("Input stream unavailable"));
	break;

      case RDHPIRecordStream::AlreadyOpen:
	return QString(tr("Stream is already open"));
	break;

      default:
	str=QString(tr("Unknown Error:"));
	return QString().sprintf("%s %d\n",(const char *)str,err);
	break;
  }
}


RDHPIRecordStream::Error RDHPIRecordStream::createWave()
{
  if(is_open) {
    return RDHPIRecordStream::AlreadyOpen;
  }
  if(!RDWaveFile::createWave()) {
    return RDHPIRecordStream::NoFile;
  }
  if(!GetStream()) {
    closeWave();
    return RDHPIRecordStream::NoStream;
  }
  is_open=true;
  return RDHPIRecordStream::Ok;
}


RDHPIRecordStream::Error RDHPIRecordStream::createWave(QString filename)
{
  if(is_open) {
    return RDHPIRecordStream::AlreadyOpen;
  }
  setName(filename);
  return createWave();
}


void RDHPIRecordStream::closeWave()
{
  if(!is_open) {
    return;
  }
  if(getState()!=RDHPIRecordStream::Stopped) {
    stop();
  }
  RDWaveFile::closeWave(samples_recorded);
  FreeStream();
  is_open=false;
}


bool RDHPIRecordStream::formatSupported(RDWaveFile::Format format)
{
#if HPI_VER < HPI_VERSION_CONSTRUCTOR(3L,10,0)
  HPI_FORMAT hformat;
#else
  struct hpi_format hformat;
#endif
  hpi_handle_t histream;
  hpi_err_t hpi_err;
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
    for(int i=0;i<sound_card->getCardInputStreams(card_number);i++) {
      if(HPI_InStreamOpen(NULL,card_index[card_number],i,&histream)==0) {
	found=true;
	break;
      }
    }
    if(!found) {
      return false;
    }
  }
  else {
    histream=hpi_stream;
  }
  switch(format) {
      case RDWaveFile::Pcm8:
	hpi_err=HPI_FormatCreate(&hformat,getChannels(),
				 HPI_FORMAT_PCM8_UNSIGNED,
				 getSamplesPerSec(),getHeadBitRate(),0);
	state=HPI_InStreamQueryFormat(NULL,histream,&hformat);
	break;

      case RDWaveFile::Pcm16:
	hpi_err=HPI_FormatCreate(&hformat,getChannels(),
				 HPI_FORMAT_PCM16_SIGNED,
				 getSamplesPerSec(),getHeadBitRate(),0);
	state=HPI_InStreamQueryFormat(NULL,histream,&hformat);
	break;

      case RDWaveFile::MpegL1:
	hpi_err=HPI_FormatCreate(&hformat,getChannels(),HPI_FORMAT_MPEG_L1,
				 getSamplesPerSec(),getHeadBitRate(),0);
	state=HPI_InStreamQueryFormat(NULL,histream,&hformat);
	break;

      case RDWaveFile::MpegL2:
	hpi_err=HPI_FormatCreate(&hformat,getChannels(),HPI_FORMAT_MPEG_L2,
				 getSamplesPerSec(),getHeadBitRate(),0);
	state=HPI_InStreamQueryFormat(NULL,histream,&hformat);
	break;

      case RDWaveFile::MpegL3:
	hpi_err=HPI_FormatCreate(&hformat,getChannels(),HPI_FORMAT_MPEG_L3,
				 getSamplesPerSec(),getHeadBitRate(),0);
	state=HPI_InStreamQueryFormat(NULL,histream,&hformat);
	break;

      default:
	state=1;
	break;
  }
  if(!is_open) {
    hpi_err=HPI_InStreamClose(NULL,histream);
  }
  if(state!=0) {
    return false;
  }
  return true;
}


bool RDHPIRecordStream::formatSupported()
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


int RDHPIRecordStream::getCard() const
{
  return card_number;
}


void RDHPIRecordStream::setCard(int card)
{
  if(!is_recording) {
    card_number=card;
    if(debug) {
      printf("RDHPIRecordStream: using card %d\n",card_number);
    }
  }
}


int RDHPIRecordStream::getStream() const
{
  return stream_number;
}


void RDHPIRecordStream::setStream(int stream)
{
  stream_number=stream;
}


bool RDHPIRecordStream::haveInputVOX() const
{
  return sound_card->haveInputStreamVOX(card_number,stream_number);
}


RDHPIRecordStream::RecordState RDHPIRecordStream::getState()
{
  if(is_recording) {
    if(record_started) {
      return RDHPIRecordStream::RecordStarted;
    }
    return RDHPIRecordStream::Recording;
  }
  if(is_paused) {
    return RDHPIRecordStream::Paused;
  }
  if(is_ready) {
    return RDHPIRecordStream::RecordReady;
  }
  return RDHPIRecordStream::Stopped;
}


int RDHPIRecordStream::getPosition() const
{
  if((!is_recording)&&(!is_ready)&&(!is_paused)) {
    return 0;
  }
  return samples_recorded;
}


unsigned RDHPIRecordStream::samplesRecorded() const
{
  return samples_recorded;
}


bool RDHPIRecordStream::recordReady()
{
  hpi_err_t hpi_error=0;
  char hpi_text[200];

  if(debug) {
    printf("RDHPIRecordStream: received recordReady()\n");
  }
  if(!is_open) {
    return false;
  }
  if((!is_recording)&&(!is_paused)) {
    resetWave();
    if(HPI_InStreamGetInfoEx(NULL,hpi_stream,
			     &state,&buffer_size,&data_recorded,
			     &samples_recorded,&reserved)!=0) {
      if(debug) {
	printf("RDHPIRecordStream: HPI_InStreamGetInfoEx() failed\n");
      }
      return false;
    }
    fragment_size=buffer_size/4;
    if(fragment_size>192000) {  // ALSA Compatibility Limitation
      fragment_size=192000;
    }
    fragment_time=(1000*fragment_size)/(getAvgBytesPerSec());
    if(pdata!=NULL) {
      delete pdata;
    }
    pdata=(uint8_t *)malloc(fragment_size);
    if(pdata==NULL) {
      if(debug) {
	printf("RDHPIRecordStream: couldn't allocate buffer\n");
      }
      return false;
    }
    switch(getFormatTag()) {
	case WAVE_FORMAT_PCM:
	  if(debug) {
	    printf("RDHPIRecordStream: using PCM%d format\n",
		   getBitsPerSample());
	  }
	  switch(getBitsPerSample()) {
	      case 8:
		hpi_error=HPI_FormatCreate(&format,getChannels(),
				 HPI_FORMAT_PCM8_UNSIGNED,getSamplesPerSec(),
				 0,0);
		break;
	      case 16:
		hpi_error=HPI_FormatCreate(&format,getChannels(),
				 HPI_FORMAT_PCM16_SIGNED,getSamplesPerSec(),
				 0,0);
		break;
	      case 32:
		hpi_error=HPI_FormatCreate(&format,getChannels(),
				 HPI_FORMAT_PCM32_SIGNED,getSamplesPerSec(),
				 0,0);
		break;
	      default:
		if(debug) {
		  printf("RDHPIRecordStream: unsupported sample size\n");
		}
		return false;
	  }
	  break;

	case WAVE_FORMAT_MPEG:
	  if(debug) {
	    printf("RDHPIRecordStream: using MPEG-1 Layer %d\n",getHeadLayer());
	  }
	  switch(getHeadLayer()) {
	      case 1:
		hpi_error=HPI_FormatCreate(&format,getChannels(),
				 HPI_FORMAT_MPEG_L1,getSamplesPerSec(),
				 getHeadBitRate(),getHeadFlags());
		break;
	      case 2:
		hpi_error=HPI_FormatCreate(&format,getChannels(),
				 HPI_FORMAT_MPEG_L2,getSamplesPerSec(),
				 getHeadBitRate(),getHeadFlags());
		break;
	      case 3:
		hpi_error=HPI_FormatCreate(&format,getChannels(),
				 HPI_FORMAT_MPEG_L3,getSamplesPerSec(),
				 getHeadBitRate(),getHeadFlags());
		break;
	      default:
		hpi_error=HPI_AdapterClose(NULL,card_index[card_number]);
		if(debug) {
		  printf("RDHPIRecordStream: invalid MPEG-1 layer\n");
		}
		return false;
	  }
	  if(getMextChunk()) {
	    setMextHomogenous(true);
	    setMextPaddingUsed(false);
	    setMextHackedBitRate(true);
	    setMextFreeFormat(false);
	    setMextFrameSize(144*getHeadBitRate()/getSamplesPerSec());
	    setMextAncillaryLength(5);
	    setMextLeftEnergyPresent(true);
	    if(getChannels()>1) {
	      setMextRightEnergyPresent(true);
	    }
	    else {
	      setMextRightEnergyPresent(false);
	    }
	    setMextPrivateDataPresent(false);
	  }
	  break;

	case WAVE_FORMAT_VORBIS:
	  if(debug) {
	    printf("RDHPIRecordStream: using OggVorbis\n");
	  }
	  hpi_error=HPI_FormatCreate(&format,getChannels(),
			   HPI_FORMAT_PCM16_SIGNED,getSamplesPerSec(),
			   0,0);
	  break;

	default:
	  if(debug) {
	    printf("RDHPIRecordStream: invalid format tag\n");
	  }
	  return false;
	  break;
    }
    if((hpi_error=HPI_InStreamQueryFormat(NULL,hpi_stream,
			       &format))!=0) {
      if(debug) {
	HPI_GetErrorText(hpi_error,hpi_text);
	printf("Num: %d\n",hpi_error);
	printf("RDHPIRecordStream: %s\n",hpi_text);
      }
      return false;
    }
  }
#if HPI_VER < 0x00030500
  HPI_DataCreate(&hpi_data,&format,pdata,fragment_size);
#endif
  hpi_error=HPI_InStreamSetFormat(NULL,hpi_stream,&format);
  hpi_error=HPI_InStreamStart(NULL,hpi_stream);
//  clock->start(2*fragment_time/3);
  clock->start(100);
  is_ready=true;
  is_recording=false;
  is_paused=false;
  stopping=false;
  emit isStopped(false);
  emit ready();
  emit stateChanged(card_number,stream_number,1);  // RecordReady
  if(debug) {
    printf("RDHPIRecordStream: emitted isStopped(false)\n");
    printf("RDHPIRecordStream: emitted ready()\n");
    printf("RDHPIRecordStream: emitted stateChanged(%d,%d,RDHPIRecordStream::RecordReady)\n",card_number,stream_number);
  }

  return true;
}


void RDHPIRecordStream::record()
{
  hpi_err_t hpi_err;

  if(debug) {
    printf("RDHPIRecordStream: received record()\n");
  }
  if(!is_open) {
    return;
  }
  if(!is_ready) {
    recordReady();
  }
  record_started=false;
  hpi_err=HPI_InStreamReset(NULL,hpi_stream);
  hpi_err=HPI_InStreamStart(NULL,hpi_stream);
  is_recording=true;
  is_paused=false;
  emit isStopped(false);
  emit recording();
  emit stateChanged(card_number,stream_number,0);  // Recording
  if(debug) {
    printf("RDHPIRecordStream: emitted isStopped(false)\n");
    printf("RDHPIRecordStream: emitted recording()\n");
    printf("RDHPIRecordStream: emitted stateChanged(%d,%d,RDHPIRecordStream::Recording)\n",card_number,stream_number);
  }
  tickClock();
}


void RDHPIRecordStream::pause()
{
  hpi_err_t hpi_err;

  if(debug) {
    printf("RDHPIRecordStream: received pause()\n");
  }
  if(!is_recording) {
    return;
  }
  hpi_err=HPI_InStreamStop(NULL,hpi_stream);
  tickClock();
  hpi_err=HPI_InStreamGetInfoEx(NULL,hpi_stream,&state,&buffer_size,
				&data_recorded,&samples_recorded,&reserved);
  is_recording=false;
  is_paused=true;
  hpi_err=HPI_InStreamStart(NULL,hpi_stream);
  emit paused();
  emit stateChanged(card_number,stream_number,2);  // Paused
  if(debug) {
    printf("RDHPIRecordStream: emitted paused()\n");
    printf("RDHPIRecordStream: emitted stateChanged(%d,%d,RDHPIRecordStream::Paused)\n",card_number,stream_number);
  }
}


void RDHPIRecordStream::stop()
{
  hpi_err_t hpi_err;

  if(debug) {
    printf("RDHPIRecordStream: received stop()\n");
  }
  if(is_ready|is_recording|is_paused) {
    hpi_err=HPI_InStreamStop(NULL,hpi_stream);
    tickClock();
    clock->stop();
    is_recording=false;
    is_paused=false;
    is_ready=false;
    if(pdata!=NULL) {
      delete pdata;
      pdata=NULL;
    }
    emit isStopped(true);
    emit stopped();
    emit stateChanged(card_number,stream_number,RDHPIRecordStream::Stopped);
    emit position(0);
    if(debug) {
      printf("RDHPIRecordStream: emitted isStopped(true)\n");
      printf("RDHPIRecordStream: emitted stopped()\n");
      printf("RDHPIRecordStream: emitted stateChanged(%d,%d,RDHPIRecordStream::Stopped)\n",card_number,stream_number);
      printf("RDHPIRecordStream: emitted position(0)\n");
    }
  }
}


void RDHPIRecordStream::setInputVOX(int gain)
{
  sound_card->setInputStreamVOX(card_number,stream_number,gain);
}


void RDHPIRecordStream::setRecordLength(int length)
{
  record_length=length;
}


void RDHPIRecordStream::tickClock()
{
  hpi_err_t hpi_err;

  hpi_err=HPI_InStreamGetInfoEx(NULL,hpi_stream,
				&state,&buffer_size,&data_recorded,
				&samples_recorded,&reserved);
  if((!record_started)&&(is_recording)) {
    if(samples_recorded>0) {
      if(record_length>0) {
        length_timer->start(record_length,true);
      }
      emit recordStart();
      emit stateChanged(card_number,stream_number,4);  // RecordStarted
      if(debug) {
	printf("RDHPIRecordStream: emitted recordStart()\n");
	printf("RDHPIRecordStream: emitted stateChanged(%d,%d,RDHPIRecordStream::RecordStarted)\n",card_number,stream_number);
      }
      record_started=true;
    }
  }
  while(data_recorded>fragment_size) {
#if HPI_VER > 0x00030500
    hpi_err=HPI_InStreamReadBuf(NULL,hpi_stream,pdata,fragment_size);
#else
    hpi_err=HPI_InStreamRead(NULL,hpi_stream,&hpi_data);
#endif
    if(is_recording) {
      writeWave(pdata,fragment_size);
    }
    hpi_err=HPI_InStreamGetInfoEx(NULL,hpi_stream,
			  &state,&buffer_size,&data_recorded,
			  &samples_recorded,&reserved);
  }
  if(state==HPI_STATE_STOPPED) {
#if HPI_VER > 0x00030500
    hpi_err=HPI_InStreamReadBuf(NULL,hpi_stream,pdata,data_recorded);
#else
    hpi_err=HPI_DataCreate(&hpi_data,&format,pdata,data_recorded);
    hpi_err=HPI_InStreamRead(NULL,hpi_stream,&hpi_data);
#endif
    if(is_recording) {
      writeWave(pdata,data_recorded);
    }
  }
  emit position(samples_recorded);
  if(debug) {
    printf("RDHPIRecordStream: emitted position(%u)\n",
	   (unsigned)samples_recorded);
  }
}


bool RDHPIRecordStream::GetStream()
{
  hpi_err_t hpi_err;
  char hpi_text[100];

  if((hpi_err=
      HPI_InStreamOpen(NULL,card_index[card_number],stream_number,&hpi_stream))!=0) {
    if(debug) {
      HPI_GetErrorText(hpi_err,hpi_text);
      fprintf(stderr,"*** HPI Error: %s ***\n",hpi_text);
    }
    return false;
  }
  return true;
}


void RDHPIRecordStream::FreeStream()
{
  hpi_err_t hpi_err;

  hpi_err=HPI_InStreamClose(NULL,hpi_stream);
}

