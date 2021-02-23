// stop_counter.cpp
//
// The stop counter widget for Rivendell
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

#include "stop_counter.h"

StopCounter::StopCounter(QWidget *parent)
  : RDPushButton(parent)
{
  stop_running=false;

  time_format="hh:mm:ss";
  stop_text=tr("Next Stop [none]");  
  old_stop_running=true;
  old_msecs=0;
  setTime(QTime());
  setState(false);
}

void StopCounter::setTimeMode(RDAirPlayConf::TimeMode mode)
{
  switch(mode) {
  case RDAirPlayConf::TwentyFourHour:
    time_format="hh:mm:ss";
    break;

  case RDAirPlayConf::TwelveHour:
    time_format="h:mm:ss ap";
    break;
  }
  old_msecs = 0;
  if (stop_running) {
    setTime (stop_time);
  }
  UpdateTime();
}


QSize StopCounter::sizeHint() const
{
  return QSize(200,60);
}


QSizePolicy StopCounter::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void StopCounter::setState(bool state)
{
  QString str;

  if(state) {
    if(!stop_running) {
      UpdateTime();
      stop_running=true;
      str=QString(tr("Next Stop"));
      stop_text=tr("Next Stop")+" ["+stop_time.toString(time_format)+"]";
    }
  }
  else {
    if(stop_running) {
      stop_text=tr("Next Stop [none]");
      stop_running=false;
      UpdateTime();
    }
  }
}


void StopCounter::setTime(QTime time)
{
  QString str;

  if(!time.isNull()) {
    stop_time=time;
    setState(true);
    str=QString(tr("Next Stop"));
    stop_text=tr("Next Stop")+" ["+stop_time.toString(time_format)+"]";
  }
  else {
    setState(false);
  }
  UpdateTime();
}


void StopCounter::tickCounter()
{
  if(stop_running) {
    UpdateTime();
  }
}


void StopCounter::UpdateTime()
{
  QString text;
  QColor system_button_text_color = palette().buttonText().color();
  int msecs=QTime::currentTime().
    addMSecs(rda->station()->timeOffset()).msecsTo(stop_time);

  if((old_stop_running != stop_running) || (msecs/1000 != old_msecs/1000)){
    QPixmap pix(sizeHint().width()-2,sizeHint().height()-2);
    QPainter *p=new QPainter(&pix);
    old_stop_running = stop_running;
    old_msecs = msecs;
	  
    p->fillRect(0,0,sizeHint().width()-2,sizeHint().height()-2,
		palette().color(QPalette::Background));
    p->setPen(QColor(system_button_text_color));
    p->setFont(subLabelFont());
    p->drawText((sizeHint().width()-2-p->fontMetrics().width(stop_text))/2,22,
		stop_text);
    p->setFont(bannerFont());
    if (msecs < 0){
      /* HACK HACK HACK TODO */
      /* msecs is **PROBABLY** in the next day 
	 (we have a log crossing midnight - logs 
	 longer then 24 hours are NOT supported by this, 
	 please fix).... */
      msecs += 86400000; /* 1 day */
    }
    if(stop_running) {
      text=QTime(0,0,1).addMSecs(msecs).toString("hh:mm:ss");
      p->drawText((sizeHint().width()-2-p->fontMetrics().width(text))/2,49,text);
    }
    else {
      p->drawText((sizeHint().width()-2-p->
		   fontMetrics().width(tr("Stopped")))/2,49,
		  tr("Stopped"));
    }
    p->end();
    delete p;
    setIcon(pix);
  }
}
