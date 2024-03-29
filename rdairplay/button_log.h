// button_log.h
//
// The button play widget for RDAirPlay.
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

#ifndef BUTTON_LOG_H
#define BUTTON_LOG_H

#include <rdwidget.h>

#include "list_log.h"
#include "loglinebox.h"
#include "pie_counter.h"
#include "post_counter.h"
#include "start_button.h"
#include "stop_counter.h"

//
// Widget Settings
//
#define BUTTON_PLAY_BUTTONS 3
#define BUTTON_TOTAL_BUTTONS 12

class ButtonLog : public RDWidget
{
 Q_OBJECT
 public:
  ButtonLog(RDLogPlay *log,int id,RDAirPlayConf *conf,bool allow_pause=false,
	    QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  RDAirPlayConf::OpMode opMode() const;
  void setOpMode(RDAirPlayConf::OpMode mode);
  RDAirPlayConf::ActionMode actionMode() const;
  void setActionMode(RDAirPlayConf::ActionMode mode,int *cartnum=0);
  int slotQuantity() const;
  PieCounter *pieCounterWidget() const;
  PostCounter *postCounterWidget() const;
  StopCounter *stopCounterWidget() const;

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

 protected:
  void resizeEvent(QResizeEvent *e);
  void paintEvent(QPaintEvent *e);
  
 signals:
  void selectClicked(int id,int line,RDLogLine::Status);
  void cartDropped(int id,int line,RDLogLine *ll);
  void slotQuantityChanged(int slots);

 private:
  void UpdateEvents();
  void UpdateButtons();
  RDLogPlay *log_log;
  int log_id;
  int *log_cart;
  RDAirPlayConf::OpMode log_op_mode;
  RDAirPlayConf::ActionMode log_action_mode;
  LogLineBox *log_line_box[BUTTON_TOTAL_BUTTONS];
  StartButton *log_start_button[BUTTON_TOTAL_BUTTONS];
  int log_slot_quantity;
  int log_line_counter;
  EditEvent *log_event_edit;
  bool log_pause_enabled;
  PieCounter *log_pie_counter_widget;
  PostCounter *log_post_counter_widget;
  StopCounter *log_stop_counter_widget;
};


#endif  // BUTTON_LOG_H
