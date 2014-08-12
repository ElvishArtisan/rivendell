## automake.am
##
## Automake.am for rivendell/utils/rddiscimport
##
##   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
##
##      $Id: Makefile.am,v 1.1.2.1 2013/12/03 23:34:35 cvs Exp $
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
	cp rddiscimport_*.qm $(DESTDIR)$(prefix)/share/rivendell

uninstall:
	rm -f $(DESTDIR)$(prefix)/share/rivendell/rddiscimport_*.qm

all:	
	@QT_BIN@/lupdate rddiscimport.pro
	@QT_BIN@/lrelease rddiscimport.pro

bin_PROGRAMS = rddiscimport

dist_rddiscimport_SOURCES = metalibrary.cpp metalibrary.h\
                          metarecord.cpp metarecord.h\
                          rddiscimport.cpp rddiscimport.h

nodist_rddiscimport_SOURCES = moc_rddiscimport.cpp

rddiscimport_LDADD = @LIB_RDLIBS@ @LIBVORBIS@

EXTRA_DIST = rddiscimport.pro\
             rddiscimport_de.ts\
             rddiscimport_es.ts\
             rddiscimport_fr.ts\
             rddiscimport_nb.ts\
             rddiscimport_nn.ts\
             rddiscimport_pt_BR.ts

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
