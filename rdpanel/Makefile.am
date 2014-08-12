## automake.am
##
## Automake.am for rivendell/rdpanel
##
## (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
##
##      $Id: Makefile.am,v 1.18.8.2 2013/01/01 21:36:32 cvs Exp $
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
	cp rdpanel_*.qm $(DESTDIR)$(prefix)/share/rivendell

uninstall:	
	rm -f $(DESTDIR)$(prefix)/share/rivendell/rdpanel_*.qm

all:	
	@QT_BIN@/lupdate rdpanel.pro
	@QT_BIN@/lrelease rdpanel.pro

bin_PROGRAMS = rdpanel

dist_rdpanel_SOURCES = globals.h\
                       rdpanel.cpp rdpanel.h

nodist_rdpanel_SOURCES = moc_rdpanel.cpp

rdpanel_LDADD = @LIB_RDLIBS@ @LIBVORBIS@

EXTRA_DIST = rdpanel.pro\
             rdpanel_cs.ts\
             rdpanel_de.ts\
             rdpanel_es.ts\
             rdpanel_fr.ts\
             rdpanel_pt_BR.ts

CLEANFILES = *~\
             *.qm\
             moc_*

MAINTAINERCLEANFILES = *~\
                       *.tar.gz\
                       aclocal.m4\
                       configure\
                       Makefile.in\
                       moc_*
