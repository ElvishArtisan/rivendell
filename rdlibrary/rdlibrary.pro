# rdlibrary.pro
#
# The QMake project file for RDLibrary.
#
# (C) Copyright 2003-2005 Fred Gleason <fredg@paravelsystems.com>
#
#      $Id: rdlibrary.pro,v 1.11.2.3 2014/01/08 02:08:38 cvs Exp $
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
  SOURCES += audio_cart.cpp
  SOURCES += cdripper.cpp
  SOURCES += disk_gauge.cpp
  SOURCES += disk_ripper.cpp
  SOURCES += edit_cart.cpp
  SOURCES += edit_macro.cpp
  SOURCES += edit_notes.cpp
  SOURCES += edit_schedulercodes.cpp
  SOURCES += filter.cpp
  SOURCES += lib_listview.cpp
  SOURCES += list_reports.cpp
  SOURCES += macro_cart.cpp
  SOURCES += rdlibrary.cpp
  SOURCES += record_cut.cpp
  SOURCES += validate_cut.cpp
}

x11 {
  HEADERS += audio_cart.h
  HEADERS += cdripper.h
  HEADERS += disk_gauge.h
  HEADERS += disk_ripper.h
  HEADERS += edit_cart.h
  HEADERS += edit_macro.h
  HEADERS += edit_notes.h
  HEADERS += edit_schedulercodes.h
  HEADERS += filter.h
  HEADERS += lib_listview.h
  HEADERS += list_reports.h
  HEADERS += macro_cart.h
  HEADERS += rdlibrary.h
  HEADERS += record_cut.h
  HEADERS += validate_cut.h
}

TRANSLATIONS += rdlibrary_cs.ts
TRANSLATIONS += rdlibrary_de.ts
TRANSLATIONS += rdlibrary_es.ts
TRANSLATIONS += rdlibrary_fr.ts
TRANSLATIONS += rdlibrary_nb.ts
TRANSLATIONS += rdlibrary_nn.ts
TRANSLATIONS += rdlibrary_pt_BR.ts
