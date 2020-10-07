// rdlog_icons.cpp
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

#include "rdlog_icons.h"

//
// Icons
//
#include "../icons/play.xpm"
#include "../icons/rml5.xpm"
#include "../icons/chain.xpm"
#include "../icons/track_cart.xpm"
#include "../icons/notemarker.xpm"
#include "../icons/music.xpm"
#include "../icons/mic16.xpm"
#include "../icons/traffic.xpm"

RDLogIcons::RDLogIcons()
{
  //
  // Create Icons
  //
  log_type_icons[RDLogLine::Cart]=QPixmap(play_xpm);
  log_type_icons[RDLogLine::Macro]=QPixmap(rml5_xpm);
  log_type_icons[RDLogLine::Marker]=QPixmap(notemarker_xpm);
  log_type_icons[RDLogLine::Chain]=QPixmap(chain_xpm);
  log_type_icons[RDLogLine::Track]=QPixmap(mic16_xpm);
  log_type_icons[RDLogLine::MusicLink]=QPixmap(music_xpm);
  log_type_icons[RDLogLine::TrafficLink]=QPixmap(traffic_xpm);
  log_track_cart_icon=QPixmap(track_cart_xpm);
}


QPixmap RDLogIcons::typeIcon(RDLogLine::Type type,RDLogLine::Source src) const
{
  if((type==RDLogLine::Cart)&&(src==RDLogLine::Tracker)) {
    return log_track_cart_icon;
  }
  return log_type_icons.value(type);
}
