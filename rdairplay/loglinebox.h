// loglinebox.h
//
// On Air Playout Utility for Rivendell.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: loglinebox.h,v 1.47.8.6 2014/02/06 20:43:51 cvs Exp $
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


#ifndef LOGLINEBOX_H
#define LOGLINEBOX_H

#include <qwidget.h>
#include <qsize.h>
#include <qsizepolicy.h>
#include <qtimer.h>
#include <qdatetime.h>
#include <qpushbutton.h>
#include <qprogressbar.h>
#include <qpalette.h>
#include <qpixmap.h>

#include <rdlabel.h>
#include <rdlog_line.h>
#include <rdlog_event.h>
#include <rdplay_deck.h>
#include <rdairplay_conf.h>
#include <rdcartdrag.h>

#include <mode_display.h>
#include <colors.h>

//
// Widget Settings
//
#define LOGLINEBOX_FULL_HEIGHT 85
#define LOGLINEBOX_HALF_HEIGHT 50

class LogLineBox : public QWidget
{
  Q_OBJECT
 public:
  enum Mode {Full=0,Half=1};
  enum BarMode {Transitioning=0,Stopping=1};
  LogLineBox(RDAirPlayConf *conf,QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  LogLineBox::Mode mode() const;
  int line() const;
  void setLine(int line);
  RDLogLine *logLine();
  void setMode(LogLineBox::Mode mode);
  void setEvent(int line,RDLogLine::TransType next_type,RDLogLine *logline);
  void setTimer(int msecs);
  void clear();
  void setBarMode(LogLineBox::BarMode);
  void setTimeMode(RDAirPlayConf::TimeMode);
  void setStatus(RDLogLine::Status status);
  void setAllowDrags(bool state);

 signals:
  void doubleClicked(int line);
  void cartDropped(int line,RDLogLine *ll);

 private slots:
  void countdownData();

 protected:
  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);
  void mouseDoubleClickEvent(QMouseEvent *e);
  void paintEvent(QPaintEvent *);
  void dragEnterEvent(QDragEnterEvent *e);
  void dropEvent(QDropEvent *e);

 private:
  void SetColor(QColor);
  void UpdateCountdown();
  void PrintTime();
  QString TimeString(const QTime &time);
  LogLineBox::Mode line_mode;
  QLabel *line_icon_label;
  QLabel *line_cart_label;
  QLabel *line_cut_label;
  QLabel *line_group_label;
  QLabel *line_trans_label;
  QLabel *line_title_label;
  QLabel *line_description_label;
  QLabel *line_artist_label;
  QLabel *line_outcue_label;
  QLabel *line_time_label;
  QLabel *line_length_label;
  QLabel *line_talktime_label;
  QLabel *line_up_label;
  QLabel *line_down_label;
  RDLabel *line_comment_label;
  QProgressBar *line_position_bar;
  QTimer *line_countdown_timer;
  QTime line_end_time;
  int log_id;
  int log_line;
  RDLogLine *line_logline;
  RDLogLine::TransType line_transition;
  QFont line_font;
  QFont talk_font;
  QFont line_bold_font;
  QPalette line_unchanged_stop_palette;
  QPalette line_unchanged_play_palette;
  QPalette line_changed_stop_palette;
  QPalette line_changed_play_palette;
  QPalette line_time_palette;
  QPalette line_hard_palette;
  QPalette line_timescale_palette;
  QPalette line_transition_palette;
  RDLogLine::TransType line_next_type;
  RDAirPlayConf::TimeMode line_time_mode;
  RDLogLine::Type line_type;
  QPixmap *line_playout_map;
  QPixmap *line_macro_map;
  QPixmap *line_chain_map;
  QPixmap *line_track_cart_map;
  QPixmap *line_mic16_map;
  QPixmap *line_notemarker_map;
  QPixmap *line_traffic_map;
  QPixmap *line_music_map;
  int line_move_count;
  RDLogLine::Status line_status;
  QString line_title_template;
  QString line_artist_template;
  QString line_outcue_template;
  QString line_description_template;
  bool line_allow_drags;
};


#endif 
