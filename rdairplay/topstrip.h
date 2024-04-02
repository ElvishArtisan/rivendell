// topstrip.h
//
// Top row of indicator widgets for rdairplay(1)
//
//   (C) Copyright 2021-2024 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef TOPSTRIP_H
#define TOPSTRIP_H

#include <QLabel>
#include <QWebView>

#include <rdmeterstrip.h>
#include <rdwidget.h>

#include "messagewidget.h"
#include "mode_display.h"
#include "wall_clock.h"

class TopStrip : public RDWidget
{
  Q_OBJECT
 public:
  TopStrip(QWidget *parent=0);
  ~TopStrip();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  WallClock *wallClockWidget() const;
  ModeDisplay *modeDisplayWidget() const;
  RDMeterStrip *meterWidget();
  MessageWidget *messageWidget() const;
 public slots:
  void setOnairFlag(bool state);   

 protected:
  void resizeEvent(QResizeEvent *e);
  void paintEvent(QPaintEvent *e);
  
 private:
  //  QFont MessageFont(QString str) const;
  WallClock *d_wall_clock_widget;
  ModeDisplay *d_mode_display_widget;
  RDMeterStrip *d_meter_widget;
  MessageWidget *d_message_widget;
  QLabel *d_logo;
  bool d_onair_flag;
};

#endif  // TOPSTRIP_H
