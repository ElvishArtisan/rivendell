// rdiconengine.cpp
//
// Icon generator for Rivendell
//
//   (C) Copyright 2021-2022 Fred Gleason <fredg@paravelsystems.com>
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
#include "../icons/rivendell-16x16.xpm"
#include "../icons/rivendell-22x22.xpm"
#include "../icons/rivendell-32x32.xpm"
#include "../icons/rivendell-64x64.xpm"
#include "../icons/rivendell-128x128.xpm"
#include "../icons/rivendell-256x256.xpm"
#include "../icons/rivendell-512x512.xpm"

#include "../icons/rdadmin-16x16.xpm"
#include "../icons/rdadmin-22x22.xpm"
#include "../icons/rdadmin-32x32.xpm"
#include "../icons/rdadmin-64x64.xpm"
#include "../icons/rdadmin-128x128.xpm"
#include "../icons/rdadmin-256x256.xpm"
#include "../icons/rdadmin-512x512.xpm"

#include "../icons/rdairplay-16x16.xpm"
#include "../icons/rdairplay-22x22.xpm"
#include "../icons/rdairplay-32x32.xpm"
#include "../icons/rdairplay-64x64.xpm"
#include "../icons/rdairplay-128x128.xpm"
#include "../icons/rdairplay-256x256.xpm"
#include "../icons/rdairplay-512x512.xpm"

#include "../icons/rdcartslots-16x16.xpm"
#include "../icons/rdcartslots-22x22.xpm"
#include "../icons/rdcartslots-32x32.xpm"
#include "../icons/rdcartslots-64x64.xpm"
#include "../icons/rdcartslots-128x128.xpm"
#include "../icons/rdcartslots-256x256.xpm"
#include "../icons/rdcartslots-512x512.xpm"

#include "../icons/rdcastmanager-16x16.xpm"
#include "../icons/rdcastmanager-22x22.xpm"
#include "../icons/rdcastmanager-32x32.xpm"
#include "../icons/rdcastmanager-64x64.xpm"
#include "../icons/rdcastmanager-128x128.xpm"
#include "../icons/rdcastmanager-256x256.xpm"
#include "../icons/rdcastmanager-512x512.xpm"

#include "../icons/rdcatch-16x16.xpm"
#include "../icons/rdcatch-22x22.xpm"
#include "../icons/rdcatch-32x32.xpm"
#include "../icons/rdcatch-64x64.xpm"
#include "../icons/rdcatch-128x128.xpm"
#include "../icons/rdcatch-256x256.xpm"
#include "../icons/rdcatch-512x512.xpm"

#include "../icons/rdlibrary-16x16.xpm"
#include "../icons/rdlibrary-22x22.xpm"
#include "../icons/rdlibrary-32x32.xpm"
#include "../icons/rdlibrary-64x64.xpm"
#include "../icons/rdlibrary-128x128.xpm"
#include "../icons/rdlibrary-256x256.xpm"
#include "../icons/rdlibrary-512x512.xpm"

#include "../icons/rdlogedit-16x16.xpm"
#include "../icons/rdlogedit-22x22.xpm"
#include "../icons/rdlogedit-32x32.xpm"
#include "../icons/rdlogedit-64x64.xpm"
#include "../icons/rdlogedit-128x128.xpm"
#include "../icons/rdlogedit-256x256.xpm"
#include "../icons/rdlogedit-512x512.xpm"

#include "../icons/rdlogmanager-16x16.xpm"
#include "../icons/rdlogmanager-22x22.xpm"
#include "../icons/rdlogmanager-32x32.xpm"
#include "../icons/rdlogmanager-64x64.xpm"
#include "../icons/rdlogmanager-128x128.xpm"
#include "../icons/rdlogmanager-256x256.xpm"
#include "../icons/rdlogmanager-512x512.xpm"

#include "../icons/rdpanel-16x16.xpm"
#include "../icons/rdpanel-22x22.xpm"
#include "../icons/rdpanel-32x32.xpm"
#include "../icons/rdpanel-64x64.xpm"
#include "../icons/rdpanel-128x128.xpm"
#include "../icons/rdpanel-256x256.xpm"
#include "../icons/rdpanel-512x512.xpm"



#include "../icons/greencheckmark.xpm"
#include "../icons/redx.xpm"
#include "../icons/blueball.xpm"
#include "../icons/greenball.xpm"
#include "../icons/redball.xpm"
#include "../icons/whiteball.xpm"
#include "../icons/switch3.xpm"
#include "../icons/input.xpm"
#include "../icons/output.xpm"
#include "../icons/record.xpm"
#include "../icons/upload.xpm"
#include "../icons/download.xpm"

#include "../icons/host-16x16.xpm"
#include "../icons/service-16x16.xpm"

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
  //
  // Application Icons
  //
  QMap<int,QPixmap> pixmap;

  // Rivendell
  pixmap.clear();
  pixmap[16]=QPixmap(rivendell_16x16_xpm);
  pixmap[22]=QPixmap(rivendell_22x22_xpm);
  pixmap[32]=QPixmap(rivendell_32x32_xpm);
  pixmap[64]=QPixmap(rivendell_64x64_xpm);
  pixmap[128]=QPixmap(rivendell_128x128_xpm);
  pixmap[256]=QPixmap(rivendell_256x256_xpm);
  pixmap[512]=QPixmap(rivendell_512x512_xpm);
  d_application_icons.push_back(pixmap);

  // RDAdmin
  pixmap.clear();
  pixmap[16]=QPixmap(rdadmin_16x16_xpm);
  pixmap[22]=QPixmap(rdadmin_22x22_xpm);
  pixmap[32]=QPixmap(rdadmin_32x32_xpm);
  pixmap[64]=QPixmap(rdadmin_64x64_xpm);
  pixmap[128]=QPixmap(rdadmin_128x128_xpm);
  pixmap[256]=QPixmap(rdadmin_256x256_xpm);
  pixmap[512]=QPixmap(rdadmin_512x512_xpm);
  d_application_icons.push_back(pixmap);

  // RDAirPlay
  pixmap.clear();
  pixmap[16]=QPixmap(rdairplay_16x16_xpm);
  pixmap[22]=QPixmap(rdairplay_22x22_xpm);
  pixmap[32]=QPixmap(rdairplay_32x32_xpm);
  pixmap[64]=QPixmap(rdairplay_64x64_xpm);
  pixmap[128]=QPixmap(rdairplay_128x128_xpm);
  pixmap[256]=QPixmap(rdairplay_256x256_xpm);
  pixmap[512]=QPixmap(rdairplay_512x512_xpm);
  d_application_icons.push_back(pixmap);

  // RDCartSlots
  pixmap.clear();
  pixmap[16]=QPixmap(rdcartslots_16x16_xpm);
  pixmap[22]=QPixmap(rdcartslots_22x22_xpm);
  pixmap[32]=QPixmap(rdcartslots_32x32_xpm);
  pixmap[64]=QPixmap(rdcartslots_64x64_xpm);
  pixmap[128]=QPixmap(rdcartslots_128x128_xpm);
  pixmap[256]=QPixmap(rdcartslots_256x256_xpm);
  pixmap[512]=QPixmap(rdcartslots_512x512_xpm);
  d_application_icons.push_back(pixmap);

  // RDCastManager
  pixmap.clear();
  pixmap[16]=QPixmap(rdcastmanager_16x16_xpm);
  pixmap[22]=QPixmap(rdcastmanager_22x22_xpm);
  pixmap[32]=QPixmap(rdcastmanager_32x32_xpm);
  pixmap[64]=QPixmap(rdcastmanager_64x64_xpm);
  pixmap[128]=QPixmap(rdcastmanager_128x128_xpm);
  pixmap[256]=QPixmap(rdcastmanager_256x256_xpm);
  pixmap[512]=QPixmap(rdcastmanager_512x512_xpm);
  d_application_icons.push_back(pixmap);

  // RDCatch
  pixmap.clear();
  pixmap[16]=QPixmap(rdcatch_16x16_xpm);
  pixmap[22]=QPixmap(rdcatch_22x22_xpm);
  pixmap[32]=QPixmap(rdcatch_32x32_xpm);
  pixmap[64]=QPixmap(rdcatch_64x64_xpm);
  pixmap[128]=QPixmap(rdcatch_128x128_xpm);
  pixmap[256]=QPixmap(rdcatch_256x256_xpm);
  pixmap[512]=QPixmap(rdcatch_512x512_xpm);
  d_application_icons.push_back(pixmap);

  // RDLibrary
  pixmap.clear();
  pixmap[16]=QPixmap(rdlibrary_16x16_xpm);
  pixmap[22]=QPixmap(rdlibrary_22x22_xpm);
  pixmap[32]=QPixmap(rdlibrary_32x32_xpm);
  pixmap[64]=QPixmap(rdlibrary_64x64_xpm);
  pixmap[128]=QPixmap(rdlibrary_128x128_xpm);
  pixmap[256]=QPixmap(rdlibrary_256x256_xpm);
  pixmap[512]=QPixmap(rdlibrary_512x512_xpm);
  d_application_icons.push_back(pixmap);

  // RDLogEdit
  pixmap.clear();
  pixmap[16]=QPixmap(rdlogedit_16x16_xpm);
  pixmap[22]=QPixmap(rdlogedit_22x22_xpm);
  pixmap[32]=QPixmap(rdlogedit_32x32_xpm);
  pixmap[64]=QPixmap(rdlogedit_64x64_xpm);
  pixmap[128]=QPixmap(rdlogedit_128x128_xpm);
  pixmap[256]=QPixmap(rdlogedit_256x256_xpm);
  pixmap[512]=QPixmap(rdlogedit_512x512_xpm);
  d_application_icons.push_back(pixmap);

  // RdLogManager
  pixmap.clear();
  pixmap[16]=QPixmap(rdlogmanager_16x16_xpm);
  pixmap[22]=QPixmap(rdlogmanager_22x22_xpm);
  pixmap[32]=QPixmap(rdlogmanager_32x32_xpm);
  pixmap[64]=QPixmap(rdlogmanager_64x64_xpm);
  pixmap[128]=QPixmap(rdlogmanager_128x128_xpm);
  pixmap[256]=QPixmap(rdlogmanager_256x256_xpm);
  pixmap[512]=QPixmap(rdlogmanager_512x512_xpm);
  d_application_icons.push_back(pixmap);

  // RDPanel
  pixmap.clear();
  pixmap[16]=QPixmap(rdpanel_16x16_xpm);
  pixmap[22]=QPixmap(rdpanel_22x22_xpm);
  pixmap[32]=QPixmap(rdpanel_32x32_xpm);
  pixmap[64]=QPixmap(rdpanel_64x64_xpm);
  pixmap[128]=QPixmap(rdpanel_128x128_xpm);
  pixmap[256]=QPixmap(rdpanel_256x256_xpm);
  pixmap[512]=QPixmap(rdpanel_512x512_xpm);
  d_application_icons.push_back(pixmap);


  //
  // Catch Icons
  //
  d_catch_icons.push_back(QPixmap(record_xpm));
  d_catch_icons.push_back(QPixmap(rml5_xpm));
  d_catch_icons.push_back(QPixmap(switch3_xpm));
  d_catch_icons.push_back(QPixmap(play_xpm));
  d_catch_icons.push_back(QPixmap(download_xpm));
  d_catch_icons.push_back(QPixmap(upload_xpm));

  //
  // List Icons
  //
  d_list_icons.push_back(QPixmap(greencheckmark_xpm));
  d_list_icons.push_back(QPixmap(redx_xpm));
  d_list_icons.push_back(QPixmap(greenball_xpm));
  d_list_icons.push_back(QPixmap(redball_xpm));
  d_list_icons.push_back(QPixmap(whiteball_xpm));
  d_list_icons.push_back(QPixmap(blueball_xpm));
  d_list_icons.push_back(QPixmap(switch3_xpm));
  d_list_icons.push_back(QPixmap(input_xpm));
  d_list_icons.push_back(QPixmap(output_xpm));
  d_list_icons.push_back(QPixmap(record_xpm));
  d_list_icons.push_back(QPixmap(rss_xpm));
  d_list_icons.push_back(QPixmap(upload_xpm));
  d_list_icons.push_back(QPixmap(download_xpm));

  //
  // Create Service Icon
  //
  d_service_icon=QPixmap(service_16x16_xpm);

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


QPixmap RDIconEngine::applicationIcon(RDIconEngine::Application app,
				      int edge_size) const
{
  return d_application_icons.at((int)app).value(edge_size,QPixmap());
}


QPixmap RDIconEngine::catchIcon(RDRecording::Type type)
{
  return d_catch_icons.at((int)type);
}


QPixmap RDIconEngine::listIcon(RDIconEngine::IconType type) const
{
  return d_list_icons.at(type);
}


QPixmap RDIconEngine::serviceIcon() const
{
  return d_service_icon;
}


QPixmap RDIconEngine::stationIcon() const
{
  return d_station_icon;
}


QPixmap RDIconEngine::nullIcon(int edge_size) const
{
  QPixmap pix(edge_size,edge_size);
  pix.fill(Qt::transparent);

  return pix;
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
