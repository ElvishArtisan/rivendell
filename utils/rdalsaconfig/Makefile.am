## automake.am
##
## Automake.am for utils/rdalsaconfig
##
## (C) Copyright 2009 Fred Gleason <fredg@paravelsystems.com>
##
##    $Id: Makefile.am,v 1.2.8.1 2012/11/29 01:37:37 cvs Exp $
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

bin_PROGRAMS = rdalsaconfig

dist_rdalsaconfig_SOURCES = alsaitem.cpp alsaitem.h\
                            rdalsa.cpp rdalsa.h\
                            rdalsaconfig.cpp rdalsaconfig.h

nodist_rdalsaconfig_SOURCES = moc_rdalsaconfig.cpp

rdalsaconfig_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @LIBALSA@

EXTRA_DIST = rdalsaconfig.pro

CLEANFILES = *~\
             *.qm\
             moc_*

MAINTAINERCLEANFILES = *~\
                       *.tar.gz\
                       aclocal.m4\
                       configure\
                       Makefile.in\
                       moc_*
