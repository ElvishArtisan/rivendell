// mode_display.h
//
// The mode display widget for RDAirPlay in Rivendell
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: mode_display.h,v 1.12.6.1 2014/02/10 20:45:13 cvs Exp $
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

#ifndef MODE_DISPLAY_H
#define MODE_DISPLAY_H

#include <qpushbutton.h>
#include <qlabel.h>
#include <qpalette.h>
#include <qpixmap.h>

#include <rdairplay_conf.h>

//
// Widget Settings
//
#define MANUAL_LABEL "Manual"
#define AUTO_LABEL "Automatic"

class ModeDisplay : public QPushButton
{
 Q_OBJECT
 public:
  ModeDisplay(QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  void setOpMode(int mach,RDAirPlayConf::OpMode mode);
  void setOpModeStyle(RDAirPlayConf::OpModeStyle style);

 private:
  void WriteMap();
  RDAirPlayConf::OpMode mode_mode[RDAIRPLAY_LOG_QUANTITY];
  RDAirPlayConf::OpModeStyle mode_style;
  QPalette auto_color;
  QPalette manual_color;
  QPalette live_assist_color;
  QFont mode_large_font;
  QFont mode_small_font;
  QFont mode_tiny_font;
};

#endif
