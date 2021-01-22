// rdiconengine.cpp
//
// Icon generator for Rivendell
//
//   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
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

#include "rdiconengine.h"

//
// Icons
//
#include "../icons/greencheckmark.xpm"
#include "../icons/redx.xpm"
#include "../icons/blueball.xpm"
#include "../icons/greenball.xpm"
#include "../icons/redball.xpm"
#include "../icons/whiteball.xpm"

#include "../icons/host-16x16.xpm"

#include "../icons/play.xpm"
#include "../icons/rml5.xpm"
#include "../icons/chain.xpm"
#include "../icons/track_cart.xpm"
#include "../icons/notemarker.xpm"
#include "../icons/music.xpm"
#include "../icons/mic16.xpm"
#include "../icons/traffic.xpm"

#include "../icons/localuser.xpm"
#include "../icons/user.xpm"
#include "../icons/rss.xpm"
#include "../icons/admin.xpm"

RDIconEngine::RDIconEngine()
{
  d_list_icons.push_back(QPixmap(greencheckmark_xpm));
  d_list_icons.push_back(QPixmap(redx_xpm));
  d_list_icons.push_back(QPixmap(greenball_xpm));
  d_list_icons.push_back(QPixmap(redball_xpm));
  d_list_icons.push_back(QPixmap(whiteball_xpm));
  d_list_icons.push_back(QPixmap(blueball_xpm));

  //
  // Create Station Icon
  //
  d_station_icon=QPixmap(host_16x16_xpm);

  //
  // Create Type Icons
  //
  log_type_icons[RDLogLine::Cart]=QPixmap(play_xpm);
  log_type_icons[RDLogLine::Macro]=QPixmap(rml5_xpm);
  log_type_icons[RDLogLine::Marker]=QPixmap(notemarker_xpm);
  log_type_icons[RDLogLine::Chain]=QPixmap(chain_xpm);
  log_type_icons[RDLogLine::Track]=QPixmap(mic16_xpm);
  log_type_icons[RDLogLine::MusicLink]=QPixmap(music_xpm);
  log_type_icons[RDLogLine::TrafficLink]=QPixmap(traffic_xpm);
  log_track_cart_icon=QPixmap(track_cart_xpm);

  //
  // User Icons
  //
  d_user_icons[RDUser::TypeAdminConfig]=QPixmap(admin_xpm);
  d_user_icons[RDUser::TypeAdminRss]=QPixmap(rss_xpm);
  d_user_icons[RDUser::TypeLocalUser]=QPixmap(localuser_xpm);
  d_user_icons[RDUser::TypeExternalUser]=QPixmap(user_xpm);
}


QPixmap RDIconEngine::listIcon(RDIconEngine::IconType type) const
{
  return d_list_icons.at(type);
}


QPixmap RDIconEngine::stationIcon() const
{
  return d_station_icon;
}


QPixmap RDIconEngine::typeIcon(RDLogLine::Type type,RDLogLine::Source src) const
{
  if((type==RDLogLine::Cart)&&(src==RDLogLine::Tracker)) {
    return log_track_cart_icon;
  }
  return log_type_icons.value(type);
}


QPixmap RDIconEngine::userIcon(RDUser::Type type) const
{
  return d_user_icons.value(type);
}
