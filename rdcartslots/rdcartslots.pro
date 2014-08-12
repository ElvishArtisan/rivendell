# rdcartslots.pro
#
# The QMake project file for RDCartSlots.
#
# (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
#
#      $Id: rdcartslots.pro,v 1.1.2.2 2012/11/16 18:10:41 cvs Exp $
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
  SOURCES += rdcartslots.cpp
}

x11 {
  HEADERS += rdcartslots.h
}

TRANSLATIONS += rdcartslots_de.ts
TRANSLATIONS += rdcartslots_es.ts
TRANSLATIONS += rdcartslots_fr.ts
TRANSLATIONS += rdcartslots_nb.ts
TRANSLATIONS += rdcartslots_nn.ts
TRANSLATIONS += rdcartslots_pt_BR.ts
