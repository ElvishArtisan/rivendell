## Makefile.am
##
##   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -I$(top_srcdir)/lib @QT4_CFLAGS@ @MUSICBRAINZ_CFLAGS@
LIBS = -L$(top_srcdir)/lib
MOC = @QT_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

# I18N Stuff
install-exec-local:	
	mkdir -p $(DESTDIR)$(prefix)/share/rivendell
	cp rddgimport_*.qm $(DESTDIR)$(prefix)/share/rivendell

uninstall-local:	
	rm -f $(DESTDIR)$(prefix)/share/rivendell/rddgimport_*.qm

all:	
	lupdate-qt4 rddgimport.pro
	lrelease-qt4 rddgimport.pro

bin_PROGRAMS = rddgimport

dist_rddgimport_SOURCES = event.cpp event.h\
                          rddgimport.cpp rddgimport.h

nodist_rddgimport_SOURCES = moc_rddgimport.cpp

rddgimport_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT4_LIBS@ @MUSICBRAINZ_LIBS@

EXTRA_DIST = rddgimport.pro\
             rddgimport_de.ts\
             rddgimport_es.ts\
             rddgimport_fr.ts\
             rddgimport_nb.ts\
             rddgimport_nn.ts\
             rddgimport_pt_BR.ts

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
