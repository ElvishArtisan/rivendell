// post_counter.cpp
//
// The post counter widget for Rivendell
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: post_counter.cpp,v 1.22.6.1 2012/11/26 20:19:40 cvs Exp $
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

#include <post_counter.h>
#include <colors.h>
#include <globals.h>

PostCounter::PostCounter(QWidget *parent,const char *name)
  : QPushButton(parent,name)
{
  post_running=false;
  post_time_format="hh:mm:ss";
  post_time=QTime();
  post_offset=0;
  post_offset_valid=false;

  //
  // Generate Fonts
  //
  post_small_font=QFont("Helvetica",12,QFont::Normal);
  post_small_font.setPixelSize(12);
  post_large_font=QFont("Helvetica",26,QFont::Normal);
  post_large_font.setPixelSize(26);

  //
  // Generate Palettes
  //
  post_idle_palette=palette();
  post_early_palette=
    QPalette(QColor(POSTPOINT_EARLY_COLOR),backgroundColor());
  post_ontime_palette=
    QPalette(QColor(POSTPOINT_ONTIME_COLOR),backgroundColor());
  post_late_palette=
    QPalette(QColor(POSTPOINT_LATE_COLOR),backgroundColor());

  post_offset = 0;
  UpdateDisplay();
}

void PostCounter::setTimeMode(RDAirPlayConf::TimeMode mode)
{
  switch(mode) {
      case RDAirPlayConf::TwentyFourHour:
	post_time_format="hh:mm:ss";
	break;

      case RDAirPlayConf::TwelveHour:
	post_time_format="h:mm:ss ap";
	break;
  }
  UpdateDisplay();
}

QSize PostCounter::sizeHint() const
{
  return QSize(200,60);
}


QSizePolicy PostCounter::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void PostCounter::setPostPoint(QTime point,int offset,bool offset_valid,
			       bool running)
{
  post_time=point;
  post_offset=offset;
  post_offset_valid=offset_valid;
  post_running=running;
  post_set_time=QTime::currentTime();
  UpdateDisplay();
}


void PostCounter::tickCounter()
{
  if(!post_running) {
    UpdateDisplay();
  }
}


void PostCounter::setEnabled(bool state)
{
  QWidget::setEnabled(state);
  UpdateDisplay();
}


void PostCounter::setDisabled(bool state)
{
  setEnabled(!state);
}


void PostCounter::keyPressEvent(QKeyEvent *e)
{
  e->ignore();
}


void PostCounter::UpdateDisplay()
{
  QColor color=backgroundColor();
  QString str;
  QString point;
  QString state;
  QTime current_time=
    QTime::currentTime().addMSecs(rdstation_conf->timeOffset());
  int offset=post_offset;
  if(!post_running) {
    offset-=current_time.msecsTo(post_set_time);
  }

  if(isEnabled()&&(!post_time.isNull())) {
    str=QString(tr("Next Timed Start"));
    point=QString().sprintf("%s [%s]",(const char *)str,
			    (const char *)post_time.toString(post_time_format));
    if(post_offset_valid) {
      if(offset<-POST_COUNTER_MARGIN) {
	state=QString().sprintf("-%s",(const char *)
				QTime().addMSecs(-offset).toString());
	setPalette(post_early_palette);
	color=POSTPOINT_EARLY_COLOR;
      }
      else {
	if(offset>POST_COUNTER_MARGIN) {
	  state=QString().sprintf("+%s",(const char *)
				  QTime().addMSecs(offset).toString());
	  setPalette(post_late_palette);
	  color=POSTPOINT_LATE_COLOR;
	}
	else {
	  state=tr("On Time");
	  setPalette(post_ontime_palette);
	  color=POSTPOINT_ONTIME_COLOR;
	}
      }
    }
    else {
      state="--------";
      setPalette(post_idle_palette);
    }
  }
  else {     // No postpoint/disabled
    point=tr("Next Timed Start [--:--:--]");
    state="--------";
    setPalette(post_idle_palette);
  }
  QPixmap pix(sizeHint().width(),sizeHint().height());
  QPainter *p=new QPainter(&pix);
  p->fillRect(0,0,sizeHint().width(),sizeHint().height(),color);
  //  p->eraseRect(0,0,sizeHint().width(),sizeHint().height());
  p->setPen(color1);
  p->setFont(post_small_font);
  p->drawText((sizeHint().width()-p->
	       fontMetrics().width(point))/2,22,point);
  p->setFont(post_large_font);
  p->drawText((sizeHint().width()-p->
	       fontMetrics().width(state))/2,48,state);
  p->end();
  delete p;
  setPixmap(pix);    
}
