## automake.am
##
## Automake.am for rivendell/utils/rmlsend
##
##   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
##
##      $Id: Makefile.am,v 1.12.8.3 2013/01/01 21:36:33 cvs Exp $
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

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -DQTDIR=\"@QT_DIR@\" @QT_CXXFLAGS@
INCLUDES = -I$(top_srcdir)/lib
LIBS = @QT_LIBS@ -L$(top_srcdir)/lib
MOC = @QT_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

# I18N Stuff
install-exec-local:	
	mkdir -p $(DESTDIR)$(prefix)/share/rivendell
	cp rmlsend_*.qm $(DESTDIR)$(prefix)/share/rivendell

uninstall:
	rm -f $(DESTDIR)$(prefix)/share/rivendell/rmlsend_*.qm

all:	
	@QT_BIN@/lupdate rmlsend.pro
	@QT_BIN@/lrelease rmlsend.pro

bin_PROGRAMS = rmlsend

dist_rmlsend_SOURCES = rmlsend.cpp rmlsend.h

nodist_rmlsend_SOURCES = moc_rmlsend.cpp

rmlsend_LDADD = @LIB_RDLIBS@ @LIBVORBIS@

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
