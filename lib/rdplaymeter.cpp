// rdplaymeter.cpp
//
// This implements a widget that represents a stereo audio level meter,
// complete with labels and scale.
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdplaymeter.cpp,v 1.3 2010/07/29 19:32:33 cvs Exp $
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

#include <qwidget.h>
#include <qstring.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qcolor.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qsize.h>
#include <stdio.h>
#include <qslider.h>
#include <qbuttongroup.h>
#include <qsizepolicy.h>
#include <qmessagebox.h>

#include <rdplaymeter.h>

RDPlayMeter::RDPlayMeter(RDSegMeter::Orientation orient,QWidget *parent,const char *name)
  : QWidget(parent,name)
{
  meter_label=QString("");
  setBackgroundColor(black);
  orientation=orient;
  makeFont();
  meter=new RDSegMeter(orientation,this,"meter");
  meter->setSegmentSize(5);
  meter->setSegmentGap(1);
}


QSize RDPlayMeter::sizeHint() const
{
  if(meter_label==QString("")) {
    return QSize(335,60);
  }
  else {
    return QSize(335,80);
  }
}


QSizePolicy RDPlayMeter::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
}


void RDPlayMeter::setRange(int min,int max)
{
  meter->setRange(min,max);
}


void RDPlayMeter::setDarkLowColor(QColor color)
{
  meter->setDarkLowColor(color);
}


void RDPlayMeter::setDarkHighColor(QColor color)
{
  meter->setDarkHighColor(color);
}


void RDPlayMeter::setDarkClipColor(QColor color)
{
  meter->setDarkClipColor(color);
}


void RDPlayMeter::setLowColor(QColor color)
{
  meter->setLowColor(color);
}


void RDPlayMeter::setHighColor(QColor color)
{
  meter->setHighColor(color);
}


void RDPlayMeter::setClipColor(QColor color)
{
  meter->setClipColor(color);
}


void RDPlayMeter::setHighThreshold(int level)
{
  meter->setHighThreshold(level);
}


void RDPlayMeter::setClipThreshold(int level)
{
  meter->setClipThreshold(level);
}


void RDPlayMeter::setLabel(QString label)
{
  meter_label=label;
  makeFont();
  setGeometry(geometry().left(),geometry().top(),
	      geometry().width(),geometry().height());
}


void RDPlayMeter::setGeometry(int x,int y,int w,int h)
{
  QWidget::setGeometry(x,y,w,h);
  if(meter_label.isEmpty()) {
    meter->setGeometry(2,2,w-4,h-4);
  }
  else {
    switch(orientation) {
	case RDSegMeter::Left:
	  meter->setGeometry(2,2,w-4-h,h-4);
	  label_font=QFont("helvetica",height()-2,QFont::Bold);
	  label_font.setPixelSize(height()-2);
	  break;
	case RDSegMeter::Right:
	  meter->setGeometry(2+h,2,w-4-h,h-4);
	  label_font=QFont("helvetica",height()-2,QFont::Bold);
	  label_font.setPixelSize(height()-2);
	  break;
	case RDSegMeter::Up:
	  meter->setGeometry(2,2,w-4,h-4-w);
	  label_font=QFont("helvetica",width()-2,QFont::Bold);
	  label_font.setPixelSize(width()-2);
	  break;
	case RDSegMeter::Down:
	  meter->setGeometry(2,2+width(),w-4,h-4-w);
	  label_font=QFont("helvetica",width()-2,QFont::Bold);
	  label_font.setPixelSize(width()-2);
	  break;
    }
    makeFont();
  }
}


void RDPlayMeter::setGeometry(QRect &rect)
{
  setGeometry(rect.left(),rect.top(),rect.width(),rect.height());
}


void RDPlayMeter::setSolidBar(int level)
{
  meter->setSolidBar(level);
}


void RDPlayMeter::setPeakBar(int level)
{
  meter->setPeakBar(level);
}


void RDPlayMeter::setFloatingBar(int level)
{
  meter->setFloatingBar(level);
}


void RDPlayMeter::setSegmentSize(int size)
{
  meter->setSegmentSize(size);
}


void RDPlayMeter::setSegmentGap(int gap)
{
  meter->setSegmentGap(gap);
}


RDSegMeter::Mode RDPlayMeter::mode() const
{
  return meter->mode();
}


void RDPlayMeter::setMode(RDSegMeter::Mode mode)
{
  meter->setMode(mode);
}


void RDPlayMeter::paintEvent(QPaintEvent *paintEvent)
{
  //
  // Setup
  //
  QPainter *p=new QPainter(this);
  p->setFont(label_font);
  p->setPen(white);
  if(!meter_label.isEmpty()) {
    switch(orientation) {
	case RDSegMeter::Left:
	  p->drawText(width()-height()+meter_label_x,height()-2,meter_label);
	  break;
	case RDSegMeter::Right:
	  p->drawText(meter_label_x,height()-2,meter_label);
	  break;
	case RDSegMeter::Up:
	  p->drawText(meter_label_x,height()-3,meter_label);
	  break;
	case RDSegMeter::Down:
	  p->drawText(meter_label_x,width()-1,meter_label);
	  break;
    }
  }
  p->end();
}



void RDPlayMeter::makeFont()
{
  switch(orientation) {
      case RDSegMeter::Left:
	label_font=QFont("helvetica",height()-2,QFont::Bold);
	label_font.setPixelSize(height()-2);
	meter_label_x=(height()-QFontMetrics(label_font).
		       width(meter_label))/2;
	break;
      case RDSegMeter::Right:
	label_font=QFont("helvetica",height()-2,QFont::Bold);
	label_font.setPixelSize(height()-2);
	meter_label_x=(height()-QFontMetrics(label_font).
		       width(meter_label))/2;
	break;
      case RDSegMeter::Up:
	label_font=QFont("helvetica",width()-2,QFont::Bold);
	label_font.setPixelSize(width()-2);
	meter_label_x=(width()-QFontMetrics(label_font).
		       width(meter_label))/2;
	break;
      case RDSegMeter::Down:
	label_font=QFont("helvetica",width()-2,QFont::Bold);
	label_font.setPixelSize(width()-2);
	meter_label_x=(width()-QFontMetrics(label_font).
		       width(meter_label))/2;
	break;
  }
}
