// stop_counter.h
//
// The post counter widget for Rivendell
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: stop_counter.h,v 1.11 2010/07/29 19:32:36 cvs Exp $
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

#ifndef STOP_COUNTER_H
#define STOP_COUNTER_H

#include <qpushbutton.h>
#include <qdatetime.h>
#include <qlabel.h>
#include <rdairplay_conf.h>

class StopCounter : public QPushButton
{
 Q_OBJECT
 public:
  StopCounter(QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  void setState(bool state);
  void setTimeMode(RDAirPlayConf::TimeMode mode);
 
 public slots:
  void setTime(QTime time);
  void tickCounter();

 private:
  void UpdateTime();
  QTime stop_time;
  bool stop_running;
  bool old_stop_running;
  int old_msecs;
  QFont stop_time_font;
  QFont stop_text_font;
  QString stop_text;
  QString time_format;
};

#endif
