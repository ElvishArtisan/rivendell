// rdairplay.h
//
// The On Air Playout Utility for Rivendell.
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdairplay.h,v 1.89.4.9 2014/02/10 20:45:14 cvs Exp $
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

#include <vector>
#include <map>

#include <qwidget.h>
#include <qsize.h>
#include <qsizepolicy.h>
#include <qsqldatabase.h>
#include <qtimer.h>
#include <qsignalmapper.h>
#include <qpalette.h>
#include <qsocketdevice.h>
#include <qpixmap.h>
#include <qsplashscreen.h>
#include <qfontmetrics.h>

#include <rdpushbutton.h>
#include <rdstereometer.h>
#include <rdlabel.h>
#include <rdinstancelock.h>
#include <rduser.h>
#include <rdripc.h>
#include <rdplay_deck.h>
#include <rdmacro.h>
#include <rdconfig.h>
#include <rd.h>
#include <rdttydevice.h>
#include <rdemptycart.h>

#include <log_play.h>
#include <loglinebox.h>
#include <post_counter.h>
#include <pie_counter.h>
#include <stop_counter.h>
#include <mode_display.h>
#include <start_button.h>
#include <list_log.h>
#include <button_log.h>
#include <colors.h>
#include <rdsound_panel.h>
#include <rdhotkeylist.h>
#include <rdhotkeys.h>

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
#define RDAIRPLAY_USAGE "[<log-spec>=[:<nextline>[+]]] [...]\n\nWhere <log-spec> refers to one of the three log machines\n('--log1', '--log2' or '--log3') and <nextline> to the line number\nto do a 'make next' to after the log is loaded (default = 0). If the\n'+' is appended, then the log is started after the 'make next'.\n\nExamples:\n rdairplay --log1=MyLog\n    Load 'MyLog' into the Main Log machine\n\n rdairplay --log2=MyLog:14+\n    Load 'MyLog' into the Aux Log 1 machine, 'Make Next'\n    to line 14 and then start the log.\n\n rdairplay --log1=YourLog --log2=MyLog\n    Load 'YourLog' into the Main Log machine and 'MyLog' into the\n    Aux Log 1 machine.\n"


class MainWidget : public QWidget
{
  Q_OBJECT
 public:
  MainWidget(QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void logLine(RDConfig::LogPriority prio,const QString &msg);
  void ripcConnected(bool state);
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
  void refreshStatusChangedData(bool active);
  void clearSplashData();
  void keyPressEvent(QKeyEvent *e);
  void keyReleaseEvent(QKeyEvent *e);
  void closeEvent(QCloseEvent *);
  void paintEvent(QPaintEvent *e);
  
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
  LogPlay *air_log[RDAIRPLAY_LOG_QUANTITY];
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
  RDLabel *air_message_label;
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
  RDLabel *air_refresh_label;
  QPixmap *air_refresh_pixmap;
  QString air_editor_cmd;
  QSocketDevice *air_nownext_socket;
  std::vector<RLMHost *> air_plugin_hosts;
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


#endif 
