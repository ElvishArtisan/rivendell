//   rdsegmeter.h
//
//   An audio meter display widget.
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdsegmeter.h,v 1.3 2010/07/29 19:32:33 cvs Exp $
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

#ifndef RDSEGMETER_H
#define RDSEGMETER_H

#include <qwidget.h>
#include <qcolor.h>
#include <qpainter.h>
#include <qsize.h>
#include <qsizepolicy.h>
#include <qtimer.h>

/*
 * Default Colors
 */
#define DEFAULT_LOW_COLOR green
#define DEFAULT_DARK_LOW_COLOR 0,80,0
#define DEFAULT_HIGH_COLOR yellow
#define DEFAULT_DARK_HIGH_COLOR 75,75,0
#define DEFAULT_CLIP_COLOR red
#define DEFAULT_DARK_CLIP_COLOR 85,0,0

/*
 * Global Settings
 */
#define PEAK_HOLD_TIME 750


class RDSegMeter : public QWidget
{
 Q_OBJECT
 public:
  enum Mode {Independent=0,Peak=1};
  enum Orientation {Left=0,Right=1,Up=2,Down=3};
  RDSegMeter(RDSegMeter::Orientation o,QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  void setRange(int min,int max);
  void setDarkLowColor(QColor color);
  void setDarkHighColor(QColor color);
  void setDarkClipColor(QColor color);
  void setLowColor(QColor color);
  void setHighColor(QColor color);
  void setClipColor(QColor color);
  void setHighThreshold(int level);
  void setClipThreshold(int level);
  void setSegmentSize(int size);
  void setSegmentGap(int gap);
  RDSegMeter::Mode mode() const;
  void setMode(RDSegMeter::Mode mode);

 public slots:
  void setSolidBar(int level);
  void setFloatingBar(int level);
  void setPeakBar(int level);

 protected:
  void paintEvent(QPaintEvent *);

 private slots:
  void peakData();

 private:
  RDSegMeter::Orientation orient;
  RDSegMeter::Mode seg_mode;
  QTimer *peak_timer;
  int range_min,range_max;
  QColor dark_low_color;
  QColor dark_high_color;
  QColor dark_clip_color;
  QColor low_color;
  QColor high_color;
  QColor clip_color;
  int high_threshold,clip_threshold;
  int solid_bar,floating_bar;
  int seg_size,seg_gap;
};


#endif  // RDSEGMETER_H
