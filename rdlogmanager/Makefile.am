## Makefile.am
##
## (C) Copyright 2002-2020 Fred Gleason <fredg@paravelsystems.com>
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

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -I$(top_srcdir)/lib @QT4_CFLAGS@ @MUSICBRAINZ_CFLAGS@ -DQT3_SUPPORT -I/usr/include/Qt3Support
LIBS = -L$(top_srcdir)/lib
MOC = @QT_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

# I18N Stuff
install-exec-local:	
	mkdir -p $(DESTDIR)$(prefix)/share/rivendell
	cp rdlogmanager_*.qm $(DESTDIR)$(prefix)/share/rivendell

uninstall-local:	
	rm -f $(DESTDIR)$(prefix)/share/rivendell/rdlogmanager_*.qm

all:	
	lupdate-qt4 rdlogmanager.pro
	lrelease-qt4 rdlogmanager.pro

bin_PROGRAMS = rdlogmanager

dist_rdlogmanager_SOURCES = add_clock.cpp add_clock.h\
                            add_event.cpp add_event.h\
                            clock_listview.cpp clock_listview.h\
                            clocklistmodel.cpp clocklistmodel.h\
                            commandline_ops.cpp globals.h\
                            edit_clock.cpp edit_clock.h\
                            edit_event.cpp edit_event.h\
                            edit_eventline.cpp edit_eventline.h\
                            edit_grid.cpp edit_grid.h\
                            edit_note.cpp edit_note.h\
                            edit_perms.cpp edit_perms.h\
                            edit_schedrules.cpp edit_schedrules.h\
                            edit_schedcoderules.cpp edit_schedcoderules.h\
                            edit_track.cpp edit_track.h\
                            eventlistmodel.cpp eventlistmodel.h\
                            generate_log.cpp generate_log.h\
                            importcartsmodel.cpp importcartsmodel.h\
                            importcartsview.cpp importcartsview.h\
                            librarytableview.cpp librarytableview.h\
                            list_clocks.cpp list_clocks.h\
                            list_events.cpp list_events.h\
                            list_grids.cpp list_grids.h\
                            list_svcs.cpp list_svcs.h\
                            logobject.cpp logobject.h\
                            pick_report_dates.cpp pick_report_dates.h\
                            rdlogmanager.cpp rdlogmanager.h\
                            rename_item.cpp rename_item.h\
                            svc_rec.cpp svc_rec.h\
                            svc_rec_dialog.cpp svc_rec_dialog.h\
                            viewreportdialog.cpp viewreportdialog.h

nodist_rdlogmanager_SOURCES = moc_add_clock.cpp\
                              moc_add_event.cpp\
                              moc_clock_listview.cpp\
                              moc_clocklistmodel.cpp\
                              moc_edit_clock.cpp\
                              moc_edit_event.cpp\
                              moc_edit_eventline.cpp\
                              moc_edit_grid.cpp\
                              moc_edit_note.cpp\
                              moc_edit_perms.cpp\
                              moc_edit_schedrules.cpp\
                              moc_edit_schedcoderules.cpp\
                              moc_edit_track.cpp\
                              moc_eventlistmodel.cpp\
                              moc_generate_log.cpp\
                              moc_importcartsmodel.cpp\
                              moc_importcartsview.cpp\
                              moc_librarytableview.cpp\
                              moc_list_clocks.cpp\
                              moc_list_events.cpp\
                              moc_list_grids.cpp\
                              moc_list_svcs.cpp\
                              moc_logobject.cpp\
                              moc_pick_report_dates.cpp\
                              moc_rdlogmanager.cpp\
                              moc_rename_item.cpp\
                              moc_svc_rec.cpp\
                              moc_svc_rec_dialog.cpp\
                              moc_viewreportdialog.cpp

rdlogmanager_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT4_LIBS@ @MUSICBRAINZ_LIBS@ -lQt3Support

EXTRA_DIST = rdlogmanager.pro\
             rdlogmanager_cs.ts\
             rdlogmanager_de.ts\
             rdlogmanager_es.ts\
             rdlogmanager_fr.ts\
             rdlogmanager_nb.ts\
             rdlogmanager_nn.ts\
             rdlogmanager_pt_BR.ts

CLEANFILES = *~\
             *.exe\
             *.idb\
             *ilk\
             *.qm\
             *.obj\
             *.pdb\
             moc_*

MAINTAINERCLEANFILES = *~\
                       *.tar.gz\
                       aclocal.m4\
                       configure\
                       Makefile.in\
                       moc_*
