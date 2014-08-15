## automake.am
##
## Automake.am for rivendell/rdselect
##
## (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
##
##      $Id: Makefile.am,v 1.1.2.4 2013/01/01 21:36:32 cvs Exp $
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

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -DQTDIR=\"@QT_DIR@\" @QT_CXXFLAGS@ -I$(top_srcdir)/lib
LIBS = @QT_LIBS@ -L$(top_srcdir)/lib
MOC = @QT_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

# I18N Stuff
install-exec-local:	
	mkdir -p $(DESTDIR)$(prefix)/share/rivendell
	cp rdselect_*.qm $(DESTDIR)$(prefix)/share/rivendell
	chmod 4755 $(DESTDIR)$(prefix)/bin/rdselect

uninstall:
	rm -f $(DESTDIR)$(prefix)/share/rivendell/rdselect_*.qm

all:	
	@QT_BIN@/lupdate rdselect.pro
	@QT_BIN@/lrelease rdselect.pro

bin_PROGRAMS = rdselect

dist_rdselect_SOURCES = rdselect.cpp rdselect.h

nodist_rdselect_SOURCES = moc_rdselect.cpp

rdselect_LDADD = @LIB_RDLIBS@ @LIBVORBIS@

EXTRA_DIST = rdselect.pro\
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
