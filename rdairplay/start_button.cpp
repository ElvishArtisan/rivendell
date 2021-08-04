// start_button.cpp
//
// The Start Button for RDAirPlay Rivendell
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

#include <QPainter>

#include "colors.h"
#include "start_button.h"

StartButton::StartButton(bool allow_pause,QWidget *parent)
  : RDPushButton(parent)
{
  start_time=QTime();
  start_allow_pause=allow_pause;

  setFocusPolicy(Qt::NoFocus);

  //
  // Create Palettes
  //
  start_stop_color=
    QPalette(QColor(BUTTON_STOPPED_BACKGROUND_COLOR),
	     palette().color(QPalette::Background));
  start_play_color=
    QPalette(QColor(BUTTON_PLAY_BACKGROUND_COLOR),
	     palette().color(QPalette::Background));
  start_play_color.
    setColor(QPalette::ButtonText,QColor(BUTTON_PLAY_TEXT_COLOR));
  start_pause_color=
    QPalette(QColor(BUTTON_PAUSE_BACKGROUND_COLOR),
	     palette().color(QPalette::Background));
  start_pause_color.
    setColor(QPalette::ButtonText,QColor(BUTTON_PAUSE_TEXT_COLOR));

  start_from_color=QPalette(QColor(BUTTON_FROM_BACKGROUND_COLOR),
			    palette().color(QPalette::Background));
  start_from_color.
    setColor(QPalette::ButtonText,QColor(BUTTON_FROM_TEXT_COLOR));

  start_to_color=QPalette(QColor(BUTTON_TO_BACKGROUND_COLOR),
			       palette().color(QPalette::Background));
  start_to_color.
    setColor(QPalette::ButtonText,QColor(BUTTON_TO_TEXT_COLOR));

  start_disabled_color=QPalette(QColor(BUTTON_DISABLED_BACKGROUND_COLOR),
				palette().color(QPalette::Background));
  start_disabled_color.
    setColor(QPalette::ButtonText,QColor(BUTTON_DISABLED_TEXT_COLOR));

  start_error_color=QPalette(QColor(BUTTON_ERROR_BACKGROUND_COLOR),
			     palette().color(QPalette::Background));
  start_error_color.
    setColor(QPalette::ButtonText,QColor(BUTTON_ERROR_TEXT_COLOR));

  start_mode=StartButton::Stop;
  setMode(StartButton::Disabled,RDCart::All);
}


void StartButton::setTime(const QTime &time)
{
  start_time=time;
  update();
}


void StartButton::setPort(const QString &port)
{
  start_port=port;
  update();
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
  update();
}


void StartButton::paintEvent(QPaintEvent *e)
{
  int w=size().width()-2;
  int h=size().width()-2;
  QPixmap *pix=new QPixmap(w,h);
  QPainter *p=new QPainter();
  p->begin(pix);
  p->fillRect(0,0,w,h,palette().color(QPalette::Active,QPalette::Button));
  if(start_mode!=StartButton::Disabled) {
    p->setPen(QColor(Qt::color1));
    p->setFont(labelFont());
    p->drawText((w-p->fontMetrics().width(start_title))/2,
		22,start_title);
    p->drawLine(10,24,70,24);
    if(!start_time.isNull()) {
      p->drawText((w-p->fontMetrics().width(rda->timeString(start_time)))/2,40,
		  rda->timeString(start_time));
    }
    else {
      p->drawText((w-p->fontMetrics().width("--:--:--"))/2,
		  40,"--:--:--");
    }
    p->setFont(bigLabelFont());
    p->drawText((w-p->fontMetrics().width(start_port))/2,70,start_port);
  }
  p->end();
  delete p;

  p=new QPainter(this);
  p->drawPixmap(1,1,w,w,*pix,0,0,pix->size().width(),pix->size().height());
  delete p;
  delete pix;
}
