## automake.am
##
## Automake.am for utils/rdhpiinfo
##
## (C) Copyright 2002-2007,2016 Fred Gleason <fredg@paravelsystems.com>
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

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -I$(top_srcdir)/lib -I$(top_srcdir)/rdhpi @QT4_CFLAGS@ -DQT3_SUPPORT -I/usr/include/Qt3Support
LIBS = -L$(top_srcdir)/lib -L$(top_srcdir)/rdhpi
MOC = @QT_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

bin_PROGRAMS = rdhpiinfo

dist_rdhpiinfo_SOURCES = change_mode.cpp change_mode.h\
                         rdhpiinfo.cpp rdhpiinfo.h\
                         virtdetails.cpp virtdetails.h

nodist_rdhpiinfo_SOURCES = moc_change_mode.cpp\
                           moc_rdhpiinfo.cpp\
                           moc_virtdetails.cpp

rdhpiinfo_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @LIBHPI@ @QT4_LIBS@ -lQt3Support

EXTRA_DIST = rdhpiinfo.pro

CLEANFILES = *~\
             *.qm\
             moc_*

MAINTAINERCLEANFILES = *~\
                       Makefile.in\
                       moc_*
