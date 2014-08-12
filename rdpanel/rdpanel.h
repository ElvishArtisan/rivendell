// rdpanel.h
//
// A Dedicated Cart Wall Utility for Rivendell.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdpanel.h,v 1.16.4.1 2013/12/30 21:11:59 cvs Exp $
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

#include <qwidget.h>
#include <qsqldatabase.h>
#include <qtimer.h>
#include <qpixmap.h>

#include <rdstereometer.h>
#include <rdcae.h>
#include <rdmacro.h>
#include <rdconfig.h>
#include <rdsound_panel.h>
#include <rdevent_player.h>
#include <rdemptycart.h>

//
// Settings
//
#define MASTER_TIMER_INTERVAL 100
#define METER_INTERVAL 50
#define RDPANEL_PANEL_BUTTON_ROWS 7
#define RDPANEL_PANEL_BUTTON_COLUMNS 9
#define RDPANEL_USAGE "\n"

class MainWidget : public QWidget
{
  Q_OBJECT
 public:
  MainWidget(QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void userData();
  void masterTimerData();
  void meterData();
  void rmlReceivedData(RDMacro *rml);

 protected:
  void closeEvent(QCloseEvent *e);

 private:
  void RunLocalMacros(RDMacro *rml);
  void SetCaption();
  RDConfig *panel_config;
  QSqlDatabase *panel_db;
  QTimer *panel_master_timer;
  RDCae *panel_cae;
  RDStereoMeter *panel_stereo_meter;
  RDSoundPanel *panel_panel;
  RDEventPlayer *panel_player;
  bool meter_data_valid[PANEL_MAX_OUTPUTS];
  QPixmap *lib_rivendell_map;
  QString panel_filter;
  QString panel_group;
  QString panel_schedcode;
  QPixmap *panel_skin_pixmap;
  RDEmptyCart *panel_empty_cart;
};


#endif  // RDPANEL_H
