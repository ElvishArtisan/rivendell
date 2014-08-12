## automake.am
##
## Core Audio Engine Makefile.am for Rivendell
##
## Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
##
##      $Id: Makefile.am,v 1.33.8.1 2012/11/29 01:37:34 cvs Exp $
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

AM_CPPFLAGS = -Wall @QT_CXXFLAGS@
INCLUDES = -I$(top_srcdir)/lib -I$(top_srcdir)/rdhpi
LIBS = @QT_LIBS@ -L$(top_srcdir)/lib -L$(top_srcdir)/rdhpi
MOC = @QT_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

bin_PROGRAMS = caed

dist_caed_SOURCES = cae.cpp cae.h\
                    cae_alsa.cpp\
                    cae_hpi.cpp\
                    cae_jack.cpp\
                    cae_socket.cpp cae_socket.h

nodist_caed_SOURCES = moc_cae.cpp\
                      moc_cae_socket.cpp

caed_LDADD = @LIB_RDLIBS@\
             @LIBALSA@\
             @LIBHPI@\
             @LIBJACK@\
             @LIBSRC@\
             @LIBVORBIS@

CLEANFILES = *~\
             moc_*

MAINTAINERCLEANFILES = *~\
                       Makefile.in
