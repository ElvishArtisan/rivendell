// loglinebox.cpp
//
// On Air Playout Utility for Rivendell.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: loglinebox.cpp,v 1.89.6.8 2014/02/06 20:43:50 cvs Exp $
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

#include <rdconf.h>
#include <rdplay_deck.h>
#include <rdairplay_conf.h>
#include <rdnownext.h>

#include <loglinebox.h>
#include <colors.h>
#include <globals.h>

#include "../icons/play.xpm"
#include "../icons/rml5.xpm"
#include "../icons/chain.xpm"
#include "../icons/track_cart.xpm"
#include "../icons/mic16.xpm"
#include "../icons/notemarker.xpm"
#include "../icons/traffic.xpm"
#include "../icons/music.xpm"


LogLineBox::LogLineBox(RDAirPlayConf *conf,QWidget *parent,const char *name)
  : QWidget(parent,name)
{
  line_status=RDLogLine::Scheduled;
  line_type=RDLogLine::UnknownType;
  line_mode=LogLineBox::Full;
  line_time_mode=RDAirPlayConf::TwentyFourHour;
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
  // Create Font
  //
  line_bold_font=QFont("Helvetica",12,QFont::Bold);
  line_bold_font.setPixelSize(12);
  line_font=QFont("Helvetica",12,QFont::Normal);
  line_font.setPixelSize(12);
  talk_font=QFont("Helvetica",12,QFont::Bold);
  talk_font.setPixelSize(12);
  QFont outcue_font=QFont("Helvetica",12,QFont::Normal);
  outcue_font.setPixelSize(12);
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
  line_unchanged_stop_palette=palette();
  line_unchanged_stop_palette.setColor(QPalette::Active,QColorGroup::Highlight,
			     QColor(BAR_UNCHANGED_STOPPING_COLOR));
  line_unchanged_stop_palette.setColor(QPalette::Inactive,
				       QColorGroup::Highlight,
			     QColor(BAR_UNCHANGED_STOPPING_COLOR));
  line_unchanged_play_palette=palette();
  line_unchanged_play_palette.setColor(QPalette::Active,QColorGroup::Highlight,
			     QColor(BAR_UNCHANGED_TRANSITION_COLOR));
  line_unchanged_play_palette.setColor(QPalette::Inactive,
				       QColorGroup::Highlight,
			     QColor(BAR_UNCHANGED_TRANSITION_COLOR));
  line_changed_stop_palette=palette();
  line_changed_stop_palette.setColor(QPalette::Active,QColorGroup::Highlight,
			     QColor(BAR_CHANGED_STOPPING_COLOR));
  line_changed_stop_palette.setColor(QPalette::Inactive,QColorGroup::Highlight,
			     QColor(BAR_CHANGED_STOPPING_COLOR));
  line_changed_play_palette=palette();
  line_changed_play_palette.setColor(QPalette::Active,QColorGroup::Highlight,
			     QColor(BAR_CHANGED_TRANSITION_COLOR));
  line_changed_play_palette.setColor(QPalette::Inactive,QColorGroup::Highlight,
			     QColor(BAR_CHANGED_TRANSITION_COLOR));
  line_time_palette=palette();
  line_hard_palette=palette();
  line_hard_palette.setColor(QPalette::Active,QColorGroup::Foreground,
			     QColor(LOG_HARDTIME_TEXT_COLOR));
  line_hard_palette.setColor(QPalette::Inactive,QColorGroup::Foreground,
			     QColor(LOG_HARDTIME_TEXT_COLOR));

  line_timescale_palette=palette();
  line_timescale_palette.setColor(QPalette::Active,QColorGroup::Foreground,
				  QColor(LOGLINEBOX_TIMESCALE_COLOR));
  line_timescale_palette.setColor(QPalette::Inactive,QColorGroup::Foreground,
				  QColor(LOGLINEBOX_TIMESCALE_COLOR));

  line_transition_palette=palette();
  line_transition_palette.setColor(QPalette::Active,QColorGroup::Foreground,
				  QColor(RD_CUSTOM_TRANSITION_COLOR));
  line_transition_palette.setColor(QPalette::Inactive,QColorGroup::Foreground,
				  QColor(RD_CUSTOM_TRANSITION_COLOR));

  //
  // Count Up
  //
  line_up_label=new QLabel(this,"line_up_label");
  line_up_label->setGeometry(5,65,65,16);
  line_up_label->setAlignment(AlignLeft|AlignVCenter);
  line_up_label->setFont(line_font);
  line_up_label->hide();

  //
  // Position Slider
  //
  line_position_bar=new QProgressBar(this,"line_position_bar");
  line_position_bar->setGeometry(75,66,sizeHint().width()-150,13);
  line_position_bar->setPercentageVisible(false);
  line_position_bar->hide();

  //
  // Count Down
  //
  line_down_label=new QLabel(this,"line_down_label");
  line_down_label->setGeometry(sizeHint().width()-72,65,65,16);
  line_down_label->setAlignment(AlignLeft|AlignVCenter);
  line_down_label->setFont(line_font);
  line_down_label->hide();

  //
  // Cut Description
  //
  line_description_label=new QLabel(this,"line_description_label");
  line_description_label->setGeometry((sizeHint().width()/2),48,(sizeHint().width()/2 -10),16);
  line_description_label->setFont(line_font);
  line_description_label->hide();

  //
  // Outcue
  //
  line_outcue_label=new QLabel(this,"line_outcue_label");
  line_outcue_label->setGeometry(5,48, (sizeHint().width()/2 -10),16);
  line_outcue_label->setFont(outcue_font);
  line_outcue_label->hide();

  //
  // Artist
  //
  line_artist_label=new QLabel(this,"line_artist_label");
  line_artist_label->setGeometry(5,33,sizeHint().width()-10,16);
  line_artist_label->setFont(line_font);

  //
  // Title
  //
  line_title_label=new QLabel(this,"line_title_label");
  line_title_label->setGeometry(5,18,sizeHint().width()-10,18);
  line_title_label->setFont(line_bold_font);

  //
  // Marker Comment
  //
  line_comment_label=new RDLabel(this,"line_comment_label");
  line_comment_label->setGeometry(5,18,sizeHint().width()-10,62);
  line_comment_label->setFont(line_font);
  line_comment_label->setAlignment(AlignTop|AlignLeft);
  line_comment_label->setWordWrapEnabled(true);
  line_comment_label->hide();

  //
  // Icon
  //
  line_icon_label=new QLabel(this,"line_icon_label");
  line_icon_label->setGeometry(5,3,45,16);

  //
  // Cart
  //
  line_cart_label=new QLabel(this,"line_cart_label");
  line_cart_label->setGeometry(25,3,53,16);
  line_cart_label->setFont(line_font);

  //
  // Cut
  //
  line_cut_label=new QLabel(this,"line_cut_label");
  line_cut_label->setGeometry(80,3,24,16);
  line_cut_label->setFont(line_font);

  //
  // Group
  //
  line_group_label=new QLabel(this,"line_group_label");
  line_group_label->setGeometry(107,3,75,16);
  line_group_label->setFont(line_bold_font);

  //
  // Time
  //
  line_time_label=new QLabel(this,"line_time_label");
  line_time_label->setGeometry(185,3,85,16);
  line_time_label->setFont(line_font);
  line_time_label->setAlignment(AlignRight);
  line_time_label->setPalette(line_time_palette);

  //
  // Talk Time
  //
  line_talktime_label=new QLabel(this,"line_talktime_label");
  line_talktime_label->setGeometry(273,3,21,16);
  line_talktime_label->setFont(talk_font);
  line_talktime_label->setAlignment(AlignRight);

  //
  // Length
  //
  line_length_label=new QLabel(this,"line_length_label");
  line_length_label->setGeometry(297,3,40,16);
  line_length_label->setFont(line_font);
  line_length_label->setAlignment(AlignRight);

  //
  // Transition Type
  //
  line_trans_label=new QLabel(this,"line_trans_label");
  line_trans_label->setGeometry(sizeHint().width()-53,3,48,16);
  line_trans_label->setAlignment(AlignRight);
  line_trans_label->setFont(line_bold_font);

  SetColor(QColor(LOGLINEBOX_BACKGROUND_COLOR));

  //
  // Countdown Timer
  //
  line_countdown_timer=new QTimer(this,"line_countdown_timer");
  connect(line_countdown_timer,SIGNAL(timeout()),this,SLOT(countdownData()));

  setAcceptDrops(true);
}


QSize LogLineBox::sizeHint() const
{
  switch(line_mode) {
      case LogLineBox::Full:
	return QSize(393,LOGLINEBOX_FULL_HEIGHT);
	break;

      case LogLineBox::Half:
	return QSize(393,LOGLINEBOX_HALF_HEIGHT);
	break;
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
	line_comment_label->setGeometry(5,18,sizeHint().width()-10,31);
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
	line_trans_label->setPalette(palette());
	break;
	
      case RDLogLine::Play:
	line_trans_label->setText(tr("PLAY"));
	line_trans_label->setPalette(palette());
	break;
	
      case RDLogLine::Segue:
	line_trans_label->setText(tr("SEGUE"));
	if(logline->hasCustomTransition()) {
	  line_trans_label->setPalette(line_transition_palette);
	}
	else {
	  line_trans_label->setPalette(palette());
	}
	break;

      default:
	break;
  }
  switch(line_type) {
  case RDLogLine::Cart:
    line_comment_label->hide();
    cart=new RDCart(logline->cartNumber());
    cut=new RDCut(logline->cartNumber(),logline->cutNumber());
    if(!cart->exists()) {
      line_cart_label->
	setText(QString().sprintf("%06u",logline->cartNumber()));
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
      switch(cart->type()) {
      case RDCart::Audio:
      case RDCart::Macro:
      case RDCart::All:
	line_icon_label->setPixmap(*line_playout_map);
	break;
      }
      SetColor(QColor(LOGLINEBOX_MISSING_COLOR));
      delete cart;
      delete cut;
    }
    else {
      if(((cart->forcedLength()==0)&&(cart->type()==RDCart::Audio))||
	 (line_logline->state()==RDLogLine::NoCut)) {
	line_cart_label->
	  setText(QString().sprintf("%06u",logline->cartNumber()));
	line_description_label->setText(cut->description());
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
	SetColor(QColor(LOGLINEBOX_MISSING_COLOR));
	delete cart;
	delete cut;
      }
      else {
	line_cart_label->
	  setText(QString().sprintf("%06u",logline->cartNumber()));
	if(line_logline->evergreen()) {
	  SetColor(QColor(LOGLINEBOX_EVERGREEN_COLOR));
	}
	else {
	  SetColor(QColor(LOGLINEBOX_BACKGROUND_COLOR));
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
	p.setColor(QColorGroup::Foreground,line_logline->groupColor());
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
	    setText(RDResolveNowNext(line_title_template,line_logline));
	}
	else {
	  line_title_label->setText(QString().
				    sprintf("%s -- %s %s",
					    (const char *)line_logline->title(),
					    (const char *)line_logline->originUser(),
					    (const char *)line_logline->originDateTime().
					    toString("M/d hh:mm")));
	}
	line_description_label->
	  setText(RDResolveNowNext(line_description_template,line_logline));
	line_artist_label->
	  setText(RDResolveNowNext(line_artist_template,line_logline));
	line_up_label->
	  setText(RDGetTimeLength(line_logline->playPosition(),true,true));
	line_down_label->
	  setText(RDGetTimeLength(line_logline->effectiveLength()-
				  line_logline->playPosition(),true,true));
	line_position_bar->setTotalSteps(line_logline->effectiveLength());
	line_position_bar->setProgress(line_logline->playPosition());
	if(logline->cutNumber()>=0) {
	  line_cut_label->
	    setText(QString().sprintf("%03u",logline->cutNumber()));
	  line_outcue_label->
	    setText(RDResolveNowNext(line_outcue_template,line_logline));
	}
	else {
	  SetColor(QColor(LOGLINEBOX_MISSING_COLOR));
	  line_cut_label->clear();
	  line_outcue_label->setText(tr("[NO VALID CUT AVAILABLE]"));
	}
	delete cart;
	delete cut;
	setMode(line_mode);
	line_title_label->show();
	line_artist_label->show();
      }
    }
    break;
    
  case RDLogLine::Marker:
    line_icon_label->setPixmap(*line_notemarker_map);
    SetColor(QColor(LOGLINEBOX_MARKER_COLOR));
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
    SetColor(QColor(LOGLINEBOX_MARKER_COLOR));
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
    SetColor(QColor(LOGLINEBOX_MARKER_COLOR));
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
    SetColor(QColor(LOGLINEBOX_MARKER_COLOR));
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
    SetColor(QColor(LOGLINEBOX_CHAIN_COLOR));
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
    cut=new RDCut(QString().sprintf("%06u_%03u",logline->cartNumber(),
				    logline->cutNumber()));
    if(!cart->exists()) {
      SetColor(QColor(LOGLINEBOX_MISSING_COLOR));
    }
    else {
      SetColor(QColor(LOGLINEBOX_BACKGROUND_COLOR));
    }
    line_cart_label->setText(QString().sprintf("%06u",cart->number()));
    line_cut_label->setText("");
    line_group_label->setText(cart->groupName());
    p=line_group_label->palette();
    p.setColor(QColorGroup::Foreground,line_logline->groupColor());
    line_group_label->setPalette(p);
    line_length_label->
      setText(RDGetTimeLength(line_logline->effectiveLength(),
			      false,false));

    line_title_label->
      setText(RDResolveNowNext(line_title_template,line_logline));
    line_description_label->
      setText(RDResolveNowNext(line_description_template,line_logline));
    line_artist_label->
      setText(RDResolveNowNext(line_artist_template,line_logline));
    line_outcue_label->
      setText(RDResolveNowNext(line_outcue_template,line_logline));
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
  line_position_bar->setProgress(msecs);
}


void LogLineBox::clear()
{
  SetColor(QColor(LOGLINEBOX_BACKGROUND_COLOR));
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
  line_time_mode=RDAirPlayConf::TwentyFourHour;
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


void LogLineBox::setTimeMode(RDAirPlayConf::TimeMode mode)
{
  if(mode==line_time_mode) {
    return;
  }
  line_time_mode=mode;
  PrintTime();
}


void LogLineBox::countdownData()
{
  UpdateCountdown();
}


void LogLineBox::mousePressEvent(QMouseEvent *e)
{
  QWidget::mousePressEvent(e);
  line_move_count=3;
}


void LogLineBox::mouseMoveEvent(QMouseEvent *e)
{
  QWidget::mouseMoveEvent(e);
  line_move_count--;
  if(line_move_count==0) {
    if(line_allow_drags&&(line_logline!=NULL)) {
      RDCartDrag *d=
	new RDCartDrag(line_logline->cartNumber(),line_icon_label->pixmap(),
		       this);
      d->dragCopy();
    }
  }
}


void LogLineBox::mouseReleaseEvent(QMouseEvent *e)
{
  line_move_count=-1;
}


void LogLineBox::mouseDoubleClickEvent(QMouseEvent *e)
{
  if(line_logline==NULL) {
    return;
  }
  emit doubleClicked(log_line);
  QWidget::mouseDoubleClickEvent(e);
}


void LogLineBox::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);
  p->drawRect(0,0,sizeHint().width(),sizeHint().height());
  p->end();
  delete p;
}


void LogLineBox::dragEnterEvent(QDragEnterEvent *e)
{
  e->accept(RDCartDrag::canDecode(e)&&(line_status==RDLogLine::Scheduled));
}


void LogLineBox::dropEvent(QDropEvent *e)
{
  RDLogLine ll;

  if(RDCartDrag::decode(e,&ll)) {
    emit cartDropped(log_line,&ll);
  }
}


void LogLineBox::SetColor(QColor color)
{
  setBackgroundColor(color);
  line_cart_label->setBackgroundColor(color);
  line_cut_label->setBackgroundColor(color);
  line_group_label->setBackgroundColor(color);
  line_trans_label->setBackgroundColor(color);
  line_title_label->setBackgroundColor(color);
  line_description_label->setBackgroundColor(color);
  line_artist_label->setBackgroundColor(color);
  line_outcue_label->setBackgroundColor(color);
  line_time_label->setBackgroundColor(color);
  line_length_label->setBackgroundColor(color);
  line_talktime_label->setBackgroundColor(color);
  line_up_label->setBackgroundColor(color);
  line_position_bar->setBackgroundColor(QColor(lightGray));
  line_down_label->setBackgroundColor(color);
  line_comment_label->setBackgroundColor(color);
  line_icon_label->setBackgroundColor(color);
}


void LogLineBox::UpdateCountdown()
{
  QTime current=QTime::currentTime().addMSecs(rdstation_conf->timeOffset());

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
  QString str;
  
  if(line_logline==NULL) {
    return;
  }
  switch(line_logline->timeType()) {
      case RDLogLine::Hard:
	line_time_label->setFont(line_bold_font);
	str=QString(tr("T"));
	line_time_label->setText(QString().sprintf("%s%s",(const char *)str,
		     (const char *)TimeString(line_logline->
					      startTime(RDLogLine::Logged))));
	line_time_label->setPalette(line_hard_palette);
	break;

      default:
	line_time_label->setFont(line_font);
	if(!line_logline->startTime(RDLogLine::Logged).isNull()) {
	  line_time_label->
	    setText(QString().sprintf("%s",
				      (const char *)TimeString(line_logline->
					   startTime(RDLogLine::Logged))));
	}
	else {
	  line_time_label->setText("");
	}
	line_time_label->setPalette(line_time_palette);
	break;
  }
}


QString LogLineBox::TimeString(const QTime &time)
{
  QString ret;
  switch(line_time_mode) {
  case RDAirPlayConf::TwelveHour:
    ret=time.toString("h:mm:ss.zzz");
    ret=ret.left(ret.length()-2);
    ret+=(" "+time.toString("ap"));
    break;

  case RDAirPlayConf::TwentyFourHour:
    ret=time.toString("hh:mm:ss.zzz").left(10);
    break;
  }
  return ret;
}
