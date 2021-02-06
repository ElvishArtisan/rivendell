# rdhpi.pro
#
# The rdhpi/ QMake project file for Rivendell.
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

#
# Maintainer's Note
#
# We don't use Qt's 'Qmake' build system, so the entries in here are
# purely for the sake of i18n support.
#

SOURCES += rdhpiinformation.cpp
SOURCES += rdhpiplaystream.cpp
SOURCES += rdhpirecordstream.cpp
SOURCES += rdhpisoundcard.cpp

HEADERS += rdhpiinformation.h
HEADERS += rdhpiplaystream.h
HEADERS += rdhpirecordstream.h
HEADERS += rdhpisoundcard.h

TRANSLATIONS += rdhpi_cs.ts
TRANSLATIONS += rdhpi_de.ts
TRANSLATIONS += rdhpi_es.ts
TRANSLATIONS += rdhpi_fr.ts
TRANSLATIONS += rdhpi_nb.ts
TRANSLATIONS += rdhpi_nn.ts
TRANSLATIONS += rdhpi_pt_BR.ts
