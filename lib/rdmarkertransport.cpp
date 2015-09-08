//   rdmarkertransport.cpp
//
//   Audio transport controls for the RDEditAudio dialog.
//
//   (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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

#include "rdconf.h"

#include "rdmarkertransport.h"
#include "rdmixer.h"

RDMarkerTransport::RDMarkerTransport(RDCut *cut,RDCae *cae,int card,int port,
				     int preroll,QWidget *parent)
  : QWidget(parent)
{
  trans_start_position=cut->startPoint();
  trans_end_position=cut->endPoint();
  trans_gain=0;
  trans_cut=cut;
  trans_cae=cae;
  trans_card=card;
  trans_port=port;
  trans_preroll=preroll;
  trans_handle=-1;
  trans_is_playing=false;
  trans_pause_mode=false;
  trans_ignore_pause=false;
  trans_is_paused=false;
  trans_is_stopped=false;
  trans_use_looping=false;
  trans_is_looping=false;

  //
  // CAE Control
  //
  connect(trans_cae,SIGNAL(playing(int)),this,SLOT(playedData(int)));
  connect(trans_cae,SIGNAL(playStopped(int)),this,SLOT(stoppedData(int)));
  connect(trans_cae,SIGNAL(playPositionChanged(int,unsigned)),
	  this,SLOT(positionData(int,unsigned)));
  trans_cae->
    loadPlay(trans_card,trans_cut->cutName(),&trans_stream,&trans_handle);
  RDSetMixerOutputPort(trans_cae,trans_card,trans_stream,trans_port);

  //
  // Generate Fonts
  //
  QFont label_font=QFont("Hevetica",12,QFont::Normal);
  label_font.setPixelSize(12);
  QFont small_font("Helvetica",10,QFont::Bold);
  small_font.setPixelSize(10);

  //
  // Time Counters
  //
  trans_position_label=new QLabel(tr("Position"),this);
  trans_position_label->setFont(QFont(small_font));
  trans_position_label->setAlignment(Qt::AlignHCenter);
  trans_position_label->
    setPalette(QPalette(backgroundColor(),QColor(RDMARKERTRANSPORT_HIGHLIGHT_COLOR)));
  trans_overall_edit=new QLineEdit(this);
  trans_overall_edit->setAcceptDrops(false);
  trans_overall_edit->setFont(label_font);
  trans_overall_edit->setReadOnly(true);

  trans_region_trans_label=
    new QLabel(RDMarkerWaveform::markerName(RDMarkerWaveform::Play),this);
  trans_region_trans_label->setFont(QFont(small_font));
  trans_region_trans_label->setAlignment(Qt::AlignHCenter);
  trans_region_trans_label->
    setPalette(QPalette(backgroundColor(),QColor(RDMARKERTRANSPORT_HIGHLIGHT_COLOR)));
  trans_region_edit=new QLineEdit(this);
  trans_region_edit->setAcceptDrops(false);
  trans_region_edit->setFont(label_font);
  trans_region_edit->setReadOnly(true);
  trans_region_edit->setText("0:00:00.0");
  trans_region_edit->setDisabled(true);

  trans_size_label=new QLabel(tr("Length"),this);
  trans_size_label->setFont(QFont(small_font));
  trans_size_label->setAlignment(Qt::AlignHCenter);
  trans_size_label->
    setPalette(QPalette(backgroundColor(),QColor(RDMARKERTRANSPORT_HIGHLIGHT_COLOR)));
  trans_size_edit=new QLineEdit(this);
  trans_size_edit->setAcceptDrops(false);
  trans_size_edit->setFont(label_font);
  trans_size_edit->setReadOnly(true);

  //
  // Transport Buttons
  //
  trans_play_cursor_button=
    new RDTransportButton(RDTransportButton::PlayBetween,this);
  trans_play_cursor_button->setEnabled((trans_card>=0)&&(trans_port>=0));
  trans_play_cursor_button->setAutoDefault(false);
  connect(trans_play_cursor_button,SIGNAL(clicked()),
	  this,SLOT(playCursorData()));

  trans_play_start_button=new RDTransportButton(RDTransportButton::Play,this);
  trans_play_start_button->setEnabled((trans_card>=0)&&(trans_port>=0));
  trans_play_start_button->setAutoDefault(false);
  connect(trans_play_start_button,SIGNAL(clicked()),
	  this,SLOT(playStartData()));

  trans_pause_button=new RDTransportButton(RDTransportButton::Pause,this);
  trans_pause_button->setOnColor(QColor(red));
  trans_pause_button->setEnabled((trans_card>=0)&&(trans_port>=0)); 
  trans_pause_button->setAutoDefault(false);
  connect(trans_pause_button,SIGNAL(clicked()),this,SLOT(pauseData()));

  trans_stop_button=new RDTransportButton(RDTransportButton::Stop,this);
  trans_stop_button->on();
  trans_stop_button->setOnColor(QColor(red));
  trans_stop_button->setEnabled((trans_card>=0)&&(trans_port>=0));
  trans_stop_button->setAutoDefault(false);
  connect(trans_stop_button,SIGNAL(clicked()),this,SLOT(stopData()));

  trans_loop_button=new RDTransportButton(RDTransportButton::Loop,this);
  trans_loop_button->off();
  trans_loop_button->setEnabled((trans_card>=0)&&(trans_port>=0));
  trans_loop_button->setAutoDefault(false);
  connect(trans_loop_button,SIGNAL(clicked()),this,SLOT(loopData()));

  //
  // The Audio Meter
  //
  trans_meter=new RDStereoMeter(this);
  trans_meter->setSegmentSize(5);
  trans_meter->setMode(RDSegMeter::Peak);
  trans_meter_timer=new QTimer(this);
  connect(trans_meter_timer,SIGNAL(timeout()),this,SLOT(meterData()));
}


RDMarkerTransport::~RDMarkerTransport()
{
  trans_cae->stopPlay(trans_handle);
  trans_cae->unloadPlay(trans_handle);
}


QSize RDMarkerTransport::sizeHint() const
{
  return QSize(400,300);
}


QSizePolicy RDMarkerTransport::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void RDMarkerTransport::setStartPosition(int msecs)
{
  trans_start_position=msecs;
}


void RDMarkerTransport::setEndPosition(int msecs)
{
  trans_end_position=msecs;
}


void RDMarkerTransport::setGain(int gain)
{
  trans_gain=gain;
}


void RDMarkerTransport::setActiveMarker(RDMarkerWaveform::CuePoints pt,
					int start_msecs,int end_msecs)
{
  trans_active_marker=pt;
  trans_marker_start_position=start_msecs;
  trans_marker_end_position=end_msecs;
  trans_region_edit->setText(RDGetTimeLength(end_msecs-start_msecs,true,true));
  trans_region_edit->setDisabled(pt==RDMarkerWaveform::Play);
  trans_region_trans_label->setText(RDMarkerWaveform::markerName(pt));
  trans_play_cursor_button->setAccentColor(RDMarkerWaveform::markerColor(pt));
  if(pt==RDMarkerWaveform::Play) {
    trans_region_trans_label->
      setBackgroundColor(RDMARKERTRANSPORT_HIGHLIGHT_COLOR);
  }
  else {
    trans_region_trans_label->
      setBackgroundColor(RDMarkerWaveform::markerColor(pt));
  }
}


void RDMarkerTransport::setLength(int msecs)
{
  trans_size_edit->setText(RDGetTimeLength(msecs,true,true));
}


void RDMarkerTransport::setPosition(int msecs)
{
  trans_cae->positionPlay(trans_handle,msecs);
  if(!trans_is_playing) {
    positionData(trans_handle,msecs);
  }
}


void RDMarkerTransport::resizeEvent(QResizeEvent *e)
{
  //
  // Time Counters
  //
  trans_position_label->setGeometry(49,1,70,20);
  trans_overall_edit->setGeometry(49,15,70,21);
  trans_region_trans_label->setGeometry(147,1,70,20);
  trans_region_edit->setGeometry(147,15,70,21);
  trans_size_label->setGeometry(245,1,70,20);
  trans_size_edit->setGeometry(245,15,70,21);

  //
  // Transport Buttons
  //
  trans_play_cursor_button->setGeometry(9,40,65,45);
  trans_play_start_button->setGeometry(79,40,65,45);
  trans_pause_button->setGeometry(149,40,65,45);
  trans_stop_button->setGeometry(219,40,65,45);
  trans_loop_button->setGeometry(289,40,65,45);

  //
  // Audio Meter
  //
  trans_meter->setGeometry(360,13,trans_meter->geometry().width(),
			   trans_meter->geometry().height());
}


void RDMarkerTransport::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);

  p->setPen(QColor(colorGroup().shadow()));
  p->fillRect(0,0,size().width(),size().height(),
	      QColor(RDMARKERTRANSPORT_HIGHLIGHT_COLOR));
  p->drawRect(0,0,size().width(),size().height());
  delete p;
}


void RDMarkerTransport::playStartData()
{
  if(trans_is_playing) {
    return;
  }
  trans_looping_length=trans_end_position-trans_start_position;
  trans_cae->positionPlay(trans_handle,trans_start_position);
  trans_cae->setPlayPortActive(trans_card,trans_port,trans_stream);
  trans_cae->setOutputVolume(trans_card,trans_stream,trans_port,trans_gain);
  trans_cae->play(trans_handle,trans_looping_length,RD_TIMESCALE_DIVISOR,0);
  if(trans_use_looping) {
    trans_is_looping=true;
  }
  trans_play_mode=RDMarkerTransport::FromStart;
  trans_play_start_button->on();
  trans_play_cursor_button->off();
  trans_pause_button->off();
}


void RDMarkerTransport::playCursorData()
{
  if(trans_is_playing) {
    return;
  }
  switch(trans_active_marker) {
  case RDMarkerWaveform::Play:
    trans_looping_length=trans_end_position-trans_position;
    trans_played_from_position=trans_position;
    break;

  case RDMarkerWaveform::Start:
  case RDMarkerWaveform::TalkStart:
  case RDMarkerWaveform::SegueStart:
  case RDMarkerWaveform::HookStart:
  case RDMarkerWaveform::FadeDown:
    trans_looping_length=trans_marker_end_position-trans_marker_start_position;
    trans_played_from_position=trans_marker_start_position;
    setPosition(trans_marker_start_position);
    break;

  case RDMarkerWaveform::End:
  case RDMarkerWaveform::TalkEnd:
  case RDMarkerWaveform::SegueEnd:
  case RDMarkerWaveform::HookEnd:
  case RDMarkerWaveform::FadeUp:
    trans_looping_length=trans_preroll;
    trans_played_from_position=trans_marker_end_position-trans_preroll;
    setPosition(trans_played_from_position);
    break;

  case RDMarkerWaveform::LastMarker:
    return;
  }

  trans_pause_mode=false;
  trans_cae->setPlayPortActive(trans_card,trans_port,trans_stream);
  trans_cae->setOutputVolume(trans_card,trans_stream,trans_port,trans_gain);
  trans_cae->play(trans_handle,trans_looping_length,RD_TIMESCALE_DIVISOR,0);
  if(trans_use_looping) {
    trans_is_looping=true;
  }
  trans_play_mode=RDMarkerTransport::FromCursor;
  trans_play_start_button->off();
  trans_play_cursor_button->on();
  trans_pause_button->off();
}


void RDMarkerTransport::pauseData()
{
  if(!trans_is_paused) {
    trans_is_playing=false;
    trans_is_looping=false;
    trans_is_paused=true;
    trans_pause_mode=true;
    trans_cae->stopPlay(trans_handle);
  }
}


void RDMarkerTransport::stopData()
{
  if(!trans_is_paused) {
    trans_is_playing=false;
    trans_is_looping=false;
    trans_pause_mode=false;
    trans_cae->stopPlay(trans_handle);
  }
}


void RDMarkerTransport::loopData()
{
  if(trans_use_looping) {
    trans_use_looping=false;
    trans_is_looping=false;
    trans_loop_button->off();
  }
  else {
    trans_use_looping=true;
    if(trans_is_playing) {
      trans_is_looping=true;
    }
    trans_loop_button->flash();
  }
}


void RDMarkerTransport::meterData()
{
  short levels[2];

  trans_cae->outputMeterUpdate(trans_card,trans_port,levels);
  trans_meter->setLeftPeakBar(levels[0]);
  trans_meter->setRightPeakBar(levels[1]);
}


void RDMarkerTransport::playedData(int handle)
{
  trans_is_playing=true;
  trans_is_paused=false;
  trans_is_stopped=false;
  trans_stop_button->off();
  trans_meter_timer->start(RD_METER_UPDATE_INTERVAL);
}


void RDMarkerTransport::stoppedData(int handle)
{
  if(trans_ignore_pause) {
    return;
  }

  if(trans_is_looping) {
    LoopRegion(trans_played_from_position,trans_end_position);
  }
  else {
    trans_play_start_button->off();
    trans_play_cursor_button->off();
    trans_pause_button->off();
    if(trans_is_paused) {
      trans_pause_button->on();
    }
    else {
      trans_stop_button->on();
      switch(trans_play_mode) {
      case RDMarkerTransport::FromStart:
	setPosition(trans_start_position);
	break;

      case RDMarkerTransport::FromCursor:
	setPosition(trans_played_from_position);
	break;
      }
    }
    trans_is_playing=false;
  }
}


void RDMarkerTransport::positionData(int handle,unsigned msecs)
{
  if(msecs!=trans_position) {
    trans_position=msecs;
    trans_overall_edit->setText(RDGetTimeLength(msecs,true,true));
    emit positionChanged(msecs);
  }
}


void RDMarkerTransport::LoopRegion(int start_msec,int end_msec)
{
  trans_cae->positionPlay(trans_handle,start_msec);
  trans_cae->setOutputVolume(trans_card,trans_stream,trans_port,trans_gain);
  trans_cae->play(trans_handle,trans_looping_length,RD_TIMESCALE_DIVISOR,0);
}
