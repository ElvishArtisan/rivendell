// rdmonitor_config.h
//
// A container class for a Rivendell RDMonitor Configuration
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdmonitor_config.h,v 1.1.2.2 2013/11/08 03:57:14 cvs Exp $
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

#ifndef RDMONITOR_CONFIG_H
#define RDMONITOR_CONFIG_H

#include <qstring.h>

class RDMonitorConfig
{
 public:
  enum Position {UpperLeft=0,UpperCenter=1,UpperRight=2,
		 LowerLeft=3,LowerCenter=4,LowerRight=5,
		 LastPosition=6};
  RDMonitorConfig();
  int screenNumber() const;
  void setScreenNumber(int screen);
  Position position() const;
  void setPosition(RDMonitorConfig::Position pos);
  int xOffset() const;
  void setXOffset(int offset);
  int yOffset() const;
  void setYOffset(int offset);
  bool load();
  bool save();
  void clear();
  static QString positionText(Position pos);

 private:
  int mon_screen_number;
  int mon_x_offset;
  int mon_y_offset;
  Position mon_position;
  QString mon_filename;
};


#endif  // RDMONITOR_CONFIG_H
