// stop_counter.h
//
// The post counter widget for Rivendell
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdairplay_conf.h>
#include <rdpushbutton.h>

class StopCounter : public RDPushButton
{
 Q_OBJECT
 public:
  StopCounter(QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  void setState(bool state);
 
 public slots:
  void setTime(QTime time);
  void tickCounter();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  void UpdateTime();
  QTime stop_time;
  bool stop_running;
  bool old_stop_running;
  int old_msecs;
  QString stop_text;
};

#endif  // STOP_COUNTER_H
