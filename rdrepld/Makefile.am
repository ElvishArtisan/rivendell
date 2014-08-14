## automake.am
##
## rdrepld/ Makefile.am for Rivendell
##
## (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
##
##      $Id: Makefile.am,v 1.2 2010/07/29 19:32:37 cvs Exp $
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

AM_CPPFLAGS = -Wall @QT_CXXFLAGS@ -I$(top_srcdir)/lib
LIBS = @QT_LIBS@ -L$(top_srcdir)/lib
MOC = @QT_MOC@

# QT's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

bin_PROGRAMS = rdrepld
dist_rdrepld_SOURCES = globals.h\
                       rdrepld.cpp rdrepld.h \
                       replconfig.cpp replconfig.h\
                       replfactory.cpp replfactory.h\
                       citadelxds.cpp citadelxds.h
nodist_rdrepld_SOURCES = moc_rdrepld.cpp
rdrepld_LDADD = @LIB_RDLIBS@ @LIBVORBIS@

CLEANFILES = *~ moc_*
DISTCLEANFILES = moc_*
MAINTAINERCLEANFILES = *~ Makefile.in configure aclocal.m4 moc_* *.tar.gz
