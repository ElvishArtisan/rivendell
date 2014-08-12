//   rdslider.h
//
//   An audio- and touchscreen-friendly slider widget.
//
//   (C) Copyright 2009 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdslider.h,v 1.4 2010/07/29 19:32:34 cvs Exp $
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

#ifndef RDSLIDER_H
#define RDSLIDER_H

#include <qwidget.h>
#include <qslider.h>
#include <qrangecontrol.h>
#include <qcolor.h>
#include <qpalette.h>
#include <qsize.h>
#include <qpixmap.h>

class RDSlider : public QWidget,public QRangeControl
{
  Q_OBJECT
 public:
  enum Orientation {Left=0,Right=1,Up=2,Down=3};
  RDSlider(QWidget *parent,const char *name);
  RDSlider(RDSlider::Orientation orient,QWidget *parent,const char *name);
  RDSlider(int minValue,int maxValue,int pageStep,int value,
	   RDSlider::Orientation orient,QWidget *parent,const char *name);
  RDSlider::Orientation orientation() const;
  void setOrientation(RDSlider::Orientation orient);
  void setTracking(bool enable);
  bool tracking() const;
  void setTickInterval(int i);
  void setTickmarks(QSlider::TickSetting s);
  void setMinValue(int min_value);
  void setMaxValue(int max_value);
  void setRange(int min_value,int max_value);
  void setLineStep(int step);
  void setPageStep(int step);
  void setKnobColor(QColorGroup group);
  void setKnobColor(QColor color);
  void setKnobSize(QSize size);
  void setKnobSize(int w,int h);

 signals:
  void valueChanged(int value);
  void sliderPressed();
  void sliderMoved(int value);
  void sliderReleased();

 public slots:
  void setGeometry(int x,int y,int w, int h);
  void setGeometry(QRect &rect);
  void setValue(int setting);
  void addStep();
  void subtractStep();

 private:
  void paintEvent(QPaintEvent *);
  void mouseMoveEvent(QMouseEvent *);
  void mousePressEvent(QMouseEvent *);
  void mouseReleaseEvent(QMouseEvent *);
  void init(RDSlider::Orientation orient);
  void calcKnob();
  void calcKnob(int x,int y,int w,int h);
  RDSlider::Orientation rdslider_orient;
  QColorGroup knob_color;
  QRect curr_knob;
  QRect prev_knob;
  QRect page_up;
  QRect page_down;
  QSize knob_size;
  QPixmap *knob_map;
  int base_x;
  int base_y;
  bool rdslider_moving;
  int page_step;
  int line_step;
  bool tracking_enabled;
  bool deferred_change;
  int tick_interval;
  QSlider::TickSetting tick_setting;
};


#endif  // RDSLIDER_H
