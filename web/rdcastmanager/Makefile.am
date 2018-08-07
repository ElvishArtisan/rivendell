## Makefile.am
##
##   (C) Copyright 2002-2007,2016-2018 Fred Gleason <fredg@paravelsystems.com>
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

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -I$(top_srcdir)/lib @QT4_CFLAGS@ -DQT3_SUPPORT -I/usr/include/Qt3Support
LIBS = -L$(top_srcdir)/lib
MOC = @QT_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

install-exec-am:	
	mkdir -p $(DESTDIR)/$(libexecdir)
	cp .libs/rdcastmanager.cgi $(DESTDIR)$(libexecdir)/
	cat rdcastmanager.js | $(top_srcdir)/helpers/jsmin > $(DESTDIR)$(libexecdir)/rdcastmanager.js

uninstall-local:	
	rm -f $(DESTDIR)/$(libexecdir)/rdcastmanager.cgi
	rm -f $(DESTDIR)/$(libexecdir)/rdcastmanager.js

libexec_PROGRAMS = rdcastmanager.cgi

dist_rdcastmanager_cgi_SOURCES = rdcastmanager.cpp rdcastmanager.h

rdcastmanager_cgi_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT4_LIBS@ -lQt3Support

EXTRA_DIST = rdcastmanager.js\
             rdcastmanager.pro

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
