// list_log.h
//
// The full log widget for RDAirPlay.
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

#ifndef LIST_LOG_H
#define LIST_LOG_H

#include <rdlogplay.h>
#include <rdwidget.h>

#include "edit_event.h"
#include "hourselector.h"
#include "list_logs.h"
#include "logtableview.h"

class ListLog : public RDWidget
{
 Q_OBJECT
 public:
  ListLog(RDLogPlay *log,int id,bool allow_pause=false,
	  QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  RDAirPlayConf::ActionMode actionMode() const;
  void setActionMode(RDAirPlayConf::ActionMode mode,int *cartnum=0);
  void setOpMode(RDAirPlayConf::OpMode mode);
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
  void doubleClickedData(const QModelIndex &index);
  void scrollButtonData();
  void refreshButtonData();
  void nextButtonData();
  void loadButtonData();
  void selectionChangedData(const QItemSelection &new_sel,
			    const QItemSelection &old_sel);
  void transportChangedData();
  void modelResetData();
  void refreshabilityChangedData(bool state);
  void cartDroppedData(int line,RDLogLine *ll);

 protected:
  void resizeEvent(QResizeEvent *e);
  void paintEvent(QPaintEvent *e);
  
 private:
  enum PlayButtonMode {ButtonDisabled=0,ButtonPlay=1,ButtonStop=2};
  int CurrentLine();
  RDLogLine::Status CurrentStatus();
  RDLogLine::State CurrentState();
  void ClearSelection();
  void ScrollTo(int line);
  void SetPlaybuttonMode(ListLog::PlayButtonMode mode);
  HourSelector *list_hour_selector;
  LogTableView *list_log_view;
  RDLogPlay *list_log;
  ListLogs *list_logs_dialog;
  RDAirPlayConf::ActionMode list_action_mode;
  ListLog::PlayButtonMode list_playbutton_mode;
  EditEvent *list_event_edit;
  QGroupBox *list_groupbox;
  QLabel *list_endtime_label;
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
  QPalette list_to_color;
  QPalette list_from_color;
  QPalette list_list_to_color;
  QPalette list_list_from_color;
  bool list_scroll;
  bool list_suspended_scroll;
  QPalette list_scroll_color[2];
  bool list_pause_allowed;
  bool list_audition_head_playing;
  bool list_audition_tail_playing;
};


#endif  // LIST_LOG_H
