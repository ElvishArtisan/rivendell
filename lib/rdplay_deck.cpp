// rdplay_deck.cpp
//
// Abstract a Rivendell Playback Deck
//
//   (C) Copyright 2003-2023 Fred Gleason <fredg@paravelsystems.com>
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

#include <QSignalMapper>

#include "rdapplication.h"
#include "rdplay_deck.h"

RDPlayDeck::RDPlayDeck(RDCae *cae,int id,QObject *parent)
  : QObject(parent)
{
  play_id=id;
  play_state=RDPlayDeck::Stopped;
  play_start_time=QTime();
  play_owner=-1;
  play_last_start_position=0;
  play_serial=-1;
  play_audio_length=0;
  play_channel=-1;
  play_hook_mode=false;

  play_cut_gain=0;
  play_duck_level=0;
  play_duck_gain[0]=0;
  play_duck_gain[1]=0;
  play_duck_up=RDPLAYDECK_DUCKUP_LENGTH;
  play_duck_down=RDPLAYDECK_DUCKDOWN_LENGTH;
  play_duck_up_point=0;
  play_duck_down_state=false;
  play_fade_down_state=false;

  //
  // CAE Connection
  //
  play_cae=cae;
  connect(play_cae,SIGNAL(playStarted(int)),this,SLOT(playStartedData(int)));
  connect(play_cae,SIGNAL(playbackStopped(int)),
	  this,SLOT(playbackStoppedData(int)));
  play_cart=NULL;
  play_cut=NULL;
  play_card=-1;
  play_stream=-1;

  //
  // Timers
  //
  QSignalMapper *mapper=new QSignalMapper(this);
  connect(mapper,SIGNAL(mapped(int)),this,SLOT(pointTimerData(int)));
  for(int i=0;i<3;i++) {
    play_point_timer[i]=new QTimer(this);
    play_point_timer[i]->setSingleShot(true);
    connect(play_point_timer[i],SIGNAL(timeout()),mapper,SLOT(map()));
    mapper->setMapping(play_point_timer[i],i);
  }
  play_position_timer=new QTimer(this);
  connect(play_position_timer,SIGNAL(timeout()),
	  this,SLOT(positionTimerData()));
  play_fade_timer=new QTimer(this);
  play_fade_timer->setSingleShot(true);
  connect(play_fade_timer,SIGNAL(timeout()),this,SLOT(fadeTimerData()));
  play_stop_timer=new QTimer(this);
  play_stop_timer->setSingleShot(true);
  connect(play_stop_timer,SIGNAL(timeout()),this,SLOT(stop()));
  play_duck_timer=new QTimer(this);
  play_duck_timer->setSingleShot(true);
  connect(play_duck_timer,SIGNAL(timeout()),this,SLOT(duckTimerData()));
}


RDPlayDeck::~RDPlayDeck()
{
  if(play_state!=RDPlayDeck::Stopped) {
    play_cae->stopPlayback(play_serial);
    //    play_cae->unloadPlay(play_handle);
  }
}


int RDPlayDeck::id() const
{
  return play_id;
}


void RDPlayDeck::setId(int id)
{
  play_id=id;
}


int RDPlayDeck::serialNumber() const
{
  return play_serial;
}


int RDPlayDeck::owner() const
{
  return play_owner;
}


void RDPlayDeck::setOwner(int owner)
{
  play_owner=owner;
}


RDCart *RDPlayDeck::cart() const
{
  return play_cart;
}


bool RDPlayDeck::setCart(RDLogLine *logline,bool rotate)
{
  play_timescale_active=logline->timescalingActive();
  if((play_cart!=NULL)&&(rotate||play_cart->number()!=logline->cartNumber())) {
    delete play_cart;
    delete play_cut;
    play_cart=NULL;
    play_cut=NULL;
  }
  if(play_cart==NULL) {
    StopTimers();
    play_cart=new RDCart(logline->cartNumber());
    if(!play_cart->exists()) {
      delete play_cart;
      play_cart=NULL;
      return false;
    }

    QString cutname=logline->cutName();
    //
    // FIXME: We need to handle the 'cut no longer valid' case better!
    //
    //if(play_cart->selectCut(&cutname)) {     This fixes problems with cuts of different length in one cart.
    //  logline->setCutName(cutname);          We do not need to select a cut, because it is done immediatly before
    //}                                        this method is called.
    if(cutname.isEmpty()) {
      return false;
    }
    play_cut=new RDCut(cutname);
    if(!play_cut->exists()) {
      delete play_cut;
      play_cut=NULL;
      return false;
    }
  }
  if(logline->startPoint(RDLogLine::LogPointer)<0) {
    // Use values from the library
    play_forced_length=logline->forcedLength();
    play_audio_point[0]=play_cut->startPoint(RDLogLine::CartPointer);
    play_audio_point[1]=play_cut->endPoint();
  }
  else {
    // Use values from the log
    play_forced_length=logline->effectiveLength();
    play_audio_point[0]=logline->startPoint(RDLogLine::LogPointer);
    play_audio_point[1]=logline->endPoint();
  }
  if(logline->endPoint(RDLogLine::LogPointer)>=0) {
    play_forced_length=logline->effectiveLength();
    play_audio_point[0]=logline->startPoint();
    play_audio_point[1]=logline->endPoint(RDLogLine::LogPointer);
  }
  if(play_timescale_active) {
    play_timescale_speed=
      (int)(RD_TIMESCALE_DIVISOR*(double)(play_audio_point[1]-
					  play_audio_point[0])/
      (double)play_forced_length);
    if((((double)play_timescale_speed)<
	(RD_TIMESCALE_DIVISOR*RD_TIMESCALE_MIN))||
       (((double)play_timescale_speed)>
	(RD_TIMESCALE_DIVISOR*RD_TIMESCALE_MAX))) {
      play_timescale_speed=(int)RD_TIMESCALE_DIVISOR;
      play_timescale_active=false;
    }
  }
  else {
    play_timescale_speed=(int)RD_TIMESCALE_DIVISOR;
  }
  play_audio_length=play_audio_point[1]-play_audio_point[0];
  if(logline->segueStartPoint(RDLogLine::AutoPointer)<0) {
    play_point_value[RDPlayDeck::Segue][0]=
      (int)((double)play_cut->segueStartPoint());
    play_point_value[RDPlayDeck::Segue][1]=
      (int)((double)play_cut->segueEndPoint());
  }
  else {
    play_point_value[RDPlayDeck::Segue][0]=
      (int)((double)logline->segueStartPoint(RDLogLine::AutoPointer));
    play_point_value[RDPlayDeck::Segue][1]=
      (int)((double)logline->segueEndPoint(RDLogLine::AutoPointer));
  }
  play_point_gain=logline->segueGain();
  play_point_value[RDPlayDeck::Hook][0]=
    (int)((double)play_cut->hookStartPoint());
  play_point_value[RDPlayDeck::Hook][1]=
    (int)((double)play_cut->hookEndPoint());
  logline->setHookStartPoint(play_point_value[RDPlayDeck::Hook][0]);
  logline->setHookEndPoint(play_point_value[RDPlayDeck::Hook][1]);
  play_point_value[RDPlayDeck::Talk][0]=
    (int)((double)play_cut->talkStartPoint()*
	  (RD_TIMESCALE_DIVISOR/(double)play_timescale_speed));
  play_point_value[RDPlayDeck::Talk][1]=
    (int)((double)play_cut->talkEndPoint()*
	  (RD_TIMESCALE_DIVISOR/(double)play_timescale_speed));
  logline->setTalkStartPoint(play_point_value[RDPlayDeck::Talk][0]);
  logline->setTalkEndPoint(play_point_value[RDPlayDeck::Talk][1]);
  if(logline->fadeupPoint(RDLogLine::LogPointer)<0) {
    play_fade_point[0]=play_cut->fadeupPoint();
    play_fade_gain[0]=RD_FADE_DEPTH;
  }
  else {
    play_fade_point[0]=logline->fadeupPoint(RDLogLine::LogPointer);
    play_fade_gain[0]=logline->fadeupGain();
  }
  if(logline->fadedownPoint(RDLogLine::LogPointer)<0) {
    play_fade_point[1]=play_cut->fadedownPoint();
    play_fade_gain[1]=RD_FADE_DEPTH;
  }
  else {
    play_fade_point[1]=logline->fadedownPoint(RDLogLine::LogPointer);
    play_fade_gain[1]=logline->fadedownGain();
  }
  play_duck_gain[0]=logline->duckUpGain();
  play_duck_gain[1]=logline->duckDownGain();
  /*
  if(play_state!=RDPlayDeck::Paused) {
    if(!play_cae->loadPlay(play_card,play_cut->cutName(),
			   &play_stream,&play_handle)) {
      return false;
    }
  }
  */
  play_state=RDPlayDeck::Stopped;
  return true;
}


RDCut *RDPlayDeck::cut() const
{
  return play_cut;
}


bool RDPlayDeck::playable() const
{
  if(play_serial<0) {
    return false;
  }
  return true;
}


int RDPlayDeck::card() const
{
  return play_card;
}


void RDPlayDeck::setCard(int card_num)
{
  play_card=card_num;
}


int RDPlayDeck::stream() const
{
  return play_stream;
}


int RDPlayDeck::port() const
{
  return play_port;
}


void RDPlayDeck::setPort(int port_num)
{
  play_port=port_num;
}


int RDPlayDeck::channel() const
{
  return play_channel;
}


void RDPlayDeck::setChannel(int chan)
{
  play_channel=chan;
}


RDPlayDeck::State RDPlayDeck::state() const
{
  return play_state;
}


QTime RDPlayDeck::startTime() const
{
  return play_start_time;
}


int RDPlayDeck::currentPosition() const
{
  switch(play_state) {
      case RDPlayDeck::Playing:
	return play_start_position+
	  play_start_time.msecsTo(QTime::currentTime());

      case RDPlayDeck::Paused:
	return play_current_position+POSITION_INTERVAL;

      default:
	return play_start_position;
  }
  return 0;
}


int RDPlayDeck::lastStartPosition() const
{
  return play_last_start_position;
}


void RDPlayDeck::clear()
{
  StopTimers();
  switch(play_state) {
      case RDPlayDeck::Playing:
      case RDPlayDeck::Stopping:
	stop();
	break;

      case RDPlayDeck::Paused:
	//	play_cae->unloadPlay(play_handle);
	emit stateChanged(play_id,RDPlayDeck::Stopped);
	break;

      default:
	emit stateChanged(play_id,RDPlayDeck::Stopped);
	break;
  }
}


void RDPlayDeck::reset()
{
  StopTimers();
  switch(play_state) {
      case RDPlayDeck::Playing:
      case RDPlayDeck::Stopping:
	play_cae->stopPlayback(play_serial);

      case RDPlayDeck::Paused:
	//	play_cae->unloadPlay(play_handle);
	break;

      default:
	break;
  }
  play_state=RDPlayDeck::Stopped;
}


QString RDPlayDeck::dumpCutPoints() const
{
  QString ret;

  ret=QString::asprintf("play_audio_point: start: %d  end: %d ",
			play_audio_point[0],play_audio_point[1]);
  if(play_stop_timer->isActive()) {
    ret+=QString::asprintf("play_stop_timer: %d",play_stop_timer->interval()); 
  }
  else {
    ret+="play_stop_timer: inactive";
  }
  ret+="\n";

  ret+=QString::asprintf("play_point_value[SEGUE]: start: %d  end: %d ",
			 play_point_value[0][0],play_point_value[0][1]);
  if(play_point_timer[0]->isActive()) {
    ret+=QString::asprintf("play_point_timer[SEGUE]: %d",
			   play_point_timer[0]->interval());
  }
  else {
    ret+="play_point_timer[SEGUE]: inactive";
  }
  ret+="\n";

  ret+=QString::asprintf("play_point_value[TALK]: start: %d  end: %d ",
			 play_point_value[1][0],play_point_value[1][1]);
  if(play_point_timer[1]->isActive()) {
    ret+=QString::asprintf("play_point_timer[TALK]: %d",
			   play_point_timer[1]->interval());
  }
  else {
    ret+="play_point_timer[TALK]: inactive";
  }
  ret+="\n";

  ret+=QString::asprintf("play_point_value[HOOK]: start: %d  end: %d ",
			 play_point_value[2][0],play_point_value[2][1]);
  if(play_point_timer[2]->isActive()) {
    ret+=QString::asprintf("play_point_timer[HOOK]: %d",
			   play_point_timer[2]->interval());
  }
  else {
    ret+="play_point_timer[HOOK]: inactive";
  }
  ret+="\n";

  return ret;
}


void RDPlayDeck::play(unsigned pos,int segue_start,int segue_end,
		      int duck_up_end)
{
  int fadeup;
  int start_volume=RD_MUTE_DEPTH;
  int fade_volume=RD_MUTE_DEPTH;
  int fade_length=0;

  play_hook_mode=false;
  play_cut_gain=play_cut->playGain();

  play_ducked=0;
  if(duck_up_end==-1) { //ducked until stop (for recording in voice tracker)
    play_ducked=play_duck_gain[0];
    play_duck_up_point=0;
  }
  else {
    play_duck_up_point=duck_up_end-play_duck_up;
  }
  if(play_duck_up_point<0)
    play_duck_up_point=0;
  else
    play_ducked=play_duck_gain[0];
  /*
  if(play_serial<0) {
    return;
  }
  */
  if(segue_start>=0) {
    play_point_value[RDPlayDeck::Segue][0]=segue_start;
  }
  if(segue_end>=0) {
    play_point_value[RDPlayDeck::Segue][1]=segue_end;
  }
  play_start_position=pos;
  play_current_position=pos;
  play_last_start_position=play_start_position;
  stop_called=false;
  pause_called=false;
  //  play_cae->positionPlay(play_serial,play_audio_point[0]+pos);
  //  play_cae->setPlayPortActive(play_card,play_port,play_stream);
  //  play_cae->setOutputVolume(play_card,play_stream,-1,RD_MUTE_DEPTH);
  if((play_fade_point[0]==-1)||(play_fade_point[0]==play_audio_point[0])||
     ((fadeup=play_fade_point[0]-play_audio_point[0]-pos)<=0)||
     (play_state==RDPlayDeck::Paused)) {
    if((play_fade_point[1]==-1)||((fadeup=pos-play_fade_point[1])<=0)||
       (play_state==RDPlayDeck::Paused)) {
      //play_cae->setOutputVolume(play_card,play_stream,play_port,
      //			play_ducked+play_cut_gain+play_duck_level);
      start_volume=play_ducked+play_cut_gain+play_duck_level;
      //play_cae->fadeOutputVolume(play_card,play_stream,play_port,
      //			 play_ducked+play_cut_gain+play_duck_level,10);
      fade_volume=play_ducked+play_cut_gain+play_duck_level;
      fade_length=10;
    }
    else {  // Fadedown event in progress, interpolate the gain accordingly
      int level=play_fade_gain[1]*((int)pos-play_fade_point[1])/
			(play_audio_point[1]-play_fade_point[1]);
      //play_cae->
      // setOutputVolume(play_card,play_stream,play_port,
      //		level+play_cut_gain+play_duck_level);
      start_volume=level+play_cut_gain+play_duck_level;
      //play_cae->fadeOutputVolume(play_card,play_stream,play_port,
      //			 play_fade_gain[1]+play_cut_gain+
      //			 play_duck_level,
      //			 play_audio_point[1]-(int)pos);
      fade_volume=play_duck_level;
      fade_length=play_audio_point[1]-(int)pos;
    }
  }
  else {  // FadeUp event in progress, interpolate the gain accordingly
    int level=(play_fade_gain[0]*fadeup/
		      (play_fade_point[0]-play_audio_point[0]));
    if (level>play_ducked) {
      //play_cae->
      //setOutputVolume(play_card,play_stream,play_port,
      //	      play_ducked+play_cut_gain+play_duck_level);
      start_volume=play_ducked+play_cut_gain+play_duck_level;
      //play_cae->fadeOutputVolume(play_card,play_stream,play_port,
      //			 play_ducked+play_cut_gain+play_duck_level,
      //			 fadeup);
      fade_volume=play_ducked+play_cut_gain+play_duck_level;
      fade_length=fadeup;
    }
    else {
      //play_cae->
      //  setOutputVolume(play_card,play_stream,play_port,
      //	      level+play_cut_gain+play_duck_level);
      start_volume=level+play_cut_gain+play_duck_level;
      //play_cae->fadeOutputVolume(play_card,play_stream,play_port,
      //		       play_ducked+play_cut_gain+play_duck_level,
      //		       fadeup);
      fade_volume=play_ducked+play_cut_gain+play_duck_level;
      fade_length=fadeup;
    }
  }
  /*
  play_cae->
    play(play_handle,
	 (int)(100000.0*(double)(play_audio_point[1]-play_audio_point[0]-pos)/
	 (double)play_timescale_speed),
	 play_timescale_speed,false);
  */
  printf("startPlayback(\"%s\",%d,%d,%d,%d,%d)\n",
	 play_cut->cutName().toUtf8().constData(),play_card,play_port,
	 play_audio_point[0]+pos,
	 play_audio_point[1],play_timescale_speed);
  play_serial=play_cae->startPlayback(play_cut->cutName(),play_card,play_port,
				      play_audio_point[0]+pos,
				      play_audio_point[1],play_timescale_speed,
				      start_volume);
  play_cae->fadeOutputVolume(play_serial,fade_volume,fade_length);
  play_start_time=QTime::currentTime();
  StartTimers(pos);
  play_state=RDPlayDeck::Playing;
  playStartedData(play_serial);
}


void RDPlayDeck::playHook()
{
  play(play_point_value[RDPlayDeck::Hook][0]-play_audio_point[0]);
  play_hook_mode=true;
}


void RDPlayDeck::pause()
{
  pause_called=true;
  play_state=RDPlayDeck::Paused;
  //  play_cae->stopPlay(play_handle);
}


void RDPlayDeck::stop()
{
  if((play_state!=RDPlayDeck::Playing)&&(play_state!=RDPlayDeck::Stopping)) {
    return;
  }
  if(pause_called) {
    play_state=RDPlayDeck::Stopped;
  }
  else {
    stop_called=true;
    play_state=RDPlayDeck::Stopping;
    //    play_cae->stopPlay(play_handle);
    play_cae->stopPlayback(play_serial);
  }
}


void RDPlayDeck::stop(int interval,int gain)
{
  int level;
  
  if(gain>play_point_gain) {
    play_point_gain=gain;
  }
  

  if((play_state!=RDPlayDeck::Playing)&&(play_state!=RDPlayDeck::Stopping)) {
    return;
  }
  if((interval<=0)||pause_called) {
    stop();
  }
  else {
    if(play_duck_gain[1]<0 && play_duck_down<interval && 
        (play_audio_point[1]-play_audio_point[0]-
        currentPosition())>play_duck_down) { // duck
      if(play_audio_point[0]+currentPosition()>play_fade_point[1]) {
        level=play_fade_gain[1]*((currentPosition()+play_audio_point[0])-
              play_fade_point[1])/(play_audio_point[1]-play_fade_point[1]);
      }
      else {
        level=0;
      }        
      if(level>play_duck_gain[1]){
	//play_cae->fadeOutputVolume(play_card,play_stream,play_port,
	//		       play_duck_gain[1]+play_cut_gain+play_duck_level,play_duck_down);
	play_cae->
	  fadeOutputVolume(play_serial,
	       play_duck_gain[1]+play_cut_gain+play_duck_level,play_duck_down);
        play_duck_timer->start(play_duck_down);
        play_duck_down_state=true;
        play_segue_interval=interval;
      }
    }
    else {
      if(play_point_gain!=0) {
        //play_cae->fadeOutputVolume(play_card,play_stream,play_port,
	//		       play_point_gain+play_cut_gain+play_duck_level,interval);
	play_cae->
	  fadeOutputVolume(play_serial,
		       play_point_gain+play_cut_gain+play_duck_level,interval);
      }
    }
    play_stop_timer->start(interval);
    stop_called=true;
    play_state=RDPlayDeck::Stopping;
  }
}




void RDPlayDeck::duckDown(int interval)
{
  if(play_duck_gain[1]<0) {
    //play_cae->fadeOutputVolume(play_card,play_stream,play_port,
    //	       play_duck_gain[1]+play_cut_gain+play_duck_level,play_duck_down);
    play_cae->
      fadeOutputVolume(play_serial,
	       play_duck_gain[1]+play_cut_gain+play_duck_level,play_duck_down);
    play_duck_timer->start(play_duck_down);
    play_duck_down_state=true;
    play_segue_interval=interval;

  }
}


void RDPlayDeck::duckVolume(int level,int fade)
{
  play_duck_level=level;
  if((state()==RDPlayDeck::Playing || state()==RDPlayDeck::Stopping) && fade>0) {
    //play_cae->fadeOutputVolume(play_card,play_stream,play_port,play_cut_gain+play_duck_level,fade);
    play_cae->fadeOutputVolume(play_serial,play_cut_gain+play_duck_level,fade);
  }
}


void RDPlayDeck::playStartedData(int serial)
{
  if(serial!=play_serial) {
    return;
  }
  play_position_timer->start(POSITION_INTERVAL);
  emit stateChanged(play_id,RDPlayDeck::Playing);
}


void RDPlayDeck::playbackStoppedData(int serial)
{ 
  if(serial!=play_serial) {
    return;
  }
  play_position_timer->stop();
  play_start_time=QTime();
  StopTimers();
  if(pause_called) {
    play_state=RDPlayDeck::Paused;
    emit stateChanged(play_id,RDPlayDeck::Paused);
  }
  else {
    //    play_cae->unloadPlay(play_handle);

    play_serial=-1;
    play_state=RDPlayDeck::Stopped;
    play_current_position=0;
    play_duck_down_state=false;
    play_fade_down_state=false;
    if(stop_called) {
      emit stateChanged(play_id,RDPlayDeck::Stopped);
    }
    else {
      emit stateChanged(play_id,RDPlayDeck::Finished);
    }
  }
}


void RDPlayDeck::pointTimerData(int point)
{
  switch(point) {
      case RDPlayDeck::Segue:
	if(play_point_state[point]) {
	  play_point_state[point]=false;
	  emit segueEnd(play_id);
	}
	else {
	  play_point_state[point]=true;
	  play_point_timer[point]->
	    start(play_point_value[point][1]-play_point_value[point][0]);
	  emit segueStart(play_id);
	}
	break;

      case RDPlayDeck::Hook:
	if(play_point_state[point]) {
	  play_point_state[point]=false;
	  emit hookEnd(play_id);
	}
	else {
	  play_point_state[point]=true;
	  play_point_timer[point]->
	    start(play_point_value[point][1]-play_point_value[point][0]);
	  emit hookStart(play_id);
	}
	break;

      case RDPlayDeck::Talk:
	if(play_point_state[point]) {
	  play_point_state[point]=false;
	  emit talkEnd(play_id);
	}
	else {
	  play_point_state[point]=true;
	  play_point_timer[point]->
	    start(play_point_value[point][1]-play_point_value[point][0]);
	  emit talkStart(play_id);
	}
	break;
  }
}


void RDPlayDeck::positionTimerData()
{
  play_current_position=
    play_start_position+play_start_time.msecsTo(QTime::currentTime());
  if(play_current_position<0) {       // Handle crossing midnight!
    play_current_position+=86400000;
  }
  if(play_hook_mode) {
    emit position(play_id,play_current_position-(play_point_value[RDPlayDeck::Hook][0]-play_audio_point[0]));
  }
  else {
    emit position(play_id,play_current_position);
  }
}


void RDPlayDeck::fadeTimerData()
{
  if(!play_duck_down_state) {
    //play_cae->
    // fadeOutputVolume(play_card,play_stream,play_port,play_fade_gain[1]+play_cut_gain+play_duck_level,play_fade_down);
    play_cae->
      fadeOutputVolume(play_serial,
		       play_fade_gain[1]+play_cut_gain+play_duck_level,
		       play_fade_down);
  }
  play_fade_down_state=true;
}


void RDPlayDeck::duckTimerData()
{
  if (!play_duck_down_state) { //duck up
    //play_cae->
    //  fadeOutputVolume(play_card,play_stream,play_port,0+play_cut_gain+play_duck_level,play_duck_up);
    play_cae->fadeOutputVolume(play_serial,
			       0+play_cut_gain+play_duck_level,play_duck_up);
    play_ducked=0;
  }
  else { //duck down
    if(play_point_gain!=0) {
      //play_cae->fadeOutputVolume(play_card,play_stream,play_port,
      //  		       play_point_gain+play_cut_gain+play_duck_level,
      //                       play_segue_interval-play_duck_down);
      play_cae->fadeOutputVolume(play_serial,
				 play_point_gain+play_cut_gain+play_duck_level,
				 play_segue_interval-play_duck_down);
    }
    else {
      if(play_fade_down_state && 
         play_fade_gain[1]<play_duck_gain[1]) { //fade down in progress
        //play_cae->fadeOutputVolume(play_card,play_stream,play_port,
	//		       play_fade_gain[1]+play_cut_gain+play_duck_level,
	//                     play_segue_interval-play_duck_down);
	play_cae->
	  fadeOutputVolume(play_serial,
			   play_fade_gain[1]+play_cut_gain+play_duck_level,
			   play_segue_interval-play_duck_down);
      }
    } 
    play_duck_down_state=false;
  }
}


void RDPlayDeck::StartTimers(int offset)
{
  int audio_point;

  for(int i=0;i<RDPlayDeck::SizeOf;i++) {
    play_point_state[i]=false;
    if(play_point_value[i][0]!=-1) {
      audio_point=(int)
	(RD_TIMESCALE_DIVISOR*(double)play_audio_point[0]/
	 (double)play_timescale_speed);
      if((play_point_value[i][0]-audio_point-offset)>=0) {
	play_point_timer[i]->
	  start(play_point_value[i][0]-audio_point-offset);
      }
      else {
	if((play_point_value[i][1]-audio_point-offset)>=0) {
	  play_point_state[i]=true;
	  play_point_timer[i]->
	    start(play_point_value[i][1]-audio_point-offset);
	}
      }
      if((i==0)&&(rda->config()->padSegueOverlaps()>0)) {
	play_point_timer[0]->stop();
	play_point_timer[0]->start(play_point_timer[0]->interval()+
				   rda->config()->padSegueOverlaps());;
      }
    }
  }
  if((play_fade_point[1]!=-1)&&(offset<play_fade_point[1])&&
     ((play_fade_down=play_audio_point[1]-play_fade_point[1])>0)) {
    play_fade_timer->start(play_fade_point[1]-play_audio_point[0]-offset);
  }
  if(offset<play_duck_up_point){
    play_duck_timer->start(play_duck_up_point-offset);
  }
}


void RDPlayDeck::StopTimers()
{
  for(int i=0;i<RDPlayDeck::SizeOf;i++) {
    if(play_point_timer[i]->isActive()) {
      play_point_timer[i]->stop();
    }
  }
  if(play_fade_timer->isActive()) {
    play_fade_timer->stop();
  }
  if(play_stop_timer->isActive()) {
    play_stop_timer->stop();
  }
  if(play_duck_timer->isActive()) {
    play_duck_timer->stop();
  }
}
