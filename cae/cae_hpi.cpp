// cae_hpi.cpp
//
// The HPI Driver for the Core Audio Engine component of Rivendell
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: cae_hpi.cpp,v 1.38.6.2 2012/11/30 16:14:58 cvs Exp $
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

#include <cae.h>

#include <rddebug.h>

void MainObject::hpiInit(RDStation *station)
{
#ifdef HPI
  for(int i=0;i<RD_MAX_CARDS;i++) {
    for(int j=0;j<RD_MAX_STREAMS;j++) {
      record[i][j]=NULL;
      play[i][j]=NULL;
    }
  }
  sound_card=new RDHPISoundCard(this,"sound_card");
  sound_card->setFadeProfile(RD_FADE_TYPE);
  for(int i=0;i<sound_card->getCardQuantity();i++) {
    cae_driver[i]=RDStation::Hpi;
    station->setCardDriver(i,RDStation::Hpi);
    station->setCardName(i,sound_card->getCardDescription(i));
    station->setCardInputs(i,sound_card->getCardInputPorts(i));
    station->setCardOutputs(i,sound_card->getCardOutputPorts(i));
  }
#endif  // HPI
}


void MainObject::hpiFree()
{
#ifdef HPI

#endif  // HPI
}


QString MainObject::hpiVersion()
{
#ifdef HPI
  if(sound_card==NULL) {
    return QString("not active");
  }
  RDHPIInformation *info=sound_card->hpiInformation(0);
  if(info->hpiVersion()==0) {
    return QString("not active");
  }
  return QString().sprintf("%d.%02d.%02d",info->hpiMajorVersion(),
			   info->hpiMinorVersion(),info->hpiPointVersion());
#else
  return QString("not enabled");
#endif  // HPI
}


bool MainObject::hpiLoadPlayback(int card,QString wavename,int *stream)
{
#ifdef HPI
  RDHPIPlayStream *playstream=new RDHPIPlayStream(sound_card);
  playstream->setCard(card);
  if(playstream->openWave(wavename)!=RDHPIPlayStream::Ok) {
    LogLine(RDConfig::LogNotice,QString().sprintf(
            "Error: hpiLoadPlayback(%s)   openWave() failed to open file",
            (const char *) wavename) );
    delete playstream;
    return false;
  }
  *stream=playstream->getStream();
  play[card][*stream]=playstream;
  connect(play[card][*stream],SIGNAL(stateChanged(int,int,int)),
	  this,SLOT(statePlayUpdate(int,int,int)));
  return true;
#else
  return false;
#endif  // HPI
}


bool MainObject::hpiUnloadPlayback(int card,int stream)
{
#ifdef HPI
  if(play[card][stream]==NULL) {
    return false;
  }
  if(play[card][stream]->getState()==RDHPIPlayStream::Playing) {
    play[card][stream]->pause();
  }
  play[card][stream]->disconnect();
  play[card][stream]->closeWave();
  delete play[card][stream];
  play[card][stream]=NULL;
  return true;
#else
  return false;
#endif  // HPI
}


bool MainObject::hpiPlaybackPosition(int card,int stream,unsigned pos)
{
#ifdef HPI
  if(play[card][stream]==NULL) {
    return false;
  }
  return play[card][stream]->
    setPosition((unsigned)((double)play[card][stream]->getSamplesPerSec()*
		      (double)pos/1000.0));
#else
  return false;
#endif  // HPI
}


bool MainObject::hpiPlay(int card,int stream,int length,int speed,bool pitch,
			 bool rates)
{
#ifdef HPI
  if(play[card][stream]==NULL) {
    return false;
  }
  if(!play[card][stream]->setSpeed(speed,pitch,rates)) {
    return false;
  }
  play[card][stream]->setPlayLength(length);
  return play[card][stream]->play();
#else
  return false;
#endif  // HPI
}


bool MainObject::hpiStopPlayback(int card,int stream)
{
#ifdef HPI
  if(play[card][stream]==NULL) {
    return false;
  }
  play[card][stream]->pause();
  return true;
#else
  return false;
#endif  // HPI
}


bool MainObject::hpiTimescaleSupported(int card)
{
#ifdef HPI
  return sound_card->haveTimescaling(card);
#else
  return false;
#endif  // HPI
}


bool MainObject::hpiLoadRecord(int card,int stream,int coding,int chans,
			       int samprate,int bitrate,QString wavename)
{
#ifdef HPI
  record[card][stream]=new RDHPIRecordStream(sound_card);
  connect(record[card][stream],SIGNAL(stateChanged(int,int,int)),
	  this,SLOT(stateRecordUpdate(int,int,int)));
  record[card][stream]->setCard(card);
  record[card][stream]->setStream(stream);
  record[card][stream]->nameWave(wavename);
  record[card][stream]->setChannels(chans);
  record[card][stream]->setSamplesPerSec(samprate);
  if(coding==0) {                 // PCM16
    record[card][stream]->setFormatTag(WAVE_FORMAT_PCM);
    record[card][stream]->setBitsPerSample(16);
  }
  if((coding>=1)&&(coding<=2)) {  // MPEG-1
    record[card][stream]->setFormatTag(WAVE_FORMAT_MPEG);
    record[card][stream]->setHeadLayer(coding);
    record[card][stream]->setHeadBitRate(bitrate);
    record[card][stream]->setMextChunk(true);
    switch(chans) {
	case 1:
	  record[card][stream]->setHeadMode(ACM_MPEG_SINGLECHANNEL);
	  break;
	case 2:
	  record[card][stream]->setHeadMode(ACM_MPEG_STEREO);
	  break;
	default:
	  delete record[card][stream];
	  record[card][stream]=NULL;
	  return false;
    }
    record[card][stream]->setHeadFlags(ACM_MPEG_ID_MPEG1);
  }
  if(coding>2) {
    delete record[card][stream];
    record[card][stream]=NULL;
    return false;
  }
  record[card][stream]->setBextChunk(true);
  record[card][stream]->setCartChunk(true);
  record[card][stream]->setLevlChunk(true);
  if(record[card][stream]->createWave()!=RDHPIRecordStream::Ok) {
    delete record[card][stream];
    record[card][stream]=NULL;
    return false;
  }
  chown((const char *)wavename,rd_config->uid(),rd_config->gid());
  if(!record[card][stream]->recordReady()) {
    delete record[card][stream];
    record[card][stream]=NULL;
    return false;
  }
  return true;
#else
  return false;
#endif  // HPI
}


bool MainObject::hpiUnloadRecord(int card,int stream,unsigned *len)
{
#ifdef HPI
  if(record[card][stream]==NULL) {
    return false;
  }
  if(record[card][stream]->getState()==RDHPIRecordStream::Recording) {
    record[card][stream]->pause();
  }
  record[card][stream]->disconnect();
  *len=record[card][stream]->samplesRecorded();
  record[card][stream]->closeWave();
  delete record[card][stream];
  record[card][stream]=NULL;
  return true;
#else
  return false;
#endif  // HPI
}


bool MainObject::hpiRecord(int card,int stream,int length,int thres)
{
#ifdef HPI
  if(record[card][stream]==NULL) {
    return false;
  }
  if(thres!=0) {
    if(record[card][stream]->haveInputVOX()) {
      record[card][stream]->setInputVOX(thres);
    }
    else {
      return false;
    }
  }
  record[card][stream]->setRecordLength(length);
  record[card][stream]->record();
  return true;
#else
  return false;
#endif  // HPI
}


bool MainObject::hpiStopRecord(int card,int stream)
{
#ifdef HPI
  if(record[card][stream]==NULL) {
    return false;
  }
  record[card][stream]->pause();
  record[card][stream]->setInputVOX(-10000);
  return true;
#else
  return false;
#endif  // HPI
}


bool MainObject::hpiSetClockSource(int card,int src)
{
#ifdef HPI
  return sound_card->setClockSource(card,(RDHPISoundCard::ClockSource)src);
#else
  return false;
#endif  // HPI
}


bool MainObject::hpiSetInputVolume(int card,int stream,int level)
{
#ifdef HPI
  sound_card->setInputVolume(card,stream,level);
  return true;
#else
  return false;
#endif  // HPI
}


bool MainObject::hpiSetOutputVolume(int card,int stream,int port,int level)
{
#ifdef HPI
  sound_card->setOutputVolume(card,stream,port,level);
  return true;
#else
  return false;
#endif  // HPI
}


bool MainObject::hpiFadeOutputVolume(int card,int stream,int port,int level,
				     int length)
{
#ifdef HPI
  sound_card->fadeOutputVolume(card,stream,port,level,length);
  return true;
#else
  return false;
#endif  // HPI
}


bool MainObject::hpiSetInputLevel(int card,int port,int level)
{
#ifdef HPI
  sound_card->setInputLevel(card,port,level);
  return true;
#else
  return false;
#endif  // HPI
}


bool MainObject::hpiSetOutputLevel(int card,int port,int level)
{
#ifdef HPI
  sound_card->setOutputLevel(card,port,level);
  return true;
#else
  return false;
#endif  // HPI
}


bool MainObject::hpiSetInputMode(int card,int stream,int mode)
{
#ifdef HPI
  switch(mode) {
      case 0:
	sound_card->setInputMode(card,stream,RDHPISoundCard::Normal);
	break;

      case 1:
	sound_card->setInputMode(card,stream,RDHPISoundCard::Swap);
	break;

      case 2:
	sound_card->setInputMode(card,stream,RDHPISoundCard::LeftOnly);
	break;

      case 3:
	sound_card->setInputMode(card,stream,RDHPISoundCard::RightOnly);
	break;

      default:
	return false;
  }
  return true;
#else
  return false;
#endif  // HPI
}


bool MainObject::hpiSetOutputMode(int card,int stream,int mode)
{
#ifdef HPI
  switch(mode) {
      case 0:
	sound_card->setOutputMode(card,stream,RDHPISoundCard::Normal);
	break;

      case 1:
	sound_card->setOutputMode(card,stream,RDHPISoundCard::Swap);
	break;

      case 2:
	sound_card->setOutputMode(card,stream,RDHPISoundCard::LeftOnly);
	break;

      case 3:
	sound_card->setOutputMode(card,stream,RDHPISoundCard::RightOnly);
	break;

      default:
	return false;
  }
  return true;
#else
  return false;
#endif  // HPI
}


bool MainObject::hpiSetInputVoxLevel(int card,int stream,int level)
{
#ifdef HPI
  sound_card->setInputStreamVOX(card,stream,level);
  return true;
#else
  return false;
#endif  // HPI
}


bool MainObject::hpiSetInputType(int card,int port,int type)
{
#ifdef HPI
  switch(type) {
      case 0:
	sound_card->setInputPortMux(card,port,RDHPISoundCard::LineIn);
	break;

      case 1:
	sound_card->setInputPortMux(card,port,RDHPISoundCard::AesEbuIn);
	break;

      default:
	return false;
  }
  return true;
#else
  return false;
#endif  // HPI
}


bool MainObject::hpiGetInputStatus(int card,int port)
{
#ifdef HPI
  if(sound_card->getInputPortError(card,port)!=0) {
    return false;
  }
  return true;
#else
  return false;
#endif  // HPI
}


bool MainObject::hpiGetInputMeters(int card,int port,short levels[2])
{
#ifdef HPI
  return sound_card->inputStreamMeter(card,port,levels);
#else
  return false;
#endif  // HPI
}


bool MainObject::hpiGetOutputMeters(int card,int port,short levels[2])
{
#ifdef HPI
  if(rd_config->useStreamMeters()) {
    //
    //  This is UGLY, but needed to semi-support cards (like the ASI4215)
    //  that lack output port metering.
    //
    for(int i=0;i<RD_MAX_STREAMS;i++) {
      if(sound_card->getOutputVolume(card,i,port)>-10000) {
	return sound_card->outputStreamMeter(card,i,levels);
      }
    }
    levels[0]=-10000;
    levels[1]=-10000;
    return true;
  }
  return sound_card->outputPortMeter(card,port,levels);
#else
  return false;
#endif  // HPI
}


bool MainObject::hpiGetStreamOutputMeters(int card,int stream,short levels[2])
{
#ifdef HPI
  return sound_card->outputStreamMeter(card,stream,levels);
#else
  return false;
#endif  // HPI
}


void MainObject::hpiGetOutputPosition(int card,unsigned *pos)
{
#ifdef HPI
  for(int i=0;i<RD_MAX_STREAMS;i++) {
    if(play[card][i]==NULL) {
      pos[i]=0;
    }
    else {
      pos[i]=1000*(unsigned long long)play[card][i]->currentPosition()/
	play[card][i]->getSamplesPerSec();
    }
  }
#endif  // HPI
  return;
}


bool MainObject::hpiSetPassthroughLevel(int card,int in_port,int out_port,
					int level)
{
#ifdef HPI
  return sound_card->setPassthroughVolume(card,in_port,out_port,level);
#else
  return false;
#endif  // HPI
}
