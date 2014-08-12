//   rdsegmeter.cpp
//
//   An audio meter display widget.
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdsegmeter.cpp,v 1.3 2010/07/29 19:32:33 cvs Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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
//

#include <qwidget.h>
#include <qstring.h>
#include <qcolor.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qsize.h>
#include <stdio.h>
#include <qslider.h>
#include <qbuttongroup.h>
#include <qsizepolicy.h>
#include <qmessagebox.h>

#include <rdsegmeter.h>

RDSegMeter::RDSegMeter(RDSegMeter::Orientation o,QWidget *parent,const char *name)
  : QWidget(parent,name)
{
  orient=o;
  setBackgroundColor(black);
  dark_low_color=QColor(DEFAULT_DARK_LOW_COLOR);
  dark_high_color=QColor(DEFAULT_DARK_HIGH_COLOR);
  dark_clip_color=QColor(DEFAULT_DARK_CLIP_COLOR);
  low_color=QColor(DEFAULT_LOW_COLOR);
  high_color=QColor(DEFAULT_HIGH_COLOR);
  clip_color=QColor(DEFAULT_CLIP_COLOR);
  high_threshold=-14;
  clip_threshold=0;
  seg_size=2;
  seg_gap=1;
  range_min=-3000;
  range_max=0;
  solid_bar=-10000;
  floating_bar=-10000;
  seg_mode=RDSegMeter::Independent;

  peak_timer=new QTimer(this,"peak_timer");
  connect(peak_timer,SIGNAL(timeout()),this,SLOT(peakData()));
}


QSize RDSegMeter::sizeHint() const
{
  return QSize(0,0);
}


QSizePolicy RDSegMeter::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void RDSegMeter::setRange(int min,int max)
{
  range_min=min;
  range_max=max;
  repaint(false);
}


void RDSegMeter::setDarkLowColor(QColor color)
{
  if(dark_low_color!=color) {
    dark_low_color=color;
    repaint(false);
  }
}


void RDSegMeter::setDarkHighColor(QColor color)
{
  if(dark_high_color!=color) {
    dark_high_color=color;
    repaint(false);
  }
}


void RDSegMeter::setDarkClipColor(QColor color)
{
  if(dark_clip_color!=color) {
    dark_clip_color=color;
    repaint(false);
  }
}


void RDSegMeter::setLowColor(QColor color)
{
  if(low_color!=color) {
    low_color=color;
    repaint(false);
  }
}


void RDSegMeter::setHighColor(QColor color)
{
  if(high_color!=color) {
    high_color=color;
    repaint(false);
  }
}


void RDSegMeter::setClipColor(QColor color)
{
  if(clip_color!=color) {
    clip_color=color;
    repaint(false);
  }
}


void RDSegMeter::setHighThreshold(int level)
{
  if(high_threshold!=level) {
    high_threshold=level;
    repaint(false);
  }
}


void RDSegMeter::setClipThreshold(int level)
{
  if(clip_threshold!=level) {
    clip_threshold=level;
    repaint(false);
  }
}


RDSegMeter::Mode RDSegMeter::mode() const
{
  return seg_mode;
}


void RDSegMeter::setMode(RDSegMeter::Mode mode)
{
  seg_mode=mode;
  switch(seg_mode) {
      case RDSegMeter::Independent:
	if(peak_timer->isActive()) {
	  peak_timer->stop();
	}
	break;
      case RDSegMeter::Peak:
	if(!peak_timer->isActive()) {
	  peak_timer->start(PEAK_HOLD_TIME);
	}
	break;
  }
}


void RDSegMeter::setSolidBar(int level)
{
  if((seg_mode==RDSegMeter::Independent)&&(solid_bar!=level)) {
    solid_bar=level;
    repaint(false);
  }
}


void RDSegMeter::setFloatingBar(int level)
{
  if((seg_mode==RDSegMeter::Independent)&&(solid_bar!=level)) {
    floating_bar=level;
    repaint(false);
  }
}


void RDSegMeter::setPeakBar(int level)
{
  if((seg_mode==RDSegMeter::Peak)&&(solid_bar!=level)) {
    solid_bar=level;
    if(solid_bar>floating_bar) {
      floating_bar=solid_bar;
    }

    if(solid_bar<range_min) {
      floating_bar=solid_bar;
    }
    repaint(false);
  }
}


void RDSegMeter::setSegmentSize(int size)
{
  if(seg_size!=size) {
    seg_size=size;
    repaint(false);
  }
}


void RDSegMeter::setSegmentGap(int gap)
{
  if(seg_gap!=gap) {
    seg_gap=gap;
    repaint(false);
  }
}


void RDSegMeter::paintEvent(QPaintEvent *paintEvent)
{
  int op_pt;
  int low_region,high_region,clip_region,float_region;
  int dark_low_region=0;
  int dark_high_region=0;
  int dark_clip_region=0;
  QColor float_color;

  //
  // Setup
  //
  QPixmap pix(this->size());
  pix.fill(this,0,0);

  int seg_total=seg_size+seg_gap;
  QPainter *p=new QPainter(&pix);
  low_region=0;
  high_region=0;
  clip_region=0;
  p->setBrush(low_color);
  p->setPen(low_color);

  //
  // Set Orientation
  //
  switch(orient) {
      case RDSegMeter::Left:
      case RDSegMeter::Up:
	p->translate(width(),height());
	p->rotate(180);
	break;

      default:
	break;
  }

  // 
  // The low range
  //
  if(solid_bar>high_threshold) {
    op_pt=high_threshold;
  }
  else {
    op_pt=solid_bar;
  }
  switch(orient) {
      case RDSegMeter::Left:
      case RDSegMeter::Right:
	low_region=(int)((double)(op_pt-range_min)/
			 (double)(range_max-range_min)*width()/seg_total);
	if(op_pt>range_min) {
	  for(int i=0;i<low_region;i++) {
	    p->fillRect(i*seg_total,0,seg_size,height(),low_color);
	  }
	}
	break;
      case RDSegMeter::Down:
      case RDSegMeter::Up:
	low_region=(int)((double)(op_pt-range_min)/
			 (double)(range_max-range_min)*height()/seg_total);
	if(op_pt>range_min) {
	  for(int i=0;i<low_region;i++) {
	    p->fillRect(0,i*seg_total,width(),seg_size,low_color);
	  }
	}
	break;
  }

  // 
  // The high range
  //
  if(solid_bar>clip_threshold) {
    op_pt=clip_threshold;
  }
  else {
    op_pt=solid_bar;
  }
  switch(orient) {
      case RDSegMeter::Left:
      case RDSegMeter::Right:
	high_region=(int)((double)(op_pt-high_threshold)/
			  (double)(range_max-range_min)*width()/seg_total);
	if(op_pt>high_threshold) {
	  for(int i=low_region;i<low_region+high_region;i++) {
	    p->fillRect(i*seg_total,0,seg_size,height(),high_color);
	  }
	}
	break;
      case RDSegMeter::Down:
      case RDSegMeter::Up:
	high_region=(int)((double)(op_pt-high_threshold)/
			  (double)(range_max-range_min)*height()/seg_total);
	if(op_pt>high_threshold) {
	  for(int i=low_region;i<low_region+high_region;i++) {
	    p->fillRect(0,i*seg_total,width(),seg_size,high_color);
	  }
	}
	break;
  }

  // 
  // The clip range
  //
  if(solid_bar>range_max) {
    op_pt=range_max;
  }
  else {
    op_pt=solid_bar;
  }
  switch(orient) {
      case RDSegMeter::Left:
      case RDSegMeter::Right:
	clip_region=(int)((double)(op_pt-clip_threshold)/
			  (double)(range_max-range_min)*width()/seg_total);
	if(op_pt>clip_threshold) {
	  for(int i=low_region+high_region;
	      i<low_region+high_region+clip_region;i++) {
	    p->fillRect(i*seg_total,0,seg_size,height(),clip_color);
	  }
	}
	break;
      case RDSegMeter::Down:
      case RDSegMeter::Up:
	clip_region=(int)((double)(op_pt-range_min)/
			  (double)(range_max-range_min)*height()/seg_total);
	if(op_pt>clip_threshold) {
	  for(int i=low_region+high_region;
	      i<low_region+high_region+clip_region;i++) {
	    p->fillRect(0,i*seg_total,width(),seg_size,clip_color);
	  }
	}
	break;
  }

  // 
  // The dark low range
  //
  switch(orient) {
      case RDSegMeter::Left:
      case RDSegMeter::Right:
	dark_low_region=(int)((double)(high_threshold-range_min)/
			      (double)(range_max-range_min)*width()/seg_total);
	if(op_pt<high_threshold) {
	  for(int i=low_region;i<dark_low_region;i++) {
	    p->fillRect(i*seg_total,0,seg_size,height(),dark_low_color);
	  }
	}
	break;
      case RDSegMeter::Down:
      case RDSegMeter::Up:
	dark_low_region=(int)((double)(high_threshold-range_min)/
		      (double)(range_max-range_min)*height()/seg_total);
	if(op_pt<high_threshold) {
	  for(int i=low_region;i<dark_low_region;i++) {
	    p->fillRect(0,i*seg_total,width(),seg_size,dark_low_color);
	  }
	}
	break;
  }

  // 
  // The dark high range
  //
  if(solid_bar>=high_threshold) {
    op_pt=low_region+high_region;
  }
  else {
    op_pt=dark_low_region;
  }
  switch(orient) {
      case RDSegMeter::Left:
	case RDSegMeter::Right:
	  dark_high_region=(int)((double)(clip_threshold-range_min)/
			      (double)(range_max-range_min)*width()/seg_total);
	  if(solid_bar<clip_threshold) {
	    for(int i=op_pt;
		i<dark_high_region;i++) {
	      p->fillRect(i*seg_total,0,seg_size,height(),dark_high_color);
	    }
	  }
	  break;
      case RDSegMeter::Down:
      case RDSegMeter::Up:
	dark_high_region=(int)((double)(clip_threshold-range_min)/
		       (double)(range_max-range_min)*height()/seg_total);
	if(solid_bar<clip_threshold) {
	  for(int i=op_pt;i<dark_high_region;i++) {
	    p->fillRect(0,i*seg_total,width(),seg_size,dark_high_color);
	  }
	}
	break;
  }

  // 
  // The dark clip range
  //
  if(solid_bar>clip_threshold) {
    op_pt=low_region+high_region+clip_region;
  }
  else {
    op_pt=dark_high_region;
  }
  switch(orient) {
      case RDSegMeter::Left:
      case RDSegMeter::Right:
	dark_clip_region=(int)((double)(range_max-range_min)/
       		       (double)(range_max-range_min)*width()/seg_total);
	if(solid_bar<range_max) {
	  for(int i=op_pt;i<dark_clip_region;i++) {
	    p->fillRect(i*seg_total,0,seg_size,height(),dark_clip_color);
	  }
	}
	break;
      case RDSegMeter::Down:
      case RDSegMeter::Up:
	dark_clip_region=(int)((double)(range_max-range_min)/
			       (double)(range_max-range_min)*height()/seg_total);
	if(solid_bar<range_max) {
	  for(int i=op_pt;
	      i<dark_clip_region;i++) {
	    p->fillRect(0,i*seg_total,width(),seg_size,dark_clip_color);
	  }
	}
	break;
  }

  //
  // The floating segment
  //
  if(floating_bar>solid_bar) {
    if(floating_bar<=high_threshold) {
      float_color=low_color;
    }
    if((floating_bar>high_threshold)&&(floating_bar<=clip_threshold)) {
      float_color=high_color;
    }
    if(floating_bar>clip_threshold) {
      float_color=clip_color;
    }
    switch(orient) {
	case RDSegMeter::Left:
	case RDSegMeter::Right:
	  float_region=(int)((double)(floating_bar-range_min)/
			     (double)(range_max-range_min)*width());
	  float_region=seg_total*(float_region/seg_total);
	  p->fillRect(float_region,0,seg_size,height(),float_color); 
	  break;

	case RDSegMeter::Down:
	case RDSegMeter::Up:
	  float_region=(int)((double)(floating_bar-range_min)/
			     (double)(range_max-range_min)*height());
	  float_region=seg_total*(float_region/seg_total);
	  p->fillRect(0,float_region,width(),seg_size,float_color); 
	  break;
    }
  } 

  p->end();
  p->begin(this);
  p->drawPixmap(0,0,pix);
  p->end();
  delete p;
}


void RDSegMeter::peakData()
{
  floating_bar=solid_bar;
  repaint(false);
}



