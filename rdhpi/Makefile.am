## Makefile.am
##
## Automake.am for rivendell/mlhpi
##
## by Fred Gleason <fredg@paravelsystems.com>
##
##   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -Wno-strict-aliasing -std=c++11 -fPIC -I$(top_srcdir)/lib @QT5_CFLAGS@ @IMAGEMAGICK_CFLAGS@
LIBS = -L$(top_srcdir)/lib
MOC = @QT_MOC@
CWRAP = ../helpers/cwrap

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

# The cwrap dependency
html_%.cpp:	%.html
	$(CWRAP) -o $@ $<

# I18N Stuff
install-exec-local:	
	../helpers/rdi18n_helper.sh --install rdhpi $(DESTDIR)$(prefix)/share/rivendell

uninstall-local:	
	../helpers/rdi18n_helper.sh --uninstall rdhpi $(DESTDIR)$(prefix)/share/rivendell

all:	
	../helpers/rdi18n_helper.sh --update rdhpi $(DESTDIR)$(prefix)/share/rivendell

lib_LTLIBRARIES = librdhpi.la
dist_librdhpi_la_SOURCES = rdhpiinformation.cpp rdhpiinformation.h\
                           rdhpiplaystream.cpp rdhpiplaystream.h\
                           rdhpirecordstream.cpp rdhpirecordstream.h\
                           rdhpisoundcard.cpp rdhpisoundcard.h

nodist_librdhpi_la_SOURCES = moc_rdhpiplaystream.cpp\
                             moc_rdhpirecordstream.cpp\
                             moc_rdhpisoundcard.cpp

librdhpi_la_LDFLAGS = -release $(VERSION)

EXTRA_DIST = rdhpi.pro\
             rdhpi_cs.ts\
             rdhpi_de.ts\
             rdhpi_es.ts\
             rdhpi_fr.ts\
             rdhpi_nb.ts\
             rdhpi_nn.ts\
             rdhpi_pt_BR.ts

CLEANFILES = *~\
             *.idb\
             *.ilk\
             *.lib\
             *.obj\
             *.pdb\
             *.qm\
             moc_*

DISTCLEAN = Makefile

MAINTAINERCLEANFILES = *~\
                       Makefile\
                       Makefile.in\
                       moc_*
