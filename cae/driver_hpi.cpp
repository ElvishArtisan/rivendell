// driver_hpi.cpp
//
// caed(8) driver for AudioScience HPI audio devices.
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

#include <rdconf.h>
#include "driver_hpi.h"

DriverHpi::DriverHpi(QObject *parent)
  : Driver(RDStation::Hpi,parent)
{
#ifdef HPI
  for(int i=0;i<RD_MAX_CARDS;i++) {
    for(int j=0;j<RD_MAX_STREAMS;j++) {
      d_record_streams[i][j]=NULL;
      d_play_streams[i][j]=NULL;
    }
  }
  d_sound_card=new RDHPISoundCard(rda->config(),this);
#endif  // HPI
}


DriverHpi::~DriverHpi()
{
#ifdef HPI
  for(int i=0;i<RD_MAX_CARDS;i++) {
    for(int j=0;j<RD_MAX_STREAMS;j++) {
      if(d_record_streams[i][j]!=NULL) {
	delete d_record_streams[i][j];
      }
      if(d_play_streams[i][j]!=NULL) {
	delete d_play_streams[i][j];
      }
    }
  }
  delete d_sound_card;
#endif  // HPI
}


QString DriverHpi::version() const
{
#ifdef HPI
  if(d_sound_card==NULL) {
    return tr("not active");
  }
  RDHPIInformation *info=d_sound_card->hpiInformation(0);
  if(info->hpiVersion()==0) {
    return tr("not active");
  }
  return QString::asprintf("%d.%02d.%02d",info->hpiMajorVersion(),
			   info->hpiMinorVersion(),info->hpiPointVersion());
#else
  return tr("not enabled");
#endif  // HPI
}


bool DriverHpi::initialize(unsigned *next_cardnum)
{
#ifdef HPI
  d_sound_card->setFadeProfile(RD_FADE_TYPE);
  for(int i=0;i<d_sound_card->getCardQuantity();i++) {
    rda->station()->setCardDriver(i,RDStation::Hpi);
    rda->station()->setCardName(i,d_sound_card->getCardDescription(i));
    rda->station()->setCardInputs(i,d_sound_card->getCardInputPorts(i));
    rda->station()->setCardOutputs(i,d_sound_card->getCardOutputPorts(i));
    addCard(*next_cardnum);
    (*next_cardnum)++;
  }
  return d_sound_card->getCardQuantity()>0;
#else
  return false;
#endif  // HPI
}


int DriverHpi::inputPortQuantity(int card) const
{
#ifdef HPI
  return d_sound_card->getCardInputPorts(card);
#else
  return 0;
#endif  // HPI
}


int DriverHpi::outputPortQuantity(int card) const
{
#ifdef HPI
  return d_sound_card->getCardOutputPorts(card);
#else
  return 0;
#endif  // HPI
}


bool DriverHpi::loadPlayback(int card,QString wavename,int *stream)
{
#ifdef HPI
  RDHPIPlayStream *playstream=new RDHPIPlayStream(d_sound_card);
  playstream->setCard(card);
  if(playstream->openWave(wavename)!=RDHPIPlayStream::Ok) {
    rda->syslog(LOG_DEBUG,"hpiLoadPlayback(%s) openWave() failed to open file",
		wavename.toUtf8().constData());
    delete playstream;
    return false;
  }
  *stream=playstream->getStream();
  d_play_streams[card][*stream]=playstream;
  connect(d_play_streams[card][*stream],SIGNAL(stateChanged(int,int,int)),
  	  this,SLOT(statePlayUpdate(int,int,int)));

  return true;
#else
  return false;
#endif  // HPI
}


bool DriverHpi::unloadPlayback(int card,int stream)
{
#ifdef HPI
  if(d_play_streams[card][stream]==NULL) {
    return false;
  }
  if(d_play_streams[card][stream]->getState()==RDHPIPlayStream::Playing) {
    d_play_streams[card][stream]->pause();
  }
  d_play_streams[card][stream]->disconnect();
  d_play_streams[card][stream]->closeWave();
  delete d_play_streams[card][stream];
  d_play_streams[card][stream]=NULL;

  return true;
#else
  return false;
#endif  // HPI
}


bool DriverHpi::playbackPosition(int card,int stream,unsigned pos)
{
#ifdef HPI
  if(d_play_streams[card][stream]==NULL) {
    return false;
  }
  return d_play_streams[card][stream]->
    setPosition((unsigned)((double)d_play_streams[card][stream]->
			   getSamplesPerSec()*(double)pos/1000.0));
#else
  return false;
#endif  // HPI
}


bool DriverHpi::play(int card,int stream,int length,int speed,bool pitch,
		     bool rates)
{
#ifdef HPI
  if(d_play_streams[card][stream]==NULL) {
    return false;
  }
  if(!d_play_streams[card][stream]->setSpeed(speed,pitch,rates)) {
    return false;
  }
  d_play_streams[card][stream]->setPlayLength(length);
  return d_play_streams[card][stream]->play();
#else
  return false;
#endif  // HPI
}


bool DriverHpi::stopPlayback(int card,int stream)
{
#ifdef HPI
  if(d_play_streams[card][stream]==NULL) {
    return false;
  }
  d_play_streams[card][stream]->pause();

  return true;
#else
  return false;
#endif  // HPI
}


bool DriverHpi::timescaleSupported(int card)
{
#ifdef HPI
  return d_sound_card->haveTimescaling(card);
#else
  return false;
#endif  // HPI
}


bool DriverHpi::loadRecord(int card,int port,int coding,int chans,int samprate,
			   int bitrate,QString wavename)
{
#ifdef HPI
  d_record_streams[card][port]=new RDHPIRecordStream(d_sound_card);
  connect(d_record_streams[card][port],SIGNAL(stateChanged(int,int,int)),
	  this,SLOT(stateRecordUpdate(int,int,int)));
  d_record_streams[card][port]->setCard(card);
  d_record_streams[card][port]->setStream(port);
  d_record_streams[card][port]->nameWave(wavename);
  d_record_streams[card][port]->setChannels(chans);
  d_record_streams[card][port]->setSamplesPerSec(samprate);
  if(coding==0) {                 // PCM16
    d_record_streams[card][port]->setFormatTag(WAVE_FORMAT_PCM);
    d_record_streams[card][port]->setBitsPerSample(16);
  }
  if((coding==1)||(coding==2)) {  // MPEG-1
    d_record_streams[card][port]->setFormatTag(WAVE_FORMAT_MPEG);
    d_record_streams[card][port]->setHeadLayer(coding);
    d_record_streams[card][port]->setHeadBitRate(bitrate);
    d_record_streams[card][port]->setMextChunk(true);
    switch(chans) {
    case 1:
      d_record_streams[card][port]->setHeadMode(ACM_MPEG_SINGLECHANNEL);
      break;

    case 2:
      d_record_streams[card][port]->setHeadMode(ACM_MPEG_STEREO);
      break;

    default:
      delete d_record_streams[card][port];
      d_record_streams[card][port]=NULL;
      return false;
    }
    d_record_streams[card][port]->setHeadFlags(ACM_MPEG_ID_MPEG1);
  }
  if(coding==4) {                 // PCM24
    d_record_streams[card][port]->setFormatTag(WAVE_FORMAT_PCM);
    d_record_streams[card][port]->setBitsPerSample(24);
  }
  if(coding>4) {
    delete d_record_streams[card][port];
    d_record_streams[card][port]=NULL;
    return false;
  }
  d_record_streams[card][port]->setBextChunk(true);
  d_record_streams[card][port]->setCartChunk(true);
  d_record_streams[card][port]->setLevlChunk(true);
  if(d_record_streams[card][port]->createWave()!=RDHPIRecordStream::Ok) {
    delete d_record_streams[card][port];
    d_record_streams[card][port]=NULL;
    return false;
  }
  RDCheckExitCode(rda->config(),"hpiLoadRecord() chown",
		  chown(wavename.toUtf8(),rda->config()->uid(),rda->config()->gid()));
  if(!d_record_streams[card][port]->recordReady()) {
    delete d_record_streams[card][port];
    d_record_streams[card][port]=NULL;
    return false;
  }

  return true;
#else
  return false;
#endif  // HPI
}


bool DriverHpi::unloadRecord(int card,int port,unsigned *len_frames)
{
#ifdef HPI
  if(d_record_streams[card][port]==NULL) {
    return false;
  }
  if(d_record_streams[card][port]->getState()==RDHPIRecordStream::Recording) {
    d_record_streams[card][port]->pause();
  }
  d_record_streams[card][port]->disconnect();
  *len_frames=d_record_streams[card][port]->samplesRecorded();
  d_record_streams[card][port]->closeWave();
  delete d_record_streams[card][port];
  d_record_streams[card][port]=NULL;

  return true;
#else
  return false;
#endif  // HPI
}


bool DriverHpi::record(int card,int port,int length,int thres)
{
#ifdef HPI
  if(d_record_streams[card][port]==NULL) {
    return false;
  }
  if(thres!=0) {
    if(d_record_streams[card][port]->haveInputVOX()) {
      d_record_streams[card][port]->setInputVOX(thres);
    }
    else {
      return false;
    }
  }
  d_record_streams[card][port]->setRecordLength(length);
  d_record_streams[card][port]->record();

  return true;
#else
  return false;
#endif  // HPI
}


bool DriverHpi::stopRecord(int card,int port)
{
#ifdef HPI
  if(d_record_streams[card][port]==NULL) {
    return false;
  }
  d_record_streams[card][port]->pause();
  d_record_streams[card][port]->setInputVOX(-10000);

  return true;
#else
  return false;
#endif  // HPI
}


bool DriverHpi::setClockSource(int card,int src)
{
#ifdef HPI
  return d_sound_card->setClockSource(card,(RDHPISoundCard::ClockSource)src);
#else
  return false;
#endif  // HPI
}


bool DriverHpi::setInputVolume(int card,int stream,int level)
{
#ifdef HPI
  d_sound_card->setInputVolume(card,stream,level);

  return true;
#else
  return false;
#endif  // HPI
}


bool DriverHpi::setOutputVolume(int card,int stream,int port,int level)
{
#ifdef HPI
  d_sound_card->setOutputVolume(card,stream,port,level);

  return true;
#else
  return false;
#endif  // HPI
}


bool DriverHpi::fadeOutputVolume(int card,int stream,int port,int level,
				 int length)
{
#ifdef HPI
  d_sound_card->fadeOutputVolume(card,stream,port,level,length);

  return true;
#else
  return false;
#endif  // HPI
}


bool DriverHpi::setInputLevel(int card,int port,int level)
{
#ifdef HPI
  d_sound_card->setInputLevel(card,port,level);

  return true;
#else
  return false;
#endif  // HPI
}


bool DriverHpi::setOutputLevel(int card,int port,int level)
{
#ifdef HPI
  d_sound_card->setOutputLevel(card,port,level);

  return true;
#else
  return false;
#endif  // HPI
}


bool DriverHpi::setInputMode(int card,int stream,int mode)
{
#ifdef HPI
  switch(mode) {
  case 0:
    d_sound_card->setInputMode(card,stream,RDHPISoundCard::Normal);
    break;

  case 1:
    d_sound_card->setInputMode(card,stream,RDHPISoundCard::Swap);
    break;

  case 2:
    d_sound_card->setInputMode(card,stream,RDHPISoundCard::LeftOnly);
    break;

  case 3:
    d_sound_card->setInputMode(card,stream,RDHPISoundCard::RightOnly);
    break;

  default:
    return false;
  }

  return true;
#else
  return false;
#endif  // HPI
}


bool DriverHpi::setOutputMode(int card,int stream,int mode)
{
#ifdef HPI
  switch(mode) {
  case 0:
    d_sound_card->setOutputMode(card,stream,RDHPISoundCard::Normal);
    break;

  case 1:
    d_sound_card->setOutputMode(card,stream,RDHPISoundCard::Swap);
    break;

  case 2:
    d_sound_card->setOutputMode(card,stream,RDHPISoundCard::LeftOnly);
    break;

  case 3:
    d_sound_card->setOutputMode(card,stream,RDHPISoundCard::RightOnly);
    break;

  default:
    return false;
  }

  return true;
#else
  return false;
#endif  // HPI
}


bool DriverHpi::setInputVoxLevel(int card,int stream,int level)
{
#ifdef HPI
  d_sound_card->setInputStreamVOX(card,stream,level);

  return true;
#else
  return false;
#endif  // HPI
}


bool DriverHpi::setInputType(int card,int port,int type)
{
#ifdef HPI
  switch(type) {
  case 0:
    d_sound_card->setInputPortMux(card,port,RDHPISoundCard::LineIn);
    break;

  case 1:
    d_sound_card->setInputPortMux(card,port,RDHPISoundCard::AesEbuIn);
    break;

  default:
    return false;
  }

  return true;
#else
  return false;
#endif  // HPI
}


bool DriverHpi::getInputStatus(int card,int port)
{
#ifdef HPI
  return d_sound_card->getInputPortError(card,port)==0;
#else
  return false;
#endif  // HPI
}


bool DriverHpi::getInputMeters(int card,int port,short levels[2])
{
#ifdef HPI
  return d_sound_card->inputStreamMeter(card,port,levels);
#else
  return false;
#endif  // HPI
}


bool DriverHpi::getOutputMeters(int card,int port,short levels[2])
{
#ifdef HPI
  return d_sound_card->outputPortMeter(card,port,levels);
#else
  return false;
#endif  // HPI
}


bool DriverHpi::getStreamOutputMeters(int card,int stream,short levels[2])
{
#ifdef HPI
  return d_sound_card->outputStreamMeter(card,stream,levels);
#else
  return false;
#endif  // HPI
}


bool DriverHpi::setPassthroughLevel(int card,int in_port,int out_port,int level)
{
#ifdef HPI
  return d_sound_card->setPassthroughVolume(card,in_port,out_port,level);
#else
  return false;
#endif  // HPI
}


void DriverHpi::getOutputPosition(int card,unsigned *pos)
{
#ifdef HPI
  for(int i=0;i<RD_MAX_STREAMS;i++) {
    if(d_play_streams[card][i]==NULL) {
      pos[i]=0;
    }
    else {
      pos[i]=
	1000*(unsigned long long)d_play_streams[card][i]->currentPosition()/
	d_play_streams[card][i]->getSamplesPerSec();
    }
  }
#endif  // HPI
}
