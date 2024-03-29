## Makefile.am
##
## Automake.am for rivendell/importers
##
## (C) Copyright 2002-2022 Fred Gleason <fredg@paravelsystems.com>
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
##
## Use automake to process this into a Makefile.in

AM_CPPFLAGS = -Wall -I$(top_srcdir)/lib -Wno-strict-aliasing -std=c++11 -fPIC @QT5_CFLAGS@ @MUSICBRAINZ_CFLAGS@ @IMAGEMAGICK_CFLAGS@
LIBS = -L$(top_srcdir)/lib
MOC = @QT_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

bin_PROGRAMS = nexgen_filter\
               panel_copy\
               rdcatch_copy\
               rivendell_filter\
               wings_filter

dist_nexgen_filter_SOURCES = nexgen_filter.cpp nexgen_filter.h
nodist_nexgen_filter_SOURCES = moc_nexgen_filter.cpp
nexgen_filter_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT5_LIBS@ @MUSICBRAINZ_LIBS@ @IMAGEMAGICK_LIBS@

dist_panel_copy_SOURCES = panel_copy.cpp panel_copy.h
nodist_panel_copy_SOURCES = moc_panel_copy.cpp
panel_copy_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT5_LIBS@ @MUSICBRAINZ_LIBS@ @IMAGEMAGICK_LIBS@

dist_rdcatch_copy_SOURCES = rdcatch_copy.cpp rdcatch_copy.h
nodist_rdcatch_copy_SOURCES = moc_rdcatch_copy.cpp
rdcatch_copy_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT5_LIBS@ @MUSICBRAINZ_LIBS@ @IMAGEMAGICK_LIBS@

dist_rivendell_filter_SOURCES = rivendell_filter.cpp rivendell_filter.h
nodist_rivendell_filter_SOURCES = moc_rivendell_filter.cpp
rivendell_filter_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT5_LIBS@ @MUSICBRAINZ_LIBS@ @IMAGEMAGICK_LIBS@

dist_wings_filter_SOURCES = wings_filter.cpp wings_filter.h
nodist_wings_filter_SOURCES = moc_wings_filter.cpp
wings_filter_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT5_LIBS@ @MUSICBRAINZ_LIBS@ @IMAGEMAGICK_LIBS@

EXTRA_DIST = export_slax

CLEANFILES = *~\
             moc_*

MAINTAINERCLEANFILES = *~\
                       aclocal.m4\
                       configure\
                       Makefile.in\
                       moc_*\
                       *.tar.gz
