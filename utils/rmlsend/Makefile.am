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

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -Wno-strict-aliasing -std=c++11 -fPIC -I$(top_srcdir)/lib @QT5_CFLAGS@ @MUSICBRAINZ_CFLAGS@
LIBS = -L$(top_srcdir)/lib
MOC = @QT_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

# I18N Stuff
install-exec-local:	
	mkdir -p $(DESTDIR)$(prefix)/share/rivendell
	cp rmlsend_*.qm $(DESTDIR)$(prefix)/share/rivendell

uninstall-local:	
	rm -f $(DESTDIR)$(prefix)/share/rivendell/rmlsend_*.qm

all:	
	lupdate-qt4 rmlsend.pro
	lrelease-qt4 rmlsend.pro

bin_PROGRAMS = rmlsend

dist_rmlsend_SOURCES = rmlsend.cpp rmlsend.h

nodist_rmlsend_SOURCES = moc_rmlsend.cpp

rmlsend_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT5_LIBS@ @MUSICBRAINZ_LIBS@

EXTRA_DIST = rmlsend.pro\
             rmlsend_cs.ts\
             rmlsend_de.ts\
             rmlsend_es.ts\
             rmlsend_fr.ts\
             rmlsend_nb.ts\
             rmlsend_nn.ts\
             rmlsend_pt_BR.ts

CLEANFILES = *~\
             *.exe\
             *.idb\
             *ilk\
             *.qm\
             *.obj\
             *.pdb\
             moc_*

MAINTAINERCLEANFILES = *~\
                       Makefile.in\
                       moc_*
