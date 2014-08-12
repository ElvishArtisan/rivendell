# rmlsend.pro
#
# The utils/ QMake project file for Rivendell
#
# (C) Copyright 2003-2007 Fred Gleason <fredg@paravelsystems.com>
#
#      $Id: rmlsend.pro,v 1.7.8.1 2013/01/01 21:36:33 cvs Exp $
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

TARGET = rmlsend

win32 {
  DEFINES += WIN32
  DEFINES += VERSION=\"$$[VERSION]\"
  DEFINES += PACKAGE=\"rivendell\" 
  DEFINES += PACKAGE_VERSION=\"$$[VERSION]\" 
  DEFINES += PACKAGE_NAME=\"rivendell\"
  DEFINES += PACKAGE_BUGREPORT=\"fredg@paravelsystems.com\"
}

SOURCES += rmlsend.cpp
HEADERS += rmlsend.h

RES_FILE += ..\..\icons\rivendell.res

INCLUDEPATH += ..\..\..\libradio\radio ..\..\lib

LIBS = -lqui -L..\..\..\libradio\radio -lradio -L..\..\lib -llib

CONFIG += qt

TRANSLATIONS += rmlsend_cs.ts
TRANSLATIONS += rmlsend_de.ts
TRANSLATIONS += rmlsend_es.ts
TRANSLATIONS += rmlsend_fr.ts
TRANSLATIONS += rmlsend_nb.ts
TRANSLATIONS += rmlsend_nn.ts
TRANSLATIONS += rmlsend_pt_BR.ts
