// rdpanel.h
//
// A Dedicated Cart Wall Utility for Rivendell.
//
//   (C) Copyright 2002-2023 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDPANEL_H
#define RDPANEL_H

#include <rdemptycart.h>
#include <rdmainwindow.h>
#include <rdsound_panel.h>
#include <rdstereometer.h>
#include <rdwidget.h>

//
// Settings
//
#define MASTER_TIMER_INTERVAL 100
#define METER_INTERVAL 50
#define RDPANEL_USAGE "\n"

class MainWidget : public RDMainWindow
{
  Q_OBJECT
 public:
  MainWidget(RDConfig *c,QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void caeConnectedData(bool state);
  void userData();
  void masterTimerData();
  void meterData();
  void rmlReceivedData(RDMacro *rml);

 protected:
  void resizeEvent(QResizeEvent *e);
  void wheelEvent(QWheelEvent *e);
  void closeEvent(QCloseEvent *e);

 private:
  void RunLocalMacros(RDMacro *rml);
  void SetCaption();
  QSqlDatabase *panel_db;
  QTimer *panel_master_timer;
  RDStereoMeter *panel_stereo_meter;
  RDSoundPanel *panel_panel;
  RDEventPlayer *panel_player;
  bool meter_data_valid[RD_SOUNDPANEL_MAX_OUTPUTS];
  QString panel_filter;
  QString panel_group;
  QString panel_schedcode;
  RDEmptyCart *panel_empty_cart;
};


#endif  // RDPANEL_H
