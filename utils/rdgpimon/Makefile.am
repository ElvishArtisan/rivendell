## Makefile.am
##
##   (C) Copyright 2002-2022 Fred Gleason <fredg@paravelsystems.com>
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

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -Wno-strict-aliasing -std=c++11 -fPIC -I$(top_srcdir)/lib @QT5_CFLAGS@ @MUSICBRAINZ_CFLAGS@ @IMAGEMAGICK_CFLAGS@
LIBS = -L$(top_srcdir)/lib
MOC = @QT_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

# I18N Stuff
install-exec-local:	
	../../helpers/rdi18n_helper.sh --install rdgpimon $(DESTDIR)$(prefix)/share/rivendell

uninstall-local:	
	../../helpers/rdi18n_helper.sh --uninstall rdgpimon $(DESTDIR)$(prefix)/share/rivendell

all:	
	../../helpers/rdi18n_helper.sh --update rdgpimon $(DESTDIR)$(prefix)/share/rivendell

bin_PROGRAMS = rdgpimon

dist_rdgpimon_SOURCES = gpi_label.cpp gpi_label.h\
                        rdgpimon.cpp rdgpimon.h

nodist_rdgpimon_SOURCES = moc_gpi_label.cpp\
                          moc_rdgpimon.cpp

rdgpimon_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT5_LIBS@ @MUSICBRAINZ_LIBS@ @IMAGEMAGICK_LIBS@

EXTRA_DIST = rdgpimon.pro\
             rdgpimon_cs.ts\
             rdgpimon_de.ts\
             rdgpimon_es.ts\
             rdgpimon_fr.ts\
             rdgpimon_nb.ts\
             rdgpimon_nn.ts\
             rdgpimon_pt_BR.ts

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
