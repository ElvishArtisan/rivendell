// loglinebox.cpp
//
// On Air Playout Utility for Rivendell.
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <QDrag>
#include <QGuiApplication>
#include <QMouseEvent>
#include <QPainter>

#include <rdcartdrag.h>
#include <rdconf.h>

#include "colors.h"
#include "loglinebox.h"

#include "../icons/play.xpm"
#include "../icons/rml5.xpm"
#include "../icons/chain.xpm"
#include "../icons/track_cart.xpm"
#include "../icons/mic16.xpm"
#include "../icons/notemarker.xpm"
#include "../icons/traffic.xpm"
#include "../icons/music.xpm"

//
// For debugging layout issues
//
// #define COLORIZE_LAYOUT

LogLineBox::LogLineBox(RDAirPlayConf *conf,QWidget *parent)
  : RDWidget(parent)
{
  line_status=RDLogLine::Scheduled;
  line_type=RDLogLine::UnknownType;
  line_mode=LogLineBox::Full;
  line_logline=NULL;
  log_id=-1;
  log_line=-1;
  line_move_count=-1;
  line_allow_drags=false;

  //
  // Templates
  //
  line_title_template=conf->titleTemplate();
  line_artist_template=conf->artistTemplate();
  line_outcue_template=conf->outcueTemplate();
  line_description_template=conf->descriptionTemplate();

  //
  // Assign Fonts
  //
  line_bold_font=labelFont();
  line_font=font();
  talk_font=labelFont();
  QFont outcue_font=font();
  outcue_font.setItalic(true);

  //
  // Create Icons
  //
  line_playout_map=new QPixmap(play_xpm);
  line_macro_map=new QPixmap(rml5_xpm);
  line_chain_map=new QPixmap(chain_xpm);
  line_track_cart_map=new QPixmap(track_cart_xpm);
  line_mic16_map=new QPixmap(mic16_xpm);
  line_notemarker_map=new QPixmap(notemarker_xpm);
  line_traffic_map=new QPixmap(traffic_xpm);
  line_music_map=new QPixmap(music_xpm);

  //
  // Create Palettes
  //
  line_default_palette=QGuiApplication::palette();

  line_chain_palette=QGuiApplication::palette();
  line_chain_palette.setColor(QPalette::Window,LOGLINEBOX_CHAIN_COLOR);
  line_chain_palette.setColor(QPalette::WindowText,Qt::black);

  line_error_palette=QGuiApplication::palette();
  line_error_palette.setColor(QPalette::Window,LOGLINEBOX_MISSING_COLOR);
  line_error_palette.setColor(QPalette::WindowText,Qt::white);

  line_evergreen_palette=QGuiApplication::palette();
  line_evergreen_palette.setColor(QPalette::Window,LOGLINEBOX_EVERGREEN_COLOR);
  line_evergreen_palette.setColor(QPalette::WindowText,Qt::white);

  line_marker_palette=QGuiApplication::palette();
  line_marker_palette.setColor(QPalette::Window,LOGLINEBOX_MARKER_COLOR);
  line_marker_palette.setColor(QPalette::WindowText,Qt::black);

  line_chain_palette=QGuiApplication::palette();
  line_chain_palette.setColor(QPalette::Window,LOGLINEBOX_CHAIN_COLOR);
  line_chain_palette.setColor(QPalette::WindowText,Qt::black);

  line_unchanged_stop_palette=QGuiApplication::palette();
  line_unchanged_stop_palette.setColor(QPalette::Active,QPalette::Highlight,
			     QColor(BAR_UNCHANGED_STOPPING_COLOR));
  line_unchanged_stop_palette.setColor(QPalette::Inactive,
				       QPalette::Highlight,
			     QColor(BAR_UNCHANGED_STOPPING_COLOR));
  line_unchanged_play_palette=QGuiApplication::palette();
  line_unchanged_play_palette.setColor(QPalette::Active,QPalette::Highlight,
			     QColor(BAR_UNCHANGED_TRANSITION_COLOR));
  line_unchanged_play_palette.setColor(QPalette::Inactive,
				       QPalette::Highlight,
			     QColor(BAR_UNCHANGED_TRANSITION_COLOR));
  line_changed_stop_palette=QGuiApplication::palette();
  line_changed_stop_palette.setColor(QPalette::Active,QPalette::Highlight,
			     QColor(BAR_CHANGED_STOPPING_COLOR));
  line_changed_stop_palette.setColor(QPalette::Inactive,QPalette::Highlight,
			     QColor(BAR_CHANGED_STOPPING_COLOR));
  line_changed_play_palette=QGuiApplication::palette();
  line_changed_play_palette.setColor(QPalette::Active,QPalette::Highlight,
			     QColor(BAR_CHANGED_TRANSITION_COLOR));
  line_changed_play_palette.setColor(QPalette::Inactive,QPalette::Highlight,
			     QColor(BAR_CHANGED_TRANSITION_COLOR));
  line_time_palette=QGuiApplication::palette();
  line_time_palette.setColor(QPalette::Active,QPalette::Foreground,
			     QColor(Qt::black));
  line_time_palette.setColor(QPalette::Inactive,QPalette::Foreground,
			     QColor(Qt::black));
  line_hard_palette=QGuiApplication::palette();
  line_hard_palette.setColor(QPalette::Active,QPalette::Foreground,
			     QColor(LOG_HARDTIME_TEXT_COLOR));
  line_hard_palette.setColor(QPalette::Inactive,QPalette::Foreground,
			     QColor(LOG_HARDTIME_TEXT_COLOR));

  line_timescale_palette=QGuiApplication::palette();
  line_timescale_palette.setColor(QPalette::Active,QPalette::Foreground,
				  QColor(LOGLINEBOX_TIMESCALE_COLOR));
  line_timescale_palette.setColor(QPalette::Inactive,QPalette::Foreground,
				  QColor(LOGLINEBOX_TIMESCALE_COLOR));

  line_transition_palette=QGuiApplication::palette();
  line_transition_palette.setColor(QPalette::Active,QPalette::Foreground,
				  QColor(RD_CUSTOM_TRANSITION_COLOR));
  line_transition_palette.setColor(QPalette::Inactive,QPalette::Foreground,
				  QColor(RD_CUSTOM_TRANSITION_COLOR));

  line_text_palette=QGuiApplication::palette();

  //
  // Count Up
  //
  line_up_label=new QLabel(this);
  line_up_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  line_up_label->setPalette(line_text_palette);
  line_up_label->setFont(line_font);
  line_up_label->hide();
  
  //
  // Position Slider
  //
  line_position_bar=new QProgressBar(this);
  line_position_bar->setTextVisible(false);
  line_position_bar->hide();

  //
  // Count Down
  //
  line_down_label=new QLabel(this);
  line_down_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  line_down_label->setPalette(line_text_palette);
  line_down_label->setFont(line_font);
  line_down_label->hide();

  //
  // Cut Description
  //
  line_description_label=new QLabel(this);
  line_description_label->setPalette(line_text_palette);
  line_description_label->setFont(line_font);
  line_description_label->hide();

  //
  // Outcue
  //
  line_outcue_label=new QLabel(this);
  line_outcue_label->setPalette(line_text_palette);
  line_outcue_label->setFont(outcue_font);
  line_outcue_label->hide();

  //
  // Artist
  //
  line_artist_label=new QLabel(this);
  line_artist_label->setPalette(line_text_palette);
  line_artist_label->setFont(line_font);

  //
  // Title
  //
  line_title_label=new QLabel(this);
  line_title_label->setPalette(line_text_palette);
  line_title_label->setFont(line_bold_font);

  //
  // Marker Comment
  //
  line_comment_label=new QLabel(this);
  line_comment_label->setPalette(line_text_palette);
  line_comment_label->setFont(line_font);
  line_comment_label->setAlignment(Qt::AlignTop|Qt::AlignLeft);
  line_comment_label->setWordWrap(true);
  line_comment_label->hide();

  //
  // Icon
  //
  line_icon_label=new QLabel(this);

  //
  // Cart
  //
  line_cart_label=new QLabel(this);
  line_cart_label->setPalette(line_text_palette);
  line_cart_label->setFont(line_font);

  //
  // Cut
  //
  line_cut_label=new QLabel(this);
  line_cut_label->setPalette(line_text_palette);
  line_cut_label->setFont(line_font);

  //
  // Group
  //
  line_group_label=new QLabel(this);
  line_group_label->setFont(line_bold_font);

  //
  // Time
  //
  line_time_label=new QLabel(this);
  line_time_label->setFont(line_font);
  line_time_label->setAlignment(Qt::AlignRight);
  line_time_label->setPalette(line_time_palette);

  //
  // Talk Time
  //
  line_talktime_label=new QLabel(this);
  line_talktime_label->setPalette(line_text_palette);
  line_talktime_label->setFont(talk_font);
  line_talktime_label->setAlignment(Qt::AlignRight);

  //
  // Length
  //
  line_length_label=new QLabel(this);
  line_length_label->setPalette(line_text_palette);
  line_length_label->setFont(line_font);
  line_length_label->setAlignment(Qt::AlignRight);

  //
  // Transition Type
  //
  line_trans_label=new QLabel(this);
  line_trans_label->setAlignment(Qt::AlignRight);
  line_trans_label->setPalette(line_text_palette);
  line_trans_label->setFont(line_bold_font);

  //
  // Countdown Timer
  //
  line_countdown_timer=new QTimer(this);
  connect(line_countdown_timer,SIGNAL(timeout()),this,SLOT(countdownData()));

#ifdef COLORIZE_LAYOUT
  line_up_label->setStyleSheet("background-color: #FF0000");
  line_down_label->setStyleSheet("background-color: #00FF00");
  line_description_label->setStyleSheet("background-color: #FF0000");
  line_outcue_label->setStyleSheet("background-color: #00FF00");
  line_artist_label->setStyleSheet("background-color: #0000FF");
  line_title_label->setStyleSheet("background-color: #FF0000");
  line_comment_label->setStyleSheet("background-color: #FFFF00");
  line_icon_label->setStyleSheet("background-color: #00FFFF");
  line_cart_label->setStyleSheet("background-color: #FF00FF");
  line_cut_label->setStyleSheet("background-color: #FF0000");
  line_group_label->setStyleSheet("background-color: #00FF00");
  line_time_label->setStyleSheet("background-color: #FFFF00");
  line_talktime_label->setStyleSheet("background-color: #00FFFF");
  line_length_label->setStyleSheet("background-color: #0000FF");
  line_trans_label->setStyleSheet("background-color: #FF00FF");
#endif  // COLORIZE_LAYOUT
  
  setAcceptDrops(true);
}


QSize LogLineBox::sizeHint() const
{
  switch(line_mode) {
  case LogLineBox::Full:
    return QSize(433,LOGLINEBOX_FULL_HEIGHT);

  case LogLineBox::Half:
    return QSize(433,LOGLINEBOX_HALF_HEIGHT);
  }
  return QSize(0,0);
}


QSizePolicy LogLineBox::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


LogLineBox::Mode LogLineBox::mode() const
{
  return line_mode;
}


int LogLineBox::line() const
{
  return log_line;
}


void LogLineBox::setLine(int line)
{
  log_line=line;
}


void LogLineBox::setAllowDrags(bool state)
{
  line_allow_drags=state;
}


RDLogLine *LogLineBox::logLine()
{
  return line_logline;
}


void LogLineBox::setMode(LogLineBox::Mode mode)
{
  switch(mode) {
  case LogLineBox::Full:
    switch(line_type) {
    case RDLogLine::Cart:
      if(log_id!=-1) {
	line_up_label->show();
	line_down_label->show();
	line_position_bar->show();
	line_description_label->show();
      }
      else {
	line_up_label->hide();
	line_down_label->hide();
	line_position_bar->hide();
	line_description_label->hide();
      }
      break;

    case RDLogLine::Marker:
    case RDLogLine::Track:
    case RDLogLine::Macro:
    case RDLogLine::Chain:
      line_up_label->hide();
      line_down_label->hide();
      line_position_bar->hide();
      line_description_label->hide();
      break;

    default:
      break;
    }
    line_comment_label->setGeometry(5,18,sizeHint().width()-10,62);
    break;

  case LogLineBox::Half:
    line_up_label->hide();
    line_down_label->hide();
    line_position_bar->hide();
    line_description_label->hide();
    line_comment_label->setGeometry(5,18,sizeHint().width()-10,30);
    break;
  }
  line_mode=mode;
  update();
}


void LogLineBox::setStatus(RDLogLine::Status status)
{
  switch(status) {
  case RDLogLine::Scheduled:
    line_outcue_label->hide();
    break;

  default:
    line_outcue_label->show();
    break;
  }
  line_status=status;
}


void LogLineBox::setEvent(int line,RDLogLine::TransType next_type,
			  RDLogLine *logline)
{
  QString cutname;
  QPalette p;

  line_logline=logline;
  line_next_type=next_type;

  RDCart *cart;
  RDCut *cut;

  log_id=logline->id();
  log_line=line;
  line_type=logline->type();
  switch(line_logline->transType()) {
  case RDLogLine::Stop:
    line_trans_label->setText(tr("STOP"));
    line_trans_label->setPalette(line_text_palette);
    break;
	
  case RDLogLine::Play:
    line_trans_label->setText(tr("PLAY"));
    line_trans_label->setPalette(line_text_palette);
    break;
	
  case RDLogLine::Segue:
    line_trans_label->setText(tr("SEGUE"));
    if(logline->hasCustomTransition()) {
      line_trans_label->setPalette(line_transition_palette);
    }
    else {
      line_trans_label->setPalette(line_text_palette);
    }
    break;

  default:
    break;
  }
  switch(line_type) {
  case RDLogLine::Cart:
    line_comment_label->hide();
    cart=new RDCart(logline->cartNumber());
    if(logline->cutNumber()>0) {
      cut=new RDCut(logline->cartNumber(),logline->cutNumber());
    }
    else {
      cut=NULL;
    }
    if(!cart->exists()) {
      line_cart_label->
	setText(QString::asprintf("%06u",logline->cartNumber()));
      line_description_label->clear();
      line_artist_label->clear();
      line_cut_label->clear();
      line_group_label->clear();
      line_outcue_label->clear();
      line_length_label->setText("00:00");
      line_talktime_label->setText(":00");
      line_up_label->setText("0:00:00");
      line_down_label->setText("0:00:00");
      line_comment_label->clear();
      line_title_label->setText(tr("[CART NOT FOUND]"));
      SetPalette(line_error_palette,Qt::black);
      switch(cart->type()) {
      case RDCart::Audio:
      case RDCart::Macro:
      case RDCart::All:
	line_icon_label->setPixmap(*line_playout_map);
	break;
      }
    }
    else {
      if(((cart->forcedLength()==0)&&(cart->type()==RDCart::Audio))||
	 (line_logline->state()==RDLogLine::NoCut)) {
	line_cart_label->
	  setText(QString::asprintf("%06u",logline->cartNumber()));
	if(cut!=NULL) {
	  line_description_label->setText(cut->description());
	}
	line_artist_label->setText(tr("[NO AUDIO AVAILABLE]"));
	line_cut_label->clear();
	line_group_label->clear();
	line_outcue_label->clear();
	line_length_label->setText("00:00");
	line_talktime_label->setText(":00");
	line_up_label->setText("0:00:00");
	line_down_label->setText("0:00:00");
	line_comment_label->clear();
	line_icon_label->setPixmap(*line_playout_map);
	line_title_label->setText(logline->title());
	SetPalette(line_error_palette,logline->groupColor());
      }
      else {
	line_cart_label->
	  setText(QString::asprintf("%06u",logline->cartNumber()));
	if(line_logline->evergreen()) {
	  SetPalette(line_evergreen_palette,logline->groupColor());
	}
	else {
	  SetPalette(line_default_palette,logline->groupColor());
	}
	if(line_logline->source()==RDLogLine::Tracker) {
	  line_icon_label->setPixmap(*line_track_cart_map);
	}
	else {
	  switch(cart->type()) {
	  case RDCart::Audio:
	  case RDCart::All:
	    line_icon_label->setPixmap(*line_playout_map);
	    break;
	    
	  case RDCart::Macro:
	    line_icon_label->setPixmap(*line_macro_map);
	    break;
	  }
	}
	line_group_label->setText(cart->groupName());
	p=line_group_label->palette();
	p.setColor(QPalette::Foreground,line_logline->groupColor());
	line_group_label->setPalette(p);
	if(line_logline->talkLength()<=0) {
	  line_talktime_label->setText(":00");
	}
	else {
	  line_talktime_label->
	    setText(RDGetTimeLength(line_logline->talkLength(),
				    false,false));
	}
	line_length_label->
	  setText(RDGetTimeLength(line_logline->effectiveLength(),
				  false,false));
	if(line_logline->timescalingActive()) {
	  line_length_label->setPalette(line_hard_palette);
	}
	else {
	  line_length_label->setPalette(line_time_palette);
	}
	if((line_logline->source()!=RDLogLine::Tracker)||
	   line_logline->originUser().isEmpty()||
	   (!line_logline->originDateTime().isValid())) {
	  line_title_label->
	    setText(line_logline->resolveWildcards(line_title_template));
	}
	else {
	  line_title_label->setText(line_logline->title()+" -- "+
				    line_logline->originUser()+" "+
				    line_logline->originDateTime().
				    toString("M/d hh:mm"));
	}
	line_description_label->
	  setText(line_logline->resolveWildcards(line_description_template));
	line_artist_label->
	  setText(line_logline->resolveWildcards(line_artist_template));	
	line_up_label->
	  setText(RDGetTimeLength(line_logline->playPosition(),true,true));
	line_down_label->
	  setText(RDGetTimeLength(line_logline->effectiveLength()-
				  line_logline->playPosition(),true,true));
	line_position_bar->setMaximum(line_logline->effectiveLength());
	line_position_bar->setValue(line_logline->playPosition());
	if(logline->cutNumber()>=0) {
	  line_cut_label->
	    setText(QString::asprintf("%03u",logline->cutNumber()));
	line_outcue_label->
	  setText(line_logline->resolveWildcards(line_outcue_template));	
	}
	else {
	  SetPalette(line_error_palette,logline->groupColor());
	  line_cut_label->clear();
	  line_outcue_label->setText(tr("[NO VALID CUT AVAILABLE]"));
	}
	setMode(line_mode);
	line_title_label->show();
	line_artist_label->show();
      }
    }
    delete cart;
    if(cut!=NULL) {
      delete cut;
    }
    break;
    
  case RDLogLine::Marker:
    line_icon_label->setPixmap(*line_notemarker_map);
    SetPalette(line_marker_palette,logline->groupColor());
    line_title_label->hide();
    line_description_label->hide();
    line_artist_label->hide();
    line_cart_label->setText(tr("MARKER"));
    line_group_label->setText(logline->markerLabel());
    line_length_label->setText(":00");
    line_transition=logline->transType();
    line_comment_label->setText(logline->markerComment());
    setMode(line_mode);
    line_comment_label->show();
    break;
    
  case RDLogLine::Track:
    line_icon_label->setPixmap(*line_mic16_map);
    SetPalette(line_marker_palette,logline->groupColor());
    line_title_label->hide();
    line_description_label->hide();
    line_artist_label->hide();
    line_cart_label->setText(tr("TRACK"));
    line_group_label->setText("");
    line_length_label->setText(":00");
    line_transition=logline->transType();
    line_comment_label->setText(logline->markerComment());
    setMode(line_mode);
    line_comment_label->show();
    break;
    
  case RDLogLine::MusicLink:
    line_icon_label->setPixmap(*line_music_map);
    SetPalette(line_marker_palette,logline->groupColor());
    line_title_label->hide();
    line_description_label->hide();
    line_artist_label->hide();
    line_cart_label->setText(tr("LINK"));
    line_group_label->setText("");
    line_length_label->setText(":00");
    line_transition=logline->transType();
    line_comment_label->setText(tr("[music import]"));
    setMode(line_mode);
    line_comment_label->show();
    break;
    
  case RDLogLine::TrafficLink:
    line_icon_label->setPixmap(*line_traffic_map);
    SetPalette(line_marker_palette,logline->groupColor());
    line_title_label->hide();
    line_description_label->hide();
    line_artist_label->hide();
    line_cart_label->setText(tr("LINK"));
    line_group_label->setText("");
    line_length_label->setText(":00");
    line_transition=logline->transType();
    line_comment_label->setText(tr("[traffic import]"));
    setMode(line_mode);
    line_comment_label->show();
    break;

  case RDLogLine::Chain:
    line_icon_label->setPixmap(*line_chain_map);
    SetPalette(line_chain_palette,logline->groupColor());
    line_title_label->setText(logline->markerLabel());
    line_description_label->setText("");
    line_artist_label->setText(logline->markerComment());
    line_cart_label->setText(tr("CHAIN"));
    line_group_label->setText("");
    line_length_label->setText(":00");
    line_transition=logline->transType();
    line_comment_label->hide();
    setMode(line_mode);
    line_title_label->show();
    line_artist_label->show();
    break;

  case RDLogLine::Macro:
    line_icon_label->setPixmap(*line_macro_map);
    line_comment_label->hide();
    cart=new RDCart(logline->cartNumber());
    cut=new RDCut(QString::asprintf("%06u_%03u",logline->cartNumber(),
				    logline->cutNumber()));
    if(!cart->exists()) {
      SetPalette(line_error_palette,logline->groupColor());
    }
    else {
      SetPalette(line_default_palette,logline->groupColor());
    }
    line_cart_label->setText(QString::asprintf("%06u",cart->number()));
    line_cut_label->setText("");
    line_group_label->setText(cart->groupName());
    p=line_group_label->palette();
    p.setColor(QPalette::Foreground,line_logline->groupColor());
    line_group_label->setPalette(p);
    line_length_label->
      setText(RDGetTimeLength(line_logline->effectiveLength(),
			      false,false));
    line_title_label->
      setText(line_logline->resolveWildcards(line_title_template));
    line_description_label->
      setText(line_logline->resolveWildcards(line_description_template));
    line_artist_label->
      setText(line_logline->resolveWildcards(line_artist_template));	
    line_outcue_label->
      setText(line_logline->resolveWildcards(line_outcue_template));	
    delete cart;
    delete cut;
    setMode(line_mode);
    line_title_label->show();
    line_artist_label->show();
    break;

  default:
    break;
  }
  PrintTime();
}


void LogLineBox::setTimer(int msecs)
{
  line_up_label->setText(RDGetTimeLength(msecs,true,true));
  line_down_label->
    setText(RDGetTimeLength(line_logline->effectiveLength()-msecs,true,true));
  line_position_bar->setValue(msecs);
}


void LogLineBox::clear()
{
  SetPalette(line_default_palette,Qt::black);
  line_cart_label->setText("");
  line_cut_label->setText("");
  line_group_label->setText("");
  line_trans_label->setText("");
  line_title_label->setText("");
  line_description_label->setText("");
  line_artist_label->setText("");
  line_outcue_label->setText("");
  line_outcue_label->hide();
  line_comment_label->setText("");
  line_time_label->setText("");
  line_talktime_label->setText("");
  line_length_label->setText("");
  line_up_label->setText("");
  line_down_label->setText("");
  line_position_bar->reset();
  line_countdown_timer->stop();
  line_end_time=QTime();
  log_id=-1;
  log_line=-1;
  line_transition=RDLogLine::Stop;
  line_logline=NULL;
  line_type=RDLogLine::Cart;
  line_up_label->hide();
  line_position_bar->hide();
  line_down_label->hide();
  line_icon_label->clear();
  setBarMode(LogLineBox::Stopping);
}


void LogLineBox::setBarMode(LogLineBox::BarMode mode)
{
  switch(mode) {
  case LogLineBox::Transitioning:
    if(line_logline!=NULL) {
      if(line_logline->playPositionChanged()) {
	line_position_bar->setPalette(line_changed_play_palette);
      }
      else {
	line_position_bar->setPalette(line_unchanged_play_palette);
      }
    }
    else {
      line_position_bar->setPalette(line_unchanged_play_palette);
    }
    break;

  case LogLineBox::Stopping:
    if(line_logline!=NULL) {
      if(line_logline->playPositionChanged()) {
	line_position_bar->setPalette(line_changed_stop_palette);
      }
      else {
	line_position_bar->setPalette(line_unchanged_stop_palette);
      }
    }
    else {
      line_position_bar->setPalette(line_unchanged_stop_palette);
    }
    break;
  }
}


void LogLineBox::countdownData()
{
  UpdateCountdown();
}


void LogLineBox::mousePressEvent(QMouseEvent *e)
{
  QWidget::mousePressEvent(e);
  line_move_count=3;
  line_drag_start_pos=e->pos();
}


void LogLineBox::mouseMoveEvent(QMouseEvent *e)
{
  QWidget::mouseMoveEvent(e);
  
  if((e->pos()-line_drag_start_pos).manhattanLength()>
     LOGLINEBOX_DRAG_THRESHOLD) {
    line_move_count--;
    if(line_move_count==0) {
      if(line_allow_drags&&(line_logline!=NULL)) {
	QDrag *drag=new QDrag(this);
	RDCartDrag *cd=new RDCartDrag(line_logline->cartNumber(),
				      line_logline->title(),
				      line_group_label->palette().
				      color(QPalette::Foreground));
	drag->setMimeData(cd);
	drag->setPixmap(*(line_icon_label->pixmap()));
	drag->exec();
      }
    }
  }
}


void LogLineBox::mouseReleaseEvent(QMouseEvent *e)
{
  line_move_count=-1;
  line_drag_start_pos=QPoint();
}


void LogLineBox::mouseDoubleClickEvent(QMouseEvent *e)
{
  if(line_logline==NULL) {
    return;
  }
  emit doubleClicked(log_line);
  QWidget::mouseDoubleClickEvent(e);
}


void LogLineBox::resizeEvent(QResizeEvent *e)
{
  if(line_mode==LogLineBox::Full) {
    line_comment_label->setGeometry(5,18,size().width()-10,15);
  }
  else {
    line_comment_label->setGeometry(5,18,size().width()-10,62);
  }

  line_icon_label->setGeometry(5,3,16,16);
  line_cart_label->setGeometry(23,3,48,16);
  line_cut_label->setGeometry(73,3,24,16);
  line_group_label->setGeometry(97,3,90,16);
  line_time_label->setGeometry(187,3,98,16);
  line_talktime_label->setGeometry(287,3,36,16);
  line_length_label->setGeometry(327,3,50,16);
  line_trans_label->setGeometry(379,3,48,16);

  line_title_label->setGeometry(5,18,size().width()-10,16);

  line_artist_label->setGeometry(5,32,size().width()-10,15);

  line_outcue_label->setGeometry(5,
				 48,
				 size().width()/2-10,
				 16);
  line_description_label->setGeometry(size().width()/2,
				      48,
				      size().width()/2-5,
				      16);

  line_up_label->setGeometry(5,65,65,16);
  line_position_bar->setGeometry(75,66,size().width()-150,13);
  line_down_label->setGeometry(size().width()-72,65,65,16);
}


void LogLineBox::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);
  p->fillRect(0,0,size().width()-2,size().height()-2,
	      palette().color(QPalette::Window));
  p->setPen(palette().color(QPalette::Dark));
  p->drawRect(0,0,size().width()-1,size().height()-1);
  p->end();
  delete p;
}


void LogLineBox::dragEnterEvent(QDragEnterEvent *e)
{
  if(RDCartDrag::canDecode(e->mimeData())&&
     ((line_status==RDLogLine::Scheduled)||
      (line_status==RDLogLine::Paused))) {
    e->accept();
  }
}


void LogLineBox::dropEvent(QDropEvent *e)
{
  RDLogLine ll;

  if(RDCartDrag::decode(e->mimeData(),&ll)) {
    emit cartDropped(log_line,&ll);
  }
}


void LogLineBox::SetPalette(const QPalette &pal,const QColor &grp_color)
{
  //  printf("%d:SetPalette(%s,%s)\n",log_line,
  //	 pal.color(QPalette::WindowText).name().toUtf8().constData(),
  //	 pal.color(QPalette::Window).name().toUtf8().constData());
  setPalette(pal);
  setStyleSheet("color:"+pal.color(QPalette::WindowText).name()+
		";background-color:"+pal.color(QPalette::Window).name());
  QPalette pal1=pal;
  pal1.setColor(QPalette::WindowText,grp_color);
  line_group_label->setPalette(pal1);
  line_group_label->
    setStyleSheet("color:"+pal1.color(QPalette::WindowText).name()+
		  ";background-color:"+pal1.color(QPalette::Window).name());
}


void LogLineBox::UpdateCountdown()
{
  QTime current=QTime::currentTime().addMSecs(rda->station()->timeOffset());

  if(current<line_end_time) {
    line_length_label->setText(RDGetTimeLength(current.msecsTo(line_end_time),
					      false,false));
  }
  else {
    line_length_label->setText(RDGetTimeLength(0,false,false));
  }
}


void LogLineBox::PrintTime()
{
  if(line_logline==NULL) {
    return;
  }
  switch(line_logline->timeType()) {
      case RDLogLine::Hard:
	line_time_label->setFont(line_bold_font);
	line_time_label->
	  setText("T"+rda->tenthsTimeString(line_logline->
					    startTime(RDLogLine::Logged)));
	line_time_label->setPalette(line_hard_palette);
	break;

      default:
	line_time_label->setFont(line_font);
	if(!line_logline->startTime(RDLogLine::Logged).isNull()) {
	  line_time_label->
	    setText(rda->tenthsTimeString(line_logline->
					  startTime(RDLogLine::Logged)));
	}
	else {
	  line_time_label->setText("");
	}
	line_time_label->setPalette(line_time_palette);
	break;
  }
}
