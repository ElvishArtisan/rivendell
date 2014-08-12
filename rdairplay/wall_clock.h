// wall_clock.h
//
// The wall clock widget for Rivendell
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: wall_clock.h,v 1.14 2011/01/11 12:20:31 cvs Exp $
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

#ifndef WALL_CLOCK_H
#define WALL_CLOCK_H

#include <qdatetime.h>
#include <qtimer.h>
#include <qevent.h>

#include <qpushbutton.h>

#include <rdairplay_conf.h>

class WallClock : public QPushButton
{
  Q_OBJECT
 public:
  WallClock(QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  void setDateDisplay(bool state);
  void setTimeMode(RDAirPlayConf::TimeMode mode);
  void setCheckSyncEnabled(bool);

 private slots:
  void clickedData();

 public slots:
  void tickClock();

 signals:
  void timeModeChanged(RDAirPlayConf::TimeMode);

 protected:
  void flashButton(bool state);
  void keyPressEvent(QKeyEvent *e);

 private:
  QTime previous_time,current_time;
  QDate previous_date,current_date;
  QString time_string;
  RDAirPlayConf::TimeMode time_mode;
  RDAirPlayConf::TimeMode previous_time_mode;
  bool show_date;
  QFont time_font;
  QFont label_font;
  QFontMetrics *label_metrics;
  bool check_sync;
  int time_offset;
  bool flash_state;
};

#endif
