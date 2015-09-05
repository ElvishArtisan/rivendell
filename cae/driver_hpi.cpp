// driver_hpi.cpp
//
// CAE driver for AudioScience HPI devices
//
//   (C) Copyright 2002-2015 Fred Gleason <fredg@paravelsystems.com>
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

#include "driver_hpi.h"

HpiDriver::HpiDriver(RDStation *station,RDConfig *config,QObject *parent)
  : Driver(Driver::Hpi,station,config,parent)
{
#ifdef HPI
  for(int i=0;i<RD_MAX_CARDS;i++) {
    for(int j=0;j<RD_MAX_STREAMS;j++) {
      hpi_record[i][j]=NULL;
      hpi_play[i][j]=NULL;
    }
  }
  hpi_sound_card=new RDHPISoundCard(this);
  hpi_sound_card->setFadeProfile(RD_FADE_TYPE);
  for(int i=0;i<hpi_sound_card->getCardQuantity();i++) {
    station->setCardDriver(i,RDStation::Hpi);
    station->setCardName(i,hpi_sound_card->getCardDescription(i));
    station->setCardInputs(i,hpi_sound_card->getCardInputPorts(i));
    station->setCardOutputs(i,hpi_sound_card->getCardOutputPorts(i));
  }
#endif  // HPI
}


HpiDriver::~HpiDriver()
{
}


QString HpiDriver::version()
{
#ifdef HPI
  if(hpi_sound_card==NULL) {
    return QString("not active");
  }
  RDHPIInformation *info=hpi_sound_card->hpiInformation(0);
  if(info->hpiVersion()==0) {
    return QString("not active");
  }
  return QString().sprintf("%d.%02d.%02d",info->hpiMajorVersion(),
			   info->hpiMinorVersion(),info->hpiPointVersion());
#else
  return QString("not enabled");
#endif  // HPI
}


QString HpiDriver::cardName(int card)
{
  return hpi_sound_card->getCardDescription(card);
}


int HpiDriver::inputs(int card)
{
  return hpi_sound_card->getCardInputPorts(card);
}


int HpiDriver::outputs(int card)
{
  return hpi_sound_card->getCardOutputPorts(card);
}


int HpiDriver::start(int next_card)
{
  return hpi_sound_card->getCardQuantity();
}


bool HpiDriver::loadPlayback(int card,QString wavename,int *stream)
{
#ifdef HPI
  RDHPIPlayStream *playstream=new RDHPIPlayStream(hpi_sound_card);
  playstream->setCard(card);
  if(playstream->openWave(wavename)!=RDHPIPlayStream::Ok) {
    logLine(RDConfig::LogNotice,QString().sprintf(
            "Error: hpiLoadPlayback(%s)   openWave() failed to open file",
            (const char *) wavename) );
    delete playstream;
    return false;
  }
  *stream=playstream->getStream();
  hpi_play[card][*stream]=playstream;
  connect(hpi_play[card][*stream],SIGNAL(stateChanged(int,int,int)),
	  this,SLOT(playStateChangedData(int,int,int)));
  return true;
#else
  return false;
#endif  // HPI
}


bool HpiDriver::unloadPlayback(int card,int stream)
{
#ifdef HPI
  if(hpi_play[card][stream]==NULL) {
    return false;
  }
  if(hpi_play[card][stream]->getState()==RDHPIPlayStream::Playing) {
    hpi_play[card][stream]->pause();
  }
  hpi_play[card][stream]->disconnect();
  hpi_play[card][stream]->closeWave();
  delete hpi_play[card][stream];
  hpi_play[card][stream]=NULL;
  return true;
#else
  return false;
#endif  // HPI
}


bool HpiDriver::playbackPosition(int card,int stream,unsigned pos)
{
#ifdef HPI
  if(hpi_play[card][stream]==NULL) {
    return false;
  }
  return hpi_play[card][stream]->
    setPosition((unsigned)((double)hpi_play[card][stream]->getSamplesPerSec()*
		      (double)pos/1000.0));
#else
  return false;
#endif  // HPI
}


bool HpiDriver::play(int card,int stream,int length,int speed,bool pitch,
		     bool rates)
{
#ifdef HPI
  if(hpi_play[card][stream]==NULL) {
    return false;
  }
  if(!hpi_play[card][stream]->setSpeed(speed,pitch,rates)) {
    return false;
  }
  hpi_play[card][stream]->setPlayLength(length);
  return hpi_play[card][stream]->play();
#else
  return false;
#endif  // HPI
}


bool HpiDriver::stopPlayback(int card,int stream)
{
#ifdef HPI
  if(hpi_play[card][stream]==NULL) {
    return false;
  }
  hpi_play[card][stream]->pause();
  return true;
#else
  return false;
#endif  // HPI
}


bool HpiDriver::timescaleSupported(int card)
{
#ifdef HPI
  return hpi_sound_card->haveTimescaling(card);
#else
  return false;
#endif  // HPI
}


bool HpiDriver::loadRecord(int card,int stream,int coding,int chans,
			   int samprate,int bitrate,QString wavename)
{
#ifdef HPI
  hpi_record[card][stream]=new RDHPIRecordStream(hpi_sound_card);
  connect(hpi_record[card][stream],SIGNAL(stateChanged(int,int,int)),
	  this,SLOT(recordStateChangedData(int,int,int)));
  hpi_record[card][stream]->setCard(card);
  hpi_record[card][stream]->setStream(stream);
  hpi_record[card][stream]->nameWave(wavename);
  hpi_record[card][stream]->setChannels(chans);
  hpi_record[card][stream]->setSamplesPerSec(samprate);
  if(coding==0) {                 // PCM16
    hpi_record[card][stream]->setFormatTag(WAVE_FORMAT_PCM);
    hpi_record[card][stream]->setBitsPerSample(16);
  }
  if((coding==1)||(coding==2)) {  // MPEG-1
    hpi_record[card][stream]->setFormatTag(WAVE_FORMAT_MPEG);
    hpi_record[card][stream]->setHeadLayer(coding);
    hpi_record[card][stream]->setHeadBitRate(bitrate);
    hpi_record[card][stream]->setMextChunk(true);
    switch(chans) {
	case 1:
	  hpi_record[card][stream]->setHeadMode(ACM_MPEG_SINGLECHANNEL);
	  break;
	case 2:
	  hpi_record[card][stream]->setHeadMode(ACM_MPEG_STEREO);
	  break;
	default:
	  delete hpi_record[card][stream];
	  hpi_record[card][stream]=NULL;
	  return false;
    }
    hpi_record[card][stream]->setHeadFlags(ACM_MPEG_ID_MPEG1);
  }
  if(coding==4) {                 // PCM24
    hpi_record[card][stream]->setFormatTag(WAVE_FORMAT_PCM);
    hpi_record[card][stream]->setBitsPerSample(24);
  }
  if(coding>4) {
    delete hpi_record[card][stream];
    hpi_record[card][stream]=NULL;
    return false;
  }
  hpi_record[card][stream]->setBextChunk(true);
  hpi_record[card][stream]->setCartChunk(true);
  hpi_record[card][stream]->setLevlChunk(true);
  if(hpi_record[card][stream]->createWave()!=RDHPIRecordStream::Ok) {
    delete hpi_record[card][stream];
    hpi_record[card][stream]=NULL;
    return false;
  }
  chown((const char *)wavename,config()->uid(),config()->gid());
  if(!hpi_record[card][stream]->recordReady()) {
    delete hpi_record[card][stream];
    hpi_record[card][stream]=NULL;
    return false;
  }
  return true;
#else
  return false;
#endif  // HPI
}


bool HpiDriver::record(int card,int stream,int length,int thres)
{
#ifdef HPI
  if(hpi_record[card][stream]==NULL) {
    return false;
  }
  if(thres!=0) {
    if(hpi_record[card][stream]->haveInputVOX()) {
      hpi_record[card][stream]->setInputVOX(thres);
    }
    else {
      return false;
    }
  }
  hpi_record[card][stream]->setRecordLength(length);
  hpi_record[card][stream]->record();
  return true;
#else
  return false;
#endif  // HPI
}


bool HpiDriver::stopRecord(int card,int stream)
{
#ifdef HPI
  if(hpi_record[card][stream]==NULL) {
    return false;
  }
  hpi_record[card][stream]->pause();
  hpi_record[card][stream]->setInputVOX(-10000);
  return true;
#else
  return false;
#endif  // HPI
}


bool HpiDriver::unloadRecord(int card,int stream,unsigned *len)
{
#ifdef HPI
  if(hpi_record[card][stream]==NULL) {
    return false;
  }
  if(hpi_record[card][stream]->getState()==RDHPIRecordStream::Recording) {
    hpi_record[card][stream]->pause();
  }
  hpi_record[card][stream]->disconnect();
  *len=hpi_record[card][stream]->samplesRecorded();
  hpi_record[card][stream]->closeWave();
  delete hpi_record[card][stream];
  hpi_record[card][stream]=NULL;
  return true;
#else
  return false;
#endif  // HPI
}


bool HpiDriver::setClockSource(int card,int src)
{
#ifdef HPI
  return hpi_sound_card->setClockSource(card,(RDHPISoundCard::ClockSource)src);
#else
  return false;
#endif  // HPI
}


bool HpiDriver::setInputVolume(int card,int stream,int level)
{
#ifdef HPI
  hpi_sound_card->setInputVolume(card,stream,level);
  return true;
#else
  return false;
#endif  // HPI
}


bool HpiDriver::setOutputVolume(int card,int stream,int port,int level)
{
#ifdef HPI
  hpi_sound_card->setOutputVolume(card,stream,port,level);
  return true;
#else
  return false;
#endif  // HPI
}


bool HpiDriver::fadeOutputVolume(int card,int stream,int port,int level,int length)
{
#ifdef HPI
  hpi_sound_card->fadeOutputVolume(card,stream,port,level,length);
  return true;
#else
  return false;
#endif  // HPI
}


bool HpiDriver::setInputLevel(int card,int port,int level)
{
#ifdef HPI
  hpi_sound_card->setInputLevel(card,port,level);
  return true;
#else
  return false;
#endif  // HPI
}


bool HpiDriver::setOutputLevel(int card,int port,int level)
{
#ifdef HPI
  hpi_sound_card->setOutputLevel(card,port,level);
  return true;
#else
  return false;
#endif  // HPI
}


bool HpiDriver::setInputMode(int card,int stream,int mode)
{
#ifdef HPI
  switch(mode) {
      case 0:
	hpi_sound_card->setInputMode(card,stream,RDHPISoundCard::Normal);
	break;

      case 1:
	hpi_sound_card->setInputMode(card,stream,RDHPISoundCard::Swap);
	break;

      case 2:
	hpi_sound_card->setInputMode(card,stream,RDHPISoundCard::LeftOnly);
	break;

      case 3:
	hpi_sound_card->setInputMode(card,stream,RDHPISoundCard::RightOnly);
	break;

      default:
	return false;
  }
  return true;
#else
  return false;
#endif  // HPI
}


bool HpiDriver::setOutputMode(int card,int stream,int mode)
{
#ifdef HPI
  switch(mode) {
      case 0:
	hpi_sound_card->setOutputMode(card,stream,RDHPISoundCard::Normal);
	break;

      case 1:
	hpi_sound_card->setOutputMode(card,stream,RDHPISoundCard::Swap);
	break;

      case 2:
	hpi_sound_card->setOutputMode(card,stream,RDHPISoundCard::LeftOnly);
	break;

      case 3:
	hpi_sound_card->setOutputMode(card,stream,RDHPISoundCard::RightOnly);
	break;

      default:
	return false;
  }
  return true;
#else
  return false;
#endif  // HPI
}


bool HpiDriver::setInputVoxLevel(int card,int stream,int level)
{
#ifdef HPI
  hpi_sound_card->setInputStreamVOX(card,stream,level);
  return true;
#else
  return false;
#endif  // HPI
}


bool HpiDriver::setInputType(int card,int port,int type)
{
#ifdef HPI
  switch(type) {
      case 0:
	hpi_sound_card->setInputPortMux(card,port,RDHPISoundCard::LineIn);
	break;

      case 1:
	hpi_sound_card->setInputPortMux(card,port,RDHPISoundCard::AesEbuIn);
	break;

      default:
	return false;
  }
  return true;
#else
  return false;
#endif  // HPI
}


bool HpiDriver::getInputStatus(int card,int port)
{
#ifdef HPI
  if(hpi_sound_card->getInputPortError(card,port)!=0) {
    return false;
  }
  return true;
#else
  return false;
#endif  // HPI
}


bool HpiDriver::getInputMeters(int card,int port,short levels[2])
{
#ifdef HPI
  return hpi_sound_card->inputStreamMeter(card,port,levels);
#else
  return false;
#endif  // HPI
}


bool HpiDriver::getOutputMeters(int card,int port,short levels[2])
{
#ifdef HPI
  if(config()->useStreamMeters()) {
    //
    //  This is UGLY, but needed to semi-support cards (like the ASI4215)
    //  that lack output port metering.
    //
    for(int i=0;i<RD_MAX_STREAMS;i++) {
      if(hpi_sound_card->getOutputVolume(card,i,port)>-10000) {
	return hpi_sound_card->outputStreamMeter(card,i,levels);
      }
    }
    levels[0]=-10000;
    levels[1]=-10000;
    return true;
  }
  return hpi_sound_card->outputPortMeter(card,port,levels);
#else
  return false;
#endif  // HPI
}


bool HpiDriver::getStreamOutputMeters(int card,int stream,short levels[2])
{
#ifdef HPI
  return hpi_sound_card->outputStreamMeter(card,stream,levels);
#else
  return false;
#endif  // HPI
}


bool HpiDriver::setPassthroughLevel(int card,int in_port,int out_port,int level)
{
#ifdef HPI
  return hpi_sound_card->setPassthroughVolume(card,in_port,out_port,level);
#else
  return false;
#endif  // HPI
}


void HpiDriver::getOutputPosition(int card,unsigned *pos)
{
#ifdef HPI
  for(int i=0;i<RD_MAX_STREAMS;i++) {
    if(hpi_play[card][i]==NULL) {
      pos[i]=0;
    }
    else {
      pos[i]=1000*(unsigned long long)hpi_play[card][i]->currentPosition()/
	hpi_play[card][i]->getSamplesPerSec();
    }
  }
#endif  // HPI
}


void HpiDriver::playStateChangedData(int card,int stream,int state)
{
  emit playStateChanged(card,stream,state);
}


void HpiDriver::recordStateChangedData(int card,int stream,int state)
{
  emit recordStateChanged(card,stream,state);
}
