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
#include "rdescape_string.h"
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
  // Marker Readouts
  //
  for(int i=0;i<7;i++) {
    d_readout_labels[i]=new QLabel(this);
    d_readout_labels[i]->setFont(labelFont());
    d_readout_labels[i]->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  }
  d_readout_labels[1]->setText(tr("Start")+":");
  d_readout_labels[2]->setText(tr("End")+":");
  d_readout_labels[3]->setText(tr("Length")+":");

  d_readout_labels[5]->setText(tr("Position")+":");
  d_readout_labels[6]->setText(tr("Length")+":");

  d_readout_mapper=new QSignalMapper(this);
  connect(d_readout_mapper,SIGNAL(mapped(int)),
	  this,SLOT(readoutClickedData(int)));

  d_cut_readout=new RDMarkerReadout(RDMarkerHandle::CutStart,this);
  connect(d_cut_readout,SIGNAL(clicked()),d_readout_mapper,SLOT(map()));
  d_readout_mapper->setMapping(d_cut_readout,(int)RDMarkerHandle::CutStart);
  d_cut_readout->setEnabled(true);

  d_talk_readout=new RDMarkerReadout(RDMarkerHandle::TalkStart,this);
  connect(d_talk_readout,SIGNAL(clicked()),d_readout_mapper,SLOT(map()));
  d_readout_mapper->setMapping(d_talk_readout,(int)RDMarkerHandle::TalkStart);

  d_segue_readout=new RDMarkerReadout(RDMarkerHandle::SegueStart,this);
  connect(d_segue_readout,SIGNAL(clicked()),d_readout_mapper,SLOT(map()));
  d_readout_mapper->setMapping(d_segue_readout,(int)RDMarkerHandle::SegueStart);

  d_hook_readout=new RDMarkerReadout(RDMarkerHandle::HookStart,this);
  connect(d_hook_readout,SIGNAL(clicked()),d_readout_mapper,SLOT(map()));
  d_readout_mapper->setMapping(d_hook_readout,(int)RDMarkerHandle::HookStart);

  d_fadeup_readout=new RDMarkerReadout(RDMarkerHandle::FadeUp,this);
  connect(d_fadeup_readout,SIGNAL(clicked()),d_readout_mapper,SLOT(map()));
  d_readout_mapper->setMapping(d_fadeup_readout,(int)RDMarkerHandle::FadeUp);

  d_fadedown_readout=new RDMarkerReadout(RDMarkerHandle::FadeDown,this);
  connect(d_fadedown_readout,SIGNAL(clicked()),d_readout_mapper,SLOT(map()));
  d_readout_mapper->setMapping(d_fadedown_readout,(int)RDMarkerHandle::FadeDown);

  //
  // Time Counters
  //
  d_position_label=new QLabel(tr("Cursor Position"),this);
  d_position_label->setFont(labelFont());
  d_position_label->setAlignment(Qt::AlignCenter);
  d_position_label->
    setPalette(QPalette(palette().color(QPalette::Background),
  			QColor(RDMARKERPLAYER_HIGHLIGHT_COLOR)));
  d_position_edit=new QLabel(this);
  d_position_edit->setAcceptDrops(false);
  d_position_edit->setAlignment(Qt::AlignCenter);
  d_position_edit->
    setStyleSheet("background-color: "+palette().color(QPalette::Base).name());

  //
  // Goto Buttons
  //
  d_goto_start_button=new QPushButton(tr("Go To\nStart"),this);
  d_goto_start_button->setFont(buttonFont());
  connect(d_goto_start_button,SIGNAL(clicked()),
	  this,SLOT(buttonGotoStartData()));

  d_goto_cursor_button=new QPushButton(tr("Go To\nCursor"),this);
  d_goto_cursor_button->setFont(buttonFont());
  connect(d_goto_cursor_button,SIGNAL(clicked()),
	  this,SLOT(buttonGotoCursorData()));

  d_goto_end_button=new QPushButton(tr("Go To\nEnd"),this);
  d_goto_end_button->setFont(buttonFont());
  connect(d_goto_end_button,SIGNAL(clicked()),this,SLOT(buttonGotoEndData()));

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

  //
  // Fade on Segue Out
  //
  d_no_segue_fade_check=new QCheckBox(this);
  d_no_segue_fade_label=new QLabel(tr("No Fade on Segue Out"),this);
  d_no_segue_fade_label->setFont(labelFont());
  connect(d_no_segue_fade_check,SIGNAL(toggled(bool)),
	  this,SLOT(noFadeOnSegueData(bool)));

  //
  // Cut Gain
  //
  d_play_gain_spin=new QSpinBox(this);
  d_play_gain_spin->setRange(-10,10);
  connect(d_play_gain_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(playGainData(int)));
  d_play_gain_label=new QLabel(tr("Cut Gain")+":",this);
  d_play_gain_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  d_play_gain_label->setFont(labelFont());
  d_play_gain_unit_label=new QLabel(tr("dB"),this);
  d_play_gain_unit_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  d_play_gain_unit_label->setFont(labelFont());

  //
  // Audio Trim
  //
  d_trim_start_button=new QPushButton(tr("Trim Start"),this);
  d_trim_start_button->setFont(buttonFont());
  connect(d_trim_start_button,SIGNAL(clicked()),
	  this,SLOT(buttonTrimStartData()));

  d_trim_end_button=new QPushButton(tr("Trim End"),this);
  d_trim_end_button->setFont(buttonFont());
  connect(d_trim_end_button,SIGNAL(clicked()),
	  this,SLOT(buttonTrimEndData()));

  d_trim_label=new QLabel(tr("Threshold"),this);
  d_trim_label->setFont(labelFont());
  d_trim_label->setAlignment(Qt::AlignCenter);
  d_trim_spin=new QSpinBox(this);
  d_trim_spin->setRange(-99,0);
  d_trim_spin->setSuffix(" dBFS");
  d_trim_spin->setValue(rda->libraryConf()->trimThreshold()/100);
  connect(d_trim_spin,SIGNAL(valueChanged(int)),
	  this,SLOT(trimThresholdChanged(int)));
  trimThresholdChanged(d_trim_spin->value());
}


RDMarkerPlayer::~RDMarkerPlayer()
{
}


QSize RDMarkerPlayer::sizeHint() const
{
  return QSize(1046,150);
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
  rda->cae()->positionPlay(d_cae_handle,0);
  RDSetMixerOutputPort(rda->cae(),d_cards.first(),d_cae_stream,d_port);

  QString sql=QString("select ")+
    "START_POINT,"+        // 00  
    "END_POINT,"+          // 01
    "TALK_START_POINT,"+   // 02
    "TALK_END_POINT,"+     // 03
    "SEGUE_START_POINT,"+  // 04
    "SEGUE_END_POINT,"+    // 05
    "HOOK_START_POINT,"+   // 06
    "HOOK_END_POINT,"+     // 07
    "FADEUP_POINT,"+       // 08
    "FADEDOWN_POINT,"+     // 09
    "SEGUE_GAIN,"+         // 10
    "PLAY_GAIN "+          // 11
    "from CUTS where "+
    "CUT_NAME=\""+RDEscapeString(RDCut::cutName(cartnum,cutnum))+"\"";
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    for(int i=0;i<RDMarkerHandle::LastRole;i++) {
      RDMarkerHandle::PointerRole role=(RDMarkerHandle::PointerRole)i;
      d_cut_readout->setValue(role,q->value(i).toInt());
      d_talk_readout->setValue(role,q->value(i).toInt());
      d_segue_readout->setValue(role,q->value(i).toInt());
      d_hook_readout->setValue(role,q->value(i).toInt());
      d_fadeup_readout->setValue(role,q->value(i).toInt());
      d_fadedown_readout->setValue(role,q->value(i).toInt());
      setPointerValue(role,q->value(i).toInt());
      setSelectedMarkers(RDMarkerHandle::LastRole,RDMarkerHandle::LastRole);
    }
    d_no_segue_fade_check->setChecked(q->value(10).toInt()==0);
    d_play_gain_spin->setValue(q->value(11).toInt()/100);
  }
  delete q;

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
    d_cut_readout->setValue(role,ptr);
    d_talk_readout->setValue(role,ptr);
    d_segue_readout->setValue(role,ptr);
    d_hook_readout->setValue(role,ptr);
    d_fadeup_readout->setValue(role,ptr);
    d_fadedown_readout->setValue(role,ptr);
  }
}


void RDMarkerPlayer::setSelectedMarkers(RDMarkerHandle::PointerRole start_role,
					RDMarkerHandle::PointerRole end_role)
{
  QColor color=RDMarkerHandle::pointerRoleColor(start_role);
  if(start_role==RDMarkerHandle::LastRole) {
    color=RDMarkerHandle::pointerRoleColor(end_role);
  }
  d_play_from_button->setAccentColor(color);
  d_play_to_button->setAccentColor(color);
  d_selected_markers[0]=start_role;
  d_selected_markers[1]=end_role;

  d_play_from_button->setDisabled(start_role==RDMarkerHandle::LastRole);
  d_play_to_button->setDisabled(end_role==RDMarkerHandle::LastRole);

  d_cut_readout->
    setSelectedMarkers(d_selected_markers[0],d_selected_markers[1]);
  d_talk_readout->
    setSelectedMarkers(d_selected_markers[0],d_selected_markers[1]);
  d_segue_readout->
    setSelectedMarkers(d_selected_markers[0],d_selected_markers[1]);
  d_hook_readout->
    setSelectedMarkers(d_selected_markers[0],d_selected_markers[1]);
  d_fadeup_readout->
    setSelectedMarkers(d_selected_markers[0],d_selected_markers[1]);
  d_fadedown_readout->
    setSelectedMarkers(d_selected_markers[0],d_selected_markers[1]);
}


void RDMarkerPlayer::buttonGotoStartData()
{
  emit gotoStartClicked();
}


void RDMarkerPlayer::buttonGotoCursorData()
{
  emit gotoCursorClicked();
}


void RDMarkerPlayer::buttonGotoEndData()
{
  emit gotoEndClicked();
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


void RDMarkerPlayer::playGainData(int db)
{
  emit playGainSet(db);
}


void RDMarkerPlayer::noFadeOnSegueData(bool state)
{
  emit noFadeOnSegueChanged(state);
}


void RDMarkerPlayer::buttonTrimStartData()
{
  emit startTrimClicked(d_trim_spin->value());
}


void RDMarkerPlayer::buttonTrimEndData()
{
  emit endTrimClicked(d_trim_spin->value());
}


void RDMarkerPlayer::readoutClickedData(int n)
{
  RDMarkerHandle::PointerRole role=(RDMarkerHandle::PointerRole)n;
  switch(role) {
  case RDMarkerHandle::CutStart:
  case RDMarkerHandle::TalkStart:
  case RDMarkerHandle::SegueStart:
  case RDMarkerHandle::HookStart:
    setSelectedMarkers(role,(RDMarkerHandle::PointerRole)((int)role+1));
    emit selectedMarkersChanged(role,
				(RDMarkerHandle::PointerRole)((int)role+1));
    break;

  case RDMarkerHandle::CutEnd:
  case RDMarkerHandle::TalkEnd:
  case RDMarkerHandle::SegueEnd:
  case RDMarkerHandle::HookEnd:
    setSelectedMarkers((RDMarkerHandle::PointerRole)((int)role-1),role);
    emit selectedMarkersChanged((RDMarkerHandle::PointerRole)((int)role-1),
				role);
    break;

  case RDMarkerHandle::FadeUp:
    setSelectedMarkers(RDMarkerHandle::LastRole,role);
    emit selectedMarkersChanged(RDMarkerHandle::LastRole,role);
    break;

  case RDMarkerHandle::FadeDown:
    setSelectedMarkers(role,RDMarkerHandle::LastRole);
    emit selectedMarkersChanged(role,RDMarkerHandle::LastRole);
    break;

  case RDMarkerHandle::LastRole:
    break;
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


void RDMarkerPlayer::trimThresholdChanged(int dbfs)
{
  d_trim_start_button->setDisabled(dbfs==0);
  d_trim_end_button->setDisabled(dbfs==0);
}


void RDMarkerPlayer::resizeEvent(QResizeEvent *)
{
  for(int i=0;i<7;i++) {
    d_readout_labels[i]->setGeometry(2,
				     i*(d_cut_readout->sizeHint().height()/4-1),
				     15+50,
				     d_cut_readout->sizeHint().height()/4+1);
  }

  d_cut_readout->setGeometry(70,
			     2+8,
			     d_cut_readout->sizeHint().width(),
			     d_cut_readout->sizeHint().height());

  d_talk_readout->setGeometry(70+1*(d_talk_readout->sizeHint().width()),
			      2+8,
			      d_talk_readout->sizeHint().width(),
			      d_talk_readout->sizeHint().height());

  d_segue_readout->setGeometry(70+2*(d_segue_readout->sizeHint().width()),
			       2+8,
			       d_segue_readout->sizeHint().width(),
			       d_segue_readout->sizeHint().height());

  d_hook_readout->setGeometry(70+3*(d_hook_readout->sizeHint().width()),
			      2+8,
			      d_hook_readout->sizeHint().width(),
			      d_hook_readout->sizeHint().height());

  d_fadeup_readout->setGeometry(70,
				10+d_hook_readout->sizeHint().height(),
				2*d_fadeup_readout->sizeHint().width(),
				d_fadeup_readout->sizeHint().height());

  d_fadedown_readout->setGeometry(70+2*d_fadedown_readout->sizeHint().width()-1,
				  10+d_hook_readout->sizeHint().height(),
				  2*d_fadedown_readout->sizeHint().width(),
				  d_fadedown_readout->sizeHint().height());

  d_position_label->setGeometry(340,3,345,16);
  d_position_edit->setGeometry(475,20,75,18);

  d_goto_start_button->setGeometry(390,45,65,45);
  d_goto_cursor_button->setGeometry(480,45,65,45);
  d_goto_end_button->setGeometry(570,45,65,45);

  d_play_button->setGeometry(340,97,65,45);
  d_play_from_button->setGeometry(410,97,65,45);
  d_play_to_button->setGeometry(480,97,65,45);
  d_stop_button->setGeometry(550,97,65,45);
  d_loop_button->setGeometry(620,97,65,45);

  d_meter->setGeometry(695,15,d_meter->sizeHint().width(),
		       d_meter->sizeHint().height());

  d_no_segue_fade_check->setGeometry(695,
				     25+d_meter->sizeHint().height(),
				     15,
				     15);
  d_no_segue_fade_label->setGeometry(715,
				     23+d_meter->sizeHint().height(),
				     200,
				     20);

  d_play_gain_label->setGeometry(655,
				 47+d_meter->sizeHint().height(),
				 100,
				 20);
  d_play_gain_spin->setGeometry(760,
				47+d_meter->sizeHint().height(),
				40,
				20);
  d_play_gain_unit_label->setGeometry(805,
				      47+d_meter->sizeHint().height(),
				      60,
				      20);

  d_trim_start_button->setGeometry(870,
				   25+d_meter->sizeHint().height(),
				   80,25);
  d_trim_end_button->setGeometry(870,
				 55+d_meter->sizeHint().height(),
				 80,25);
  d_trim_label->setGeometry(960,
			    30+d_meter->sizeHint().height(),
			    80,
			    20);
  d_trim_spin->setGeometry(960,
			   47+d_meter->sizeHint().height(),
			   80,
			   20);
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
