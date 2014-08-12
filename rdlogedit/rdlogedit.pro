# rdlogedit.pro
#
# The rdlogedit/ QMake project file for Rivendell
#
# (C) Copyright 2003-2004 Fred Gleason <fredg@paravelsystems.com>
#
#      $Id: rdlogedit.pro,v 1.17.2.3 2013/12/27 22:12:29 cvs Exp $
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

TARGET = rdlogedit

win32 {
  DEFINES += WIN32
  DEFINES += VERSION=\"$$[VERSION]\"
  DEFINES += PACKAGE=\"rivendell\" 
  DEFINES += PACKAGE_VERSION=\"$$[VERSION]\" 
  DEFINES += PACKAGE_NAME=\"rivendell\"
  DEFINES += PACKAGE_BUGREPORT=\"fredg@paravelsystems.com\"
}

SOURCES += add_meta.cpp
SOURCES += drop_listview.cpp
SOURCES += edit_chain.cpp
SOURCES += edit_log.cpp
SOURCES += edit_logline.cpp
SOURCES += edit_marker.cpp
SOURCES += edit_track.cpp
SOURCES += list_listviewitem.cpp
SOURCES += list_logs.cpp
SOURCES += list_reports.cpp
SOURCES += rdlogedit.cpp
x11 {
  SOURCES += voice_tracker.cpp
}

HEADERS += add_meta.h
HEADERS += drop_listview.h
HEADERS += edit_chain.h
HEADERS += edit_log.h
HEADERS += edit_logline.h
HEADERS += edit_marker.h
HEADERS += edit_track.h
HEADERS += globals.h
HEADERS += list_listviewitem.h
HEADERS += list_logs.h
HEADERS += list_reports.h
HEADERS += rdlogedit.h
x11 {
  HEADERS += voice_tracker.h
}

RES_FILE += ..\icons\rivendell.res

INCLUDEPATH += ..\lib

LIBS = -lqui -L..\lib -llib

CONFIG += qt

TRANSLATIONS += rdlogedit_cs.ts
TRANSLATIONS += rdlogedit_de.ts
TRANSLATIONS += rdlogedit_es.ts
TRANSLATIONS += rdlogedit_fr.ts
TRANSLATIONS += rdlogedit_nb.ts
TRANSLATIONS += rdlogedit_nn.ts
TRANSLATIONS += rdlogedit_pt_BR.ts
