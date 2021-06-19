## automake.am
##
## Automake.am for rivendell/rdlibrary
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

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -Wno-strict-aliasing -std=c++11 -fPIC -I$(top_srcdir)/lib @QT5_CFLAGS@
LIBS = -L$(top_srcdir)/lib
MOC = @QT_MOC@
LUPDATE = @QT_LUPDATE@
LRELEASE = @QT_LRELEASE@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

# I18N Stuff
install-exec-local:	
	mkdir -p $(DESTDIR)$(prefix)/share/rivendell
	cp rdlibrary_*.qm $(DESTDIR)$(prefix)/share/rivendell

uninstall-local:	
	rm -f $(DESTDIR)$(prefix)/share/rivendell/rdlibrary_*.qm

all:	
	$(LUPDATE) rdlibrary.pro
	$(LRELEASE) rdlibrary.pro

bin_PROGRAMS = rdlibrary

dist_rdlibrary_SOURCES = audio_cart.cpp audio_cart.h\
                         audio_controls.h\
                         cdripper.cpp cdripper.h\
                         disk_gauge.cpp disk_gauge.h\
                         disk_ripper.cpp disk_ripper.h\
                         edit_cart.cpp edit_cart.h\
                         edit_macro.cpp edit_macro.h\
                         edit_notes.cpp edit_notes.h\
                         edit_schedulercodes.cpp edit_schedulercodes.h\
                         globals.h\
                         libraryview.cpp libraryview.h\
                         list_reports.cpp list_reports.h\
                         macro_cart.cpp macro_cart.h\
                         mainwindow.cpp mainwindow.h\
                         notebubble.cpp notebubble.h\
                         rdlibrary.cpp rdlibrary.h\
                         record_cut.cpp record_cut.h\
                         validate_cut.cpp validate_cut.h

nodist_rdlibrary_SOURCES = moc_audio_cart.cpp\
                           moc_cdripper.cpp\
                           moc_disk_gauge.cpp\
                           moc_disk_ripper.cpp\
                           moc_edit_cart.cpp\
                           moc_edit_macro.cpp\
                           moc_edit_notes.cpp\
                           moc_edit_schedulercodes.cpp\
                           moc_libraryview.cpp\
                           moc_list_reports.cpp\
                           moc_macro_cart.cpp\
                           moc_mainwindow.cpp\
                           moc_notebubble.cpp\
                           moc_rdlibrary.cpp\
                           moc_record_cut.cpp

rdlibrary_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT5_LIBS@ @MUSICBRAINZ_LIBS@

EXTRA_DIST = rdlibrary.pro\
             rdlibrary_cs.ts\
             rdlibrary_de.ts\
             rdlibrary_es.ts\
             rdlibrary_fr.ts\
             rdlibrary_nb.ts\
             rdlibrary_nn.ts\
             rdlibrary_pt_BR.ts

CLEANFILES = *~\
             *.qm\
             moc_*

MAINTAINERCLEANFILES = *~\
                       *.tar.gz\
                       aclocal.m4\
                       configure\
                       Makefile.in\
                       moc_*
