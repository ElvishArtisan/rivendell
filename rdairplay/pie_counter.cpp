// pie_counter.cpp
//
// The pie counter widget for Rivendell
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: pie_counter.cpp,v 1.39 2010/10/06 19:24:02 cvs Exp $
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

#include <qtimer.h>
#include <qpixmap.h>
#include <qpainter.h>

#include <rd.h>

#include <pie_counter.h>
#include <colors.h>

PieCounter::PieCounter(int count_length,QWidget *parent,const char *name)
  : QWidget(parent,name)
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
  pie_onair=false;
  const int ring = (PIE_COUNTER_BBOX-PIE_COUNTER_SIZE)/2; 
  QFont font=QFont("Helvetica",30,QFont::Bold);
  font.setPixelSize(30);
  pie_time_label=new QLabel(":00",this,"pie_time_label");
  pie_time_label->
    setGeometry(PIE_X_PADDING+ring+25,PIE_Y_PADDING+ring+32,48,36);
  pie_time_label->setFont(font);
  pie_time_label->setAlignment(AlignCenter);
  pie_time_label->hide();

  pie_talk_label=new QLabel(":00",this,"pie_talk_label");
  pie_talk_label->
    setGeometry(PIE_X_PADDING+ring+25,PIE_Y_PADDING+ring+32,48,36);
  QPalette pal=palette();
  pal.
    setColor(QPalette::Active,QColorGroup::Foreground,QColor(PIE_TALK_COLOR));
  pie_talk_label->setPalette(pal);
  pie_talk_label->setFont(font);
  pie_talk_label->setAlignment(AlignCenter);
  pie_talk_label->hide();
  onair_off_color=backgroundColor();
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
  repaint();
}

void PieCounter::setLogline(RDLogLine *logline)
{
  pie_logline=logline;
}


void PieCounter::setErasePixmap(const QPixmap &pix)
{
  pie_pixmap=pix;
  if(!pie_onair) {
    QWidget::setErasePixmap(pie_pixmap);
  }
}


void PieCounter::stop()
{
  pie_running=false;
  pie_time_label->hide();
  pie_talk_label->hide();
  repaint();
}


void PieCounter::setOnairFlag(bool state)
{
  if(state) {
    setBackgroundColor(PIE_ONAIR_COLOR);
    QWidget::setErasePixmap(QPixmap());
    pie_time_label->setBackgroundColor(PIE_ONAIR_COLOR);
    pie_talk_label->setBackgroundColor(PIE_ONAIR_COLOR);
  }
  else {
    setBackgroundColor(onair_off_color);
    if(!pie_pixmap.isNull()) {
      QWidget::setErasePixmap(pie_pixmap);
    }
    pie_time_label->setBackgroundColor(onair_off_color);
    pie_talk_label->setBackgroundColor(onair_off_color);
  }
  pie_onair=state;
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
      repaint(false);
    }
  }
}


void PieCounter::paintEvent(QPaintEvent *e)
{
  int pie_pos=pie_length-pie_time;
  static QPixmap pix=QPixmap(PIE_COUNTER_BBOX,PIE_COUNTER_BBOX);
  const int ring = (PIE_COUNTER_BBOX-PIE_COUNTER_SIZE)/2; 
  int angle=(int)(((double)pie_time*5760.0)/(double)pie_length);
  int talk_start=
    (int)(1440.0-5760.0*(double)pie_talk_start/(double)pie_length);
  int talk_angle=(int)
    (-5760.0*((double)pie_talk_end-(double)pie_talk_start)/(double)pie_length);
  QPainter *p=new QPainter(&pix,false);
  double ring_angle = ((pie_length < (pie_count_length) ? pie_length :  pie_count_length) - 
	  ((pie_time >  pie_count_length)  ? pie_count_length : pie_time));
  if (pie_count_length)
	  ring_angle *= 5760.0/(pie_length < pie_count_length ? pie_length : pie_count_length);
  else ring_angle = 0;
  if (pie_time > pie_count_length) 
	  ring_angle = 0;

  if(pie_onair||pie_pixmap.isNull()) {
    p->fillRect(0,0,PIE_COUNTER_BBOX,PIE_COUNTER_BBOX,backgroundColor());
  }
  else {
    p->drawPixmap(-PIE_X_PADDING,-PIE_Y_PADDING,pie_pixmap);
  }
  if((!pie_running)||(pie_length==0)||(angle>5760)) {
    p->setBrush (PIE_FINAL_BG_COLOR);
    p->setPen(PIE_FINAL_BG_COLOR);
    p->drawPie(0,0,PIE_COUNTER_BBOX,PIE_COUNTER_BBOX,0,5759);
    p->setBrush(PIE_COUNTER_COLOR);
    p->setPen(PIE_COUNTER_COLOR);
    p->drawPie(ring,ring,PIE_COUNTER_SIZE,PIE_COUNTER_SIZE,0,5759);
  }
  else { 
    // Clear the pixmap 	  
    // This is the outer ring 
    p->setBrush (PIE_FINAL_BG_COLOR);
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
      p->setBrush(backgroundColor());
      p->setPen(backgroundColor());
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
