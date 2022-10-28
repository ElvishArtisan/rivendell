# rdcatch.pro
#
# The QMake project file for RDCatch.
#
# (C) Copyright 2003-2022 Fred Gleason <fredg@paravelsystems.com>
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
  SOURCES += catchtableview.cpp
  SOURCES += deckmon.cpp
  SOURCES += dowselector.cpp
  SOURCES += edit_cartevent.cpp
  SOURCES += edit_playout.cpp
  SOURCES += edit_recording.cpp
  SOURCES += edit_switchevent.cpp
  SOURCES += edit_download.cpp
  SOURCES += edit_upload.cpp
  SOURCES += eventlight.cpp
  SOURCES += eventwidget.cpp
  SOURCES += rdcatch.cpp
  SOURCES += list_reports.cpp
  SOURCES += recordlistmodel.cpp
}

x11 {
  HEADERS += add_recording.h
  HEADERS += catchtableview.h
  HEADERS += deckmon.h
  HEADERS += dowselector.h
  HEADERS += edit_cartevent.h
  HEADERS += edit_playout.h
  HEADERS += edit_recording.h
  HEADERS += edit_switchevent.h
  HEADERS += edit_download.h
  HEADERS += edit_upload.h
  HEADERS += eventlight.h
  HEADERS += eventwidget.h
  HEADERS += rdcatch.h
  HEADERS += list_reports.h
  HEADERS += recordlistmodel.h
}

TRANSLATIONS += rdcatch_cs.ts
TRANSLATIONS += rdcatch_de.ts
TRANSLATIONS += rdcatch_es.ts
TRANSLATIONS += rdcatch_fr.ts
TRANSLATIONS += rdcatch_nb.ts
TRANSLATIONS += rdcatch_nn.ts
TRANSLATIONS += rdcatch_pt_BR.ts
