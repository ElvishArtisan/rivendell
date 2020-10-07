## automake.am
##
## Automake.am for rivendell/rdcastmanager
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
	cp rdcastmanager_*.qm $(DESTDIR)$(prefix)/share/rivendell

uninstall-local:	
	rm -f $(DESTDIR)$(prefix)/share/rivendell/rdcastmanager_*.qm

all:	
	lupdate-qt4 rdcastmanager.pro
	lrelease-qt4 rdcastmanager.pro

bin_PROGRAMS = rdcastmanager

dist_rdcastmanager_SOURCES = edit_cast.cpp edit_cast.h\
                             globals.h\
                             list_casts.cpp list_casts.h\
                             logdialog.cpp logdialog.h\
                             logmodel.cpp logmodel.h\
                             rdcastmanager.cpp rdcastmanager.h\
                             render_dialog.cpp render_dialog.h

nodist_rdcastmanager_SOURCES = moc_edit_cast.cpp\
                               moc_list_casts.cpp\
                               moc_logdialog.cpp\
                               moc_logmodel.cpp\
                               moc_rdcastmanager.cpp\
                               moc_render_dialog.cpp

rdcastmanager_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT4_LIBS@ @MUSICBRAINZ_LIBS@ -lQt3Support

EXTRA_DIST = rdcastmanager.pro\
             rdcastmanager_cs.ts\
             rdcastmanager_de.ts\
             rdcastmanager_es.ts\
             rdcastmanager_fr.ts\
             rdcastmanager_nb.ts\
             rdcastmanager_nn.ts\
             rdcastmanager_pt_BR.ts

CLEANFILES = *~\
             *.idb\
             *ilk\
             *.obj\
             *.pdb\
             *.qm\
             moc_*

MAINTAINERCLEANFILES = *~\
                       *.tar.gz\
                       aclocal.m4\
                       configure\
                       Makefile.in\
                       moc_*
