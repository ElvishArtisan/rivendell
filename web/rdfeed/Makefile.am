## Makefile.am
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
## Use automake to process this into a Makefile.in

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -I$(top_srcdir)/lib @QT4_CFLAGS@ @MUSICBRAINZ_CFLAGS@ -DQT3_SUPPORT -I/usr/include/Qt3Support
LIBS = -L$(top_srcdir)/lib
MOC = @QT_MOC@

install-exec-local:	
	if test ! -f $(DESTDIR)@libexecdir@/rdfeed.mp3 ; then ln -s @libexecdir@/rdfeed.xml $(DESTDIR)@libexecdir@/rdfeed.mp3 ; fi

uninstall-local:	
	rm -f $(DESTDIR)@libexecdir@/rdfeed.mp3

libexec_PROGRAMS = rdfeed.xml

dist_rdfeed_xml_SOURCES = rdfeed_script.cpp rdfeed_script.h

rdfeed_xml_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT4_LIBS@ @MUSICBRAINZ_LIBS@ -lQt3Support

EXTRA_DIST = rdfeed.pro

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
