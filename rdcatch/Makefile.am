## automake.am
##
## Automake.am for rivendell/rdcatch
##
## (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
##
##      $Id: Makefile.am,v 1.40.8.2 2013/01/01 21:36:30 cvs Exp $
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
	cp rdcatch_*.qm $(DESTDIR)$(prefix)/share/rivendell

uninstall:
	rm -f $(DESTDIR)$(prefix)/share/srlabs/rdcatch_*.qm

all:	
	@QT_BIN@/lupdate rdcatch.pro
	@QT_BIN@/lrelease rdcatch.pro

bin_PROGRAMS = rdcatch

dist_rdcatch_SOURCES = add_recording.cpp add_recording.h\
                       catch_monitor.cpp catch_monitor.h\
                       catch_listview.cpp catch_listview.h\
                       colors.h\
                       deckmon.cpp deckmon.h\
                       edit_cartevent.cpp edit_cartevent.h\
                       edit_download.cpp edit_download.h\
                       edit_playout.cpp edit_playout.h\
                       edit_recording.cpp edit_recording.h\
                       edit_switchevent.cpp edit_switchevent.h\
                       edit_upload.cpp edit_upload.h\
                       globals.h\
                       list_reports.cpp list_reports.h\
                       rdcatch.cpp rdcatch.h\
                       vbox.cpp vbox.h

nodist_rdcatch_SOURCES = moc_add_recording.cpp\
                         moc_catch_listview.cpp\
                         moc_deckmon.cpp\
                         moc_edit_cartevent.cpp\
                         moc_edit_download.cpp\
                         moc_edit_playout.cpp\
                         moc_edit_recording.cpp\
                         moc_edit_switchevent.cpp\
                         moc_edit_upload.cpp\
                         moc_list_reports.cpp\
                         moc_rdcatch.cpp\
                         moc_vbox.cpp


rdcatch_LDADD = @LIB_RDLIBS@ @LIBVORBIS@

EXTRA_DIST = rdcatch.pro\
             rdcatch_cs.ts\
             rdcatch_de.ts\
             rdcatch_es.ts\
             rdcatch_fr.ts\
             rdcatch_nb.ts\
             rdcatch_nn.ts\
             rdcatch_pt_BR.ts

CLEANFILES = *~\
             *.qm\
             moc_*

MAINTAINERCLEANFILES = *~\
                       *.tar.gz\
                       aclocal.m4\
                       configure\
                       Makefile.in
