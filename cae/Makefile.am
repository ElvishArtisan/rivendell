## automake.am
##
## Core Audio Engine Makefile.am for Rivendell
##
## Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

AM_CPPFLAGS = -Wall -I$(top_srcdir)/lib -I$(top_srcdir)/rdhpi -Wno-strict-aliasing -std=c++11 -fPIC @QT5_CFLAGS@ @MUSICBRAINZ_CFLAGS@
LIBS = -L$(top_srcdir)/lib -L$(top_srcdir)/rdhpi
MOC = @QT_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

sbin_PROGRAMS = caed

dist_caed_SOURCES = alsadriver.cpp alsadriver.h\
                    cae.cpp cae.h\
                    cae_server.cpp cae_server.h\
                    caedriver.cpp caedriver.h\
                    hpidriver.cpp hpidriver.h\
                    jackdriver.cpp jackdriver.h

nodist_caed_SOURCES = moc_alsadriver.cpp\
                      moc_cae.cpp\
                      moc_cae_server.cpp\
                      moc_caedriver.cpp\
                      moc_hpidriver.cpp\
                      moc_jackdriver.cpp

caed_LDADD = @LIB_RDLIBS@\
             @LIBALSA@\
             @LIBHPI@\
             @LIBJACK@\
             @LIBSRC@\
             @LIBVORBIS@\
             @QT5_LIBS@\
             @MUSICBRAINZ_LIBS@

CLEANFILES = *~\
             moc_*

MAINTAINERCLEANFILES = *~\
                       Makefile.in
