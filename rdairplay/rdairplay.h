// rdairplay.h
//
// The On Air Playout Utility for Rivendell.
//
//   (C) Copyright 2002-2020 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDAIRPLAY_H
#define RDAIRPLAY_H

#include <qsplashscreen.h>

#include <rdemptycart.h>
#include <rdhotkeylist.h>
#include <rdhotkeys.h>
#include <rdinstancelock.h>
#include <rdsound_panel.h>
#include <rdstereometer.h>
#include <rdwidget.h>

#include "button_log.h"
#include "colors.h"
#include "mode_display.h"
#include "post_counter.h"
#include "pie_counter.h"
#include "stop_counter.h"

//
// Debug Settings
//
//#define RESIZABLE

//
// Widget Settings
//
#define AIR_PLAY_SPLASH_TIME 5000
#define AIR_PLAY_BUTTONS 3
#define AIR_START_BUTTONS 4
#define AIR_TOTAL_BUTTONS 7
#define AIR_LOG_PORTS 2
#define AIR_PANEL_PORTS 1
#define AIR_PANEL_BUTTON_ROWS 5
#define AIR_PANEL_BUTTON_COLUMNS 5
#define AIR_TOTAL_PORTS 3
#define AIR_MESSAGE_FONT_QUANTITY 8
#define AIR_CHANNEL_LOCKOUT_INTERVAL 1000
#define METER_INTERVAL 20
#define MASTER_TIMER_INTERVAL 100
#define MESSAGE_WIDGET_WIDTH 410
#define RDAIRPLAY_USAGE "[OPTIONS]\n"

class MainWidget : public RDWidget
{
  Q_OBJECT
 public:
  MainWidget(RDConfig *config,QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void caeConnectedData(bool state);
  void ripcConnectedData(bool state);
  void rmlReceivedData(RDMacro *rml);
  void gpiStateChangedData(int matrix,int line,bool state);
  void logChannelStartedData(int id,int mport,int card,int port);
  void logChannelStoppedData(int id,int mport,int card,int port);
  void panelChannelStartedData(int mport,int card,int port);
  void panelChannelStoppedData(int mport,int card,int port);
  void logRenamedData(int log);
  void logReloadedData(int log);
  void userData();
  void addButtonData();
  void deleteButtonData();
  void moveButtonData();
  void copyButtonData();
  void fullLogButtonData(int);
  void panelButtonData();
  void modeButtonData();
  void selectClickedData(int id,int line,RDLogLine::Status status);
  void selectClickedData(unsigned cartnum,int row,int col);
  void cartDroppedData(int id,int line,RDLogLine *ll);
  void meterData();
  void masterTimerData();
  void transportChangedData();
  void timeModeData(RDAirPlayConf::TimeMode mode);
  void clearSplashData();
  void keyPressEvent(QKeyEvent *e);
  void keyReleaseEvent(QKeyEvent *e);
  void closeEvent(QCloseEvent *);
  void paintEvent(QPaintEvent *e);
  
 protected:
  void wheelEvent(QWheelEvent *e);

 private:
  void RunLocalMacros(RDMacro *rml);
  void StopEvent(int button_id);
  void SetCaption();
  void SetMode(int mach,RDAirPlayConf::OpMode mode);
  void SetManualMode(int mach);
  void SetAutoMode(int mach);
  void SetLiveAssistMode(int mach);
  void SetActionMode(StartButton::Mode mode);
  bool FirstPort(int index);
  bool GetPanel(QString str,RDAirPlayConf::PanelType *type,int *panel);
  QFont MessageFont(QString str);
  bool AssertChannelLock(int dir,int card,int port);
  bool AssertChannelLock(int dir,int achan);
  int AudioChannel(int card,int port) const;
  RDAirPlayConf::Channel PanelChannel(int mport) const;
  RDLogPlay *air_log[RDAIRPLAY_LOG_QUANTITY];
  RDSoundPanel *air_panel;
  PostCounter *air_post_counter;
  PieCounter *air_pie_counter;
  RDStereoMeter *air_stereo_meter;
  StopCounter *air_stop_counter;
  ModeDisplay *air_mode_display;
  RDPushButton *air_add_button;
  RDPushButton *air_delete_button;
  RDPushButton *air_move_button;
  RDPushButton *air_copy_button;
  QPushButton *air_log_button[RDAIRPLAY_LOG_QUANTITY];
  QPushButton *air_panel_button;
  ListLog *air_log_list[RDAIRPLAY_LOG_QUANTITY];
  ButtonLog *air_button_list;
  StartButton::Mode air_action_mode;
  QString air_logname;
  QTimer *air_master_timer;
  int air_line_counter;
  int air_segue_length;
  int air_next_button;
  RDAirPlayConf::OpModeStyle air_op_mode_style;
  RDAirPlayConf::OpMode air_op_mode[RDAIRPLAY_LOG_QUANTITY];
  bool air_start_next;
  int air_time_start_line;
  RDAirPlayConf::PieEndPoint air_pie_end;
  QPalette auto_color;
  QPalette manual_color;
  QPalette active_color;
  int air_add_cart;
  int air_copy_line;
  QString air_add_filter;
  QString air_add_group;
  QString air_add_schedcode;
  QLabel *air_message_label;
  int air_source_id;
  int air_meter_card[3];
  int air_meter_port[3];
  int air_cue_card;
  int air_cue_port;
  RDInstanceLock *air_lock;
  bool air_clear_filter;
  RDAirPlayConf::BarAction air_bar_action;
  bool air_pause_enabled;
  QPixmap *air_rivendell_map;
  QString air_start_logname[RDAIRPLAY_LOG_QUANTITY];
  int air_start_line[RDAIRPLAY_LOG_QUANTITY];
  bool air_start_start[RDAIRPLAY_LOG_QUANTITY];
  RDAirPlayConf::ExitCode rdairplay_previous_exit_code;
  QDateTime air_startup_datetime;
  QPixmap *air_refresh_pixmap;
  QString air_editor_cmd;
  QSplashScreen *air_splash_screen;
  int  keystrokecount;
  bool AltKeyHit ;
  bool CtrlKeyHit;
  QFont air_message_fonts[AIR_MESSAGE_FONT_QUANTITY];
  QFontMetrics *air_message_metrics[AIR_MESSAGE_FONT_QUANTITY];
  int air_audio_channels[RDAirPlayConf::LastChannel];
  int air_start_gpi_matrices[RDAirPlayConf::LastChannel];
  int air_start_gpi_lines[RDAirPlayConf::LastChannel];
  int air_start_gpo_matrices[RDAirPlayConf::LastChannel];
  int air_start_gpo_lines[RDAirPlayConf::LastChannel];
  int air_stop_gpi_matrices[RDAirPlayConf::LastChannel];
  int air_stop_gpi_lines[RDAirPlayConf::LastChannel];
  int air_stop_gpo_matrices[RDAirPlayConf::LastChannel];
  int air_stop_gpo_lines[RDAirPlayConf::LastChannel];
  RDAirPlayConf::GpioType air_channel_gpio_types[RDAirPlayConf::LastChannel];
  std::map<unsigned,QTimer *> air_channel_timers[2];
  RDEmptyCart *air_empty_cart;
};


#endif  // RDAIRPLAY_H
