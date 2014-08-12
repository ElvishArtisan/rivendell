// hourselector.h
//
// Hour Selector widget for RDAirPlay
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: hourselector.h,v 1.1.2.1 2012/11/13 23:45:13 cvs Exp $
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

#ifndef HOURSELECTOR_H
#define HOURSELECTOR_H

#include <qwidget.h>
#include <qpushbutton.h>
#include <qtimer.h>

#include <rdairplay_conf.h>
#include <rdlistview.h>
#include <rdlistviewitem.h>

#include <log_play.h>

class HourSelector : public QWidget
{
 Q_OBJECT
 public:
  HourSelector(QWidget *parent=0,const char *name=0);
  QSizePolicy sizePolicy() const;
  void setTimeMode(RDAirPlayConf::TimeMode mode);
  void updateHours(bool states[24]);

 signals:
  void hourSelected(int hour);

 protected:
  void resizeEvent(QResizeEvent *e);

 private slots:
  void hourClicked(int hour);
  void updateTimeData();

 private:
  QPushButton *hour_button[24];
  QPalette hour_active_palette;
  QTimer *hour_update_timer;
};


#endif  // HOURSELECTOR_H
