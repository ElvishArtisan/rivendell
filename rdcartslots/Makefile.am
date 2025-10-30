## Makefile.am
##
## Automake.am for rivendell/rdcartslots
##
## (C) Copyright 2012-2022 Fred Gleason <fredg@paravelsystems.com>
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

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -Wno-strict-aliasing -std=c++11 -fPIC -I$(top_srcdir)/lib @QT5_CFLAGS@ @MUSICBRAINZ_CFLAGS@ @IMAGEMAGICK_CFLAGS@
LIBS = -L$(top_srcdir)/lib
MOC = @QT_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

# I18N Stuff
install-exec-local:	
	../helpers/rdi18n_helper.sh --install rdcartslots $(DESTDIR)$(prefix)/share/rivendell

uninstall-local:	
	../helpers/rdi18n_helper.sh --uninstall rdcartslots $(DESTDIR)$(prefix)/share/rivendell

all:	
	../helpers/rdi18n_helper.sh --update rdcartslots $(DESTDIR)$(prefix)/share/rivendell

bin_PROGRAMS = rdcartslots

dist_rdcartslots_SOURCES = local_macros.cpp\
                           rdcartslots.cpp rdcartslots.h

nodist_rdcartslots_SOURCES = moc_rdcartslots.cpp

rdcartslots_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT5_LIBS@ @MUSICBRAINZ_LIBS@ @IMAGEMAGICK_LIBS@

EXTRA_DIST = rdcartslots.pro\
             rdcartslots_de.ts\
             rdcartslots_es.ts\
             rdcartslots_fr.ts\
             rdcartslots_nb.ts\
             rdcartslots_nn.ts\
             rdcartslots_pt_BR.ts

CLEANFILES = *~\
             moc_*\
             *.qm

MAINTAINERCLEANFILES = *~\
                       Makefile.in\
                       moc_*
