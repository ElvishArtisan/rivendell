## Makefile.am
##
## Automake.am for rivendell/rdcatch
##
## (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -Wno-strict-aliasing -std=c++11 -fPIC -I$(top_srcdir)/lib @QT5_CFLAGS@ @MUSICBRAINZ_CFLAGS@
LIBS = -L$(top_srcdir)/lib
MOC = @QT_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

# I18N Stuff
install-exec-local:	
	../helpers/rdi18n_helper.sh --install rdcatch $(DESTDIR)$(prefix)/share/rivendell

uninstall-local:	
	../helpers/rdi18n_helper.sh --uninstall rdcatch $(DESTDIR)$(prefix)/share/rivendell

all:	
	../helpers/rdi18n_helper.sh --update rdcatch $(DESTDIR)$(prefix)/share/rivendell

bin_PROGRAMS = rdcatch

dist_rdcatch_SOURCES = add_recording.cpp add_recording.h\
                       catch_monitor.cpp catch_monitor.h\
                       catchtableview.cpp catchtableview.h\
                       colors.h\
                       deckmon.cpp deckmon.h\
                       dowselector.cpp dowselector.h\
                       edit_cartevent.cpp edit_cartevent.h\
                       edit_download.cpp edit_download.h\
                       edit_playout.cpp edit_playout.h\
                       edit_recording.cpp edit_recording.h\
                       edit_switchevent.cpp edit_switchevent.h\
                       edit_upload.cpp edit_upload.h\
                       eventlight.cpp eventlight.h\
                       eventwidget.cpp eventwidget.h\
                       globals.h\
                       list_reports.cpp list_reports.h\
                       rdcatch.cpp rdcatch.h\
                       recordlistmodel.cpp recordlistmodel.h\
                       vbox.cpp vbox.h

nodist_rdcatch_SOURCES = moc_add_recording.cpp\
                         moc_catchtableview.cpp\
                         moc_deckmon.cpp\
                         moc_dowselector.cpp\
                         moc_edit_cartevent.cpp\
                         moc_edit_download.cpp\
                         moc_edit_playout.cpp\
                         moc_edit_recording.cpp\
                         moc_edit_switchevent.cpp\
                         moc_edit_upload.cpp\
                         moc_eventlight.cpp\
                         moc_eventwidget.cpp\
                         moc_list_reports.cpp\
                         moc_rdcatch.cpp\
                         moc_recordlistmodel.cpp\
                         moc_vbox.cpp


rdcatch_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT5_LIBS@ @MUSICBRAINZ_LIBS@


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
