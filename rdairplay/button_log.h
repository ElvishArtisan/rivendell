
// button_log.h
//
// The button play widget for RDAirPlay.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: button_log.h,v 1.17.6.2 2014/01/07 18:18:32 cvs Exp $
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

#ifndef BUTTON_LOG_H
#define BUTTON_LOG_H

#include <qwidget.h>
#include <qdatetime.h>
#include <qlabel.h>
#include <qpushbutton.h>

#include <rdlistview.h>
#include <rdlistviewitem.h>
#include <rdairplay_conf.h>

#include <rdairplay_conf.h>
#include <log_play.h>
#include <list_log.h>
#include <edit_event.h>
#include <start_button.h>
#include <loglinebox.h>
#include <edit_event.h>

//
// Widget Settings
//
#define BUTTON_PLAY_BUTTONS 3
#define BUTTON_TOTAL_BUTTONS 7


class ButtonLog : public QWidget
{
 Q_OBJECT
 public:
  ButtonLog(LogPlay *log,int id,RDAirPlayConf *conf,bool allow_pause=false,
	    QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  RDAirPlayConf::OpMode opMode() const;
  void setOpMode(RDAirPlayConf::OpMode mode);
  RDAirPlayConf::ActionMode actionMode() const;
  void setActionMode(RDAirPlayConf::ActionMode mode,int *cartnum=0);
  void setTimeMode(RDAirPlayConf::TimeMode mode);

 public slots:
  void startButton(int);
  void stopButtonHotkey(int);
  void pauseButtonHotkey(int);

 private slots:
  void transportChangedData();
  void modifiedData(int line);
  void boxDoubleClickedData(int line);
  void playedData(int line);
  void stoppedData(int line);
  void pausedData(int line);
  void positionData(int line,int point);
  void cartDroppedData(int line,RDLogLine *ll);

 signals:
  void selectClicked(int id,int line,RDLogLine::Status);
  void cartDropped(int id,int line,RDLogLine *ll);

 private:
  void UpdateEvents();
  void UpdateButtons();
  LogPlay *log_log;
  int log_id;
  int *log_cart;
  RDAirPlayConf::OpMode log_op_mode;
  RDAirPlayConf::ActionMode log_action_mode;
  LogLineBox *log_line_box[BUTTON_TOTAL_BUTTONS];
  StartButton *log_start_button[BUTTON_TOTAL_BUTTONS];
  int log_line_counter;
  RDAirPlayConf::TimeMode log_time_mode;
  EditEvent *log_event_edit;
  bool log_pause_enabled;
};


#endif  // BUTTON_LOG_H
