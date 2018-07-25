## automake.am
##
## Makefile.am for rivendell/drivers/qt3-mysql-utf
##
##  by Fred Gleason <fredg@paravelsystems.com>
##   (C) Copyright 2002-2007,2016 Fred Gleason <fredg@paravelsystems.com>
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
##  Use automake to process this into a Makefile.in
##

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -DQTDIR=\"@QT_DIR@\" @QT_CXXFLAGS@ @MYSQL_CLIENT_CFLAGS@
LIBS = @QT_LIBS@ 
MOC = @QT_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

install-exec-local:	
	mkdir -p $(DESTDIR)$(QTDIR)/plugins/sqldrivers
	cp .libs/libqmysqlutf.so.0.0.0 $(DESTDIR)$(QTDIR)/plugins/sqldrivers/libqmysqlutf.so

uninstall-exec:
	rm -f $(DESTDIR)$(QTDIR)/plugins/sqldrivers/libqmysqlutf.so

lib_LTLIBRARIES = libqmysqlutf.la
dist_libqmysqlutf_la_SOURCES = qsqlextension.cpp qsqlextension.h\
                               qt3-mysql-utf.cpp qt3-mysql-utf.h

libqmysqlutf_la_LIBADD = @MYSQL_CLIENT_LIBS@
libqmysqlutf_la_LDFLAGS = -rpath `pwd`

EXTRA_DIST = LICENSE.QPL\
             qt3-mysql-utf.pro

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
