// wall_clock.cpp
//
// A wall-clock widget with date.
//
//   (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
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

#include "colors.h"
#include "wall_clock.h"

WallClock::WallClock(QWidget *parent)
  : RDPushButton(parent)
{
  time_offset=rda->station()->timeOffset();
  previous_time=QTime::currentTime().addMSecs(time_offset);
  time_mode=RDAirPlayConf::TwentyFourHour;
  previous_time_mode = RDAirPlayConf::TwentyFourHour;
  show_date=true;
  check_sync=true;
  flash_state=false;

  //
  // Generate Fonts
  //
  label_metrics=new QFontMetrics(subLabelFont());

  connect(this,SIGNAL(clicked()),this,SLOT(clickedData()));
}


QSize WallClock::sizeHint() const
{
  return QSize(200,60);
}


QSizePolicy WallClock::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
}


void WallClock::setDateDisplay(bool state)
{
  show_date=state;
}


void WallClock::setTimeMode(RDAirPlayConf::TimeMode mode)
{
  if(mode==time_mode) {
    return;
  }
  time_mode=mode;
  emit timeModeChanged(time_mode);
}


void WallClock::setCheckSyncEnabled(bool state)
{
  check_sync=state;
}


void WallClock::clickedData()
{
  if(time_mode==RDAirPlayConf::TwelveHour) {
    setTimeMode(RDAirPlayConf::TwentyFourHour);
  }
  else {
    setTimeMode(RDAirPlayConf::TwelveHour);
  }
}


void WallClock::tickClock()
{
  static QString date;
  QString accum;
  QColor system_button_text_color = palette().active().buttonText();
  static QPixmap *pix=new QPixmap(sizeHint().width()-2,sizeHint().height()-2);
  static bool synced=true;

  if(check_sync) {
    if(RDTimeSynced()!=synced) {
      synced=RDTimeSynced();
    }
  }
  current_time=QTime::currentTime().addMSecs(time_offset);
  current_date=QDate::currentDate();
  if((current_time.second()==previous_time.second()) && (previous_time_mode == time_mode)) {
    return;
  }
  previous_time_mode = time_mode;
  previous_time=current_time;

  //
  // Clock Display
  //
  if(time_mode==RDAirPlayConf::TwelveHour) {
    accum=current_time.toString("h:mm:ss ap");
  }
  else {
    accum=current_time.toString("hh:mm:ss");
  }
  if(time_string==accum) {
    return;
  }
  time_string=accum;
  if(synced) {
    flash_state=false;
  }
  else {
    flash_state=!flash_state;
  }
  if(previous_date!=current_date) {
    previous_date=current_date;
    date=current_date.toString("dddd, MMMM d, yyyy");
  }
  QPainter p(pix);
  if(flash_state) {
    p.fillRect(0,0,width()-2,height()-2,BUTTON_TIME_SYNC_LOST_COLOR);
    p.setPen(Qt::color1);
  }
  else {
    p.fillRect(0,0,width()-2,height()-2,backgroundColor());
    p.setPen(QColor(system_button_text_color));
  }
  p.setFont(subLabelFont());
  p.drawText((size().width()-2-p.fontMetrics().width(date))/2,22,date);
  p.setFont(bannerFont());
  p.drawText((size().width()-2-p.fontMetrics().width(accum))/2,48,accum);
  p.end();
  setPixmap(*pix);
}


void WallClock::flashButton(bool state)
{
  flash_state=state;
  tickClock();
}


void WallClock::keyPressEvent(QKeyEvent *e)
{
  e->ignore();
}
