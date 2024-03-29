# rdairplay.pro
#
# The QMake project file for RDAirPlay.
#
# (C) Copyright 2003-2021 Fred Gleason <fredg@paravelsystems.com>
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
  SOURCES += list_log.cpp
  SOURCES += list_logs.cpp
  SOURCES += local_macros.cpp
  SOURCES += loglinebox.cpp
  SOURCES += logtableview.cpp
  SOURCES += mode_display.cpp
  SOURCES += pie_counter.cpp
  SOURCES += post_counter.cpp
  SOURCES += rdairplay.cpp
  SOURCES += soundpanel.cpp
  SOURCES += start_button.cpp
  SOURCES += stop_counter.cpp
  SOURCES += wall_clock.cpp
}

x11 {
  HEADERS += button_log.h
  HEADERS += colors.h
  HEADERS += edit_event.h
  HEADERS += hourselector.h
  HEADERS += list_log.h
  HEADERS += list_logs.h
  HEADERS += loglinebox.h
  HEADERS += logtableview.h
  HEADERS += mode_display.h
  HEADERS += pie_counter.h
  HEADERS += post_counter.h
  HEADERS += rdairplay.h
  HEADERS += soundpanel.h
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
