//   rdstereometer.h
//
//   A Stereo Audio Meter Widget
//
//   (C) Copyright 2002 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdstereometer.h,v 1.4 2010/07/29 19:32:34 cvs Exp $
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

#ifndef RDSTEREOMETER_H
#define RDSTEREOMETER_H

#include <qwidget.h>
#include <qcolor.h>
#include <qpainter.h>
#include <qsize.h>
#include <qsizepolicy.h>
#include <qstring.h>
#include <rdsegmeter.h>

#define CLIP_LIGHT_COLOR red


class RDStereoMeter : public QWidget
{
 Q_OBJECT
 public:
  RDStereoMeter(QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  void setReference(int level);
  void setClipLight(int level);
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
  void setLabel(QString label);
  RDSegMeter::Mode mode() const;
  void setMode(RDSegMeter::Mode mode);

 public slots:
  void setLeftSolidBar(int level);
  void setRightSolidBar(int level);
  void setLeftFloatingBar(int level);
  void setRightFloatingBar(int level);
  void setLeftPeakBar(int level);
  void setRightPeakBar(int level);
  void resetClipLight();

 signals:
  void clip();

 protected:
  void paintEvent(QPaintEvent *);

 private:
  RDSegMeter *left_meter,*right_meter;
  int ref_level;
  int clip_light_level;
  bool clip_light_on;
  int label_x;
  QString meter_label;
  QFont meter_label_font;
  QFont meter_scale_font;
};


#endif  // RDSTEREOMETER_H
