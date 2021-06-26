// pie_counter.cpp
//
// The pie counter widget for Rivendell
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

#include <rd.h>

#include "colors.h"
#include "pie_counter.h"

PieCounter::PieCounter(int count_length,QWidget *parent)
  : RDWidget(parent)
{
  pie_length=0;
  pie_time=0;
  pie_count_length=count_length;
  pie_running=false;
  pie_color=BUTTON_PLAY_BACKGROUND_COLOR;
  pie_talk_start=-1;
  pie_talk_end=-1;
  pie_op_mode=RDAirPlayConf::LiveAssist;
  pie_trans_type=RDLogLine::Stop;
  air_line=-1;
  const int ring=(PIE_COUNTER_BBOX-PIE_COUNTER_SIZE)/2; 

  onair_on_palette=palette();
  onair_on_palette.setColor(QPalette::Background,PIE_ONAIR_COLOR);
  onair_off_palette=palette();
  onair_on_palette.setColor(QPalette::Background,QColor("#D0D0D0"));
  setPalette(onair_on_palette);
  
  setAutoFillBackground(true);

  pie_time_label=new QLabel(":00",this);
  pie_time_label->
    setGeometry(PIE_X_PADDING+ring+25,PIE_Y_PADDING+ring+32,48,36);
  time_label_palette=palette();
  time_label_palette.setColor(QPalette::Background,Qt::lightGray);
  time_label_palette.setColor(QPalette::Foreground,PIE_FINAL_COLOR);
  pie_time_label->setPalette(time_label_palette);
  pie_time_label->setFont(bannerFont());
  pie_time_label->setAlignment(Qt::AlignCenter);
  pie_time_label->setAutoFillBackground(true);
  pie_time_label->hide();

  pie_talk_label=new QLabel(":00",this);
  pie_talk_label->
    setGeometry(PIE_X_PADDING+ring+25,PIE_Y_PADDING+ring+32,48,36);
  talk_label_palette=palette();
  talk_label_palette.setColor(QPalette::Background,Qt::lightGray);
  talk_label_palette.setColor(QPalette::Foreground,PIE_TALK_COLOR);
  pie_talk_label->setPalette(talk_label_palette);
  pie_talk_label->setFont(bannerFont());
  pie_talk_label->setAlignment(Qt::AlignCenter);
  pie_talk_label->setAutoFillBackground(true);
  pie_talk_label->hide();
  pie_logline=NULL;

  resetTime();
}


QSize PieCounter::sizeHint() const
{
  return QSize(2*PIE_X_PADDING+PIE_COUNTER_BBOX,
	       2*PIE_Y_PADDING+PIE_COUNTER_BBOX);
}


QSizePolicy PieCounter::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int PieCounter::line() const
{
  return air_line;
}


void PieCounter::setLine(int line)
{
  air_line=line;
}


void PieCounter::setTime(int msecs)
{
  stop();
  pie_length=msecs;
  pie_time=msecs;
  pie_time_label->setText(QString().sprintf(":%02d",pie_time/1000));
}


void PieCounter::setCountLength(int msecs)
{
  pie_count_length=msecs;
}


void PieCounter::setTalkStart(int msecs)
{
  pie_talk_start=msecs;
}


void PieCounter::setTalkEnd(int msecs)
{
  pie_talk_end=msecs;
}


void PieCounter::resetTime()
{
  stop();
  pie_time=pie_length;
  pie_length=1;
}


void PieCounter::setOpMode(RDAirPlayConf::OpMode opmode)
{
  if(opmode==pie_op_mode) {
    return;
  }
  pie_op_mode=opmode;
  SetPieColor();
}


void PieCounter::setTransType(RDLogLine::TransType trans)
{
  if(trans==pie_trans_type) {
    return;
  }
  pie_trans_type=trans;
  SetPieColor();
}


void PieCounter::start(int offset)
{
  pie_time=pie_length-offset;
  pie_running=true;
  update();
}

void PieCounter::setLogline(RDLogLine *logline)
{
  pie_logline=logline;
}


void PieCounter::stop()
{
  pie_running=false;
  pie_time_label->hide();
  pie_talk_label->hide();
  update();
}


void PieCounter::setOnairFlag(bool state)
{
  if(state) {
    setPalette(onair_on_palette);
  }
  else {
    setPalette(onair_off_palette);
  }
}


void PieCounter::tickCounter()
{
  int pie_pos=pie_length-pie_time;
  if(pie_running && (pie_time>0)) {
    if (pie_logline!=NULL) {
      pie_pos=pie_logline->playPosition();
      pie_time=pie_length-pie_pos;
    } else {
      pie_time-=100;
    }
    if(pie_time>pie_count_length) {
      pie_time_label->hide();
    }
    else {
      pie_time_label->show();
    }
    if((pie_pos<pie_talk_start)||(pie_pos>pie_talk_end)||
       (((750+pie_talk_end-pie_pos)/1000)>99)) {
      pie_talk_label->hide();
    }
    else {
      pie_talk_label->show();
      pie_talk_label->
	setText(QString().sprintf(":%02d",(750+pie_talk_end-pie_pos)/1000));
    }
    if(pie_time<=0) {
      stop();
    }
    else {
      pie_time_label->setText(QString().sprintf(":%02d",(1000+pie_time)/1000));
      update();
    }
  }
}


void PieCounter::paintEvent(QPaintEvent *e)
{
  int pie_pos=pie_length-pie_time;
  static QPixmap pix=QPixmap(PIE_COUNTER_BBOX,PIE_COUNTER_BBOX);
  pix.fill(Qt::transparent);
  const int ring = (PIE_COUNTER_BBOX-PIE_COUNTER_SIZE)/2; 
  int angle=(int)(((double)pie_time*5760.0)/(double)pie_length);
  int talk_start=
    (int)(1440.0-5760.0*(double)pie_talk_start/(double)pie_length);
  int talk_angle=(int)
    (-5760.0*((double)pie_talk_end-(double)pie_talk_start)/(double)pie_length);
  QPainter *p=new QPainter(&pix);
  p->setRenderHint(QPainter::Antialiasing,true);
  p->setRenderHint(QPainter::SmoothPixmapTransform,true);
  double ring_angle = ((pie_length < (pie_count_length) ? pie_length :  pie_count_length) - 
	  ((pie_time >  pie_count_length)  ? pie_count_length : pie_time));
  if (pie_count_length)
	  ring_angle *= 5760.0/(pie_length < pie_count_length ? pie_length : pie_count_length);
  else ring_angle = 0;
  if (pie_time > pie_count_length) 
	  ring_angle = 0;

  if((!pie_running)||(pie_length==0)||(angle>5760)) {
    p->setBrush(PIE_FINAL_BG_COLOR);
    p->setPen(PIE_FINAL_BG_COLOR);
    p->drawPie(0,0,PIE_COUNTER_BBOX,PIE_COUNTER_BBOX,0,5759);
    p->setBrush(PIE_COUNTER_COLOR);
    p->setPen(PIE_COUNTER_COLOR);
    p->drawPie(ring,ring,PIE_COUNTER_SIZE,PIE_COUNTER_SIZE,0,5759);
  }
  else { 
    // Clear the pixmap 	  
    // This is the outer ring 
    p->setBrush(PIE_FINAL_BG_COLOR);
    p->setPen(PIE_FINAL_BG_COLOR);
    p->drawPie(0,0,PIE_COUNTER_BBOX,PIE_COUNTER_BBOX,0,5759);
    p->setBrush(PIE_FINAL_COLOR);
    p->setPen(PIE_FINAL_COLOR);
    if (ring_angle > 0)
      p->drawPie(0,0,PIE_COUNTER_BBOX,PIE_COUNTER_BBOX,1440,
		(int)(ring_angle < 5760 ? (-1 *  ring_angle) : 0));

    // The background for the inner ring
    p->setBrush(PIE_COUNTER_COLOR);
    p->setPen(PIE_COUNTER_COLOR);
    p->drawPie(ring,ring,PIE_COUNTER_SIZE,PIE_COUNTER_SIZE,0,5759);
    // And this the inner
    p->setBrush(PIE_COUNTER_COLOR);
    p->setPen(PIE_COUNTER_COLOR);
    p->drawPie(ring,ring,PIE_COUNTER_SIZE,PIE_COUNTER_SIZE,1440,angle);
    p->setBrush(PIE_TALK_COLOR);
    if((pie_talk_start>=0)&&(pie_talk_end>=0)) {
      p->setPen(PIE_TALK_COLOR);
      p->drawPie(ring,ring,PIE_COUNTER_SIZE,PIE_COUNTER_SIZE,talk_start,talk_angle);
    }
    p->setBrush(pie_color);
    p->setPen(pie_color);
    p->drawPie(ring,ring,PIE_COUNTER_SIZE,PIE_COUNTER_SIZE,1440+angle,5760-angle);
    if(((pie_time<pie_count_length)&&(pie_time>0))||
       ((((750+pie_talk_end-pie_pos)/1000)<100)&&
	((pie_pos>=pie_talk_start)&&(pie_pos<=pie_talk_end)))) {
      p->setBrush(Qt::lightGray);
      p->setPen(Qt::lightGray);
      p->drawPie(ring+20,ring+20,PIE_COUNTER_SIZE-40,PIE_COUNTER_SIZE-40,0,5760);
    }
  }
  p->end();
  delete p;
  p=new QPainter(this);
  p->drawPixmap(PIE_X_PADDING,PIE_Y_PADDING,pix);
  p->end();
  delete p;
}


void PieCounter::SetPieColor()
{
  switch(pie_op_mode) {
      case RDAirPlayConf::Manual:
      case RDAirPlayConf::LiveAssist:
	pie_color=BUTTON_PLAY_BACKGROUND_COLOR;
	break;
	
      case RDAirPlayConf::Auto:
	switch(pie_trans_type) {
	    case RDLogLine::Play:
	    case RDLogLine::Segue:
	      pie_color=BUTTON_STOPPED_BACKGROUND_COLOR;
	      break;
	      
	    case RDLogLine::Stop:
	      pie_color=BUTTON_PLAY_BACKGROUND_COLOR;
	      break;

	    default:
	      break;
	}
	break;

      default:
	break;
  }
  update();
}
