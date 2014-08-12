# rdhpi.pro
#
# The rdhpi/ QMake project file for Rivendell.
#
# (C) Copyright 2003-2007 Fred Gleason <fredg@paravelsystems.com>
#
#      $Id: rdhpi.pro,v 1.4.8.1 2013/01/01 21:36:31 cvs Exp $
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

x11 {
  SOURCES += rdhpicardselector.cpp
  SOURCES += rdhpiinformation.cpp
  SOURCES += rdhpiplaystream.cpp
  SOURCES += rdhpirecordstream.cpp
  SOURCES += rdhpisoundcard.cpp
  SOURCES += rdhpisoundselector.cpp
}

x11 {
  HEADERS += rdhpicardselector.h
  HEADERS += rdhpiinformation.h
  HEADERS += rdhpiplaystream.h
  HEADERS += rdhpirecordstream.h
  HEADERS += rdhpisoundcard.h
  HEADERS += rdhpisoundselector.h
}

CONFIG += qt staticlib

TRANSLATIONS += rdhpi_cs.ts
TRANSLATIONS += rdhpi_de.ts
TRANSLATIONS += rdhpi_es.ts
TRANSLATIONS += rdhpi_fr.ts
TRANSLATIONS += rdhpi_nb.ts
TRANSLATIONS += rdhpi_nn.ts
TRANSLATIONS += rdhpi_pt_BR.ts
