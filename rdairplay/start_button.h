// start_button.h
//
// The Start Button for RDAirPlay Rivendell
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

#ifndef START_BUTTON_H
#define START_BUTTON_H

#include <rdairplay_conf.h>
#include <rdpushbutton.h>

class StartButton : public RDPushButton
{
 Q_OBJECT
 public:
  enum Mode {Stop=0,Play=1,Pause=2,AddFrom=3,AddTo=4,DeleteFrom=5,
	     MoveFrom=6,MoveTo=7,CopyFrom=8,CopyTo=9,Disabled=10,Error=11};
  StartButton(bool allow_pause=false,QWidget *parent=0);
  void setTime(const QTime &time);
  void setPort(const QString &port);
  Mode mode() const;
  void setMode(Mode mode,RDCart::Type cart_type);
  void setTimeMode(RDAirPlayConf::TimeMode mode);

 protected:
  void paintEvent(QPaintEvent *e);

 private:
  StartButton::Mode start_mode;
  QString start_title;
  QString start_port;
  QPalette start_stop_color;
  QPalette start_play_color;
  QPalette start_pause_color;
  QPalette start_from_color;
  QPalette start_to_color;
  QPalette start_disabled_color;
  QPalette start_error_color;
  RDAirPlayConf::TimeMode start_time_mode;
  QTime start_time;
  bool start_allow_pause;
};

#endif  // START_BUTTON_H
