## Makefile.am
##
## (C) Copyright 2012-2021 Fred Gleason <fredg@paravelsystems.com>
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

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -Wno-strict-aliasing -std=c++11 -fPIC -I$(top_srcdir)/lib @QT5_CFLAGS@ @MUSICBRAINZ_CFLAGS@ @IMAGEMAGICK_CFLAGS@
LIBS = -L$(top_srcdir)/lib
MOC = @QT_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

# I18N Stuff
install-exec-local:	
	../helpers/rdi18n_helper.sh --install rdmonitor $(DESTDIR)$(prefix)/share/rivendell

uninstall-local:	
	../helpers/rdi18n_helper.sh --uninstall rdmonitor $(DESTDIR)$(prefix)/share/rivendell

all:	
	../helpers/rdi18n_helper.sh --update rdmonitor $(DESTDIR)$(prefix)/share/rivendell

bin_PROGRAMS = rdmonitor

dist_rdmonitor_SOURCES = positiondialog.cpp positiondialog.h\
                         rdmonitor.cpp rdmonitor.h

nodist_rdmonitor_SOURCES = moc_positiondialog.cpp\
                           moc_rdmonitor.cpp

rdmonitor_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT5_LIBS@ @MUSICBRAINZ_LIBS@ @IMAGEMAGICK_LIBS@

EXTRA_DIST = rdmonitor.pro\
             rdmonitor_cs.ts\
             rdmonitor_de.ts\
             rdmonitor_es.ts\
             rdmonitor_fr.ts\
             rdmonitor_nb.ts\
             rdmonitor_nn.ts\
             rdmonitor_pt_BR.ts

CLEANFILES = *~\
             *.qm\
             moc_*

MAINTAINERCLEANFILES = *~\
                       *.tar.gz\
                       aclocal.m4\
                       configure\
                       Makefile.in\
                       moc_*
