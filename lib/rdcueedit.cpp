// rdcueedit.cpp
//
// Cueing Editor for RDLogLine-based Events
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcueedit.cpp,v 1.1.2.3.2.1 2014/05/20 01:45:16 cvs Exp $
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
#include <qmessagebox.h>

#include <rdconf.h>
#include <rdcueedit.h>

RDCueEdit::RDCueEdit(RDCae *cae,int card,int port,
		     QWidget *parent,const char *name)
  : QWidget(parent,name)
{
  edit_cae=cae;
  edit_play_card=card;
  edit_play_port=port;
  edit_height=325;
  edit_slider_pressed=false;
  edit_shift_pressed=false;
  edit_right_click_stop=false;
  edit_event_player=NULL;
  edit_start_rml="";
  edit_stop_rml="";

  //
  // Create Fonts
  //
  QFont radio_font=QFont("Helvetica",10,QFont::Normal);
  radio_font.setPixelSize(10);
  QFont label_font=QFont("Helvetica",12,QFont::Bold);
  label_font.setPixelSize(12);
  QFont button_font=QFont("Helvetica",12,QFont::Bold);
  button_font.setPixelSize(12);
  QFont counter_font=QFont("Helvetica",20,QFont::Bold);
  counter_font.setPixelSize(20);

  //
  // Create Palettes
  //
  edit_play_color=
    QPalette(QColor(BUTTON_PLAY_BACKGROUND_COLOR),backgroundColor());
  edit_start_color=palette();
  edit_start_color.setColor(QColorGroup::Foreground,RD_CUEEDITOR_START_MARKER);

  edit_position_label=new QLabel(this,"edit_position_label");
  edit_position_label->setGeometry(0,0,sizeHint().width()-30,30);
  edit_position_label->setBackgroundColor(QColor(white));
  edit_position_label->setLineWidth(1);
  edit_position_label->setMidLineWidth(0);
  edit_position_label->setFrameStyle(QFrame::Box|QFrame::Plain);

  edit_position_bar=new RDMarkerBar(this,"edit_position_bar");
  edit_position_bar->setGeometry(85,8,sizeHint().width()-200,14);

  edit_up_label=new QLabel("00:00:00",this,"edit_up_label");
  edit_up_label->setGeometry(5,8,70,14);
  edit_up_label->setBackgroundColor(white);
  edit_up_label->setFont(label_font);
  edit_up_label->setAlignment(AlignRight|AlignVCenter);

  edit_down_label=new QLabel("00:00:00",this,"edit_down_label");
  edit_down_label->setGeometry(sizeHint().width()-110,8,70,14);
  edit_down_label->setBackgroundColor(white);
  edit_down_label->setFont(label_font);
  edit_down_label->setAlignment(AlignRight|AlignVCenter);

  //
  // Position Slider
  //
  edit_slider=new RDSlider(RDSlider::Right,this,"edit_slider");
  edit_slider->setGeometry(60,30,sizeHint().width()-150,50);
  edit_slider->setKnobSize(50,50);
  edit_slider->setKnobColor(QColor(RD_CUEEDITOR_KNOB_COLOR));
  connect(edit_slider,SIGNAL(sliderMoved(int)),
	  this,SLOT(sliderChangedData(int)));
  connect(edit_slider,SIGNAL(sliderPressed()),this,SLOT(sliderPressedData()));
  connect(edit_slider,SIGNAL(sliderReleased()),
	  this,SLOT(sliderReleasedData()));

  //
  // Button Area
  //
  QLabel *label=new QLabel(this,"button_area");
  label->setGeometry(0,85,sizeHint().width()-30,60);
  label->setBackgroundColor(QColor(gray));
  label->setLineWidth(1);
  label->setMidLineWidth(0);
  label->setFrameStyle(QFrame::Box|QFrame::Plain);

  //
  //  Audition Button
  //
  edit_audition_button=new RDTransportButton(RDTransportButton::PlayBetween,
					    this,"edit_audition_button");
  edit_audition_button->setGeometry(sizeHint().width()/2-130,90,80,50);
  edit_audition_button->
    setPalette(QPalette(backgroundColor(),QColor(gray)));
  edit_audition_button->setFont(button_font);
  edit_audition_button->setText(tr("&Audition"));
  connect(edit_audition_button,SIGNAL(clicked()),
	  this,SLOT(auditionButtonData()));

  //
  //  Pause Button
  //
  edit_pause_button=new RDTransportButton(RDTransportButton::Pause,
					 this,"edit_pause_button");
  edit_pause_button->setGeometry(sizeHint().width()/2-40,90,80,50);
  edit_pause_button->
    setPalette(QPalette(backgroundColor(),QColor(gray)));
  edit_pause_button->setFont(button_font);
  edit_pause_button->setText(tr("&Pause"));
  connect(edit_pause_button,SIGNAL(clicked()),this,SLOT(pauseButtonData()));

  //
  //  Stop Button
  //
  edit_stop_button=new RDTransportButton(RDTransportButton::Stop,
					this,"edit_stop_button");
  edit_stop_button->setGeometry(sizeHint().width()/2+50,90,80,50);
  edit_stop_button->setOnColor(QColor(red));
  edit_stop_button->
    setPalette(QPalette(backgroundColor(),QColor(gray)));
  edit_stop_button->setFont(button_font);
  edit_stop_button->setText(tr("&Stop"));
  connect(edit_stop_button,SIGNAL(clicked()),this,SLOT(stopButtonData()));

  //
  // Start Marker Control
  //
  edit_start_button=new RDPushButton(this,"button");
  edit_start_button->setToggleButton(true);
  edit_start_button->setGeometry(0,155,66,45);
  edit_start_button->setFlashColor(backgroundColor());
  edit_start_button->setFlashPeriod(RD_CUEEDITOR_BUTTON_FLASH_PERIOD);
  edit_start_button->setPalette(QPalette(QColor(RD_CUEEDITOR_START_MARKER),
					   backgroundColor()));
  edit_start_button->setFont(button_font);
  edit_start_button->setText(tr("Start"));
  connect(edit_start_button,SIGNAL(clicked()),this,SLOT(startClickedData()));

  //
  // End Marker Control
  //
  edit_end_button=new RDPushButton(this);
  edit_end_button->setToggleButton(true);
  edit_end_button->setGeometry(90,155,66,45);
  edit_end_button->setFlashColor(backgroundColor());
  edit_end_button->setFlashPeriod(RD_CUEEDITOR_BUTTON_FLASH_PERIOD);
  edit_end_button->setPalette(QPalette(QColor(RD_CUEEDITOR_START_MARKER),
				       backgroundColor()));
  edit_end_button->setFont(button_font);
  edit_end_button->setText(tr("End"));
  connect(edit_end_button,SIGNAL(clicked()),this,SLOT(endClickedData()));

  //
  // Recue Marker Control
  //
  edit_recue_button=new RDPushButton(this);
  edit_recue_button->setToggleButton(true);
  edit_recue_button->setGeometry(180,155,66,45);
  edit_recue_button->setFlashColor(backgroundColor());
  edit_recue_button->setFlashPeriod(RD_CUEEDITOR_BUTTON_FLASH_PERIOD);
  edit_recue_button->setPalette(QPalette(QColor(RD_CUEEDITOR_START_MARKER),
				       backgroundColor()));
  edit_recue_button->setFont(button_font);
  edit_recue_button->setText(tr("&Recue"));
  connect(edit_recue_button,SIGNAL(clicked()),this,SLOT(recue()));

  //
  // Audition Stop Timer
  //
  edit_audition_timer=new QTimer(this,"edit_audition_timer");
  connect(edit_audition_timer,SIGNAL(timeout()),this,SLOT(auditionTimerData()));

  //
  // Play Deck
  //
  edit_play_deck=new RDPlayDeck(edit_cae,RDPLAYDECK_AUDITION_ID,
				this,"edit_play_deck");
  connect(edit_play_deck,SIGNAL(stateChanged(int,RDPlayDeck::State)),this,
	  SLOT(stateChangedData(int,RDPlayDeck::State)));
  connect(edit_play_deck,SIGNAL(position(int,int)),
	  this,SLOT(positionData(int,int)));
}


RDCueEdit::~RDCueEdit()
{
}


QSize RDCueEdit::sizeHint() const
{
  return QSize(610,215);
} 


QSizePolicy RDCueEdit::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void RDCueEdit::setRml(RDEventPlayer *player,const QString &start_rml,
		       const QString &stop_rml)
{
  edit_event_player=player;
  edit_start_rml=start_rml;
  edit_stop_rml=stop_rml;
}


bool RDCueEdit::initialize(RDLogLine *logline)
{
  edit_logline=logline;
  edit_position_bar->setLength(edit_logline->forcedLength());
  edit_start_button->setOn(false);
  if(!(edit_logline->status()==RDLogLine::Scheduled) && 
     !(edit_logline->status()==RDLogLine::Paused)) {
    edit_start_button->hide();
    edit_end_button->hide();
  }
  else {
    edit_start_button->show();
    edit_end_button->show();
  }
  edit_slider->setRange(0,edit_logline->forcedLength());
  edit_slider->setValue(edit_logline->playPosition());
  sliderChangedData(edit_logline->playPosition());
  startClickedData();
  edit_stop_button->on();
  edit_position_bar->
    setMarker(RDMarkerBar::Play,edit_logline->playPosition());
  edit_position_bar->
    setMarker(RDMarkerBar::Start,edit_logline->playPosition());
  edit_position_bar->
    setMarker(RDMarkerBar::End,edit_logline->endPoint());
  edit_slider->setValue(edit_logline->playPosition());
  UpdateCounters();
  return true;
}


unsigned RDCueEdit::playPosition(RDMarkerBar::Marker marker) const
{
  return edit_position_bar->marker(marker);
}


void RDCueEdit::stop()
{
  if(edit_play_deck->state()==RDPlayDeck::Playing) {
    edit_play_deck->stop();
  }
}


void RDCueEdit::recue()
{
  edit_position_bar->setMarker(RDMarkerBar::Start,0);
  if(edit_start_button->isOn()) {
    edit_slider->setValue(0);
  }
  UpdateCounters();
}


void RDCueEdit::sliderChangedData(int pos)
{
  if(edit_start_button->isOn()) {
    edit_position_bar->setMarker(RDMarkerBar::Start,pos);
  }
  else {
    if(edit_end_button->isOn()) {
      edit_position_bar->setMarker(RDMarkerBar::End,pos);
    }
    else {
      edit_position_bar->setMarker(RDMarkerBar::Play,pos);
    }
  }
  UpdateCounters();
}


void RDCueEdit::sliderPressedData()
{
  if(edit_play_deck->state()==RDPlayDeck::Playing) {
    edit_play_deck->stop();
    edit_slider_pressed=true;
  }
}


void RDCueEdit::sliderReleasedData()
{
  if(edit_slider_pressed) {
    auditionButtonData();
    edit_slider_pressed=false;
  }
}


void RDCueEdit::auditionButtonData()
{
  int start_pos=edit_slider->value();
  int play_len=-1;

  if(edit_play_deck->state()==RDPlayDeck::Playing) {
    return;
  }
  edit_play_deck->setCard(edit_play_card);
  edit_play_deck->setPort(edit_play_port);
  if(!edit_play_deck->setCart(edit_logline,false)) {
    return;
  }
  if(edit_start_button->isOn()) {
    if(edit_play_deck->state()==RDPlayDeck::Stopped) {
      start_pos=edit_position_bar->marker(RDMarkerBar::Start);
    }
    if(edit_play_deck->state()==RDPlayDeck::Paused) {
      start_pos=edit_play_deck->currentPosition();
    }
    play_len=edit_position_bar->marker(RDMarkerBar::End)-start_pos;
  }
  else {
    if(edit_end_button->isOn()) {
      if(edit_play_deck->state()==RDPlayDeck::Stopped) {
	play_len=RD_CUEEDITOR_AUDITION_PREROLL;
	if(play_len>(edit_position_bar->marker(RDMarkerBar::End)-
		     edit_position_bar->marker(RDMarkerBar::Start))) {
	  play_len=edit_position_bar->marker(RDMarkerBar::End)-
	    edit_position_bar->marker(RDMarkerBar::Start);
	}
	start_pos=edit_position_bar->marker(RDMarkerBar::End)-play_len;	  
      }
    }
    else {
      if((edit_play_deck->state()==RDPlayDeck::Stopped)&&
	 (!edit_slider_pressed)) {
	edit_start_pos=edit_slider->value();
      }
    }
  }
  edit_play_deck->play(start_pos);
  if(play_len>=0) {
    edit_audition_timer->start(play_len,true);
  }
  if((!edit_start_rml.isEmpty())&&(edit_event_player!=NULL)) {
    edit_event_player->exec(edit_logline->resolveWildcards(edit_start_rml));
  }
}


void RDCueEdit::pauseButtonData()
{
  if(edit_play_deck->state()==RDPlayDeck::Playing) {
    edit_play_deck->pause();
  }
}


void RDCueEdit::stopButtonData()
{
  switch(edit_play_deck->state()) {
      case RDPlayDeck::Playing:
      case RDPlayDeck::Paused:
	edit_play_deck->stop();
	break;

      default:
	break;
  }
}


void RDCueEdit::stateChangedData(int id,RDPlayDeck::State state)
{
  if(id!=RDPLAYDECK_AUDITION_ID) {
    return;
  }
  switch(state) {
      case RDPlayDeck::Playing:
	Playing(id);
	break;

      case RDPlayDeck::Stopping:
	break;

      case RDPlayDeck::Paused:
	Paused(id);
	break;

      case RDPlayDeck::Stopped:
	Stopped(id);
	break;

      case RDPlayDeck::Finished:
	Stopped(id);
	break;
  }
}


void RDCueEdit::positionData(int id,int msecs)
{
  if(id!=RDPLAYDECK_AUDITION_ID) {
    return;
  }
  edit_position_bar->setMarker(RDMarkerBar::Play,msecs);
  if((!edit_start_button->isOn())&&(!edit_end_button->isOn())) {
    edit_slider->setValue(msecs);
  }
  UpdateCounters();
}


void RDCueEdit::auditionTimerData()
{
  edit_play_deck->stop();
}


void RDCueEdit::startClickedData()
{
  if(edit_end_button->isOn()) {
    edit_end_button->toggle();
    SetEndMode(false);
  }
  SetStartMode(edit_start_button->isOn());
}


void RDCueEdit::endClickedData()
{
  if(edit_start_button->isOn()) {
    edit_start_button->toggle();
    SetStartMode(false);
  }
  SetEndMode(edit_end_button->isOn());
}


void RDCueEdit::SetStartMode(bool state)
{
  if(state) {
    edit_slider->setRange(0,edit_position_bar->marker(RDMarkerBar::End));
    edit_slider->setGeometry(60,30,
			     (int)(50.0+((double)(sizeHint().width()-200)*
					 (double)edit_position_bar->
					 marker(RDMarkerBar::End)/
					 (double)edit_logline->
					 forcedLength())),50);
    edit_slider->setValue(edit_position_bar->marker(RDMarkerBar::Start));
    edit_slider->setKnobColor(RD_CUEEDITOR_START_MARKER);
    edit_audition_button->setAccentColor(RD_CUEEDITOR_START_MARKER);
    edit_start_button->setFlashingEnabled(true);
    edit_up_label->setPalette(edit_start_color);
    edit_down_label->setPalette(edit_start_color);
    UpdateCounters();
  }
  else {
    edit_slider->setRange(0,edit_logline->forcedLength());
    edit_slider->setGeometry(60,30,sizeHint().width()-150,50);
    edit_slider->setValue(edit_position_bar->marker(RDMarkerBar::Play));
    edit_slider->setKnobColor(RD_CUEEDITOR_PLAY_MARKER); 
    edit_audition_button->setAccentColor(RD_CUEEDITOR_PLAY_MARKER);
    edit_start_button->setFlashingEnabled(false);
    edit_up_label->setPalette(palette());
    edit_down_label->setPalette(palette());
    UpdateCounters();
  }
}


void RDCueEdit::SetEndMode(bool state)
{
  if(state) {
    edit_slider->setRange(edit_position_bar->marker(RDMarkerBar::Start),
			  edit_logline->forcedLength());
    edit_slider->setGeometry((int)(60.0+(double)(sizeHint().width()-200)*
				   (double)edit_position_bar->
				   marker(RDMarkerBar::Start)/
				   (double)edit_logline->forcedLength()),
			     30,(int)(50.0+((double)(sizeHint().width()-200)*
					     ((double)edit_logline->
					      forcedLength()-
					      (double)edit_position_bar->
					      marker(RDMarkerBar::Start))/
					     (double)edit_logline->
					     forcedLength())),50);
    edit_slider->setValue(edit_position_bar->marker(RDMarkerBar::End));
    edit_slider->setKnobColor(RD_CUEEDITOR_START_MARKER);
    edit_audition_button->setAccentColor(RD_CUEEDITOR_START_MARKER);
    edit_end_button->setFlashingEnabled(true);
    edit_up_label->setPalette(edit_start_color);
    edit_down_label->setPalette(edit_start_color);
    UpdateCounters();
  }
  else {
    edit_slider->setRange(0,edit_logline->forcedLength());
    edit_slider->setGeometry(60,30,sizeHint().width()-150,50);
    edit_slider->setValue(edit_position_bar->marker(RDMarkerBar::Play));
    edit_slider->setKnobColor(RD_CUEEDITOR_PLAY_MARKER); 
    edit_audition_button->setAccentColor(RD_CUEEDITOR_PLAY_MARKER);
    edit_end_button->setFlashingEnabled(false);
    edit_up_label->setPalette(palette());
    edit_down_label->setPalette(palette());
    UpdateCounters();
  }
}

/*
void RDCueEdit::ShowAudioControls(bool state)
{
  if(state) {
    edit_height=325;
    edit_slider->show();
    edit_up_label->show();
    edit_down_label->show();
    edit_audition_button->show();
    edit_pause_button->show();
    edit_position_bar->show();
    edit_position_label->show();
  }
  else {
    edit_height=170;
    edit_slider->hide();
    edit_up_label->hide();
    edit_down_label->hide();
    edit_audition_button->hide();
    edit_pause_button->hide();
    edit_position_bar->hide();
    edit_position_label->hide();
  }
}
*/

void RDCueEdit::Playing(int id)
{
  edit_audition_button->on();
  edit_pause_button->off();
  edit_stop_button->off();
  edit_right_click_stop=true;
}


void RDCueEdit::Paused(int id)
{
  if(!edit_slider_pressed) {
    edit_audition_button->off();
    edit_pause_button->on();
    edit_stop_button->off();
    ClearChannel();
    edit_right_click_stop=false;
  }
}


void RDCueEdit::Stopped(int id)
{
  if(!edit_slider_pressed) {
    edit_audition_button->off();
    edit_pause_button->off();
    edit_stop_button->on();
    ClearChannel();
    edit_right_click_stop=false;
  }
  if(edit_start_button->isOn()) {
    edit_position_bar->
      setMarker(RDMarkerBar::Play,edit_position_bar->marker(RDMarkerBar::Start));
    edit_slider->setValue(edit_position_bar->marker(RDMarkerBar::Start));
  }
  else {
    if(edit_end_button->isOn()) {
      edit_slider->setValue(edit_position_bar->marker(RDMarkerBar::End));
    }
    else {
      edit_position_bar->setMarker(RDMarkerBar::Play,edit_start_pos);
      edit_slider->setValue(edit_start_pos);
    }
  }
}


void RDCueEdit::UpdateCounters()
{
  if(edit_start_button->isOn()) {
   edit_up_label->
     setText(RDGetTimeLength(edit_position_bar->marker(RDMarkerBar::Start),true));
   edit_down_label->
     setText(RDGetTimeLength(edit_logline->
			    forcedLength()-edit_position_bar->
			    marker(RDMarkerBar::Start),true));
  }
  else {
    if(edit_end_button->isOn()) {
      edit_up_label->
	setText(RDGetTimeLength(edit_position_bar->marker(RDMarkerBar::End),
				true));
      edit_down_label->
	setText(RDGetTimeLength(edit_logline->
				forcedLength()-edit_position_bar->
				marker(RDMarkerBar::End),true));
    }
    else {
      edit_up_label->
	setText(RDGetTimeLength(edit_position_bar->marker(RDMarkerBar::Play),
				true));
      edit_down_label->
	setText(RDGetTimeLength(edit_logline->
				forcedLength()-edit_position_bar->
				marker(RDMarkerBar::Play),true));
    }
  }
}


void RDCueEdit::ClearChannel()
{
  if(edit_cae->playPortActive(edit_play_deck->card(),edit_play_deck->port(),
			      edit_play_deck->stream())) {
    return;
  }
  if((!edit_stop_rml.isEmpty())&&(edit_event_player!=NULL)) {
    edit_event_player->exec(edit_stop_rml);
  }
}


void RDCueEdit::wheelEvent(QWheelEvent *e)
{
  if(edit_audition_button->isShown()) {
    if(edit_play_deck->state()==RDPlayDeck::Playing) {
      edit_play_deck->pause();
    }
    if(edit_shift_pressed) {
      edit_slider->setValue(edit_slider->value()+(e->delta()*10)/12);
      }
    else {
      edit_slider->setValue(edit_slider->value()+(e->delta()*100)/12);
      }
    sliderChangedData(edit_slider->value());
  }
}


void RDCueEdit::mousePressEvent(QMouseEvent *e)
{
  switch(e->button()) {
      case QMouseEvent::RightButton:
        if(edit_audition_button->isShown()) {
          if(edit_right_click_stop) {
            stopButtonData();
            }
          else {
 	    auditionButtonData();
            }
          }
        break;

      case QMouseEvent::MidButton:
        if(edit_audition_button->isShown()) {
          if(edit_logline->forcedLength()>10000) {
            if(edit_play_deck->state()==RDPlayDeck::Playing) {
              edit_play_deck->pause();
              }
            edit_slider->setValue((edit_logline->forcedLength())-10000);
            sliderChangedData(edit_slider->value());
            }
          auditionButtonData();
          }
        break;

      default:
	QWidget::mousePressEvent(e);
	break;
  }
}


void RDCueEdit::keyPressEvent(QKeyEvent *e)
{
  switch(e->key()) {
      case Qt::Key_Shift:
        edit_shift_pressed=true;
  	break;

      default:
	e->ignore();
	break;
  }
}


void RDCueEdit::keyReleaseEvent(QKeyEvent *e)
{
  switch(e->key()) {
      case Qt::Key_Shift:
        edit_shift_pressed=false;
	QWidget::keyPressEvent(e);
  	break;

      default:
	QWidget::keyPressEvent(e);
	break;
  }
}
