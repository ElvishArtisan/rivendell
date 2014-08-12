# rdlogmanager.pro
#
# The rdlogmanager/ QMake project file for Rivendell
#
# (C) Copyright 2003-2004 Fred Gleason <fredg@paravelsystems.com>
#
#      $Id: rdlogmanager.pro,v 1.21.2.3 2013/01/01 21:36:31 cvs Exp $
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

TARGET = rdlogmanager

win32 {
  DEFINES += WIN32
  DEFINES += VERSION=\"$$[VERSION]\"
  DEFINES += PACKAGE=\"rivendell\" 
  DEFINES += PACKAGE_VERSION=\"$$[VERSION]\" 
  DEFINES += PACKAGE_NAME=\"rivendell\"
  DEFINES += PACKAGE_BUGREPORT=\"fredg@paravelsystems.com\"
}

SOURCES += add_clock.cpp
SOURCES += add_event.cpp
SOURCES += clock_listview.cpp
SOURCES += commandline_ops.cpp
SOURCES += edit_clock.cpp
SOURCES += edit_event.cpp
SOURCES += edit_eventline.cpp
SOURCES += edit_grid.cpp
SOURCES += edit_note.cpp
SOURCES += edit_perms.cpp
SOURCES += edit_schedrules.cpp
SOURCES += edit_schedcoderules.cpp
SOURCES += edit_track.cpp
SOURCES += generate_log.cpp
SOURCES += import_listview.cpp
SOURCES += lib_listview.cpp
SOURCES += list_clocks.cpp
SOURCES += list_events.cpp
SOURCES += list_grids.cpp
SOURCES += list_svcs.cpp
SOURCES += pick_report_dates.cpp
SOURCES += rdlogmanager.cpp
SOURCES += rename_item.cpp
SOURCES += svc_rec.cpp
SOURCES += svc_rec_dialog.cpp

HEADERS += add_clock.h
HEADERS += add_event.h
HEADERS += clock_listview.h
HEADERS += edit_clock.h
HEADERS += edit_event.h
HEADERS += edit_eventline.h
HEADERS += edit_grid.h
HEADERS += edit_note.h
HEADERS += edit_perms.h
HEADERS += edit_schedrules.h
HEADERS += edit_schedcoderules.h
HEADERS += edit_track.h
HEADERS += generate_log.h
HEADERS += globals.h
HEADERS += import_listview.h
HEADERS += lib_listview.h
HEADERS += list_clocks.h
HEADERS += list_events.h
HEADERS += list_grids.h
HEADERS += list_svcs.h
HEADERS += pick_report_dates.h
HEADERS += rdlogmanager.h
HEADERS += rename_item.h
HEADERS += svc_rec.h
HEADERS += svc_rec_dialog.h

RES_FILE += ..\icons\rivendell.res

INCLUDEPATH += ..\lib

LIBS = -lqui -L..\lib -llib

CONFIG += qt

TRANSLATIONS += rdlogmanager_cs.ts
TRANSLATIONS += rdlogmanager_de.ts
TRANSLATIONS += rdlogmanager_es.ts
TRANSLATIONS += rdlogmanager_fr.ts
TRANSLATIONS += rdlogmanager_nb.ts
TRANSLATIONS += rdlogmanager_nn.ts
TRANSLATIONS += rdlogmanager_pt_BR.ts
