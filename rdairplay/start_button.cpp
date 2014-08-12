// start_button.cpp
//
// The Start Button for RDAirPlay Rivendell
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: start_button.cpp,v 1.27 2011/02/11 23:06:05 cvs Exp $
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

#include <qpixmap.h>
#include <qpainter.h>

#include <rd.h>

#include <start_button.h>
#include <colors.h>


StartButton::StartButton(bool allow_pause,QWidget *parent,const char *name)
  : QPushButton(parent,name)
{
  start_time_mode=RDAirPlayConf::TwentyFourHour;
  start_time=QTime();
  start_allow_pause=allow_pause;

  //
  // Create Fonts
  //
  start_label_font=QFont("Helvetica",12,QFont::Bold);
  start_label_font.setPixelSize(12);
  start_counter_font=QFont("Helvetica",10,QFont::Normal);
  start_counter_font.setPixelSize(10);
  start_port_font=QFont("Helvetica",20,QFont::Bold);
  start_port_font.setPixelSize(20);

  //
  // Create Palettes
  //
  start_stop_color=
    QPalette(QColor(BUTTON_STOPPED_BACKGROUND_COLOR),backgroundColor());
  start_play_color=
    QPalette(QColor(BUTTON_PLAY_BACKGROUND_COLOR),backgroundColor());
  start_play_color.
    setColor(QColorGroup::ButtonText,QColor(BUTTON_PLAY_TEXT_COLOR));
  start_pause_color=
    QPalette(QColor(BUTTON_PAUSE_BACKGROUND_COLOR),backgroundColor());
  start_pause_color.
    setColor(QColorGroup::ButtonText,QColor(BUTTON_PAUSE_TEXT_COLOR));

  start_from_color=
    QPalette(QColor(BUTTON_FROM_BACKGROUND_COLOR),backgroundColor());
  start_from_color.
    setColor(QColorGroup::ButtonText,QColor(BUTTON_FROM_TEXT_COLOR));

  start_to_color=
    QPalette(QColor(BUTTON_TO_BACKGROUND_COLOR),backgroundColor());
  start_to_color.
    setColor(QColorGroup::ButtonText,QColor(BUTTON_TO_TEXT_COLOR));

  start_disabled_color=
    QPalette(QColor(BUTTON_DISABLED_BACKGROUND_COLOR),backgroundColor());
  start_disabled_color.
    setColor(QColorGroup::ButtonText,QColor(BUTTON_DISABLED_TEXT_COLOR));

  start_error_color=
    QPalette(QColor(BUTTON_ERROR_BACKGROUND_COLOR),backgroundColor());
  start_error_color.
    setColor(QColorGroup::ButtonText,QColor(BUTTON_ERROR_TEXT_COLOR));

  start_mode=StartButton::Stop;
  setMode(StartButton::Disabled,RDCart::All);
}


void StartButton::setTime(QString str)
{
  start_time=QTime();
  Resize(geometry().x(),geometry().y(),geometry().width(),geometry().height());
}


void StartButton::setTime(QTime time)
{
  start_time=time;
  Resize(geometry().x(),geometry().y(),geometry().width(),geometry().height());
}


void StartButton::setPort(QString port)
{
  start_port=port;
  Resize(geometry().x(),geometry().y(),geometry().width(),geometry().height());
}


StartButton::Mode StartButton::mode() const
{
  return start_mode;
}


void StartButton::setMode(Mode mode,RDCart::Type cart_type)
{
  if(mode==start_mode) {
    return;
  }
  start_mode=mode;
  switch(mode) {
      case StartButton::Stop:
	setPalette(start_stop_color);
	start_title=STOP_MODE_TITLE;
	break;

      case StartButton::Play:
	setPalette(start_play_color);
	if(start_allow_pause&&(cart_type!=RDCart::Macro)) {
	  start_title=PLAY1_MODE_TITLE;
	}
	else {
	  start_title=PLAY0_MODE_TITLE;
	}
	break;

      case StartButton::Pause:
	setPalette(start_pause_color);
	start_title=PAUSE_MODE_TITLE;
	break;

      case StartButton::MoveFrom:
	setPalette(start_from_color);
	start_title=MOVE_FROM_MODE_TITLE;
	break;

      case StartButton::DeleteFrom:
	setPalette(start_from_color);
	start_title=DELETE_FROM_MODE_TITLE;
	break;

      case StartButton::AddFrom:
	break;

      case StartButton::AddTo:
	setPalette(start_to_color);
	start_title=ADD_TO_MODE_TITLE;
	break;

      case StartButton::MoveTo:
	setPalette(start_to_color);
	start_title=MOVE_TO_MODE_TITLE;
	break;

      case StartButton::CopyTo:
	setPalette(start_to_color);
	start_title=COPY_TO_MODE_TITLE;
	break;

      case StartButton::CopyFrom:
	setPalette(start_from_color);
	start_title=COPY_FROM_MODE_TITLE;
	break;

      case StartButton::Disabled:
	setPalette(start_disabled_color);
	start_title=DISABLED_MODE_TITLE;
	break;

      case StartButton::Error:
	setPalette(start_error_color);
	start_title=ERROR_MODE_TITLE;
	break;
  }
  Resize(geometry().x(),geometry().y(),geometry().width(),geometry().height());
}


void StartButton::setTimeMode(RDAirPlayConf::TimeMode mode)
{
  if(mode==start_time_mode) {
    return;
  }
  start_time_mode=mode;
  Resize(geometry().x(),geometry().y(),geometry().width(),geometry().height());
}


void StartButton::setGeometry(int x,int y,int w,int h)
{
  Resize(x,y,w,h);
  QPushButton::setGeometry(x,y,w,h);
}


void StartButton::setGeometry(QRect rect)
{
  setGeometry(rect.x(),rect.y(),rect.width(),rect.height());
}


void StartButton::Resize(int x,int y,int w,int h)
{
  QPixmap *pix=new QPixmap(w,h);
  QPainter *p=new QPainter();
  p->begin(pix);
  p->fillRect(0,0,w,h,palette().color(QPalette::Active,QColorGroup::Button));
  //p->eraseRect(0,0,w,h);
  if(start_mode!=StartButton::Disabled) {
    p->setPen(QColor(color1));
    p->setFont(start_label_font);
    p->drawText((geometry().width()-p->fontMetrics().width(start_title))/2,
		22,start_title);
    p->moveTo(10,24);
    p->lineTo(70,24);
    p->setFont(start_counter_font);
    if(!start_time.isNull()) {
      if(start_time_mode==RDAirPlayConf::TwentyFourHour) {
	p->drawText((geometry().width()-p->
		     fontMetrics().width(start_time.toString("hh:mm:ss")))/2,
		    40,start_time.toString("hh:mm:ss"));
      }
      else {
	p->drawText((geometry().width()-p->
		     fontMetrics().width(start_time.toString("h:mm:ss ap")))/2,
		    40,start_time.toString("h:mm:ss ap"));
      }
    }
    else {
      p->drawText((geometry().width()-p->fontMetrics().width("--:--:--"))/2,
		  40,"--:--:--");
    }
    p->setFont(start_port_font);
    p->drawText(15,70,start_port);
  }
  p->end();
  setPixmap(*pix);
  delete p;
  delete pix;
}
