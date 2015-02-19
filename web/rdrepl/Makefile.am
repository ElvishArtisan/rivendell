## automake.am
##
## Automake.am for rivendell/web/rdrepl
##
##   (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
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

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -DQTDIR=\"@QT_DIR@\" @QT_CXXFLAGS@ -I$(top_srcdir)/lib
LIBS = @QT_LIBS@ -L$(top_srcdir)/lib
MOC = @QT_MOC@

libexec_PROGRAMS = rdrepl.cgi

install-exec-hook:	
	if test -z $(DESTDIR) ; then chown root $(DESTDIR)$(libexecdir)/rdrepl.cgi ; chmod 4755 $(DESTDIR)$(libexecdir)/rdrepl.cgi ; fi

dist_rdrepl_cgi_SOURCES = ack.cpp\
                          export.cpp\
                          import.cpp\
                          rdrepl.cpp rdrepl.h

rdrepl_cgi_LDADD = @LIB_RDLIBS@ -lsndfile @LIBVORBIS@

EXTRA_DIST = rdrepl.pro

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
