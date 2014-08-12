# rdcatch.pro
#
# The QMake project file for RDCatch.
#
# (C) Copyright 2003-2005 Fred Gleason <fredg@paravelsystems.com>
#
#      $Id: rdcatch.pro,v 1.9.8.1 2013/01/01 21:36:30 cvs Exp $
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
  SOURCES += add_recording.cpp
  SOURCES += deckmon.cpp
  SOURCES += edit_cartevent.cpp
  SOURCES += edit_playout.cpp
  SOURCES += edit_recording.cpp
  SOURCES += edit_switchevent.cpp
  SOURCES += edit_download.cpp
  SOURCES += edit_upload.cpp
  SOURCES += rdcatch.cpp
  SOURCES += list_reports.cpp
  SOURCES += catch_listview.cpp
}

x11 {
  HEADERS += add_recording.h
  HEADERS += deckmon.h
  HEADERS += edit_cartevent.h
  HEADERS += edit_playout.h
  HEADERS += edit_recording.h
  HEADERS += edit_switchevent.h
  HEADERS += edit_download.h
  HEADERS += edit_upload.h
  HEADERS += rdcatch.h
  HEADERS += list_reports.h
  HEADERS += catch_listview.h
}

TRANSLATIONS += rdcatch_cs.ts
TRANSLATIONS += rdcatch_de.ts
TRANSLATIONS += rdcatch_es.ts
TRANSLATIONS += rdcatch_fr.ts
TRANSLATIONS += rdcatch_nb.ts
TRANSLATIONS += rdcatch_nn.ts
TRANSLATIONS += rdcatch_pt_BR.ts
