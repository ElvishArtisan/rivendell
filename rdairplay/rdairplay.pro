# rdairplay.pro
#
# The QMake project file for RDAirPlay.
#
# (C) Copyright 2003-2005 Fred Gleason <fredg@paravelsystems.com>
#
#      $Id: rdairplay.pro,v 1.8.4.4 2013/12/28 00:00:34 cvs Exp $
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

TEMPLATE = app

x11 {
  SOURCES += button_log.cpp
  SOURCES += edit_event.cpp
  SOURCES += hourselector.cpp
  SOURCES += lib_listview.cpp
  SOURCES += list_log.cpp
  SOURCES += list_logs.cpp
  SOURCES += local_macros.cpp
  SOURCES += log_play.cpp
  SOURCES += log_traffic.cpp
  SOURCES += loglinebox.cpp
  SOURCES += mode_display.cpp
  SOURCES += pie_counter.cpp
  SOURCES += post_counter.cpp
  SOURCES += rdairplay.cpp
  SOURCES += start_button.cpp
  SOURCES += stop_counter.cpp
  SOURCES += wall_clock.cpp
}

x11 {
  HEADERS += button_log.h
  HEADERS += colors.h
  HEADERS += edit_event.h
  HEADERS += hourselector.h
  HEADERS += lib_listview.h
  HEADERS += list_log.h
  HEADERS += list_logs.h
  HEADERS += log_play.h
  HEADERS += log_traffic.h
  HEADERS += loglinebox.h
  HEADERS += mode_display.h
  HEADERS += pie_counter.h
  HEADERS += post_counter.h
  HEADERS += rdairplay.h
  HEADERS += start_button.h
  HEADERS += stop_counter.h
  HEADERS += wall_clock.h
}

TRANSLATIONS += rdairplay_cs.ts
TRANSLATIONS += rdairplay_de.ts
TRANSLATIONS += rdairplay_es.ts
TRANSLATIONS += rdairplay_fr.ts
TRANSLATIONS += rdairplay_nb.ts
TRANSLATIONS += rdairplay_nn.ts
TRANSLATIONS += rdairplay_pt_BR.ts
