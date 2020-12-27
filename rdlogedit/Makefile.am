## Makefile.am
##
## Use automake to process this into a Makefile.in
##
##   (C) Copyright 2002-2020 Fred Gleason <fredg@paravelsystems.com>
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

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -I$(top_srcdir)/lib @QT4_CFLAGS@ @MUSICBRAINZ_CFLAGS@ -DQT3_SUPPORT -I/usr/include/Qt3Support
LIBS = -L$(top_srcdir)/lib
MOC = @QT_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

# I18N Stuff
install-exec-local:	
	mkdir -p $(DESTDIR)$(prefix)/share/rivendell
	cp rdlogedit_*.qm $(DESTDIR)$(prefix)/share/rivendell

uninstall-local:	
	rm -f $(DESTDIR)$(prefix)/share/rivendell/rdlogedit_*.qm

all:	
	lupdate-qt4 rdlogedit.pro
	lrelease-qt4 rdlogedit.pro

bin_PROGRAMS = rdlogedit

dist_rdlogedit_SOURCES = add_meta.cpp add_meta.h\
                         edit_chain.cpp edit_chain.h\
                         edit_event.cpp edit_event.h\
                         edit_log.cpp edit_log.h\
                         edit_logline.cpp edit_logline.h\
                         edit_marker.cpp edit_marker.h\
                         edit_track.cpp edit_track.h\
                         list_reports.cpp list_reports.h\
                         logmodel.cpp logmodel.h\
                         logtableview.cpp logtableview.h\
                         rdlogedit.cpp rdlogedit.h globals.h\
                         render_dialog.cpp render_dialog.h\
                         voice_tracker.cpp voice_tracker.h

nodist_rdlogedit_SOURCES = moc_add_meta.cpp\
                           moc_edit_chain.cpp\
                           moc_edit_event.cpp\
                           moc_edit_log.cpp\
                           moc_edit_logline.cpp\
                           moc_edit_marker.cpp\
                           moc_edit_track.cpp\
                           moc_list_reports.cpp\
                           moc_logmodel.cpp\
                           moc_logtableview.cpp\
                           moc_rdlogedit.cpp\
                           moc_render_dialog.cpp\
                           moc_voice_tracker.cpp

rdlogedit_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT4_LIBS@ @MUSICBRAINZ_LIBS@ -lQt3Support

EXTRA_DIST = rdlogedit.pro\
             rdlogedit_cs.ts\
             rdlogedit_de.ts\
             rdlogedit_es.ts\
             rdlogedit_fr.ts\
             rdlogedit_nb.ts\
             rdlogedit_nn.ts\
             rdlogedit_pt_BR.ts

CLEANFILES = *~\
             *.exe\
             *.idb\
             *ilk\
             *.obj\
             *.pdb\
             *.qm\
             moc_*

MAINTAINERCLEANFILES = *~\
                       Makefile.in\
                       moc_*
