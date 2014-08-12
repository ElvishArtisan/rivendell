// rdslotbox.cpp
//
// Cart slot label widget for RDCartSlot
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdslotbox.cpp,v 1.5.2.8 2014/02/06 20:43:47 cvs Exp $
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

#include "rdconf.h"
#include "rdplay_deck.h"

#include "rdnownext.h"
#include "rdslotbox.h"

#include "../icons/play.xpm"
#include "../icons/rml5.xpm"

RDSlotBox::RDSlotBox(RDPlayDeck *deck,RDAirPlayConf *conf,QWidget *parent)
  : QWidget(parent)
{
  line_deck=deck;
  line_airplay_conf=conf;
  line_type=RDLogLine::UnknownType;
  line_logline=NULL;
  line_mode=RDSlotOptions::LastMode;
  log_id=-1;

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
				  QColor(LABELBOX_TIMESCALE_COLOR));
  line_timescale_palette.setColor(QPalette::Inactive,QColorGroup::Foreground,
				  QColor(LABELBOX_TIMESCALE_COLOR));

  //
  // Audio Meter
  //
  for(int i=0;i<2;i++) {
    line_meter[i]=new RDPlayMeter(RDSegMeter::Up,this);
    line_meter[i]->setGeometry(5+15*i,5,15,sizeHint().height()-10);
    line_meter[i]->setRange(-4600,0);
    line_meter[i]->setHighThreshold(-1600);
    line_meter[i]->setClipThreshold(-1100);
    line_meter[i]->setSegmentSize(2);
    line_meter[i]->setSegmentGap(1);
    line_meter[i]->setMode(RDSegMeter::Peak);
  }
  line_meter[0]->setLabel(tr("L"));
  line_meter[1]->setLabel(tr("R"));

  //
  // Count Up
  //
  line_up_label=new QLabel(this);
  line_up_label->setGeometry(45,65,65,16);
  line_up_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  line_up_label->setFont(line_font);
  line_up_label->hide();

  //
  // Position Slider
  //
  line_position_bar=new QProgressBar(this);
  line_position_bar->setGeometry(115,69,sizeHint().width()-190,13);
  line_position_bar->setPercentageVisible(false);
  line_position_bar->hide();

  //
  // Count Down
  //
  line_down_label=new QLabel(this);
  line_down_label->setGeometry(sizeHint().width()-72,65,65,16);
  line_down_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  line_down_label->setFont(line_font);
  line_down_label->hide();

  //
  // Cut Description
  //
  line_description_label=new QLabel(this);
  line_description_label->setGeometry((sizeHint().width()/2),49,(sizeHint().width()/2 -10),16);
  line_description_label->setFont(line_font);

  //
  // Outcue
  //
  line_outcue_label=new QLabel(this);
  line_outcue_label->setGeometry(45,49, (sizeHint().width()/2 -50),16);
  line_outcue_label->setFont(outcue_font);

  //
  // Artist
  //
  line_artist_label=new QLabel(this);
  line_artist_label->setGeometry(45,36,sizeHint().width()-50,16);
  line_artist_label->setFont(line_font);
  line_artist_label->hide();

  //
  // Title
  //
  line_title_label=new QLabel(this);
  line_title_label->setGeometry(45,18,sizeHint().width()-50,18);
  line_title_label->setFont(line_bold_font);

  //
  // Icon
  //
  line_icon_label=new QLabel(this);
  line_icon_label->setGeometry(45,3,45,16);

  //
  // Cart
  //
  line_cart_label=new QLabel(this);
  line_cart_label->setGeometry(65,3,53,16);
  line_cart_label->setFont(line_font);

  //
  // Cut
  //
  line_cut_label=new QLabel(this);
  line_cut_label->setGeometry(120,3,24,16);
  line_cut_label->setFont(line_font);

  //
  // Group
  //
  line_group_label=new QLabel(this);
  line_group_label->setGeometry(147,3,75,16);
  line_group_label->setFont(line_bold_font);

  //
  // Talk Time
  //
  line_talktime_label=new QLabel(this);
  line_talktime_label->setGeometry(313,3,21,16);
  line_talktime_label->setFont(talk_font);
  line_talktime_label->setAlignment(Qt::AlignRight);

  //
  // Length
  //
  line_length_label=new QLabel(this);
  line_length_label->setGeometry(337,3,40,16);
  line_length_label->setFont(line_font);
  line_length_label->setAlignment(Qt::AlignRight);

  SetColor(QColor(LABELBOX_BACKGROUND_COLOR));
}


QSize RDSlotBox::sizeHint() const
{
  return QSize(393,RDSLOTBOX_FULL_HEIGHT);
}


QSizePolicy RDSlotBox::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


RDLogLine *RDSlotBox::logLine()
{
  return line_logline;
}


void RDSlotBox::updateMeters(short levels[2])
{
  line_meter[0]->setPeakBar(levels[0]);
  line_meter[1]->setPeakBar(levels[1]);
}


void RDSlotBox::setCart(RDLogLine *logline)
{
  QString cutname;
  QPalette p;

  line_logline=logline;

  RDCart *cart;
  RDCut *cut;

  line_type=logline->type();
  switch(line_type) {
  case RDLogLine::Cart:
    cart=new RDCart(logline->cartNumber());
    cut=new RDCut(QString().sprintf("%06u_%03u",logline->cartNumber(),
				    logline->cutNumber()));
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
      line_title_label->setText(tr("[CART NOT FOUND]"));
      switch(cart->type()) {
      case RDCart::Audio:
      case RDCart::Macro:
      case RDCart::All:
	line_icon_label->setPixmap(*line_playout_map);
	break;
      }
      SetColor(QColor(LABELBOX_MISSING_COLOR));
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
	line_icon_label->setPixmap(*line_playout_map);
	line_title_label->setText(logline->title());
	SetColor(QColor(LABELBOX_MISSING_COLOR));
      }
      else {
	line_cart_label->
	  setText(QString().sprintf("%06u",logline->cartNumber()));
	if(line_logline->evergreen()) {
	  SetColor(QColor(LABELBOX_EVERGREEN_COLOR));
	}
	else {
	  SetColor(QColor(LABELBOX_BACKGROUND_COLOR));
	}
	switch(cart->type()) {
	case RDCart::Audio:
	case RDCart::All:
	  line_icon_label->setPixmap(*line_playout_map);
	  break;
	  
	case RDCart::Macro:
	  line_icon_label->setPixmap(*line_macro_map);
	  break;
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
	if(line_logline->originUser().isEmpty()||
	   (!line_logline->originDateTime().isValid())) {
	  line_title_label->
	    setText(RDResolveNowNext(line_airplay_conf->titleTemplate(),
				     logline));
	}
	else {
	  line_title_label->setText(line_logline->title()+" -- "+
				    line_logline->originUser()+" "+
				    line_logline->originDateTime().
				    toString("M/d hh:mm"));
	}
	line_description_label->
	  setText(RDResolveNowNext(line_airplay_conf->descriptionTemplate(),
				   logline));
	line_artist_label->
	  setText(RDResolveNowNext(line_airplay_conf->artistTemplate(),
				   logline));
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
	    setText(RDResolveNowNext(line_airplay_conf->outcueTemplate(),
				     logline));
	  line_position_bar->show();
	  line_up_label->show();
	  line_down_label->show();
	}
	else {
	  SetColor(QColor(LABELBOX_MISSING_COLOR));
	  line_cut_label->clear();
	  line_outcue_label->setText(tr("[NO VALID CUT AVAILABLE]"));
	}
	line_title_label->show();
	line_artist_label->show();
      }
    }
    delete cart;
    delete cut;
    break;
    
  case RDLogLine::Macro:
    line_icon_label->setPixmap(*line_macro_map);
    line_position_bar->hide();
    line_up_label->hide();
    line_down_label->hide();
    cart=new RDCart(logline->cartNumber());
    cut=new RDCut(QString().sprintf("%06u_%03u",logline->cartNumber(),
				    logline->cutNumber()));
    if(!cart->exists()) {
      SetColor(QColor(LABELBOX_MISSING_COLOR));
    }
    else {
      SetColor(QColor(LABELBOX_BACKGROUND_COLOR));
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
    line_title_label->setText(cart->title());
    line_description_label->setText("");
    line_artist_label->setText(cart->artist());
    line_outcue_label->setText("");
    delete cart;
    delete cut;
    line_title_label->show();
    line_artist_label->show();
    break;

  default:
    break;
  }
}


void RDSlotBox::setMode(RDSlotOptions::Mode mode)
{
  line_mode=mode;
}


void RDSlotBox::setService(const QString &svcname)
{
  clear();
  line_title_label->setText("Service: "+svcname);
}


void RDSlotBox::setStatusLine(const QString &str)
{
  line_artist_label->setText(str);
}


void RDSlotBox::setTimer(int msecs)
{
  if(line_logline==NULL) {
    line_up_label->setText(RDGetTimeLength(0,true,true));
    line_down_label->setText(RDGetTimeLength(0,true,true));
    line_position_bar->setProgress(0);
  }
  else {
    line_up_label->setText(RDGetTimeLength(msecs,true,true));
    line_down_label->
     setText(RDGetTimeLength(line_logline->effectiveLength()-msecs,true,true));
    line_position_bar->setProgress(msecs);
  }
}


void RDSlotBox::clear()
{
  SetColor(QColor(LABELBOX_BACKGROUND_COLOR));
  line_cart_label->setText("");
  line_cut_label->setText("");
  line_group_label->setText("");
  line_title_label->setText("");
  line_description_label->setText("");
  line_artist_label->setText("");
  line_outcue_label->setText("");
  line_talktime_label->setText("");
  line_length_label->setText("");
  line_up_label->setText("");
  line_down_label->setText("");
  line_position_bar->reset();
  line_end_time=QTime();
  log_id=-1;
  line_logline=NULL;
  line_type=RDLogLine::Cart;
  line_up_label->hide();
  line_position_bar->hide();
  line_down_label->hide();
  line_icon_label->clear();
  line_allow_drags=false;
  setBarMode(RDSlotBox::Stopping);
}


void RDSlotBox::setBarMode(bool changed)
{
  if(changed) {
    line_position_bar->setPalette(line_changed_stop_palette);
  }
  else {
    line_position_bar->setPalette(line_unchanged_stop_palette);
  }
}


void RDSlotBox::setAllowDrags(bool state)
{
  line_allow_drags=state;
}


void RDSlotBox::mousePressEvent(QMouseEvent *e)
{
  QWidget::mousePressEvent(e);

  if((line_logline!=NULL)&&(line_mode==RDSlotOptions::CartDeckMode)&&
     line_allow_drags) {
    RDCartDrag *d=new RDCartDrag(line_logline->cartNumber(),
				 line_icon_label->pixmap(),this);
    d->dragCopy();
  }
}


void RDSlotBox::mouseDoubleClickEvent(QMouseEvent *e)
{
  emit doubleClicked();
  QWidget::mouseDoubleClickEvent(e);
}


void RDSlotBox::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);
  p->drawRect(0,0,sizeHint().width()-1,sizeHint().height()-1);
  p->fillRect(1,1,sizeHint().width()-3,sizeHint().height()-3,
	      backgroundColor());
  p->end();
  delete p;
}


void RDSlotBox::dragEnterEvent(QDragEnterEvent *e)
{
  e->accept(RDCartDrag::canDecode(e)&&
	    (line_mode==RDSlotOptions::CartDeckMode)&&
	    (line_deck->state()==RDPlayDeck::Stopped));
}


void RDSlotBox::dropEvent(QDropEvent *e)
{
  unsigned cartnum;

  if(RDCartDrag::decode(e,&cartnum)) {
    emit cartDropped(cartnum);
  }
}


void RDSlotBox::SetColor(QColor color)
{
  setBackgroundColor(color);
  line_cart_label->setBackgroundColor(color);
  line_cut_label->setBackgroundColor(color);
  line_group_label->setBackgroundColor(color);
  line_title_label->setBackgroundColor(color);
  line_description_label->setBackgroundColor(color);
  line_artist_label->setBackgroundColor(color);
  line_outcue_label->setBackgroundColor(color);
  line_length_label->setBackgroundColor(color);
  line_talktime_label->setBackgroundColor(color);
  line_up_label->setBackgroundColor(color);
  line_position_bar->setBackgroundColor(QColor(Qt::lightGray));
  line_down_label->setBackgroundColor(color);
  line_icon_label->setBackgroundColor(color);
}
