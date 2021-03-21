// rdmarkerplayer.cpp
//
// Audio player for RDMarkerDialog
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

#include "rdconf.h"
#include "rdmixer.h"
#include "rdmarkerplayer.h"

RDMarkerPlayer::RDMarkerPlayer(int card,int port,QWidget *parent)
  : RDWidget(parent)
{
  d_cards.push_back(card);
  d_port=port;
  d_cae_stream=-1;
  d_cae_handle=-1;
  d_is_playing=false;
  d_looping=false;

  //
  // CAE
  //
  rda->cae()->enableMetering(&d_cards);
  connect(rda->cae(),SIGNAL(playing(int)),this,SLOT(caePlayedData(int)));
  connect(rda->cae(),SIGNAL(playStopped(int)),this,SLOT(caePausedData(int)));
  connect(rda->cae(),SIGNAL(playPositionChanged(int,unsigned)),
	  this,SLOT(caePositionData(int,unsigned)));

  //
  // Time Counters
  //
  d_position_label=new QLabel(tr("Position"),this);
  d_position_label->setFont(subLabelFont());
  d_position_label->setAlignment(Qt::AlignCenter);
  d_position_label->
    setPalette(QPalette(palette().color(QPalette::Background),
  			QColor(RDMARKERPLAYER_HIGHLIGHT_COLOR)));
  d_position_edit=new QLabel(this);
  d_position_edit->setAcceptDrops(false);
  d_position_edit->setAlignment(Qt::AlignCenter);
  d_position_edit->
    setStyleSheet("background-color: "+palette().color(QPalette::Base).name());

  d_region_edit_label=new QLabel("Region",this);
  d_region_edit_label->setFont(subLabelFont());
  d_region_edit_label->setAlignment(Qt::AlignCenter);
  d_region_edit_label->
    setPalette(QPalette(palette().color(QPalette::Background),
			QColor(RDMARKERPLAYER_HIGHLIGHT_COLOR)));
  d_region_edit=new QLabel(this);
  d_region_edit->setAcceptDrops(false);
  d_region_edit->setAlignment(Qt::AlignCenter);
  d_region_edit->
    setStyleSheet("background-color: "+palette().color(QPalette::Base).name());

  d_length_label=new QLabel(tr("Length"),this);
  d_length_label->setFont(subLabelFont());
  d_length_label->setAlignment(Qt::AlignCenter);
  d_length_label->
    setPalette(QPalette(palette().color(QPalette::Background),
			QColor(RDMARKERPLAYER_HIGHLIGHT_COLOR)));
  d_length_edit=new QLabel(this);
  d_length_edit->setAcceptDrops(false);
  d_length_edit->setAlignment(Qt::AlignCenter);
  d_length_edit->
    setStyleSheet("background-color: "+palette().color(QPalette::Base).name());

  //
  // Transport Buttons
  //
  d_play_button=
    new RDTransportButton(RDTransportButton::Play,this);
  d_play_button->setFocusPolicy(Qt::NoFocus);
  d_play_button->setEnabled((d_cards.first()>=0)&&(d_port>=0));
  connect(d_play_button,SIGNAL(clicked()),
  	  this,SLOT(buttonPlayData()));

  d_play_from_button=
    new RDTransportButton(RDTransportButton::PlayFrom,this);
  d_play_from_button->setFocusPolicy(Qt::NoFocus);
  d_play_from_button->setEnabled((d_cards.first()>=0)&&(d_port>=0));
  connect(d_play_from_button,SIGNAL(clicked()),
  	  this,SLOT(buttonPlayFromData()));

  d_play_to_button=new RDTransportButton(RDTransportButton::PlayTo,this);
  d_play_to_button->setFocusPolicy(Qt::NoFocus);
  d_play_to_button->setEnabled((d_cards.first()>=0)&&(d_port>=0));
  connect(d_play_to_button,SIGNAL(clicked()),this,SLOT(buttonPlayToData()));

  d_stop_button=new RDTransportButton(RDTransportButton::Stop,this);
  d_stop_button->setFocusPolicy(Qt::NoFocus);
  d_stop_button->on();
  d_stop_button->setOnColor(QColor(Qt::red));
  d_stop_button->setEnabled((d_cards.first()>=0)&&(d_port>=0));
  connect(d_stop_button,SIGNAL(clicked()),this,SLOT(buttonStopData()));

  d_loop_button=new RDTransportButton(RDTransportButton::Loop,this);
  d_loop_button->off();
  d_loop_button->setEnabled((d_cards.first()>=0)&&(d_port>=0));
  connect(d_loop_button,SIGNAL(clicked()),this,SLOT(buttonLoopData()));

  //
  // The Audio Meter
  //
  d_meter=new RDStereoMeter(this);
  d_meter->setSegmentSize(5);
  d_meter->setMode(RDSegMeter::Peak);
  d_meter_timer=new QTimer(this);
  connect(d_meter_timer,SIGNAL(timeout()),this,SLOT(meterData()));
}


RDMarkerPlayer::~RDMarkerPlayer()
{
}


QSize RDMarkerPlayer::sizeHint() const
{
  return QSize(736,96);
}


QSizePolicy RDMarkerPlayer::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


bool RDMarkerPlayer::setCut(unsigned cartnum,int cutnum)
{
  clearCut();

  if(!rda->cae()->loadPlay(d_cards.first(),RDCut::cutName(cartnum,cutnum),
			   &d_cae_stream,&d_cae_handle)) {
    return false;
  }
  RDSetMixerOutputPort(rda->cae(),d_cards.first(),d_cae_stream,d_port);

  return true;
}


void RDMarkerPlayer::clearCut()
{
  if(d_cae_handle>=0) {
    rda->cae()->stopPlay(d_cae_handle);
    rda->cae()->unloadPlay(d_cae_handle);
    d_cae_stream=-1;
    d_cae_handle=-1;
    d_is_playing=false;
  }
  for(int i=0;i<RDMarkerHandle::LastRole;i++) {
    d_pointers[i]=-1;
  }
  d_looping=false;
  d_stopping=false;
}


int RDMarkerPlayer::cursorPosition() const
{
  return d_cursor_position;
}


void RDMarkerPlayer::setCursorPosition(int msec)
{
  rda->cae()->positionPlay(d_cae_handle,msec);
}


void RDMarkerPlayer::setPointerValue(RDMarkerHandle::PointerRole role,int ptr)
{
  if(ptr!=d_pointers[role]) {
    d_pointers[role]=ptr;
    UpdateReadouts();
  }
}


void RDMarkerPlayer::setSelectedMarkers(RDMarkerHandle::PointerRole start_role,
					RDMarkerHandle::PointerRole end_role)
{
  QColor color=RDMarkerHandle::pointerRoleColor(start_role);
  if(start_role==RDMarkerHandle::LastRole) {
    color=RDMarkerHandle::pointerRoleColor(end_role);
  }
  QString ss=
    "color:"+RDGetTextColor(color).name()+";background-color:"+color.name();
  d_region_edit_label->setStyleSheet(ss);
  d_play_from_button->setAccentColor(color);
  d_play_to_button->setAccentColor(color);
  d_selected_markers[0]=start_role;
  d_selected_markers[1]=end_role;

  d_play_from_button->setDisabled(start_role==RDMarkerHandle::LastRole);
  d_play_to_button->setDisabled(end_role==RDMarkerHandle::LastRole);

  UpdateReadouts();
}


void RDMarkerPlayer::buttonPlayData()
{
  d_active_play_button=d_play_button;

  if(d_cae_handle>=0) {
    if(d_is_playing) {
      rda->cae()->stopPlay(d_cae_handle);
    }
  }
  d_loop_start_msec=d_cursor_position;
  d_loop_start_length=0;
  rda->cae()->play(d_cae_handle,d_loop_start_length,100000,false);
  rda->cae()->setPlayPortActive(d_cards.first(),d_port,d_cae_stream);
  // FIXME: Implement variable gain here!
  rda->cae()->setOutputVolume(d_cards.first(),d_cae_stream,d_port,0);
  //    rda->cae()->
  //      setOutputVolume(d_cards.first(),d_cae_stream,d_port,0+edit_gain_control->value());
  d_meter_timer->start(RD_METER_UPDATE_INTERVAL);
}


void RDMarkerPlayer::buttonPlayFromData()
{
  d_active_play_button=d_play_from_button;
  if(d_cae_handle>=0) {
    if(d_is_playing) {
      rda->cae()->stopPlay(d_cae_handle);
    }
  }
  if(d_selected_markers[RDMarkerHandle::Start]!=RDMarkerHandle::LastRole) {
    d_loop_start_msec=d_pointers[d_selected_markers[0]];
    rda->cae()->positionPlay(d_cae_handle,d_loop_start_msec);
    d_loop_start_length=0;
    rda->cae()->play(d_cae_handle,d_loop_start_length,100000,false);
    rda->cae()->setPlayPortActive(d_cards.first(),d_port,d_cae_stream);
    // FIXME: Implement variable gain here!
    rda->cae()->setOutputVolume(d_cards.first(),d_cae_stream,d_port,0);
    //    rda->cae()->
    //      setOutputVolume(d_cards.first(),d_cae_stream,d_port,0+edit_gain_control->value());
    d_meter_timer->start(RD_METER_UPDATE_INTERVAL);
  }
}


void RDMarkerPlayer::buttonPlayToData()
{
  d_active_play_button=d_play_to_button;
  if(d_cae_handle>=0) {
    if(d_is_playing) {
      rda->cae()->stopPlay(d_cae_handle);
    }
  }
  if(d_selected_markers[RDMarkerHandle::End]!=RDMarkerHandle::LastRole) {
    d_loop_start_msec=d_pointers[d_selected_markers[1]]-2000;
    d_loop_start_length=2000;
    if(d_loop_start_msec<0) {
      d_loop_start_msec=0;
      d_loop_start_length=d_pointers[d_selected_markers[1]];
    }
    rda->cae()->positionPlay(d_cae_handle,d_loop_start_msec);
    rda->cae()->play(d_cae_handle,d_loop_start_length,100000,false);
    rda->cae()->setPlayPortActive(d_cards.first(),d_port,d_cae_stream);
    // FIXME: Implement variable gain here!
    rda->cae()->setOutputVolume(d_cards.first(),d_cae_stream,d_port,0);
    //    rda->cae()->
    //      setOutputVolume(d_cards.first(),d_cae_stream,d_port,0+edit_gain_control->value());
    d_meter_timer->start(RD_METER_UPDATE_INTERVAL);
  }
}


void RDMarkerPlayer::buttonStopData()
{
  if(d_cae_handle>=0) {
    if(d_is_playing) {
      d_stopping=true;
      rda->cae()->stopPlay(d_cae_handle);
    }
  }
}


void RDMarkerPlayer::buttonLoopData()
{
  d_looping=!d_looping;
  if(d_looping) {
    d_loop_button->setState(RDTransportButton::On);
  }
  else {
    d_loop_button->setState(RDTransportButton::Off);
  }
}


void RDMarkerPlayer::meterData()
{
  short lvls[2];

  rda->cae()->outputMeterUpdate(d_cards.first(),d_port,lvls);
  d_meter->setLeftPeakBar(lvls[0]);
  d_meter->setRightPeakBar(lvls[1]);
}


void RDMarkerPlayer::caePlayedData(int handle)
{
  if(handle==d_cae_handle) {
    if(!d_is_playing) {
      d_active_play_button->setState(RDTransportButton::On);
      d_stop_button->setState(RDTransportButton::Off);
      d_is_playing=true;
    }
  }
}


void RDMarkerPlayer::caePausedData(int handle)
{
  if(handle==d_cae_handle) {
    if(d_is_playing) {
      if(d_looping&&(!d_stopping)) {
	rda->cae()->positionPlay(d_cae_handle,d_loop_start_msec);
	rda->cae()->play(d_cae_handle,d_loop_start_length,100000,false);
      }
      else {
	d_stopping=false;
	if(d_meter_timer->isActive()) {
	  d_meter_timer->stop();
	  d_meter->setLeftPeakBar(-10000);
	  d_meter->setRightPeakBar(-10000);
	}
	d_play_from_button->setState(RDTransportButton::Off);
	d_play_button->setState(RDTransportButton::Off);
	d_play_to_button->setState(RDTransportButton::Off);
	d_stop_button->setState(RDTransportButton::On);
	d_is_playing=false;
      }
    }
  }
}


void RDMarkerPlayer::caePositionData(int handle,unsigned msec)
{
  if(handle==d_cae_handle) {
    d_position_edit->setText(RDGetTimeLength(msec-d_pointers[RDMarkerHandle::CutStart],true,true));
    d_cursor_position=msec;
    emit cursorPositionChanged(msec);
  }
}


void RDMarkerPlayer::resizeEvent(QResizeEvent *)
{
  d_position_label->setGeometry(50,3,70,16);
  d_position_edit->setGeometry(50,20,70,18);
  d_region_edit_label->setGeometry(148,3,70,16);
  d_region_edit->setGeometry(148,20,70,18);
  d_length_label->setGeometry(246,3,70,16);
  d_length_edit->setGeometry(246,20,70,18);

  d_play_button->setGeometry(10,42,65,45);
  d_play_from_button->setGeometry(80,42,65,45);
  d_play_to_button->setGeometry(150,42,65,45);
  d_stop_button->setGeometry(220,42,65,45);
  d_loop_button->setGeometry(290,42,65,45);

  d_meter->setGeometry(370,15,d_meter->sizeHint().width(),
		       d_meter->sizeHint().height());
}


void RDMarkerPlayer::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);

  //
  // Transport Control Area
  //
  p->setPen(QColor(palette().shadow().color()));
  p->fillRect(1,1,size().width()-2,size().height()-2,
	      QColor(RDMARKERPLAYER_HIGHLIGHT_COLOR));
  p->drawRect(0,0,size().width(),size().height());

  delete p;
}


void RDMarkerPlayer::UpdateReadouts()
{
  //
  // Region
  //
  switch(d_selected_markers[0]) {
  case RDMarkerHandle::CutStart:
  case RDMarkerHandle::TalkStart:
  case RDMarkerHandle::SegueStart:
  case RDMarkerHandle::HookStart:
    d_region_edit->
      setText(RDGetTimeLength(d_pointers[d_selected_markers[1]]-
			      d_pointers[d_selected_markers[0]],true,true));
    break;

  case RDMarkerHandle::CutEnd:
  case RDMarkerHandle::TalkEnd:
  case RDMarkerHandle::SegueEnd:
  case RDMarkerHandle::HookEnd:
    break;

  case RDMarkerHandle::FadeDown:
    d_region_edit->
      setText(RDGetTimeLength(d_pointers[RDMarkerHandle::CutEnd]-
			      d_pointers[d_selected_markers[0]],true,true));
    break;

  case RDMarkerHandle::LastRole:
    if(d_selected_markers[1]==RDMarkerHandle::FadeUp) {
      d_region_edit->
	setText(RDGetTimeLength(d_pointers[d_selected_markers[1]]-
       			d_pointers[RDMarkerHandle::CutStart],true,true));
    }
    break;

  case RDMarkerHandle::FadeUp:
    break;
  }

  //
  // Length
  //
  d_length_edit->
    setText(RDGetTimeLength(d_pointers[RDMarkerHandle::CutEnd]-
			    d_pointers[RDMarkerHandle::CutStart],true,true));
}
