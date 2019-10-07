// rdmonitor.h
//
// System Monitor Applet for Rivendell
//
//   (C) Copyright 2012-2019 Fred Gleason <fredg@paravelsystems.com>
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


#ifndef RDMONITOR_H
#define RDMONITOR_H

#include <qtimer.h>
#include <QMouseEvent>

#include <rdconfig.h>
#include <rdmonitor_config.h>
#include <rdwidget.h>

#include "positiondialog.h"

#define RDSELECT_WIDTH 400
#define RDSELECT_HEIGHT 300

class MainWidget : public RDWidget
{
  Q_OBJECT
 public:
  MainWidget(RDConfig *c,QWidget *parent=0);
  QSizePolicy sizePolicy() const;

 private slots:
  void validate();
  void quitMainWidget();

 protected:
  void enterEvent(QEvent *e);
  void leaveEvent(QEvent *e);
  void mousePressEvent(QMouseEvent *e);
  void mouseDoubleClickEvent(QMouseEvent *e);
  void paintEvent(QPaintEvent *e);
  void resizeEvent(QResizeEvent *e);

 private:
  void SetSummaryState(bool state);
  void SetPosition();
  void SetToolTip(bool db_status,int schema,bool snd_status);
  void SetStatusPosition();
  QLabel *mon_name_label;
  QLabel *mon_green_label;
  QLabel *mon_red_label;
  QTimer *mon_validate_timer;
  QFontMetrics *mon_metrics;
  PositionDialog *mon_position_dialog;
  int mon_dialog_x;
  int mon_dialog_y;
  int mon_rdselect_x;
  int mon_rdselect_y;
  QLabel *mon_status_label;
  //  StatusTip *mon_tooltip;
  QDesktopWidget *mon_desktop_widget;
  RDMonitorConfig *mon_config;
  RDConfig *mon_rdconfig;
};


#endif  // RDMONITOR_H
