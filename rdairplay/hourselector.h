// hourselector.h
//
// Hour Selector widget for RDAirPlay
//
//   (C) Copyright 2012-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdlogplay.h>
#include <rdwidget.h>

#define HOURSELECTOR_BUTTON_EDGE 41

class HourSelector : public RDWidget
{
 Q_OBJECT
 public:
  HourSelector(QWidget *parent=0);
  QSizePolicy sizePolicy() const;

 public slots:
  void updateHour(int hour,bool state);

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
