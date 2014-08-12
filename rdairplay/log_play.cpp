// log_play.cpp
//
// Rivendell Log Playout Machine
//
//   (C) Copyright 2002-2009 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: log_play.cpp,v 1.197.8.7.2.2 2014/05/22 19:37:45 cvs Exp $
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

#include <unistd.h>
#include <syslog.h>

#include <rdmixer.h>
#include <rddebug.h>
#include <rdlog.h>
#include <rddb.h>
#include <log_play.h>
#include <log_traffic.h>
#include <globals.h>

LogPlay::LogPlay(RDCae *cae,int id,QSocketDevice *nn_sock,QString logname,
		 std::vector<RLMHost *> *rlm_hosts,
		 QObject *parent,const char *name)
  : QObject(parent,name),RDLogEvent(logname)
{
  //
  // Initialize Data Structures
  //
  play_cae=cae;
  play_log=NULL;
  play_id=id;
  play_rlm_hosts=rlm_hosts;
  play_onair_flag=false;
  play_segue_length=rdairplay_conf->segueLength()+1;
  play_trans_length=rdairplay_conf->transLength()+1;
  play_duck_volume_port1=0;
  play_duck_volume_port2=0;
  play_start_next=false;
  play_running=false;
  play_next_line=0;
  play_post_time=QTime();
  play_post_offset=-1;
  play_active_line=-1;
  play_active_trans=RDLogLine::Play;
  play_trans_line=-1;
  play_grace_line=-1;
  next_channel=0;
  play_nownext_socket=nn_sock;
  play_timescaling_available=false;
  play_rescan_pos=0;
  play_refreshable=false;
  play_audition_preroll=rdairplay_conf->auditionPreroll();
  for(int i=0;i<LOGPLAY_MAX_PLAYS;i++) {
    play_slot_id[i]=i;
  }
  for(int i=0;i<2;i++) {
    play_card[i]=0;
    play_port[i]=0;
  }
  for(int i=0;i<RD_MAX_STREAMS;i++) {
    play_deck[i]=new RDPlayDeck(play_cae,0,this);
    play_deck_active[i]=false;
  }
  play_macro_running=false;
  play_refresh_pending=false;
  play_nownext_string=rdairplay_conf->udpString(id);
  play_nownext_address=rdairplay_conf->udpAddress(id);
  play_nownext_port=rdairplay_conf->udpPort(id);
  play_nownext_rml=rdairplay_conf->logRml(id);
  play_now_cartnum=0;
  play_next_cartnum=0;
  play_prevnow_cartnum=0;
  play_prevnext_cartnum=0;
  play_op_mode=RDAirPlayConf::Auto;

  //
  // Macro Cart Decks
  //
  play_macro_deck=new RDMacroEvent(rdstation_conf->address(),rdripc,
				   this,"play_macro_deck");
  connect(play_macro_deck,SIGNAL(started()),this,SLOT(macroStartedData()));
  connect(play_macro_deck,SIGNAL(finished()),this,SLOT(macroFinishedData()));
  connect(play_macro_deck,SIGNAL(stopped()),this,SLOT(macroStoppedData()));

  //
  // CAE Signals
  //
  connect(rdcae,SIGNAL(timescalingSupported(int,bool)),
	  this,SLOT(timescalingSupportedData(int,bool)));

  //
  // RIPC Signals
  //
  connect(rdripc,SIGNAL(onairFlagChanged(bool)),
	  this,SLOT(onairFlagChangedData(bool)));

  //
  // Audition Player
  //
  play_audition_line=-1;
  if((rdairplay_conf->card(RDAirPlayConf::CueChannel)>=0)&&
     (rdairplay_conf->port(RDAirPlayConf::CueChannel)>=0)) {
    play_audition_player=new RDSimplePlayer(rdcae,rdripc,
			  rdairplay_conf->card(RDAirPlayConf::CueChannel),
		       	  rdairplay_conf->port(RDAirPlayConf::CueChannel),
					    0,0);
    play_audition_player->playButton()->hide();
    play_audition_player->stopButton()->hide();
    connect(play_audition_player,SIGNAL(played()),
	    this,SLOT(auditionStartedData()));
    connect(play_audition_player,SIGNAL(stopped()),
	    this,SLOT(auditionStoppedData()));
  }
  else {
    play_audition_player=NULL;
  }

  //
  // Transition Timers
  //
  play_trans_timer=new QTimer(this,"play_trans_timer");
  connect(play_trans_timer,SIGNAL(timeout()),
	  this,SLOT(transTimerData()));
  play_grace_timer=new QTimer(this,"play_grace_timer");
  connect(play_grace_timer,SIGNAL(timeout()),
	  this,SLOT(graceTimerData()));

  //
  // Rescan Timer
  //
  play_rescan_timer=new QTimer(this,"play_rescan_timer");
  connect(play_rescan_timer,SIGNAL(timeout()),
	  this,SLOT(rescanEventsData()));
  play_rescan_timer->start(LOGPLAY_RESCAN_INTERVAL);
}


QString LogPlay::serviceName() const
{
  if(play_svc_name.isEmpty()) {
    return play_defaultsvc_name;
  }
  return play_svc_name;
}


void LogPlay::setServiceName(const QString &svcname)
{
  play_svc_name=svcname;
}


QString LogPlay::defaultServiceName() const
{
  return play_defaultsvc_name;
}


void LogPlay::setDefaultServiceName(const QString &svcname)
{
  play_defaultsvc_name=svcname;
}


int LogPlay::card(int channum) const
{
  return play_card[channum];
}


int LogPlay::port(int channum) const
{
  return play_port[channum];
}


RDAirPlayConf::OpMode LogPlay::mode() const
{
  return play_op_mode;
}


void LogPlay::setOpMode(RDAirPlayConf::OpMode mode)
{
  if(mode==play_op_mode) {
    return;
  }
  play_op_mode=mode;
  UpdateStartTimes(play_line_counter);
}


void LogPlay::setLogName(QString name)
{
  if(logName()!=name) {
    RDLogEvent::setLogName(name);
    emit renamed();
    rdairplay_conf->setCurrentLog(play_id,name.left(name.length()-4));
  }
}


void LogPlay::setChannels(int cards[2],int ports[2],
			  const QString start_rml[2],const QString stop_rml[2])
{
  for(int i=0;i<2;i++) {
    play_card[i]=cards[i];
    play_port[i]=ports[i];
    play_start_rml[i]=start_rml[i];
    play_stop_rml[i]=stop_rml[i];
    rdcae->requestTimescale(play_card[i]);
  }
}


void LogPlay::setSegueLength(int len)
{
  play_segue_length=len;
}


void LogPlay::setNowCart(unsigned cartnum)
{
  play_now_cartnum=cartnum;
}


void LogPlay::setNextCart(unsigned cartnum)
{
  play_next_cartnum=cartnum;
}


void LogPlay::auditionHead(int line)
{
  RDLogLine *logline=logLine(line);
  if((play_audition_player==NULL)||(logline==NULL)) {
    return;
  }
  if(play_audition_line>=0) {
    play_audition_player->stop();
  }
  play_audition_line=line;
  play_audition_head_played=true;
  play_audition_player->setCart(logline->cartNumber());
  play_audition_player->play();
}


void LogPlay::auditionTail(int line)
{
  RDLogLine *logline=logLine(line);
  if((play_audition_player==NULL)||(logline==NULL)) {
    return;
  }
  if(play_audition_line>=0) {
    play_audition_player->stop();
  }
  play_audition_line=line;
  play_audition_head_played=false;
  play_audition_player->setCart(logline->cartNumber());
  int start_pos=logline->endPoint()-play_audition_preroll;
  if(start_pos<0) {
    start_pos=0;
  }
  play_audition_player->play(start_pos);
}


void LogPlay::auditionStop()
{
  if(play_audition_player==NULL) {
    return;
  }
  if(play_audition_line>=0) {
    play_audition_player->stop();
  }
}


bool LogPlay::play(int line,RDLogLine::StartSource src,
		   int mport,bool skip_meta)
{
  QTime current_time=QTime::currentTime();
  RDLogLine *logline;
  if((logline=logLine(line))==NULL) {
    return false;
  }
  if((runningEvents(NULL)>=LOGPLAY_MAX_PLAYS)&&
     (logline->status()!=RDLogLine::Paused)) {
    return false;
  }
  if(play_op_mode==RDAirPlayConf::Auto) {
    skip_meta=false;
  }

  //
  // Remove any intervening events
  //
  if(play_line_counter!=line) {
    int start_line=-1;
    int num_lines;
    for(int i=play_line_counter;i<line;i++) {
      if((logline=logLine(i))!=NULL) {
	if(logline->status()==RDLogLine::Scheduled) {
	  if(start_line==-1) {
	    start_line=i;
	    num_lines=1;
	  }
	  else {
	    num_lines++;
	  }
	}
      }
    }
  }
  //
  // Play it
  //
  if(!GetNextPlayable(&line,skip_meta,true)) {
    return false;
  }

  bool ret = false;
  if(play_segue_length==0) {
    ret = StartEvent(line,RDLogLine::Play,0,src,mport);
  } else {
    ret = StartEvent(line,RDLogLine::Segue,play_segue_length,src,mport);
  }
  SetTransTimer(current_time);
  return ret;
}


bool LogPlay::channelPlay(int mport)
{
  if(nextLine()<0) {
    return false;
  }
  return play(nextLine(),RDLogLine::StartChannel,mport,false);
}


bool LogPlay::stop(bool all,int port,int fade)
{
  RDLogLine *logline;
  int lines[TRANSPORT_QUANTITY];

  int n=runningEvents(lines);
  for(int i=0;i<n;i++) {
    if(all || port<1) { 
      stop(lines[i],fade);
      }
    else {
      logline=logLine(lines[i]);
      if((logline->cartType()==RDCart::Audio)
	 &&(RDPlayDeck *)logline->playDeck()!=NULL
	 &&logline->portName().toInt()==port ) {
	stop(lines[i],fade);
      }
    }
  }
  if(n>0) {
    return true;
  }
  return false;
}


bool LogPlay::stop(int line,int fade)
{
  RDLogLine *logline;

  if((logline=logLine(line))==NULL) {
    return false;
  }
  switch(logline->cartType()) {
  case RDCart::Audio:
    if(((RDPlayDeck *)logline->playDeck())==NULL) {
      return false;
    }
    ((RDPlayDeck *)logline->playDeck())->stop(fade,RD_FADE_DEPTH);
    return true;
    break;
    
  case RDCart::Macro:
    play_macro_deck->stop();
    break;
    
  case RDCart::All:
    break;
  }
  return false;
}


bool LogPlay::channelStop(int mport)
{
  RDLogLine *logline;
  int lines[TRANSPORT_QUANTITY];
  bool ret=false;

  int n=runningEvents(lines);
  for(int i=0;i<n;i++) {
    logline=logLine(lines[i]);
    if((logline->cartType()==RDCart::Audio)
       &&((RDPlayDeck *)logline->playDeck()!=NULL)) {
      if(((RDPlayDeck *)logline->playDeck())->channel()==mport) {
	stop(lines[i]);
	ret=true;
      }
    }
  }
  return ret;
}


bool LogPlay::pause(int line)
{
  RDLogLine *logline;
  
  if((logline=logLine(line))==NULL) {
    return false;
  }
  switch(logline->cartType()) {
  case RDCart::Audio:
    if(logline->playDeck()==NULL) {
      return false;
    }
    ((RDPlayDeck *)logline->playDeck())->pause();
    return true;
    break;

  case RDCart::Macro:
  case RDCart::All:
    break;
  }
  return false;
}


void LogPlay::duckVolume(int level,int fade,int mport)
{
  RDLogLine *logline;
  int lines[TRANSPORT_QUANTITY];

  if(mport==-1 || mport==1) {
	  play_duck_volume_port1=level;
  }
  if(mport==-1 || mport==2) {
	  play_duck_volume_port2=level;
  }
  int n=runningEvents(lines);
  for(int i=0;i<n;i++) {
    logline=logLine(lines[i]);
    if((logline->cartType()==RDCart::Audio) 
       && (RDPlayDeck *)logline->playDeck()!=NULL
       && ((logline->portName().toInt()==mport) || mport<1) ) {
      ((RDPlayDeck *)logline->playDeck())->duckVolume(level,fade);
    }
  }
}
		


void LogPlay::makeNext(int line,bool refresh_status)
{
  play_next_line=line;
  if(refresh_status) {
    RefreshEvents(line,LOGPLAY_LOOKAHEAD_EVENTS);
  }
  SendNowNext();
  SetTransTimer();
  UpdatePostPoint();
  emit nextEventChanged(line);
  emit transportChanged();
}


void LogPlay::load()
{
  int lines[TRANSPORT_QUANTITY];
  int running=0;

  play_duck_volume_port1=0;
  play_duck_volume_port2=0;
  
  //
  // Remove All Idle Events
  //
  if((running=runningEvents(lines))==0) {
    remove(0,size(),false);
  }
  else {
    if(lines[running-1]<(size()-1)) {
      remove(lines[running-1]+1,size()-lines[running-1]-1,false);
    }
    for(int i=running-2;i>0;i--) {
      remove(lines[i-1]+1,lines[i]-lines[i-1]-1,false);
    }
    if(lines[0]!=0) {
      remove(0,lines[0],false);
    }
  }

  //
  // Load Events
  //
  RDLogEvent::load();
  play_rescan_pos=0;
  if(play_timescaling_available) {
    for(int i=0;i<size();i++) {
      logLine(i)->setTimescalingActive(logLine(i)->enforceLength());
    }
  }
  RefreshEvents(0,size());
  RDLog *log=new RDLog(logName().left(logName().length()-4));
  play_svc_name=log->service();
  delete log;
  play_line_counter=0;
  play_next_line=0;
  UpdateStartTimes(0);
  emit reloaded();
  SetTransTimer();
  emit transportChanged();
  UpdatePostPoint();
  if((running>0)&&(size()>running)) {
    makeNext(running);
  }

  //
  // Update Refreshability
  //
  if(play_log!=NULL) {
    delete play_log;
  }
  play_log=new RDLog(logName().left(logName().length()-4));
  play_link_datetime=play_log->linkDatetime();
  play_modified_datetime=play_log->modifiedDatetime();
  if(play_refreshable) {
    play_refreshable=false;
    emit refreshabilityChanged(play_refreshable);
  }
}


void LogPlay::append(const QString &log_name)
{
  int old_size=size();

  if(size()==0) {
    setLogName(RDLog::tableName(log_name));
    load();
    return;
  }

  RDLogEvent::append(log_name);
  if(play_timescaling_available) {
    for(int i=old_size;i<size();i++) {
      logLine(i)->setTimescalingActive(logLine(i)->enforceLength());
    }
  }
  RefreshEvents(old_size,size()-old_size);
  UpdateStartTimes(old_size);
  emit reloaded();
  SetTransTimer();
  emit transportChanged();
  UpdatePostPoint();
}


bool LogPlay::refresh()            
{                                  
  RDLogLine *s;
  RDLogLine *d;
  int prev_line;
  int prev_id;
  int next_line=-1;
  int next_id=-1;
  int current_id=-1;
  int lines[TRANSPORT_QUANTITY];
  int running;

  if(play_macro_running) {
    play_refresh_pending=true;
    return true;
  }
  emit refreshStatusChanged(true);
  if((size()==0)||(play_log==NULL)) {
    emit refreshStatusChanged(false);
    return true;
  }

  //
  // Load the Updated Log
  //
  RDLogEvent *e=new RDLogEvent();
  e->setLogName(logName());
  e->load();
  play_modified_datetime=play_log->modifiedDatetime();

  //
  // Get the Next Event
  //
  if(nextEvent()!=NULL) {   //End of the log?
    next_id=nextEvent()->id();
  }

  //
  // Get Running Events
  //
  running=runningEvents(lines);
  for(int i=0;i<running;i++) {
    if(lines[i]==play_next_line-1) {
      current_id=logLine(lines[i])->id();
    }
  }
  if(running>0 && next_id==-1) {                  //Last Event of Log Running?
    current_id=logLine(lines[running-1])->id();
  }

  //
  // Pass 1: Finished or Active Events
  //
  for(int i=0;i<size();i++) {
    d=logLine(i);
    if(d->status()!=RDLogLine::Scheduled) {
      if((s=e->loglineById(d->id()))!=NULL) {
	s->incrementPass();
      }
      d->incrementPass();
    }
  }

  //
  // Pass 2: Purge Deleted Events
  //
  for(int i=size()-1;i>=0;i--) {
    if(logLine(i)->pass()==0) {
      remove(i,1,false,true);
    }
  }

  //
  // Pass 3: Add New Events
  //
  for(int i=0;i<e->size();i++) {
    s=e->logLine(i);
    if(s->pass()==0) {
      if((prev_line=(i-1))<0) {  // First Event
	insert(0,s,false,true);
      }
      else {
	prev_id=e->logLine(prev_line)->id();   
	insert(lineById(prev_id)+1,s,false,true);   
      }
    }
    else {
      loglineById(s->id())->incrementPass();
    }
  }

  //
  // Pass 4: Delete Orphaned Past Playouts
  //
  for(int i=size()-1;i>=0;i--) {
    d=logLine(i);
    if((d->status()==RDLogLine::Finished)&&(d->pass()!=2)) {
      remove(i,1,false,true);
    }
  }

  //
  // Restore Next Event
  //
  if(current_id!=-1 && e->loglineById(current_id)!=NULL) {    //Make Next after currently playing cart
    if((next_line=lineById(current_id))>=0) {    
      makeNext(next_line+1,false);              
    }
  }
  else {
    if((next_line=lineById(next_id))>=0) {     
     makeNext(next_line,false);               
    }
  } 
  
  //
  // Clean Up
  //
  delete e;
  for(int i=0;i<size();i++) {
    logLine(i)->clearPass();
  }
  RefreshEvents(0,size());
  UpdateStartTimes(next_line);
  UpdatePostPoint();
  SetTransTimer();
  emit transportChanged();
  emit reloaded();
  if(!play_refreshable) {
    play_refreshable=false;
    emit refreshabilityChanged(play_refreshable);
  }

  emit refreshStatusChanged(false);

  return true;
}


void LogPlay::save(int line)
{
  RDLogEvent::save(line);
  if(play_log!=NULL) {
    delete play_log;
  }
  play_log=new RDLog(logName().left(logName().length()-4));
  QDateTime current_datetime=
    QDateTime(QDate::currentDate(),QTime::currentTime());
  play_log->setModifiedDatetime(current_datetime);
  play_modified_datetime=current_datetime;
  if(play_refreshable) {
    play_refreshable=false;
    emit refreshabilityChanged(play_refreshable);
  }
}


void LogPlay::clear()
{
  setLogName("");
  int start_line=0;
  play_duck_volume_port1=0;
  play_duck_volume_port2=0;
  while(ClearBlock(start_line++));
  play_svc_name=play_defaultsvc_name;
  play_rescan_pos=0;
  if(play_log!=NULL) {
    delete play_log;
    play_log=NULL;
  }
  SetTransTimer();
  UpdatePostPoint();
  if(play_refreshable) {
    play_refreshable=false;
    emit refreshabilityChanged(play_refreshable);
  }
  emit reloaded();
}


void LogPlay::insert(int line,int cartnum,RDLogLine::TransType next_type,
		     RDLogLine::TransType type)
{
  RDLogLine *logline;
  int lines[TRANSPORT_QUANTITY];
  RDPlayDeck *playdeck;
  int mod_line=-1;
  
  if(line<(size()-1)) {
    if(logLine(line)->hasCustomTransition()) {
      mod_line=line+1;
    }
  }

  int running=runningEvents(lines);
  for(int i=0;i<running;i++) {
    if((logline=logLine(lines[i]))!=NULL) {
      if((playdeck=(RDPlayDeck *)logline->playDeck())!=NULL) {
	if((playdeck->id()>=0)&&
	   (playdeck->id()>=line)) {
	  playdeck->setId(playdeck->id()+1);
	}
      }
    }
  }
  if(play_macro_deck->line()>=0) {
    play_macro_deck->setLine(play_macro_deck->line()+1);
  }
  RDLogEvent::insert(line,1);
  if((logline=logLine(line))==NULL) {
    RDLogEvent::remove(line,1);
    return;
  }
  if(nextLine()>line) {
    makeNext(nextLine()+1);
  }
  if(nextLine()<0) {
    play_next_line=line;
  }
  logline->loadCart(cartnum,next_type,play_id,play_timescaling_available,
		    rdairplay_conf->defaultTransType());
  logline->
    setTimescalingActive(play_timescaling_available&&logline->enforceLength());
  UpdateStartTimes(line);
  emit inserted(line);
  UpdatePostPoint();
  if(mod_line>=0) {
    emit modified(mod_line);
  }
  emit transportChanged();
  SetTransTimer();
  UpdatePostPoint();
}


void LogPlay::insert(int line,RDLogLine *l,bool update,bool preserv_custom_transition)
{
  RDLogLine *logline;
  int lines[TRANSPORT_QUANTITY];
  RDPlayDeck *playdeck;
  int mod_line=-1;
  
  if(line<(size()-1)) {
    if(logLine(line)->hasCustomTransition()) {
      mod_line=line+1;
    }
  }

  int running=runningEvents(lines);
  for(int i=0;i<running;i++) {
    if((logline=logLine(lines[i]))!=NULL) {
      if((playdeck=(RDPlayDeck *)logline->playDeck())!=NULL) {
	if((playdeck->id()>=0)&&
	   (playdeck->id()>=line)) {
	  playdeck->setId(playdeck->id()+1);
	}
      }
    }
  }
  if(play_macro_deck->line()>=0) {
    play_macro_deck->setLine(play_macro_deck->line()+1);
  }
  RDLogEvent::insert(line,1,preserv_custom_transition);
  if((logline=logLine(line))==NULL) {
    RDLogEvent::remove(line,1);
    return;
  }
  *logline=*l;
  if(nextLine()>line && update) {
    makeNext(nextLine()+1);
  }
  if(nextLine()<0) {
    play_next_line=line;
  }
  logline->
    setTimescalingActive(play_timescaling_available&&logline->enforceLength());
  if(update) {
    UpdateStartTimes(line);
    emit inserted(line);
    UpdatePostPoint();
    if(mod_line>=0) {
      emit modified(mod_line);
    }
    emit transportChanged();
    SetTransTimer();
    UpdatePostPoint();
  }
}


void LogPlay::remove(int line,int num_lines,bool update,bool preserv_custom_transition)
{
  RDPlayDeck *playdeck;
  RDLogLine *logline;
  int mod_line=-1;

  if((num_lines==0)||(line<0)||(line>=size())) {
    return;
  }
  if((line+num_lines)<(size()-1)) {
    if(logLine(line+num_lines)->hasCustomTransition()) {
      mod_line=line;
    }
  }

  for(int i=line;i<(line+num_lines);i++) {
    if((logline=logLine(i))!=NULL) {
      if((playdeck=(RDPlayDeck *)logline->playDeck())!=NULL) {
	playdeck->clear();
	FreePlayDeck(playdeck);
      }
    }
  }

  int lines[TRANSPORT_QUANTITY];

  if(update) {
    emit removed(line,num_lines,false);
    }
  int running=runningEvents(lines);
  for(int i=0;i<running;i++) {
    if((logline=logLine(lines[i]))!=NULL) {
      if(logline->type()==RDLogLine::Cart) {
	playdeck=(RDPlayDeck *)logline->playDeck();
	if((playdeck->id()>=0)&&(playdeck->id()>line)) {
	  playdeck->setId(playdeck->id()-num_lines);
	}
      }
    }
  }
  if(play_macro_deck->line()>0) {
    play_macro_deck->setLine(play_macro_deck->line()-num_lines);
  }

  RDLogEvent::remove(line,num_lines,preserv_custom_transition);
  if(update) {
    if(nextLine()>line) {
      makeNext(nextLine()-num_lines);
    }
    UpdateStartTimes(line);
    if(size()==0) {
      emit reloaded();
    }
    if(mod_line>=0) {
      emit modified(mod_line);
    }
    emit transportChanged();
    SetTransTimer();
    UpdatePostPoint();
  }
}


void LogPlay::move(int from_line,int to_line)
{
  int offset=0;
  int lines[TRANSPORT_QUANTITY];
  RDLogLine *logline;
  RDPlayDeck *playdeck;
  int mod_line[2]={-1,-1};

  if(from_line<(size()-1)) {
    if(logLine(from_line+1)->hasCustomTransition()) {
      if(from_line<to_line) {
	mod_line[0]=from_line;
      }
      else {
	mod_line[0]=from_line+1;
      }
    }
  }
  if(to_line<size()) {
    if(logLine(to_line)->hasCustomTransition()) {
      if(to_line>to_line) {
	mod_line[1]=to_line;
      }
      else {
	mod_line[1]=to_line+1;
      }
    }
  }

  emit removed(from_line,1,true);
  int running=runningEvents(lines,false);
  for(int i=0;i<running;i++) {
    if((logline=logLine(lines[i]))!=NULL) {
      playdeck=(RDPlayDeck *)logline->playDeck();
      if(playdeck->id()>=0) {
	if((playdeck->id()>from_line)&&
	   (playdeck->id()<=to_line)) {
	  playdeck->setId(playdeck->id()-1);
	}
	else {
	  if((playdeck->id()<from_line)&&
	     (playdeck->id()>to_line)) {
	    playdeck->setId(playdeck->id()+1);
	  }
	}
      }
    }
  }
  if(play_macro_deck->line()>=0) {
    if((play_macro_deck->line()>from_line)&&
       (play_macro_deck->line()<=to_line)) {
      play_macro_deck->setLine(play_macro_deck->line()-1);
    }
    else {
      if((play_macro_deck->line()<from_line)&&
	 (play_macro_deck->line()>to_line)) {
	play_macro_deck->setLine(play_macro_deck->line()+1);
      }
    }
  }

  if(to_line>from_line) {
    offset=1;
  }
  RDLogEvent::move(from_line,to_line);
  if(from_line>to_line) {
    UpdateStartTimes(to_line);
  }
  else {
    UpdateStartTimes(from_line);
  }
  SetTransTimer();
  UpdatePostPoint();
  emit inserted(to_line);
  for(int i=0;i<2;i++) {
    if(mod_line[i]>=0) {
      emit modified(mod_line[i]);
    }
  }
  if((nextLine()>from_line)&&(nextLine()<=(to_line+offset))) {
    makeNext(nextLine()-1);
  }
  else {
    if((nextLine()<from_line)&&(nextLine()>to_line)) {
      makeNext(nextLine()+1);
    }
    else {
      emit transportChanged();
    }
  }
}


void LogPlay::copy(int from_line,int to_line,RDLogLine::TransType type)
{
  RDLogLine *logline;

  if((logline=logLine(from_line))==NULL) {
    return;
  }
  insert(to_line,logline->cartNumber(),RDLogLine::Play,type);
}


int LogPlay::topLine()
{
  for(int i=0;i<size();i++) {
    if((logLine(i)->status()==RDLogLine::Playing)||
       (logLine(i)->status()==RDLogLine::Finishing)||
       (logLine(i)->status()==RDLogLine::Paused)) {
      return i;
    }
  }
  return nextLine();
}


int LogPlay::currentLine() const
{
  return play_line_counter;
}


int LogPlay::nextLine() const
{
  return play_next_line;
}


int LogPlay::nextLine(int line)
{
  int lines[TRANSPORT_QUANTITY];

  // FIXME: Do we really need this codeblock?
  transportEvents(lines);
  for(int i=0;i<(TRANSPORT_QUANTITY-1);i++) {
    if(line==lines[i]) {
      for(int j=i+1;j<TRANSPORT_QUANTITY;j++) {
	if(logLine(lines[j])==NULL) {
	  return -1;
	}
	if(logLine(lines[j])->status()==RDLogLine::Scheduled) {
	  return lines[j];
	}
      }
    }
  }
  // End of FIXME

  for(int i=line+1;i<size();i++) {
    if(logLine(i)->status()==RDLogLine::Scheduled) {
      return i;
    }
  }
  return -1;
}


RDLogLine *LogPlay::nextEvent()
{
  if(play_next_line<0) {
    return NULL;
  }
  return logLine(play_next_line);
}


RDLogLine::TransType LogPlay::nextTrans()
{
  RDLogLine *logline=nextEvent();
  if(logline==NULL) {
    return RDLogLine::Stop;
  }
  return logline->transType();
}


RDLogLine::TransType LogPlay::nextTrans(int line)
{
  RDLogLine *logline;

//  if((logline=logLine(nextLine(line)))!=NULL) {

  int next_line; 
  next_line=nextLine(line);
  logline=logLine(next_line);
  if(logline!=NULL) {
    return logline->transType();
  }
  return RDLogLine::Stop;
}


void LogPlay::transportEvents(int line[])
{
  int count=0;
  int start=topLine();
  RDLogLine *logline;

  for(int i=0;i<TRANSPORT_QUANTITY;i++) {
    line[i]=-1;
  }
  if((start<0)||(size()==0)) {
    return;
  }

  count=runningEvents(line);
  if(nextLine()<0) {
    return;
  }
  start=play_next_line;
  if((logline=logLine(start))==NULL) {
    return;
  }
  for(int i=start;i<size();i++) {
    if((logline=logLine(i))==NULL) {
      return;
    }
    switch(logline->status()) {
	case RDLogLine::Scheduled:
	  if(count<TRANSPORT_QUANTITY) {
	    line[count++]=i;
	  }
	  break;

	default:
	  break;
    }
    if(count==TRANSPORT_QUANTITY) {
      return;
    }
  }
}


int LogPlay::runningEvents(int *lines, bool include_paused)
{
  int count=0;
  int events[TRANSPORT_QUANTITY];
  int table[TRANSPORT_QUANTITY];
  bool changed=true;

  if(size()==0) {
    return 0;
  }
  for(int i=0;i<TRANSPORT_QUANTITY;i++) {
    if (lines){
      lines[i]=-1;
    }
    table[i]=i;
  }

  //
  // Build Running Event List
  //
  if(include_paused) {
    for(int i=0;i<size();i++) {
      if((logLine(i)->status()==RDLogLine::Playing)||
	 (logLine(i)->status()==RDLogLine::Finishing)||
	 (logLine(i)->status()==RDLogLine::Paused)) {
	events[count++]=i;
	if(count==TRANSPORT_QUANTITY) {
	  break;
	}
      }
    }
  }
  else {
    for(int i=0;i<size();i++) {
      if((logLine(i)->status()==RDLogLine::Playing)||
	 (logLine(i)->status()==RDLogLine::Finishing)) {
	events[count++]=i;
	if(count==TRANSPORT_QUANTITY) {
	  break;
	}
      }
    }
  }
  if (!lines){
    return count;
  }
  //
  // Sort 'Em (by start time)
  //
  while(changed) {
    changed=false;
    for(int i=0;i<(count-1);i++) {
      if(logLine(events[table[i]])->startTime(RDLogLine::Initial)>
	 logLine(events[table[i+1]])->startTime(RDLogLine::Initial)) {
	int event=table[i];
	table[i]=table[i+1];
	table[i+1]=event;
	changed=true;
      }
    }
  }

  //
  // Write out the table
  //
  for(int i=0;i<count;i++) {
    lines[i]=events[table[i]];
  }

  return count;
}


void LogPlay::lineModified(int line)
{
  RDLogLine *logline;
  RDLogLine *next_logline;

  SetTransTimer();
  UpdateStartTimes(line);

  if((logline=logLine(line))!=NULL) {
    if((next_logline=logLine(line+1))==NULL) {
      logline->loadCart(logline->cartNumber(),RDLogLine::Play,
			play_id,logline->timescalingActive());
    }
    else {
      logline->loadCart(logline->cartNumber(),next_logline->transType(),
			play_id,logline->timescalingActive());
    }
  }
  emit modified(line);
  int lines[TRANSPORT_QUANTITY] = {-1};
  int count;
  count = runningEvents(lines,false);
  if (count > 0){
    line=lines[count-1];
  }
  UpdatePostPoint();
  emit transportChanged();
}


RDLogLine::Status LogPlay::status(int line)
{
  RDLogLine *logline;

  if((logline=logLine(line))==NULL) {
    return RDLogLine::Scheduled;
  }
  return logline->status();
}


QTime LogPlay::startTime(int line)
{
  RDLogLine *logline;

  if((logline=logLine(line))==NULL) {
    return QTime();
  }
  switch(logline->cartType()) {
  case RDCart::Audio:
    if(((RDPlayDeck *)logline->playDeck())==NULL) {
      return logline->startTime(RDLogLine::Predicted);
    }
    return logline->startTime(RDLogLine::Actual);
    break;

  case RDCart::Macro:
  case RDCart::All:
    return logline->startTime(RDLogLine::Predicted);
  break;
  }
  return QTime();
}


QTime LogPlay::nextStop() const
{
  return play_next_stop;
}


bool LogPlay::running(bool include_paused)
{
  if(runningEvents(NULL,include_paused)==0) {
    return false;
  }
  return true;
}


void LogPlay::resync()
{
  SetTransTimer();
}


void LogPlay::transTimerData()
{
  int lines[TRANSPORT_QUANTITY];
  RDLogLine *logline=NULL;
  int grace=0;
  int trans_line=play_trans_line;

  if(play_grace_timer->isActive()) {
    play_grace_timer->stop();
  }

  if(play_op_mode==RDAirPlayConf::Auto) {
    if(!GetNextPlayable(&play_trans_line,false)) {
      SetTransTimer();
      return;
    }
    if((logline=logLine(play_trans_line))!=NULL) {
      grace=logline->graceTime();
    }
    if((runningEvents(lines)==0)) {
      makeNext(play_trans_line);
      if(logline->transType()!=RDLogLine::Stop || grace>=0) {
        StartEvent(trans_line,RDLogLine::Play,0,RDLogLine::StartTime);
      } 
    }
    else {
      if(logline==NULL) {
	LogLine(RDConfig::LogNotice,"  invalid logline");
	SetTransTimer();
	return;
      }
      switch(logline->graceTime()) {
	  case 0:
	    makeNext(play_trans_line);
	    if(play_trans_length==0) {
	      StartEvent(trans_line,RDLogLine::Play,0,RDLogLine::StartTime);
	    }
	    else {
	      StartEvent(trans_line,RDLogLine::Segue,play_trans_length,
			 RDLogLine::StartTime);
	    }
	    break;

	  case -1:
	    makeNext(play_trans_line);
	    break;
	    
	  default:
	    if(logline->transType()==RDLogLine::Stop) {
	      logline->setTransType(RDLogLine::Play);
	    }
	    logline->setStartTime(RDLogLine::Predicted,logline->
				  startTime(RDLogLine::Predicted).
				  addMSecs(grace));
	    play_grace_line=play_trans_line;
	    play_grace_timer->start(grace,true);
	    break;
      }
    }
  }
  SetTransTimer();
}


void LogPlay::graceTimerData()
{
  int lines[TRANSPORT_QUANTITY];
  int line=play_grace_line;

  if(play_op_mode==RDAirPlayConf::Auto) {
    if(!GetNextPlayable(&line,false)) {
      SetTransTimer();
      return;
    }
    if(line!=play_grace_line) {
      return;
    }
    if((runningEvents(lines)==0)) {
      makeNext(play_grace_line);
      StartEvent(play_grace_line,RDLogLine::Play,0,RDLogLine::StartTime);
    }
    else {
      makeNext(play_grace_line);
      if(play_trans_length==0) {
	StartEvent(play_grace_line,RDLogLine::Play,0,RDLogLine::StartTime);
      }
      else {
	StartEvent(play_grace_line,RDLogLine::Segue,play_trans_length,
		   RDLogLine::StartTime);
      }
    }
  }
}


void LogPlay::playStateChangedData(int id,RDPlayDeck::State state)
{
#ifdef SHOW_SLOTS
  printf("playStateChangedData(%d)\n",id);
#endif
  switch(state) {
      case RDPlayDeck::Playing:
	Playing(id);
	break;

      case RDPlayDeck::Paused:
	Paused(id);
	break;

      case RDPlayDeck::Stopping:
	Stopping(id);
	break;

      case RDPlayDeck::Stopped:
	Stopped(id);
	break;

      case RDPlayDeck::Finished:
	Finished(id);
	break;
  }
}


void LogPlay::onairFlagChangedData(bool state)
{
  play_onair_flag=state;
}


void LogPlay::segueStartData(int id)
{
#ifdef SHOW_SLOTS
  printf("segueStartData(%d)\n",id);
#endif
  int line=GetLineById(id);
  RDLogLine *logline;
  RDLogLine *next_logline=nextEvent();
  if(next_logline==NULL) {
    return;
  }
  if((logline=logLine(line))==NULL) {
    return;
  }
  if((play_op_mode==RDAirPlayConf::Auto)&&
     ((next_logline->transType()==RDLogLine::Segue))&&
     (logline->status()==RDLogLine::Playing)&&
     (logline->id()!=-1)) {
    if(!GetNextPlayable(&play_next_line,false)) {
      return;
    }
    StartEvent(play_next_line,next_logline->transType(),
	       logline->segueTail(next_logline->transType()),
	       RDLogLine::StartSegue,-1,
	       logline->segueTail(next_logline->transType()));
    SetTransTimer();
  }
}


void LogPlay::segueEndData(int id)
{
#ifdef SHOW_SLOTS
  printf("segueEndData(%d)\n",id);
#endif

  int line=GetLineById(id);
  RDLogLine *logline;
  if((logline=logLine(line))==NULL) {
    return;
  }
  if((play_op_mode==RDAirPlayConf::Auto)&&
     (logline->status()==RDLogLine::Finishing)) {
    ((RDPlayDeck *)logline->playDeck())->stop();
    CleanupEvent(id);
    UpdateStartTimes(line);
    LogTraffic(serviceName(),logName().left(logName().length()-4),logline,
	       (RDLogLine::PlaySource)(play_id+1),
	       RDAirPlayConf::TrafficFinish,play_onair_flag);
    emit stopped(line);
    emit transportChanged();
  }
}


void LogPlay::talkStartData(int id)
{
#ifdef SHOW_SLOTS
  printf("talkStartData(%d)\n",id);
#endif
}


void LogPlay::talkEndData(int id)
{
#ifdef SHOW_SLOTS
  printf("talkEndData(%d)\n",id);
#endif
}


void LogPlay::positionData(int id,int pos)
{
  int line=GetLineById(id);

  RDLogLine *logline;
  if((logline=logLine(line))==NULL) {
    return;
  }
  if(pos>logline->effectiveLength()) {
    LogLine(RDConfig::LogWarning,QString().sprintf("*** position out of bounds on signal: Line: %d  Cart: %d  Pos: %d ***",line,logline->cartNumber(),logline->playPosition()));
    return;
  }
  logline->setPlayPosition(pos);
  emit position(line,pos);
}


void LogPlay::macroStartedData()
{
#ifdef SHOW_SLOTS
  printf("macroStartedData()\n");
#endif
  play_macro_running=true;
  int line=play_macro_deck->line();
  RDLogLine *logline;
  if((logline=logLine(line))==NULL) {
    return;
  }
  logline->setStatus(RDLogLine::Playing);
  logline->
    setStartTime(RDLogLine::Initial,
		 QTime::currentTime().addMSecs(rdstation_conf->timeOffset()));
  UpdateStartTimes(line);
  emit played(line);
  UpdatePostPoint();
  emit transportChanged();
}


void LogPlay::macroFinishedData()
{
#ifdef SHOW_SLOTS
  printf("macroFinishedData()\n");
#endif
  int line=play_macro_deck->line();
  play_macro_deck->clear();
  FinishEvent(line);
  RDLogLine *logline;
  if((logline=logLine(line))!=NULL) {
    logline->setStatus(RDLogLine::Finished);
    LogTraffic(serviceName(),logName().left(logName().length()-4),logline,
	       (RDLogLine::PlaySource)(play_id+1),RDAirPlayConf::TrafficMacro,
	       play_onair_flag);
  }
  play_macro_running=false;
  UpdatePostPoint();
  if(play_refresh_pending) {
    refresh();
    play_refresh_pending=false;
  }
  emit transportChanged();
}


void LogPlay::macroStoppedData()
{
#ifdef SHOW_SLOTS
  printf("macroStoppedData()\n");
#endif
  int line=play_macro_deck->line();
  play_macro_deck->clear();
  RDLogLine *logline;
  if((logline=logLine(line))!=NULL) {
    logline->setStatus(RDLogLine::Finished);
    LogTraffic(serviceName(),logName().left(logName().length()-4),logline,
	       (RDLogLine::PlaySource)(play_id+1),RDAirPlayConf::TrafficMacro,
	       play_onair_flag);
  }
  UpdatePostPoint();
  emit transportChanged();
}


void LogPlay::timescalingSupportedData(int card,bool state)
{
  if(card>=0) {
    play_timescaling_supported[card]=state;
    if(play_timescaling_supported[play_card[0]]&&
       play_timescaling_supported[play_card[1]]) {
      play_timescaling_available=true;
    }
    else {
      play_timescaling_available=false;
    }
  }
  else {
    play_timescaling_available=false;
  }
}


void LogPlay::rescanEventsData()
{
  int start_pos=play_rescan_pos;
  int start_size=LOGPLAY_RESCAN_SIZE;
  if((start_pos+start_size)>=size()) {
    start_size=size()-start_pos;
    play_rescan_pos=0;
  }
  else {
    play_rescan_pos+=LOGPLAY_RESCAN_SIZE;
  }
  RefreshEvents(start_pos,start_size);
}


void LogPlay::auditionStartedData()
{
  if(play_audition_head_played) {
    emit auditionHeadPlayed(play_audition_line);
  }
  else {
    emit auditionTailPlayed(play_audition_line);
  }
}


void LogPlay::auditionStoppedData()
{
  int line=play_audition_line;
  play_audition_line=-1;
  emit auditionStopped(line);
}


bool LogPlay::StartEvent(int line,RDLogLine::TransType trans_type,
			 int trans_length,RDLogLine::StartSource src,int mport,int duck_length)
{
  int running;
  int lines[TRANSPORT_QUANTITY];
  RDLogLine *logline;
  RDLogLine *next_logline;
  RDPlayDeck *playdeck;
  int card;
  int port;
  int aport;
  bool was_paused=false;

  if((logline=logLine(line))==NULL) {
    return false;
  }
  if(logline->id()<0) {
    return false;
  }

  //
  // Transition running events
  //
  running=runningEvents(lines);
  if(play_op_mode!=RDAirPlayConf::Manual) {
    switch(trans_type) {
	case RDLogLine::Play:
	  for(int i=0;i<running;i++) {
	    if(logLine(lines[i])!=NULL) {
	      if(((logLine(lines[i])->type()==RDLogLine::Cart)||
		  (logLine(lines[i])->type()==RDLogLine::Macro))&&
		 (logLine(lines[i])->status()!=RDLogLine::Paused)) {
		switch(logLine(lines[i])->cartType()) {
		case RDCart::Audio:
		  ((RDPlayDeck *)logLine(lines[i])->playDeck())->stop();
		  break;
		  
		case RDCart::Macro:
		  play_macro_deck->stop();
		  break;

		case RDCart::All:
		  break;
		}
	      }
	    }
	  }
	  break;
	  
	case RDLogLine::Segue:
	  for(int i=0;i<running;i++) {
	    RDLogLine *prev_logline=logLine(lines[i]);
	    if(prev_logline!=NULL) {
	      if(prev_logline->status()==RDLogLine::Playing) {
		if(((prev_logline->type()==RDLogLine::Cart)||
		    (prev_logline->type()==RDLogLine::Macro))&&
		   (prev_logline->status()!=RDLogLine::Paused)) {
		  switch(logLine(lines[i])->cartType()) {
		  case RDCart::Audio:
		    prev_logline->setStatus(RDLogLine::Finishing);
		    ((RDPlayDeck *)prev_logline->playDeck())->
		      stop(trans_length);
		    break;

		  case RDCart::Macro:
		    play_macro_deck->stop();
		    break;

		  case RDCart::All:
		    break;
		  }
		}
	      }
	    }
	  }
	  break;

	default:
	  break;
    }
  }

  //
  // Clear Unplayed Custom Transition
  //
  if(logLine(line-1)!=NULL) {
    if(logLine(line-1)->status()==RDLogLine::Scheduled) {
      logLine(line-1)->clearTrackData(RDLogLine::TrailingTrans);
    }
  }

  //
  // Start Playout
  //
  logline->setStartSource(src);
  switch(logline->type()) {
      case RDLogLine::Cart:
	if(!StartAudioEvent(line)) {
	  rdairplay_conf->setLogCurrentLine(play_id,nextLine());
	  return false;
	}
	aport=GetNextChannel(mport,&card,&port);
	playdeck=(RDPlayDeck *)logline->playDeck();
	playdeck->setCard(card);
	playdeck->setPort(port);
	playdeck->setChannel(aport);
	logline->setPauseCard(card);
	logline->setPausePort(port);
	logline->setPortName(GetPortName(playdeck->card(),
					 playdeck->port()));
	if(logline->portName().toInt()==2){
	  playdeck->duckVolume(play_duck_volume_port2,0);
	  }
	else  {
	  playdeck->duckVolume(play_duck_volume_port1,0);
	  }
		
	if(!playdeck->setCart(logline,logline->status()!=RDLogLine::Paused)) {
	  // No audio to play, so fake it
	  logline->setZombified(true);
	  playStateChangedData(playdeck->id(),RDPlayDeck::Playing);
	  logline->setStatus(RDLogLine::Playing);
	  playStateChangedData(playdeck->id(),RDPlayDeck::Finished);
	  logline->setStatus(RDLogLine::Finished);
	  LogLine(RDConfig::LogErr,QString().
		  sprintf("LogPlay::StartEvent(): no audio,CUT=%s",
			  (const char *)logline->cutName()));
	  rdairplay_conf->setLogCurrentLine(play_id,nextLine());
	  return false;
	}
	emit modified(line);
	logline->setCutNumber(playdeck->cut()->cutNumber());
	logline->setEvergreen(playdeck->cut()->evergreen());
	if(play_timescaling_available&&logline->enforceLength()) {
	  logline->setTimescalingActive(true);
	}
	RDSetMixerOutputPort(play_cae,playdeck->card(),
			     playdeck->stream(),
			     playdeck->port());
	if((int)logline->playPosition()>logline->effectiveLength()) {
	  LogLine(RDConfig::LogWarning,QString().sprintf("*** position out of bounds: Line: %d  Cart: %d  Pos: %d ***",line,logline->cartNumber(),logline->playPosition()));
	  logline->setPlayPosition(0);
	}
	playdeck->play(logline->playPosition(),-1,-1,duck_length);
	if(logline->status()==RDLogLine::RDLogLine::Paused) {
	  logline->
	    setStartTime(RDLogLine::Actual,playdeck->startTime());
	  was_paused=true;
	}
	else {
	  logline->
	    setStartTime(RDLogLine::Initial,playdeck->startTime());
	}
	logline->setStatus(RDLogLine::Playing);
	if(!play_start_rml[aport].isEmpty()) {
	  rdevent_player->
	    exec(logline->resolveWildcards(play_start_rml[aport]));
	}
	emit channelStarted(play_id,playdeck->channel(),
			    playdeck->card(),playdeck->port());
	LogLine(RDConfig::LogInfo,QString().sprintf(
		  "started audio cart: Line: %d  Cart: %u  Cut: %u Pos: %d  Card: %d  Stream: %d  Port: %d",
		  line,logline->cartNumber(),
		  playdeck->cut()->cutNumber(),
		  logline->playPosition(),
		  playdeck->card(),
		  playdeck->stream(),
		  playdeck->port()));

	//
	// Assign Next Event
	//
	if((play_next_line>=0)&&(!was_paused)) {
	  play_next_line=line+1;
	  if((next_logline=logLine(play_next_line))!=NULL) {
	    if(next_logline->id()==-2) {
	      play_start_next=false;
	    }
	  }
	  emit nextEventChanged(play_next_line);
	}
	break;

      case RDLogLine::Macro:
	//
	// Assign Next Event
	//
	if(play_next_line>=0) {
	  play_next_line=line+1;
	  if((next_logline=logLine(play_next_line))!=NULL) {
	    if(logline->id()==-2) {
	      play_start_next=false;
	    }
	    if(logline->forcedStop()) {
	      next_logline->setTransType(RDLogLine::Stop);
	    }
	  }
	}
	if(logline->asyncronous()) {
	  RDMacro *rml=new RDMacro();
	  rml->setCommand(RDMacro::EX);
	  QHostAddress addr;
	  addr.setAddress("127.0.0.1");
	  rml->setAddress(addr);
	  rml->setRole(RDMacro::Cmd);
	  rml->setEchoRequested(false);
	  rml->setArgQuantity(1);
	  rml->setArg(0,logline->cartNumber());
	  rdripc->sendRml(rml);
	  delete rml;
	  emit played(line);
	  logline->setStartTime(RDLogLine::Actual,QTime::currentTime());
	  logline->setStatus(RDLogLine::Finished);
	  LogTraffic(serviceName(),logName().left(logName().length()-4),
		     logline,(RDLogLine::PlaySource)(play_id+1),
		     RDAirPlayConf::TrafficMacro,play_onair_flag);
	  FinishEvent(line);
	  emit transportChanged();
	  LogLine(RDConfig::LogInfo,QString().
	     sprintf("asynchronously executed macro cart: Line: %d  Cart: %u",
		     line,logline->cartNumber()));
	}
	else {
	  play_macro_deck->load(logline->cartNumber());
	  play_macro_deck->setLine(line);
	  LogLine(RDConfig::LogInfo,QString().
		  sprintf("started macro cart: Line: %d  Cart: %u",
			  line,logline->cartNumber()));
	  play_macro_deck->exec();
	}
	break;

      case RDLogLine::Marker:
      case RDLogLine::Track:
      case RDLogLine::MusicLink:
      case RDLogLine::TrafficLink:
	//
	// Assign Next Event
	//
	if(play_next_line>=0) {
	  play_next_line=line+1;
	  if((next_logline=logLine(play_next_line))!=NULL) {
	    if(logLine(play_next_line)->id()==-2) {
	      play_start_next=false;
	    }
	  }
	  else {
	    play_start_next=false;
	  }
	}

	//
	// Skip Past
	//
	logline->setStatus(RDLogLine::Finished);
	UpdateStartTimes(line);
	emit played(line);
	FinishEvent(line);
	emit nextEventChanged(play_next_line);
	break;

      case RDLogLine::Chain:
	//
	// Assign Next Event
	//
	if(play_next_line>0) {
	  play_next_line=line+1;
	  if((next_logline=logLine(play_next_line))!=NULL) {
	    if(logLine(play_next_line)->id()==-2) {
	      play_start_next=false;
	    }
	  }
	  else {
	    play_start_next=false;
	  }
	}
	if(GetTransType(logline->markerLabel(),0)!=RDLogLine::Stop) {
	  play_macro_deck->
	    load(QString().sprintf("LL %d %s -2!",
				   play_id+1,
				   (const char *)logline->markerLabel()));
	}
	else {
	  play_macro_deck->
	    load(QString().sprintf("LL %d %s -2!",
				   play_id+1,
				   (const char *)logline->markerLabel()));
	}
	play_macro_deck->setLine(line);
	play_macro_deck->exec();
	LogLine(RDConfig::LogInfo,QString().
		sprintf("chained to log: Line: %d  Log: %s",
			line,
			(const char *)logline->markerLabel()));
	break;

      default:
	break;
  }
  while((play_next_line<size())&&((logline=logLine(play_next_line))!=NULL)) {
    if((logline->state()==RDLogLine::Ok)||
       (logline->state()==RDLogLine::NoCart)||
       (logline->state()==RDLogLine::NoCut)) {
      rdairplay_conf->setLogCurrentLine(play_id,nextLine());
      return true;
    }
    play_next_line++;
  }
  play_next_line=-1;
  rdairplay_conf->setLogCurrentLine(play_id,nextLine());
  return true;
}


bool LogPlay::StartAudioEvent(int line)
{
  RDLogLine *logline;
  RDPlayDeck *playdeck=NULL;

  if((logline=logLine(line))==NULL) {
    return false;
  }

  //
  // Get a Play Deck
  //
  if(logline->status()!=RDLogLine::Paused) {
    logline->setPlayDeck(GetPlayDeck());
    if(logline->playDeck()==NULL) {
      return false;
    }
    playdeck=(RDPlayDeck *)logline->playDeck();
    playdeck->setId(line);
  }
  else {
    playdeck=(RDPlayDeck *)logline->playDeck();
  }

  //
  // Assign Mappings
  //
  connect(playdeck,SIGNAL(stateChanged(int,RDPlayDeck::State)),
	  this,SLOT(playStateChangedData(int,RDPlayDeck::State)));
  connect(playdeck,SIGNAL(position(int,int)),
	  this,SLOT(positionData(int,int)));
  connect(playdeck,SIGNAL(segueStart(int)),
	  this,SLOT(segueStartData(int)));
  connect(playdeck,SIGNAL(segueEnd(int)),
	  this,SLOT(segueEndData(int)));
  connect(playdeck,SIGNAL(talkStart(int)),
	  this,SLOT(talkStartData(int)));
  connect(playdeck,SIGNAL(talkEnd(int)),
	  this,SLOT(talkEndData(int)));

  return true;
}


void LogPlay::CleanupEvent(int id)
{
  int line=GetLineById(id);
  bool top_changed=false;
  RDLogLine *logline;
  RDPlayDeck *playdeck=NULL;
  if((logline=logLine(line))==NULL) {
    return;
  }
  playdeck=(RDPlayDeck *)logline->playDeck();
  if(playdeck->cut()==NULL) {
    LogLine(RDConfig::LogErr,QString().
	    sprintf("event failed: Line: %d  Cart: %u",line,
		    logline->cartNumber()));
  }
  else {
    LogLine(RDConfig::LogInfo,QString().
	    sprintf("finished event: Line: %d  Cart: %u  Cut: %u Card: %d  Stream: %d  Port: %d",
		    line,logline->cartNumber(),
		    playdeck->cut()->cutNumber(),
		    playdeck->card(),
		    playdeck->stream(),playdeck->port()));
  }
  RDLogLine *prev_logline;
  if((prev_logline=logLine(line-1))==NULL) {
  }
  else {
    if((line==0)||(prev_logline->status()!=RDLogLine::Playing)) {
      play_line_counter++;
      top_changed=true;
    }
  }
  logline->setStatus(RDLogLine::Finished);
  FreePlayDeck(playdeck);
  logline->setPlayDeck(NULL);
  UpdatePostPoint();
  if(top_changed) {
    emit topEventChanged(play_line_counter);
  }
}


void LogPlay::UpdateStartTimes(int line)
{
  QTime time;
  QTime new_time;
  QTime end_time;
  QTime prev_time;
  QTime next_stop;
  int running=0;
  int prev_total_length=0;
  int prev_segue_length=0;
  bool playing=false;
  bool stop_set=false;
  bool stop;
  RDLogLine *logline;
  RDLogLine *next_logline;
  RDLogLine::TransType next_trans;
  int next_length=0;
  int lines[TRANSPORT_QUANTITY];

  if((running=runningEvents(lines,false))>0) {
    line=lines[0];
  }
  else {
    line=play_next_line;
  }
  for(int i=line;i<size();i++) {
    if((logline=logLine(i))!=NULL) {
      if((next_logline=logLine(nextLine(i)))!=NULL) {
	next_trans=next_logline->transType();
	next_length=next_logline->segueLength(next_trans);
      }
      else {
	next_trans=RDLogLine::Stop;
      }
      stop=false;
      switch(logline->status()) {
	  case RDLogLine::Playing:
	  case RDLogLine::Finishing:
	    time=logline->startTime(RDLogLine::Actual);
	    playing=true;
	    break;

	  default:
	    time=GetStartTime(logline->startTime(RDLogLine::Logged),
				  logline->transType(),
				  logline->timeType(),
				  time,prev_total_length,prev_segue_length,
				  &stop,running);
	    logline->setStartTime(RDLogLine::Predicted,time);
	    break;
      }
      if(stop&&(!stop_set)) {
	next_stop=time.addMSecs(prev_total_length);
	stop_set=true;
      }

      prev_total_length=logline->effectiveLength()-
	logline->playPosition();
      prev_segue_length=
	logline->segueLength(next_trans)-logline->playPosition();
      end_time=
	time.addMSecs(logline->effectiveLength()-
		      logline->playPosition());

      switch(logline->status()) {
      case RDLogLine::Scheduled:
      case RDLogLine::Paused:
	prev_total_length=logline->effectiveLength()-
	  logline->playPosition();
	prev_segue_length=
	  logline->segueLength(next_trans)-logline->playPosition();
	end_time=
	  time.addMSecs(logline->effectiveLength()-
			logline->playPosition());
	break; 
      default: 
	prev_total_length=logline->effectiveLength();
	prev_segue_length=
	  logline->segueLength(next_trans);
	end_time=
	  time.addMSecs(logline->effectiveLength());
      }
    }
  }
  next_stop=GetNextStop(line);

  if(next_stop!=play_next_stop) {
    play_next_stop=next_stop;
    emit nextStopChanged(play_next_stop);
  }
  SendNowNext();
}


void LogPlay::FinishEvent(int line)
{
  int prev_next_line=play_next_line;
  if(GetNextPlayable(&play_next_line,false)) {
    if(play_next_line>=0) {
      RDLogLine *logline;
      if((logline=logLine(play_next_line))==NULL) {
	return;
      }
      if((play_op_mode==RDAirPlayConf::Auto)&&
	 (logline->id()!=-1)&&(play_next_line<size())) {
	if(play_next_line>=0) {
	  if(logline->transType()==RDLogLine::Play) {
	    StartEvent(play_next_line,RDLogLine::Play,0,RDLogLine::StartPlay);
  	    SetTransTimer(QTime(),prev_next_line==play_trans_line);
	  }
	  if(logline->transType()==RDLogLine::Segue) {
	    StartEvent(play_next_line,RDLogLine::Segue,0,RDLogLine::StartPlay);
  	    SetTransTimer(QTime(),prev_next_line==play_trans_line);
	  }
	}
      }
    }
  }
  UpdateStartTimes(line);
  emit stopped(line);
}


QTime LogPlay::GetStartTime(QTime sched_time,
			    RDLogLine::TransType trans_type,
			    RDLogLine::TimeType time_type,QTime prev_time,
			    int prev_total_length,int prev_segue_length,
			    bool *stop,int running_events)
{
  QTime time;

  if((play_op_mode==RDAirPlayConf::LiveAssist)||
     (play_op_mode==RDAirPlayConf::Manual)) {
    *stop=true;
    return QTime();
  }
  switch(trans_type) {
      case RDLogLine::Play:
	if(!prev_time.isNull()) {
	  time=prev_time.addMSecs(prev_total_length);
	}
	break;
	
      case RDLogLine::Segue:
	if(!prev_time.isNull()) {
	  time=prev_time.addMSecs(prev_segue_length);
	}
	break;

      case RDLogLine::Stop:
	time=QTime();
	break;

      default:
	break;
  }
  switch(time_type) {
      case RDLogLine::Relative:
	if(!prev_time.isNull()) {
	  *stop=false;
	  return time;
	}
	*stop=true;
	return QTime();
	break;

      case RDLogLine::Hard:
	if((time<sched_time)||(time.isNull())) {
	  *stop=true;
	}
	else {
	  *stop=false;
	}
	if(running_events&&(time<sched_time)&&(trans_type!=RDLogLine::Stop)) {
	  return time;
	}
	return sched_time;
	break;
  }
  return QTime();
}


QTime LogPlay::GetNextStop(int line)
{
  bool running=false;
  QTime time;
  RDLogLine *logline;
  if((logline=logLine(line))==NULL) {
    return QTime();
  }

  for(int i=line;i<size();i++) {
    if((status(i)==RDLogLine::Playing)||
       (status(i)==RDLogLine::Finishing)) {
      if((logLine(i)->type()==RDLogLine::Cart)&&
	((logLine(i)->status()==RDLogLine::Playing)||
	 (logLine(i)->status()==RDLogLine::Finishing))) {
	time=
	  startTime(i).addMSecs(logLine(i)->segueLength(nextTrans(i))-
				((RDPlayDeck *)logLine(i)->playDeck())->
				lastStartPosition());
      }
      else {
	time=startTime(i).addMSecs(logLine(i)->segueLength(nextTrans(i)));
      }
      running=true;
    }
    else {
      if(running&&(play_op_mode==RDAirPlayConf::Auto)&&
	 (status(i)==RDLogLine::Scheduled)) {
	switch(logLine(i)->transType()) {
	    case RDLogLine::Stop:
	      return time;
	      break;

	    case RDLogLine::Play:
	    case RDLogLine::Segue:
	      time=time.addMSecs(logLine(i)->segueLength(nextTrans(i))-
					 logLine(i)->playPosition());
	      break;

	    default:
	      break;
	}
      }
    }
  }
  if(running!=play_running) {
    play_running=running;
    emit runStatusChanged(running);
  }
  return time;
}

void LogPlay::UpdatePostPoint()
{
  int lines[TRANSPORT_QUANTITY] = {-1};
  int count = runningEvents(lines,false);
  if (count > 0){
    UpdatePostPoint(lines[count -1]);
    return;
  }
  transportEvents(lines);
  UpdatePostPoint(lines[0]);
}

void LogPlay::UpdatePostPoint(int line)
{
  int post_line=-1;
  QTime post_time;
  int offset=0;

  if((line<0)||(play_trans_line<0)) {
    post_line=-1;
    post_time=QTime();
    offset=0;
  }
  else {
    if((line<size())&&(play_trans_line>=0)&&(play_trans_line<size())) {
      post_line=play_trans_line;
      post_time=logLine(post_line)->startTime(RDLogLine::Logged);
      offset=length(line,post_line)-QTime::currentTime().msecsTo(post_time);
    }
  }
  if((post_time!=play_post_time)||(offset!=play_post_offset)) {
    play_post_time=post_time;
    play_post_offset=offset;
    emit postPointChanged(play_post_time,offset,post_line>=line,running(false));
  }
}


void LogPlay::AdvanceActiveEvent()
{
  int line=-1;
  RDLogLine::TransType trans=RDLogLine::Play;

  for(int i=0;i<LOGPLAY_MAX_PLAYS;i++) {
    RDLogLine *logline;
    if((logline=logLine(play_line_counter+1))!=NULL) {
      if(logline->deck()!=-1) {
	line=play_line_counter+i;
      }
    }
  }
  if(line==-1) {
    if(line!=play_active_line) {
      play_active_line=line;
      emit activeEventChanged(line,RDLogLine::Stop);
    }
  }
  else {
    if(line<(size()-1)) {
      RDLogLine *logline;
      if((logline=logLine(line+1))!=NULL) {
	trans=logLine(line+1)->transType();
      }
    }
    else {
      trans=RDLogLine::Stop;
    }
    if((line!=play_active_line)||(trans!=play_active_trans)) {
      play_active_line=line;
      play_active_trans=trans;
      emit activeEventChanged(line,trans);
    }
  }
}


QString LogPlay::GetPortName(int card,int port)
{
  for(int i=0;i<2;i++) {
    for(int j=0;j<2;j++) {
      if((play_card[i]==card)&&(play_port[i]==port)) {
	return QString().sprintf("%d",i+1);
      }
    }
  }
  return QString();
}


void LogPlay::SetTransTimer(QTime current_time,bool stop)
{
  int next_line=-1;
  QTime next_time=QTime(23,59,59);

  if(current_time.isNull()) {
    current_time=QTime::currentTime();
  }
  RDLogLine *logline;

  if(play_trans_timer->isActive()) {
    if(stop)
	    play_trans_timer->stop();
    else
      return;
  }
  play_trans_line=-1;
  for(int i=0;i<size();i++) {
    if((logline=logLine(i))!=NULL) {
      if((logline->timeType()==RDLogLine::Hard)&&
	 ((logline->status()==RDLogLine::Scheduled)||
	  (logline->status()==RDLogLine::Auditioning))&&
	 (logline->startTime(RDLogLine::Logged)>current_time)&&
	 (logline->startTime(RDLogLine::Logged)<=next_time)) {
	next_time=logline->startTime(RDLogLine::Logged);
	next_line=i;
      }
    }
  }
  if(next_line>=0) {
    play_trans_line=next_line;
    play_trans_timer->start(current_time.msecsTo(next_time),true);
  }
}


int LogPlay::GetNextChannel(int mport,int *card,int *port)
{
  int chan=next_channel;
  if(mport<0) {
    *card=play_card[next_channel];
    *port=play_port[next_channel];
    if(++next_channel>1) {
      next_channel=0;
    }
  }
  else {
    chan=mport;
    *card=play_card[mport];
    *port=play_port[mport];
    next_channel=mport+1;
    if(next_channel>1) {
      next_channel=0;
    }
  }
  return chan;
}


int LogPlay::GetLineById(int id)
{
  return id;
}


RDPlayDeck *LogPlay::GetPlayDeck()
{
  for(int i=0;i<RD_MAX_STREAMS;i++) {
    if(!play_deck_active[i]) {
      play_deck_active[i]=true;
      return play_deck[i];
    }
  }
  return NULL;
}


void LogPlay::FreePlayDeck(RDPlayDeck *deck)
{
  for(int i=0;i<RD_MAX_STREAMS;i++) {
    if(play_deck[i]==deck) {
      ClearChannel(i);
      play_deck[i]->disconnect();
      play_deck[i]->reset();
      play_deck_active[i]=false;
      return;
    }
  }
}


bool LogPlay::GetNextPlayable(int *line,bool skip_meta,bool forced_start)
{
  RDLogLine *logline;
  RDLogLine *next_logline;
  RDLogLine::TransType next_type=RDLogLine::Play;
  int skipped=0;

  for(int i=*line;i<size();i++) {
    if((logline=logLine(i))==NULL) {
      return false;
    }
    if(skip_meta&&((logline->type()==RDLogLine::Marker)||
		   (logline->type()==RDLogLine::OpenBracket)||
		   (logline->type()==RDLogLine::CloseBracket)||
		   (logline->type()==RDLogLine::Track)||
		   (logline->type()==RDLogLine::MusicLink)||
		   (logline->type()==RDLogLine::TrafficLink))) {
      logline->setStatus(RDLogLine::Finished);
      skipped++;
      emit modified(i);
    }
    else {
      if(logline->status()==RDLogLine::Scheduled || logline->status()==RDLogLine::Paused || 
		      logline->status()==RDLogLine::Auditioning) {
        if(((logline->transType()==RDLogLine::Stop)||
	    (play_op_mode==RDAirPlayConf::LiveAssist))&&((i-skipped)!=*line)) {
	  makeNext(i);
	  return false;
        }
        if((next_logline=logLine(i+1))!=NULL) {
	  next_type=next_logline->transType();
        }
        if((logline->setEvent(play_id,next_type,logline->timescalingActive())==
	    RDLogLine::Ok)&&((logline->status()==RDLogLine::Scheduled)||
	  		     (logline->status()==RDLogLine::Paused))&&
	   (!logline->zombified())) {
	  emit modified(i);
	  *line=i;
	  return true;
        }
        else {
	  logline->setStartTime(RDLogLine::Initial,QTime::currentTime());
	  if((logline->transType()==RDLogLine::Stop)) {
            if((logline->cutNumber()>=0)&&(!logline->zombified())) {
                emit modified(i);
	        *line=i;
	        return true;
	    }
	    else {
	      if(!forced_start) {
                emit modified(i);
	        *line=i;
	        return true;
	      }
  	    }
	  }
        }
        emit modified(i);
      }
    }
  }
  return false;
}


void LogPlay::LogPlayEvent(RDLogLine *logline)
{
  RDCut *cut=new RDCut(QString().sprintf("%06u_%03d",
					 logline->cartNumber(),
					 logline->cutNumber()));
  cut->logPlayout();
  delete cut;
}


void LogPlay::RefreshEvents(int line,int line_quan,bool force_update)
{
  //QTime st=QTime::currentTime();

  //
  // Check Event Status
  //
  RDLogLine *logline;
  RDLogLine *next_logline;
  RDLogLine::State state=RDLogLine::Ok;

  for(int i=line;i<(line+line_quan);i++) {
    if((logline=logLine(i))!=NULL) {
      if(logline->type()==RDLogLine::Cart) {
	switch(logline->state()) {
	    case RDLogLine::Ok:
	    case RDLogLine::NoCart:
	    case RDLogLine::NoCut:
	      if(logline->status()==RDLogLine::Scheduled) {
		state=logline->state();
		if((next_logline=logLine(i+1))!=NULL) {
		  logline->
		    loadCart(logline->cartNumber(),next_logline->transType(),
			     play_id,logline->timescalingActive());
		}
		else {
		  logline->loadCart(logline->cartNumber(),RDLogLine::Play,
				    play_id,logline->timescalingActive());
		}
		if(force_update||(state!=logline->state())) {
		  emit modified(i);
		}
	      }
	      break;

	    default:
	      break;
	}
      }
    }
  }

  //
  // Check Refreshability
  //
  if(play_log!=NULL) {
    if((!play_log->exists())||(play_log->linkDatetime()!=play_link_datetime)||
       (play_log->modifiedDatetime()<=play_modified_datetime)) {
      if(play_refreshable) {
	play_refreshable=false;
	emit refreshabilityChanged(play_refreshable);
      }
    }
    else {
      if(play_log->autoRefresh()) {
	refresh();
      }
      else {
	if(!play_refreshable) {
	  play_refreshable=true;
	  emit refreshabilityChanged(play_refreshable);
	}
      }
    }
  }

/*
  if(play_id==0) {
    printf("LogPlay::RefreshEvents(%d,%d) took: %d msec\n",line,line_quan,
	   st.msecsTo(QTime::currentTime()));
  }
*/
}


void LogPlay::Playing(int id)
{
  RDLogLine *logline;

  int line=GetLineById(id);
  if((logline=logLine(line))==NULL) {
    return;
  }
  UpdateStartTimes(line);
  emit played(line);
  AdvanceActiveEvent();
  UpdatePostPoint();
  // TEST
  RefreshEvents(line,LOGPLAY_LOOKAHEAD_EVENTS);
  //
  LogPlayEvent(logline);
  emit transportChanged();
}


void LogPlay::Paused(int id)
{
  int line=GetLineById(id);
  RDLogLine *logline=logLine(line);
  if(logline!=NULL) {
    logline->playDeck()->disconnect();
    logline->setPortName("");
    logline->setStatus(RDLogLine::Paused);
  }
  UpdateStartTimes(line);
  emit paused(line);
  UpdatePostPoint();
  LogTraffic(serviceName(),logName().left(logName().length()-4),logLine(line),
	     (RDLogLine::PlaySource)(play_id+1),RDAirPlayConf::TrafficPause,
	     play_onair_flag);
  emit transportChanged();
}


void LogPlay::Stopping(int id)
{
}


void LogPlay::Stopped(int id)
{
  int line=GetLineById(id);
  int lines[TRANSPORT_QUANTITY];
  CleanupEvent(id);
  UpdateStartTimes(line);
  emit stopped(line);
  AdvanceActiveEvent();
  UpdatePostPoint();
  if(runningEvents(lines)==0) {
    next_channel=0;
  }
  LogTraffic(serviceName(),logName().left(logName().length()-4),logLine(line),
	     (RDLogLine::PlaySource)(play_id+1),RDAirPlayConf::TrafficStop,
	     play_onair_flag);
  emit transportChanged();
}


void LogPlay::Finished(int id)
{
  int line=GetLineById(id);
  RDLogLine *logline;
  int lines[TRANSPORT_QUANTITY];
  if((logline=logLine(line))==NULL) {
    return;
  }
  switch(logline->status()) {
      case RDLogLine::Playing:
	CleanupEvent(id);
	FinishEvent(line);
	break;

      case RDLogLine::Auditioning:
	break;

      default:
	break;
  }
  UpdatePostPoint();
  if(runningEvents(lines)==0) {
    next_channel=0;
  }
  LogTraffic(serviceName(),logName().left(logName().length()-4),logline,
	     (RDLogLine::PlaySource)(play_id+1),RDAirPlayConf::TrafficFinish,
	     play_onair_flag);
  emit transportChanged();
}


void LogPlay::ClearChannel(int deckid)
{
  if(play_deck[deckid]->channel()<0) {
    return;
  }
  if(rdcae->playPortActive(play_deck[deckid]->card(),
			   play_deck[deckid]->port(),
			   play_deck[deckid]->stream())) {
    return;
  }

  if(play_deck[deckid]->channel()>=0) {
    rdevent_player->exec(play_stop_rml[play_deck[deckid]->channel()]);
    emit channelStopped(play_id,play_deck[deckid]->channel(),
			play_deck[deckid]->card(),
			play_deck[deckid]->port());
  }
  play_deck[deckid]->setChannel(-1);
}


RDLogLine::TransType LogPlay::GetTransType(const QString &logname,int line)
{
  RDLogLine::TransType trans=RDLogLine::Stop;
  QString sql=QString("select TRANS_TYPE from ")+
    RDLog::tableName(logname)+" where "+QString().sprintf("COUNT=%d",line);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    trans=(RDLogLine::TransType)q->value(0).toUInt();
  }
  delete q;
  return trans;
}


bool LogPlay::ClearBlock(int start_line)
{
  RDLogLine::Status status;

  for(int i=start_line;i<size();i++) {
    status=logLine(i)->status();
    if((status!=RDLogLine::Scheduled)&&(status!=RDLogLine::Finished)) {
      remove(start_line,i-start_line);
      return true;
    }
  }
  remove(start_line,size()-start_line);
  return false;
}
