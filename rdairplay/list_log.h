// list_log.h
//
// The full log list widget for RDAirPlay.
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_log.h,v 1.36.6.2 2013/12/28 00:00:33 cvs Exp $
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

#ifndef LIST_LOG_H
#define LIST_LOG_H

#include <qwidget.h>
#include <qdatetime.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qpushbutton.h>

#include <rdlistview.h>
#include <rdlistviewitem.h>
#include <rdairplay_conf.h>

#include <lib_listview.h>
#include <list_logs.h>
#include <log_play.h>
#include <edit_event.h>
#include <hourselector.h>

#define END_MARKER_ID -2

class ListLog : public QWidget
{
 Q_OBJECT
 public:
  ListLog(LogPlay *log,int id,bool allow_pause=false,
	  QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  void refresh();
  void refresh(int line);
  void setStatus(int line,RDLogLine::Status status);
  RDAirPlayConf::ActionMode actionMode() const;
  void setActionMode(RDAirPlayConf::ActionMode mode,int *cartnum=0);
  void setOpMode(RDAirPlayConf::OpMode mode);
  void setTimeMode(RDAirPlayConf::TimeMode mode);
  void userChanged(bool add_allowed,bool delete_allowed,
		   bool arrange_allowed,bool playout_allowed);

 signals:
  void selectClicked(int id,int line,RDLogLine::Status status);
  void cartDropped(int id,int line,RDLogLine *ll);

 private slots:
  void selectHour(int hour);
  void headButtonData();
  void tailButtonData();
  void auditionHeadData(int line);
  void auditionTailData(int line);
  void auditionStoppedData(int line);
  void takeButtonData();
  void playButtonData();
  void modifyButtonData();
  void doubleclickedData(QListViewItem *,const QPoint &,int);
  void scrollButtonData();
  void refreshButtonData();
  void nextButtonData();
  void loadButtonData();
  void logReloadedData();
  void logPlayedData(int line);
  void logPausedData(int line);
  void logStoppedData(int line);
  void logInsertedData(int line);
  void logRemovedData(int line,int num,bool moving);
  void selectionChangedData();
  void transportChangedData();
  void modifiedData(int line);
  void refreshabilityChangedData(bool state);
  void cartDroppedData(int line,RDLogLine *ll);

 protected:
  void paintEvent(QPaintEvent *e);

 private:
  enum PlayButtonMode {ButtonDisabled=0,ButtonPlay=1,ButtonStop=2};
  void RefreshList();
  void RefreshList(int line);
  void RefreshItem(RDListViewItem *l,int line);
  int CurrentLine();
  RDLogLine::Status CurrentStatus();
  RDLogLine::State CurrentState();
  void ClearSelection();
  RDListViewItem *GetItem(int line);
  void UpdateTimes(int removed_line=-1,int num_lines=0);
  void ScrollTo(int line);
  void UpdateColor(int line,bool next=false);
  void SetColor();
  void SetPlaybuttonMode(ListLog::PlayButtonMode mode);
  QString TimeString(const QTime &time) const;
  void UpdateHourSelector();
  int PredictedStartHour(RDListViewItem *item);
  HourSelector *list_hour_selector;
  LibListView *list_log_list;
  LogPlay *list_log;
  ListLogs *list_logs_dialog;
  RDAirPlayConf::ActionMode list_action_mode;
  ListLog::PlayButtonMode list_playbutton_mode;
  EditEvent *list_event_edit;
  QLineEdit *list_endtime_edit;
  QLabel *list_stoptime_label;
  QLineEdit *list_stoptime_edit;
  QPushButton *list_take_button;
  QPushButton *list_play_button;
  QPushButton *list_modify_button;
  QPushButton *list_scroll_button;
  QPushButton *list_next_button;
  QPushButton *list_refresh_button;
  QPushButton *list_load_button;
  QPushButton *list_head_button;
  QPushButton *list_tail_button;
  int *list_cart;
  int list_id;
  RDAirPlayConf::OpMode list_op_mode;
  RDAirPlayConf::TimeMode list_time_mode;
  QPalette list_to_color;
  QPalette list_from_color;
  QPalette list_list_to_color;
  QPalette list_list_from_color;
  bool list_scroll;
  bool list_suspended_scroll;
  QPalette list_scroll_color[2];
  QPixmap *list_playout_map;
  QPixmap *list_macro_map;
  QPixmap *list_chain_map;
  QPixmap *list_track_cart_map;
  QPixmap *list_mic16_map;
  QPixmap *list_notemarker_map;
  QPixmap *list_traffic_map;
  QPixmap *list_music_map;
  bool list_pause_allowed;
  bool list_audition_head_playing;
  bool list_audition_tail_playing;
};


#endif  // LIST_LOG_H
