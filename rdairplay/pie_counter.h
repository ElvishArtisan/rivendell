// pie_counter.h
//
// The pie counter widget for Rivendell
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: pie_counter.h,v 1.20 2010/07/29 19:32:36 cvs Exp $
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

#ifndef PIE_COUNTER_H
#define PIE_COUNTER_H

#include <qwidget.h>
#include <qdatetime.h>
#include <qlabel.h>
#include <qpixmap.h>

#include <rdlog_line.h>
#include <rdairplay_conf.h>

#include <mode_display.h>

//
// Inner pie timer
//
#define PIE_COUNTER_SIZE 100

//
// diameter of last seconds ring (and the bounding box for the whole thing)
//
#define PIE_COUNTER_BBOX 112

//
// Extra padding for the bounding box
//
#define PIE_X_PADDING 28
#define PIE_Y_PADDING 7


class PieCounter : public QWidget
{
 Q_OBJECT
 public:
  PieCounter(int count_length,QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  int line() const;
  void setLine(int line);
  void setTime(int msecs);
  void setCountLength(int msecs);
  void setTalkStart(int msecs);
  void setTalkEnd(int msecs);
  void resetTime();
  void setOpMode(RDAirPlayConf::OpMode opmode);
  void setTransType(RDLogLine::TransType trans);
  void start(int offset=0);
  void stop();
  void setLogline(RDLogLine *logline);
  void setErasePixmap(const QPixmap &pix);

 public slots:
  void setOnairFlag(bool state);
  void tickCounter();

 protected:
  void paintEvent(QPaintEvent *e);

 private:
  void SetPieColor();
  QLabel *pie_time_label;
  QLabel *pie_talk_label;
  int pie_time;
  int pie_talk_start;
  int pie_talk_end;
  int pie_length;
  int pie_count_length;
  bool pie_running;
  QColor pie_color;
  RDAirPlayConf::OpMode pie_op_mode;
  RDLogLine::TransType pie_trans_type;
  int air_line;
  QColor onair_off_color;
  RDLogLine *pie_logline;
  QPixmap pie_pixmap;
  bool pie_onair;
};

#endif
