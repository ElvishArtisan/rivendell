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
install-exec-hook:
	../helpers/rdi18n_helper.sh --install rdselect $(DESTDIR)$(prefix)/share/rivendell
	mkdir -p $(DESTDIR)/etc/auto.master.d
	cp rd.audiostore.autofs $(DESTDIR)/etc/auto.master.d/
	touch $(DESTDIR)/etc/auto.rd.audiostore

uninstall-local:	
	../helpers/rdi18n_helper.sh --uninstall rdselect $(DESTDIR)$(prefix)/share/rivendell
	rm -f $(DESTDIR)/etc/auto.master.d/rd.audiostore.autofs

all:	
	../helpers/rdi18n_helper.sh --update rdselect $(DESTDIR)$(prefix)/share/rivendell

bin_PROGRAMS = rdselect

dist_rdselect_SOURCES = rdselect.cpp rdselect.h

nodist_rdselect_SOURCES = moc_rdselect.cpp

rdselect_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT5_LIBS@ @MUSICBRAINZ_LIBS@ @IMAGEMAGICK_LIBS@

EXTRA_DIST = rd.audiostore.autofs\
             rdselect.pro\
             rdselect_cs.ts\
             rdselect_de.ts\
             rdselect_es.ts\
             rdselect_fr.ts\
             rdselect_nb.ts\
             rdselect_nn.ts\
             rdselect_pt_BR.ts

CLEANFILES = *~\
             *.qm\
             moc_*

MAINTAINERCLEANFILES = *~\
                       *.tar.gz\
                       aclocal.m4\
                       configure\
                       Makefile.in\
                       moc_*
