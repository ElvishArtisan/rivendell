// rdcartslot.cpp
//
// The cart slot widget.
//
//   (C) Copyright 2012-2014 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcartslot.cpp,v 1.13.2.19.2.2 2014/06/24 18:27:03 cvs Exp $
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

#include <qpainter.h>
#include <qbitmap.h>

#include "rdconfig.h"
#include "rdconf.h"
#include "rdescape_string.h"
#include "rdcart.h"
#include "rdcartslot.h"

RDCartSlot::RDCartSlot(int slotnum,RDRipc *ripc,RDCae *cae,RDStation *station,
		       RDConfig *config,RDListSvcs *svcs_dialog,
		       RDSlotDialog *slot_dialog,RDCartDialog *cart_dialog,
		       RDCueEditDialog *cue_dialog,
		       const QString &caption,RDAirPlayConf *conf,
		       QWidget *parent)
  : QWidget(parent)
{
  slot_number=slotnum;
  slot_ripc=ripc;
  slot_cae=cae;
  slot_station=station;
  slot_config=config;
  slot_svcs_dialog=svcs_dialog;
  slot_slot_dialog=slot_dialog;
  slot_cart_dialog=cart_dialog;
  slot_cue_dialog=cue_dialog;
  slot_caption=caption;
  slot_airplay_conf=conf;

  slot_svc_names=NULL;
  slot_stop_requested=false;
  slot_logline=new RDLogLine();
  slot_pause_enabled=false;
  slot_user=NULL;
  slot_svcname="";
  slot_breakaway_cart=0;
  slot_breakaway_length=0;
  slot_timescaling_active=false;
  slot_temp_cart=false;

  //
  // Fonts
  //
  QFont big_font("helvetica",36,QFont::Bold);
  big_font.setPixelSize(36);
  QFont mid_font("helvetica",14,QFont::Bold);
  mid_font.setPixelSize(14);

  //
  // Palettes
  //
  slot_ready_color=
    QPalette(QColor(BUTTON_STOPPED_BACKGROUND_COLOR),backgroundColor());
  slot_playing_color=
    QPalette(QColor(BUTTON_PLAY_BACKGROUND_COLOR),backgroundColor());

  //
  // Slot Options
  //
  slot_options=new RDSlotOptions(station->name(),slotnum);
  slot_options->load();

  //
  // Play Deck
  //
  slot_deck=new RDPlayDeck(slot_cae,0,this);
  connect(slot_deck,SIGNAL(stateChanged(int,RDPlayDeck::State)),
	  this,SLOT(stateChangedData(int,RDPlayDeck::State)));
  connect(slot_deck,SIGNAL(position(int,int)),
	  this,SLOT(positionData(int,int)));
  connect(slot_deck,SIGNAL(hookEnd(int)),this,SLOT(hookEndData(int)));
  connect(slot_cae,SIGNAL(timescalingSupported(int,bool)),
	  this,SLOT(timescalingSupportedData(int,bool)));

  //
  // Start Button
  //
  slot_start_button=new QPushButton(QString().sprintf("%d",slotnum+1),this);
  slot_start_button->setGeometry(0,0,sizeHint().height(),sizeHint().height());
  slot_start_button->setFont(big_font);
  slot_start_button->setDisabled(true);
  connect(slot_start_button,SIGNAL(clicked()),this,SLOT(startData()));

  //
  // Slot Box
  //
  slot_box=new RDSlotBox(slot_deck,conf,this);
  slot_box->setBarMode(false);
  slot_box->setAllowDrags(station->enableDragdrop());
  slot_box->setAcceptDrops(station->enableDragdrop());
  slot_box->setGeometry(5+sizeHint().height(),0,
			slot_box->sizeHint().width(),
			slot_box->sizeHint().height());
  connect(slot_box,SIGNAL(doubleClicked()),this,SLOT(doubleClickedData()));
  connect(slot_box,SIGNAL(cartDropped(unsigned)),
	  this,SLOT(cartDroppedData(unsigned)));

  //
  // Load Button
  //
  slot_load_button=new QPushButton(tr("Load"),this);
  slot_load_button->
    setGeometry(sizeHint().height()+5+slot_box->sizeHint().width()+5,0,
		sizeHint().height(),sizeHint().height());
  slot_load_button->setFont(mid_font);
  connect(slot_load_button,SIGNAL(clicked()),this,SLOT(loadData()));

  //
  // Options Button
  //
  slot_options_button=new QPushButton(this);
  slot_options_button->
    setGeometry(2*sizeHint().height()+10+slot_box->sizeHint().width()+5,0,
		sizeHint().height(),sizeHint().height());
  slot_options_button->setFont(mid_font);
  connect(slot_options_button,SIGNAL(clicked()),this,SLOT(optionsData()));

  updateOptions();
  InitializeOptions();
}


RDCartSlot::~RDCartSlot()
{
  stop();
  ClearTempCart();
  delete slot_logline;
  delete slot_options;
}


QSize RDCartSlot::sizeHint() const
{
  return QSize(670,86);
}


QSizePolicy RDCartSlot::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void RDCartSlot::setUser(RDUser *user)
{
  slot_user=user;
}


RDSlotOptions *RDCartSlot::slotOptions() const
{
  return slot_options;
}


void RDCartSlot::updateOptions()
{
  slot_deck->setCard(slot_options->card());
  slot_deck->setPort(slot_options->outputPort());
  switch(slot_options->mode()) {
  case RDSlotOptions::CartDeckMode:
    SetInput(false);
    slot_logline->setHookMode(slot_options->hookMode());
    if(slot_options->hookMode()) {
      slot_options_button->setText(tr("Options")+"\n"+tr("[Hook]"));
    }
    else {
      slot_options_button->setText(tr("Options")+"\n"+tr("[Full]"));
    }

    break;

  case RDSlotOptions::BreakawayMode:
    SetInput(true);
    slot_start_button->setDisabled(true);
    slot_box->setService(slot_svcname);
    slot_box->setStatusLine(tr("Waiting for break..."));
    slot_load_button->setText(tr("Load"));
    slot_logline->setHookMode(false);
    slot_options_button->setText(tr("Options")+"\n"+tr("[Breakaway]"));
    break;

  case RDSlotOptions::LastMode:
    break;
  }
  slot_box->setMode(slot_options->mode());
  slot_options->save();
  if(slot_logline->cartNumber()!=0) {
    load(slot_logline->cartNumber());
  }
}


void RDCartSlot::setSvcNames(std::vector<QString> *svcnames)
{
  slot_svc_names=svcnames;
}


void RDCartSlot::setCart(RDCart *cart,int break_len)
{
  if(cart==NULL) {
    slot_logline->clear();
    if(slot_options->mode()!=RDSlotOptions::BreakawayMode) {
      slot_logline->setHookMode(slot_options->hookMode());
    }
    slot_box->clear();
  }
  else {
    slot_logline->loadCart(cart->number(),RDLogLine::Play,0,true,
			   RDLogLine::NoTrans,break_len);
    slot_logline->
      setEvent(0,RDLogLine::Play,slot_logline->timescalingActive(),break_len);
    slot_box->setCart(slot_logline);
    slot_box->setBarMode(false);
  }
}


bool RDCartSlot::load(int cartnum,int break_len)
{
  bool ret=false;
  RDCart *cart=new RDCart(cartnum);
  if(cart->exists()) {
    if(slot_deck->state()!=RDPlayDeck::Stopped) {
      stop();
    }
    setCart(cart,break_len);
    slot_start_button->
      setEnabled(slot_options->mode()==RDSlotOptions::CartDeckMode);
    slot_start_button->setPalette(slot_ready_color);
    slot_load_button->setText(tr("Unload"));
    slot_options->setCartNumber(cartnum);
    slot_options->save();
    ret=true;
  }
  delete cart;
  return ret;
}


void RDCartSlot::unload()
{
  if(slot_deck->state()==RDPlayDeck::Stopped) {
    ClearTempCart();
    setCart(NULL);
    slot_start_button->setDisabled(true);
    slot_start_button->setPalette(palette());
    slot_load_button->setText(tr("Load"));
    slot_options->setCartNumber(0);
    slot_options->save();
  }
}


bool RDCartSlot::play()
{
  bool ret=false;
  if(slot_logline->cartNumber()!=0) {
    if(slot_deck->setCart(slot_logline,true)) {
      if(slot_options->hookMode()&&(slot_logline->hookStartPoint()>=0)) {
	slot_deck->playHook();
      }
      else {
	slot_deck->play(slot_logline->playPosition());
      }
      slot_logline->setStartTime(RDLogLine::Actual,QTime::currentTime());
      //      LogPlayout(RDAirPlayConf::TrafficStart);
      ret=true;
    }
  }
  return ret;
}


bool RDCartSlot::pause()
{
  return false;
}


bool RDCartSlot::stop()
{
  bool ret=false;
  if(slot_logline->cartNumber()!=0) {
    slot_stop_requested=true;
    slot_deck->stop();
    //    LogPlayout(RDAirPlayConf::TrafficStop);
    ret=true;
  }
  return ret;
}


bool RDCartSlot::breakAway(unsigned msecs)
{
  bool ret=false;
  unsigned cartnum=0;

  if(slot_options->mode()==RDSlotOptions::BreakawayMode) {
    if(msecs==0) {
      stop();
      SetInput(true);
      unload();
      slot_box->setService(slot_svcname);
      slot_box->setStatusLine(tr("Waiting for break..."));
    }
    else {
      cartnum=SelectCart(slot_svcname,msecs);
      if(cartnum!=0) {
	switch(slot_deck->state()) {
	case RDPlayDeck::Playing:
	case RDPlayDeck::Paused:
	case RDPlayDeck::Stopping:
	  slot_breakaway_cart=cartnum;
	  slot_breakaway_length=msecs;
	  stop();
	  break;

	case RDPlayDeck::Stopped:
	case RDPlayDeck::Finished:
	SetInput(false);
	if(slot_timescaling_active) {
	  load(cartnum,msecs);
	}
	else {
	  load(cartnum);
	}
	play();
	syslog(LOG_INFO,"started breakaway, len: %u  cart: %u  cut: %d",
	       msecs,cartnum,slot_logline->cutNumber());
	break;
	}
      }
      else {
	slot_box->setStatusLine(tr("No cart found for length")+" "+
				RDGetTimeLength(msecs,false,false));
      }
    }
  }
  return ret;
}


bool RDCartSlot::pauseEnabled() const
{
  return slot_pause_enabled;
}


void RDCartSlot::setPauseEnabled(bool state)
{
  slot_pause_enabled=state;
}


void RDCartSlot::updateMeters()
{
  short lvls[2];

  switch(slot_deck->state()) {
  case RDPlayDeck::Playing:
  case RDPlayDeck::Stopping:
    slot_cae->
      outputStreamMeterUpdate(slot_deck->card(),slot_deck->stream(),lvls);
    slot_box->updateMeters(lvls);
    break;

  case RDPlayDeck::Paused:
  case RDPlayDeck::Stopped:
  case RDPlayDeck::Finished:
    break;
  }
}


void RDCartSlot::startData()
{
  switch(slot_deck->state()) {
  case RDPlayDeck::Playing:
  case RDPlayDeck::Stopping:
  case RDPlayDeck::Paused:
    stop();
    break;

  case RDPlayDeck::Stopped:
  case RDPlayDeck::Finished:
    play();
    break;
  }
}


void RDCartSlot::doubleClickedData()
{
  if(slot_logline->cartNumber()==0) {
    loadData();
  }
  else {
    if(slot_cue_dialog->exec(slot_logline)==0) {
      slot_box->setBarMode(true);
      slot_box->setCart(slot_logline);
    }
  }
}


void RDCartSlot::loadData()
{
  int cartnum;
  QString svcname;

  switch(slot_options->mode()) {
  case RDSlotOptions::CartDeckMode:
    cartnum=slot_logline->cartNumber();
    if(cartnum==0) {
      if(slot_cart_dialog->exec(&cartnum,RDCart::All,&svcname,0,
				slot_user->name(),slot_user->password(),
				&slot_temp_cart)==0) {
	load(cartnum);
      }
    }
    else {
      unload();
    }
    break;

  case RDSlotOptions::BreakawayMode:
    if(slot_svcs_dialog->exec(&slot_svcname)==0) {
      slot_box->setService(slot_svcname);
      slot_box->setStatusLine(tr("Waiting for break..."));
    }
    break;

  case RDSlotOptions::LastMode:
    break;
  }
}


void RDCartSlot::optionsData()
{
  RDSlotOptions::Mode old_mode=slot_options->mode();
  if(slot_slot_dialog->exec(slot_options)==0) {
    if(old_mode!=slot_options->mode()) {
      slot_box->clear();
    }
    updateOptions();
  }
}


void RDCartSlot::stateChangedData(int id,RDPlayDeck::State state)
{
  //printf("stateChangedData(%d,%d)\n",id,state);
  short lvls[2]={-10000,-10000};

  switch(state) {
  case RDPlayDeck::Playing:
    LogPlayout(state);
    slot_start_button->
      setEnabled(slot_options->mode()==RDSlotOptions::CartDeckMode);
    slot_start_button->setPalette(slot_playing_color);
    slot_load_button->setDisabled(true);
    slot_options_button->setDisabled(true);
    break;

  case RDPlayDeck::Stopped:
  case RDPlayDeck::Finished:
    LogPlayout(state);
    slot_start_button->
      setEnabled(slot_options->mode()==RDSlotOptions::CartDeckMode);
    slot_start_button->setPalette(slot_ready_color);
    slot_load_button->setEnabled(true);
    slot_options_button->setEnabled(true);
    slot_box->setTimer(0);
    slot_box->updateMeters(lvls);
    slot_box->setCart(slot_logline);
    switch(slot_options->mode()) {
    case RDSlotOptions::CartDeckMode:
      if(!slot_stop_requested) {
	switch(slot_options->stopAction()) {
	case RDSlotOptions::RecueOnStop:
	  break;
	  
	case RDSlotOptions::UnloadOnStop:
	  unload();
	  break;
	  
	case RDSlotOptions::LoopOnStop:
	  play();
	  break;
	  
	case RDSlotOptions::LastStop:
	  break;
	}
      }
      break;

    case RDSlotOptions::BreakawayMode:
      if(slot_breakaway_cart>0) {
	SetInput(false);
	load(slot_breakaway_cart);
	play();
	syslog(LOG_INFO,"started breakaway, len: %u  cart: %u  cut: %d",
	       slot_breakaway_length,slot_breakaway_cart,
	       slot_logline->cutNumber());
	slot_breakaway_cart=0;
	slot_breakaway_length=0;
      }
      else {
	SetInput(true);
	unload();
	slot_box->setService(slot_svcname);
	slot_box->setStatusLine(tr("Waiting for break..."));
	//	LogPlayout(RDAirPlayConf::TrafficFinish);
      }
      break;

    case RDSlotOptions::LastMode:
      break;
    }
    slot_stop_requested=false;
    break;

  case RDPlayDeck::Stopping:
  case RDPlayDeck::Paused:
    break;
  }
  
}


void RDCartSlot::positionData(int id,int msecs)
{
  slot_box->setTimer(msecs);
}


void RDCartSlot::hookEndData(int id)
{
  if(slot_options->hookMode()) {
    stop();
  }
}


void RDCartSlot::timescalingSupportedData(int card,bool state)
{
  if(card==slot_options->card()) {
    slot_timescaling_active=state;
  }
}


void RDCartSlot::cartDroppedData(unsigned cartnum)
{
  if(cartnum==0) {
    unload();
  }
  else {
    load(cartnum);
  }
}


void RDCartSlot::InitializeOptions()
{
  slot_svcname=slot_options->service();

  switch(slot_options->mode()) {
  case RDSlotOptions::CartDeckMode:
    if(slot_options->cartNumber()>0) {
      load(slot_options->cartNumber());
    }
    break;

  case RDSlotOptions::BreakawayMode:
    slot_box->setService(slot_svcname);
    slot_box->setStatusLine(tr("Waiting for break..."));
    break;

  case RDSlotOptions::LastMode:
    break;
  }
  slot_cae->requestTimescale(slot_options->card());
}


unsigned RDCartSlot::SelectCart(const QString &svcname,unsigned msecs)
{
  QString sql;
  RDSqlQuery *q;
  unsigned cartnum=0;
  int diff=1000000;

  sql=QString("select AUTOFILLS.CART_NUMBER,CART.FORCED_LENGTH from ")+
    "AUTOFILLS left join CART on AUTOFILLS.CART_NUMBER=CART.NUMBER"+
    QString().
    sprintf(" where (CART.FORCED_LENGTH>%u)&&(CART.FORCED_LENGTH<%u)&&",
	    (unsigned)((double)msecs*RD_TIMESCALE_MIN),
	    (unsigned)((double)msecs*RD_TIMESCALE_MAX))+
    "(SERVICE=\""+RDEscapeString(svcname)+"\")";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if(::abs(msecs-q->value(1).toInt())<diff) {
      cartnum=q->value(0).toUInt();
      diff=::abs(msecs-q->value(1).toInt());
    }
  }
  delete q;
  return cartnum;
}


void RDCartSlot::SetInput(bool state)
{
  int level=-10000;
  if(state) {
    level=0;
  }
  slot_cae->
    setPassthroughVolume(slot_options->card(),slot_options->inputPort(),
			 slot_options->outputPort(),level);
}


void RDCartSlot::LogPlayout(RDPlayDeck::State state)
{
  if(state==RDPlayDeck::Playing) {
    RDCut *cut=new RDCut(slot_logline->cutName());
    cut->logPlayout();
    delete cut;
  }
  if((state!=RDPlayDeck::Stopped)&&(state!=RDPlayDeck::Finished)) {
    return;
  }
  RDAirPlayConf::TrafficAction action=RDAirPlayConf::TrafficFinish;
  if(state==RDPlayDeck::Stopped) {
    action=RDAirPlayConf::TrafficStop;
  }
  QString sql;
  RDSqlQuery *q;
  QDateTime datetime=QDateTime(QDate::currentDate(),QTime::currentTime());
  int length=
    slot_logline->startTime(RDLogLine::Actual).msecsTo(datetime.time());
  if(length<0) {  // Event crossed midnight!
    length+=86400000;
    datetime.setDate(datetime.date().addDays(-1));
  }
  QString svctablename=slot_svcname;
  svctablename.replace(" ","_");
  sql=QString("insert into `")+svctablename+"_SRT` set "+
    QString().sprintf("LENGTH=%d,LOG_ID=%d,CART_NUMBER=%u,EVENT_TYPE=%d,\
                       EVENT_SOURCE=%d,EXT_LENGTH=%d,PLAY_SOURCE=%d,\
                       CUT_NUMBER=%d,USAGE_CODE=%d,START_SOURCE=%d,",
		      length,
		      slot_number+1,
		      slot_logline->cartNumber(),
		      action,
		      slot_logline->source(),
		      slot_logline->extLength(),
		      RDLogLine::CartSlot,
		      slot_logline->cutNumber(),
		      slot_logline->usageCode(),
		      slot_logline->startSource())+
    "STATION_NAME=\""+RDEscapeString(slot_station->name())+"\","+
    "EVENT_DATETIME=\""+datetime.toString("yyyy-MM-dd")+
    " "+slot_logline->startTime(RDLogLine::Actual).toString("hh:mm:ss")+"\","+
    "EXT_START_TIME=\""+slot_logline->extStartTime().toString("hh:mm:ss")+"\","+
    "EXT_DATA=\""+RDEscapeString(slot_logline->extData())+"\","+
    "EXT_EVENT_ID=\""+RDEscapeString(slot_logline->extEventId())+"\","+
    "EXT_ANNC_TYPE=\""+RDEscapeString(slot_logline->extAnncType())+"\","+
    "EXT_CART_NAME=\""+RDEscapeString(slot_logline->extCartName())+"\","+
    "TITLE=\""+RDEscapeString(slot_logline->title())+"\","+
    "ARTIST=\""+RDEscapeString(slot_logline->artist())+"\","+
    "SCHEDULED_TIME=\""+slot_logline->startTime(RDLogLine::Logged).
    toString("hh:mm:ss")+"\","+
    "ISRC=\""+RDEscapeString(slot_logline->isrc())+"\","+
    "PUBLISHER=\""+RDEscapeString(slot_logline->publisher())+"\","+
    "COMPOSER=\""+RDEscapeString(slot_logline->composer())+"\","+
    "ONAIR_FLAG=\""+RDYesNo(slot_ripc->onairFlag())+"\","+
    "ALBUM=\""+RDEscapeString(slot_logline->album())+"\","+
    "LABEL=\""+RDEscapeString(slot_logline->label())+"\","+
    "CONDUCTOR=\""+RDEscapeString(slot_logline->conductor())+"\","+
    "USER_DEFINED=\""+RDEscapeString(slot_logline->userDefined())+"\","+
    "SONG_ID=\""+RDEscapeString(slot_logline->songId())+"\","+
    "ISCI=\""+RDEscapeString(slot_logline->isci())+"\"";
  q=new RDSqlQuery(sql);
  delete q;
}

void RDCartSlot::ClearTempCart()
{
  RDCart *cart=NULL;

  if(slot_temp_cart) {
    cart=new RDCart(slot_logline->cartNumber());
    if(cart->exists()) {
      cart->remove(slot_station,slot_user,slot_config);
    }
    slot_temp_cart=false;
    delete cart;
  }
}
