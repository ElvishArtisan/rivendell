# rdselect.pro
#
# The QMake project file for RDSelect.
#
# (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
#
#      $Id: rdselect.pro,v 1.1.2.2 2013/01/01 21:36:33 cvs Exp $
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
  SOURCES += rdselect.cpp
}

x11 {
  HEADERS += rdselect.h
}

TRANSLATIONS += rdselect_cs.ts
TRANSLATIONS += rdselect_de.ts
TRANSLATIONS += rdselect_es.ts
TRANSLATIONS += rdselect_fr.ts
TRANSLATIONS += rdselect_nb.ts
TRANSLATIONS += rdselect_nn.ts
TRANSLATIONS += rdselect_pt_BR.ts
