## Makefile.am
##
##   (C) Copyright 2018-2020 Fred Gleason <fredg@paravelsystems.com>
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

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" @QT4_CFLAGS@ @MUSICBRAINZ_CFLAGS@
MOC = @QT_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

install-exec-hook:
	chmod 4755 $(DESTDIR)$(prefix)/bin/rdselect_helper

bin_PROGRAMS = rdselect_helper

dist_rdselect_helper_SOURCES = rdselect_helper.cpp rdselect_helper.h

nodist_rdselect_helper_SOURCES = moc_rdselect_helper.cpp\
                                 rd.h\
                                 rdconfig.cpp rdconfig.h\
                                 rdprofile.cpp rdprofile.h

rdselect_helper_LDADD = -lQtCore -lQtNetwork -lQt3Support @MUSICBRAINZ_LIBS@

CLEANFILES = *~\
             *.idb\
             *ilk\
             *.obj\
             *.pdb\
             *.qm\
             moc_*

DISTCLEANFILES = rd.h\
                 rdconfig.cpp rdconfig.h\
                 rdprofile.cpp rdprofile.h

MAINTAINERCLEANFILES = *~\
                       Makefile.in\
                       moc_*
