# rdcastmanager.pro
#
# The rdcastmanager/ QMake project file for Rivendell
#
# (C) Copyright 2003-2020 Fred Gleason <fredg@paravelsystems.com>
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

SOURCES += edit_cast.cpp
SOURCES += list_casts.cpp
SOURCES += logdialog.cpp
SOURCES += logmodel.cpp
SOURCES += pick_report_dates.cpp
SOURCES += rdcastmanager.cpp
SOURCES += render_dialog.cpp

HEADERS += edit_cast.h
HEADERS += list_casts.h
HEADERS += logdialog.h
HEADERS += logmodel.h
HEADERS += pick_report_dates.h
HEADERS += rdcastmanager.h
HEADERS += render_dialog.h

TRANSLATIONS += rdcastmanager_cs.ts
TRANSLATIONS += rdcastmanager_de.ts
TRANSLATIONS += rdcastmanager_es.ts
TRANSLATIONS += rdcastmanager_fr.ts
TRANSLATIONS += rdcastmanager_nb.ts
TRANSLATIONS += rdcastmanager_nn.ts
TRANSLATIONS += rdcastmanager_pt_BR.ts
