# rdlogin.pro
#
# The QMake project file for RDLogin.
#
# (C) Copyright 2003-2005 Fred Gleason <fredg@paravelsystems.com>
#
#      $Id: rdlogin.pro,v 1.7.8.1 2013/01/01 21:36:31 cvs Exp $
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License version 2 as
#   published by the Free Software Foundation.
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as
#   published by the Free Software Foundation; either version 2 of
#   the License, or (at your option) any later version.
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
  SOURCES += rdlogin.cpp
}

x11 {
  HEADERS += rdlogin.h
}

TRANSLATIONS += rdlogin_cs.ts
TRANSLATIONS += rdlogin_de.ts
TRANSLATIONS += rdlogin_es.ts
TRANSLATIONS += rdlogin_fr.ts
TRANSLATIONS += rdlogin_nb.ts
TRANSLATIONS += rdlogin_nn.ts
TRANSLATIONS += rdlogin_pt_BR.ts
