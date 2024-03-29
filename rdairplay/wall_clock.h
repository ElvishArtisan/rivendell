// wall_clock.h
//
// The wall clock widget for Rivendell
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

#ifndef WALL_CLOCK_H
#define WALL_CLOCK_H

#include <rdairplay_conf.h>
#include <rdpushbutton.h>

class WallClock : public RDPushButton
{
  Q_OBJECT
 public:
  WallClock(QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  void setDateDisplay(bool state);
  void setCheckSyncEnabled(bool);

 public slots:
  void tickClock();

 protected:
  void flashButton(bool state);
  void keyPressEvent(QKeyEvent *e);
  void resizeEvent(QResizeEvent *e);

 private:
  QTime previous_time,current_time;
  QDate previous_date,current_date;
  QString time_string;
  bool show_date;
  QFont time_font;
  QFont label_font;
  bool check_sync;
  bool flash_state;
};

#endif  // WALL_CLOCK_H
