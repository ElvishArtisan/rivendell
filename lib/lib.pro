# lib.pro
#
# The lib/ QMake project file for Rivendell.
#
# (C) Copyright 2003-2007 Fred Gleason <fredg@paravelsystems.com>
#
#      $Id: lib.pro,v 1.36.8.13.2.1 2014/06/02 22:52:24 cvs Exp $
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License version 2 as
#   published by the Free Software Foundation.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public
#   License along with this program; if not, write to the Free Software
#   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

TEMPLATE = lib

win32 {
  DEFINES += WIN32
  DEFINES += VERSION=\"$$[VERSION]\"
  DEFINES += PACKAGE=\"rivendell\" 
  DEFINES += PACKAGE_VERSION=\"$$[VERSION]\" 
  DEFINES += PACKAGE_NAME=\"rivendell\"
  DEFINES += PACKAGE_BUGREPORT=\"fredg@paravelsystems.com\"
}

SOURCES += export_bmiemr.cpp
SOURCES += export_deltaflex.cpp
SOURCES += export_musicclassical.cpp
SOURCES += export_musicplayout.cpp
SOURCES += export_musicsummary.cpp
SOURCES += export_nprsoundex.cpp
SOURCES += export_radiotraffic.cpp
SOURCES += export_soundex.cpp
SOURCES += export_technical.cpp
SOURCES += export_textlog.cpp
SOURCES += rdadd_log.cpp
SOURCES += rdaudiosettings.cpp
SOURCES += rdaudiosettings_dialog.cpp
SOURCES += rdbusybar.cpp
SOURCES += rdbusydialog.cpp
SOURCES += rdcart.cpp
SOURCES += rdcart_dialog.cpp
SOURCES += rdcart_search_text.cpp
SOURCES += rdcartdrag.cpp
SOURCES += rdclock.cpp
SOURCES += rdcmd_switch.cpp
SOURCES += rdcombobox.cpp
SOURCES += rdconf.cpp
SOURCES += rdconfig.cpp
SOURCES += rdcreate_log.cpp
SOURCES += rdcut.cpp
SOURCES += rddatedialog.cpp
SOURCES += rddatedecode.cpp
SOURCES += rddatepicker.cpp
SOURCES += rddb.cpp
SOURCES += rddbheartbeat.cpp
SOURCES += rddebug.cpp
SOURCES += rddropbox.cpp
SOURCES += rdemptycart.cpp
SOURCES += rdencoder.cpp
SOURCES += rdencoderlist.cpp
SOURCES += rdescape_string.cpp
SOURCES += rdevent.cpp
SOURCES += rdevent_line.cpp
SOURCES += rdexception_dialog.cpp
SOURCES += rdget_ath.cpp
SOURCES += rdgetpasswd.cpp
SOURCES += rdgroup_list.cpp
SOURCES += rdidvalidator.cpp
SOURCES += rdintegeredit.cpp
SOURCES += rdintegerdialog.cpp
SOURCES += rdlabel.cpp
SOURCES += rdlibrary_conf.cpp
SOURCES += rdlicense.cpp
SOURCES += rdlineedit.cpp
SOURCES += rdlistselector.cpp
SOURCES += rdlistview.cpp
SOURCES += rdlistviewitem.cpp
SOURCES += rdlog.cpp
SOURCES += rdlog_event.cpp
SOURCES += rdlog_line.cpp
SOURCES += rdlogedit_conf.cpp
SOURCES += rdmacro.cpp
SOURCES += rdmacro_event.cpp
SOURCES += rdoneshot.cpp
SOURCES += rdplaymeter.cpp
SOURCES += rdprofile.cpp
SOURCES += rdprofileline.cpp
SOURCES += rdprofilesection.cpp
SOURCES += rdpushbutton.cpp
SOURCES += rdreport.cpp
SOURCES += rdripc.cpp
SOURCES += rdsegmeter.cpp
SOURCES += rdslider.cpp
SOURCES += rdsocket.cpp
SOURCES += rdstation.cpp
SOURCES += rdstereometer.cpp
SOURCES += rdsvc.cpp
SOURCES += rdsystem.cpp
SOURCES += rdtextfile.cpp
SOURCES += rdtextvalidator.cpp
SOURCES += rdtimeedit.cpp
SOURCES += rdtimeengine.cpp
SOURCES += rdtransportbutton.cpp
SOURCES += rduser.cpp
SOURCES += rdwavedata.cpp
SOURCES += schedcartlist.cpp
SOURCES += schedruleslist.cpp
win32 {
  SOURCES += html_gpl2_win32.cpp
  SOURCES += rdttydevice_win32.cpp
  SOURCES += rdwin32.cpp
}
x11 {
  SOURCES += html_gpl2.cpp
  SOURCES += rdadd_cart.cpp
  SOURCES += rdairplay_conf.cpp
  SOURCES += rdaudio_exists.cpp
  SOURCES += rdaudio_port.cpp
  SOURCES += rdbutton_dialog.cpp
  SOURCES += rdbutton_panel.cpp
  SOURCES += rdcae.cpp
  SOURCES += rdcardselector.cpp
  SOURCES += rdcatch_connect.cpp
  SOURCES += rdcddblookup.cpp
  SOURCES += rdcdplayer.cpp
  SOURCES += rdcddbrecord.cpp
  SOURCES += rdcheck_daemons.cpp
  SOURCES += rdcheck_version.cpp
  SOURCES += rdcmd_cache.cpp
  SOURCES += rdcueedit.cpp
  SOURCES += rdcueeditdialog.cpp
  SOURCES += rdcut_dialog.cpp
  SOURCES += rdcut_path.cpp
  SOURCES += rddeck.cpp
  SOURCES += rdedit_audio.cpp
  SOURCES += rdedit_panel_name.cpp
  SOURCES += rdexport_settings_dialog.cpp
  SOURCES += rdgpioselector.cpp
  SOURCES += rdgrid.cpp
  SOURCES += rdgroup.cpp
  SOURCES += rdimport_audio.cpp
  SOURCES += rdlist_groups.cpp
  SOURCES += rdlist_logs.cpp
  SOURCES += rdmarker_button.cpp
  SOURCES += rdmarker_edit.cpp
  SOURCES += rdmatrix.cpp
  SOURCES += rdmonitor_config.cpp
  SOURCES += rdpanel_button.cpp
  SOURCES += rdpasswd.cpp
  SOURCES += rdplay_deck.cpp
  SOURCES += rdrecording.cpp
  SOURCES += rdsettings.cpp
  SOURCES += rdsimpleplayer.cpp
  SOURCES += rdsound_panel.cpp
  SOURCES += rdstatus.cpp
  SOURCES += rdtimeedit.cpp
  SOURCES += rdtty.cpp
  SOURCES += rdttydevice.cpp
  SOURCES += rdttyout.cpp
  SOURCES += rdversion.cpp
  SOURCES += rdwavefile.cpp
}

HEADERS += schedcartlist.h
HEADERS += schedruleslist.h
HEADERS += rdadd_log.h
HEADERS += rdaudiosettings.h
HEADERS += rdaudiosettings_dialog.h
HEADERS += rdbusybar.h
HEADERS += rdbusydialog.h
HEADERS += rdcae.h
HEADERS += rdcart.h
HEADERS += rdcart_dialog.h
HEADERS += rdcart_search_text.h
HEADERS += rdcartdrag.h
HEADERS += rdclock.h
HEADERS += rdcmd_switch.h
HEADERS += rdcombobox.h
HEADERS += rdconf.h
HEADERS += rdconfig.h
HEADERS += rdcreate_log.h
HEADERS += rdcut.h
HEADERS += rddatedecode.h
HEADERS += rddatedialog.h
HEADERS += rddatepicker.h
HEADERS += rddb.h
HEADERS += rddbheartbeat.h
HEADERS += rddebug.h
HEADERS += rddropbox.h
HEADERS += rdemptycart.h
HEADERS += rdencoder.h
HEADERS += rdencoderlist.h
HEADERS += rdescape_string.h
HEADERS += rdevent.h
HEADERS += rdevent_line.h
HEADERS += rdexception_dialog.h
HEADERS += rdget_ath.h
HEADERS += rdgetpasswd.h
HEADERS += rdgroup_list.h
HEADERS += rd.h
HEADERS += rdidvalidator.h
HEADERS += rdintegeredit.h
HEADERS += rdintegerdialog.h
HEADERS += rdlabel.h
HEADERS += rdlibrary_conf.h
HEADERS += rdlicense.h
HEADERS += rdlineedit.h
HEADERS += rdlistselector.h
HEADERS += rdlistview.h
HEADERS += rdlistviewitem.h
HEADERS += rdlog.h
HEADERS += rdlog_event.h
HEADERS += rdlog_line.h
HEADERS += rdlogedit_conf.h
HEADERS += rdmacro.h
HEADERS += rdmacro_event.h
HEADERS += rdoneshot.h
HEADERS += rdplaymeter.h
HEADERS += rdprofile.h
HEADERS += rdprofileline.h
HEADERS += rdprofilesection.h
HEADERS += rdpushbutton.h
HEADERS += rdreport.h
HEADERS += rdripc.h
HEADERS += rdsegmeter.h
HEADERS += rdslider.h
HEADERS += rdsocket.h
HEADERS += rdstation.h
HEADERS += rdstereometer.h
HEADERS += rdsvc.h
HEADERS += rdsystem.h
HEADERS += rdtextfile.h
HEADERS += rdtextvalidator.h
HEADERS += rdtimeedit.h
HEADERS += rdtimeengine.h
HEADERS += rdtransportbutton.h
HEADERS += rdttydevice.h
HEADERS += rduser.h
HEADERS += rdwavedata.h

win32 {
  HEADERS += rdwin32.h
}
x11 {
  HEADERS += rdadd_cart.h
  HEADERS += rdairplay_conf.h
  HEADERS += rdaudio_exists.h
  HEADERS += rdaudio_port.h
  HEADERS += rdbutton_dialog.h
  HEADERS += rdbutton_panel.h
  HEADERS += rdcae.h
  HEADERS += rdcardselector.h
  HEADERS += rdcatch_connect.h
  HEADERS += rdcddblookup.h
  HEADERS += rdcdplayer.h
  HEADERS += rdcddbrecord.h
  HEADERS += rdcheck_daemons.h
  HEADERS += rdcheck_version.h
  HEADERS += rdcmd_cache.h
  HEADERS += rdcueedit.h
  HEADERS += rdcueeditdialog.h
  HEADERS += rdcut_dialog.h
  HEADERS += rdcut_path.h
  HEADERS += rddeck.h
  HEADERS += rdedit_audio.h
  HEADERS += rdedit_panel_name.h
  HEADERS += rdexport_settings_dialog.h
  HEADERS += rdgpioselector.h
  HEADERS += rdgrid.h
  HEADERS += rdgpio.h
  HEADERS += rdgroup.h
  HEADERS += rdimport_audio.h
  HEADERS += rdlist_groups.h
  HEADERS += rdlist_logs.h
  HEADERS += rdmarker_button.h
  HEADERS += rdmarker_edit.h
  HEADERS += rdmatrix.h
  HEADERS += rdmonitor_config.h
  HEADERS += rdpanel_button.h
  HEADERS += rdpaths.h
  HEADERS += rdpasswd.h
  HEADERS += rdplay_deck.h
  HEADERS += rdrecording.h
  HEADERS += rdsettings.h
  HEADERS += rdsimpleplayer.h
  HEADERS += rdsound_panel.h
  HEADERS += rdstatus.h
  HEADERS += rdtimeedit.h
  HEADERS += rdtty.h
  HEADERS += rdttyout.h
  HEADERS += rdversion.h
}

INCLUDEPATH += ..\..\libradio\radio

CONFIG += qt staticlib

TRANSLATIONS += librd_cs.ts
TRANSLATIONS += librd_de.ts
TRANSLATIONS += librd_es.ts
TRANSLATIONS += librd_fr.ts
TRANSLATIONS += librd_nb.ts
TRANSLATIONS += librd_nn.ts
TRANSLATIONS += librd_pt_BR.ts
