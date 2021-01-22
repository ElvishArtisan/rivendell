// rdiconengine.h
//
// Icon generator for Rivendell
//
//   (C) Copyright 2020-2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDICONENGINE_H
#define RDICONENGINE_H

#include <QMap>
#include <QPixmap>

#include <rdlog_line.h>
#include <rduser.h>

class RDIconEngine
{
 public:
  enum IconType {GreenCheck=0,RedX=1,GreenBall=2,RedBall=3,WhiteBall=4,
		 BlueBall=5};
  RDIconEngine();
  QPixmap listIcon(IconType type) const;
  QPixmap stationIcon() const;
  QPixmap typeIcon(RDLogLine::Type type,
		   RDLogLine::Source src=RDLogLine::Manual) const;
  QPixmap userIcon(RDUser::Type type) const;

 private:
  QList<QPixmap> d_list_icons;
  QPixmap d_station_icon;
  QMap<RDLogLine::Type,QPixmap> log_type_icons;
  QPixmap log_track_cart_icon;
  QMap<RDUser::Type,QPixmap> d_user_icons;
};


#endif  // RDICONENGINE
