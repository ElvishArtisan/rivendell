// rdlog_icons.h
//
// Icons for Rivendell log events.
//
//   (C) Copyright 2020 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDLOG_ICONS_H
#define RDLOG_ICONS_H

#include <QMap>
#include <QPixmap>

#include <rdlog_line.h>

class RDLogIcons
{
 public:
  RDLogIcons();
  QPixmap typeIcon(RDLogLine::Type type,
		   RDLogLine::Source src=RDLogLine::Manual) const;

 private:
  QMap<RDLogLine::Type,QPixmap> log_type_icons;
  QPixmap log_track_cart_icon;
};


#endif  // RDLOG_ICONS
