## Makefile.am
##
## (C) Copyright 2009,2016-2018 Fred Gleason <fredg@paravelsystems.com>
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
##   Use automake to process this into a Makefile.in
##

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -I$(top_srcdir)/lib -I$(top_srcdir)/rdhpi @QT4_CFLAGS@ -DQT3_SUPPORT -I/usr/include/Qt3Support
LIBS = -L$(top_srcdir)/lib -L$(top_srcdir)/rdhpi
MOC = @QT_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

bin_PROGRAMS = rddbconfig

dist_rddbconfig_SOURCES = rddbconfig.cpp rddbconfig.h mysql_login.cpp mysql_login.h \
	db.cpp db.h createdb.cpp createdb.h

nodist_rddbconfig_SOURCES = moc_rddbconfig.cpp moc_mysql_login.cpp

rddbconfig_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @LIBALSA@ @QT4_LIBS@ -lQt3Support

EXTRA_DIST = rddbconfig.pro

CLEANFILES = *~\
             *.qm\
             moc_*

MAINTAINERCLEANFILES = *~\
                       *.tar.gz\
                       aclocal.m4\
                       configure\
                       Makefile.in\
                       moc_*
