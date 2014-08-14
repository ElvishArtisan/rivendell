## automake.am
##
## Automake.am for rivendell/utils/rdmarkerset
##
##   (C) Copyright 2002-2014 Fred Gleason <fredg@paravelsystems.com>
##
##      $Id: Makefile.am,v 1.1.2.1 2014/01/16 02:45:00 cvs Exp $
##
##   This program is free software; you can redistribute it and/or modify
##   it under the terms of the GNU General Public License version 2 as
##   published by the Free Software Foundation.
##
##   This program is distributed in the hope that it will be useful,
##   but WITHOUT ANY WARRANTY; without even the implied warranty of
##   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##   GNU General Public License for more details.
##
##   You should have received a copy of the GNU General Public
##   License along with this program; if not, write to the Free Software
##   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
##
## Use automake to process this into a Makefile.in

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -DQTDIR=\"@QT_DIR@\" @QT_CXXFLAGS@ -I$(top_srcdir)/lib
LIBS = @QT_LIBS@ -L$(top_srcdir)/lib
MOC = @QT_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

sbin_PROGRAMS = rdmarkerset

dist_rdmarkerset_SOURCES = rdmarkerset.cpp rdmarkerset.h

nodist_rdmarkerset_SOURCES = moc_rdmarkerset.cpp

rdmarkerset_LDADD = @LIB_RDLIBS@ @LIBVORBIS@

EXTRA_DIST = 

CLEANFILES = *~\
             *.idb\
             *ilk\
             *.obj\
             *.pdb\
             *.qm\
             moc_*

MAINTAINERCLEANFILES = *~\
                       Makefile.in\
                       moc_*
