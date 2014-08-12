## automake.am
##
## Automake.am for utils/rdhpiinfo
##
## (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
##
##    $Id: Makefile.am,v 1.4.8.1 2012/11/29 01:37:38 cvs Exp $
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
##   Use automake to process this into a Makefile.in
##

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -DQTDIR=\"@QT_DIR@\" @QT_CXXFLAGS@
INCLUDES = -I$(top_srcdir)/lib -I$(top_srcdir)/rdhpi
LIBS = @QT_LIBS@ -L$(top_srcdir)/lib -L$(top_srcdir)/rdhpi
MOC = @QT_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

bin_PROGRAMS = rdhpiinfo

dist_rdhpiinfo_SOURCES = change_mode.cpp change_mode.h\
                         rdhpiinfo.cpp rdhpiinfo.h

nodist_rdhpiinfo_SOURCES = moc_change_mode.cpp\
                           moc_rdhpiinfo.cpp

rdhpiinfo_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @LIBHPI@

EXTRA_DIST = rdhpiinfo.pro

CLEANFILES = *~\
             *.qm\
             moc_*

MAINTAINERCLEANFILES = *~\
                       Makefile.in\
                       moc_*
