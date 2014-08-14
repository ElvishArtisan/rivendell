## automake.am
##
## rdcatchd/ Makefile.am for Rivendell
##
## (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
##
##      $Id: Makefile.am,v 1.20.8.1 2012/11/29 01:37:36 cvs Exp $
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

bin_PROGRAMS = rdcatchd

dist_rdcatchd_SOURCES = batch.cpp\
                        catch_event.cpp catch_event.h\
                        local_macros.cpp\
                        rdcatchd.cpp rdcatchd.h\
                        rdcatchd_socket.cpp rdcatchd_socket.h

nodist_rdcatchd_SOURCES = moc_rdcatchd.cpp\
                          moc_rdcatchd_socket.cpp

rdcatchd_LDADD = @LIB_RDLIBS@ @LIBVORBIS@

CLEANFILES = *~\
             moc_*

DISTCLEANFILES = moc_*

MAINTAINERCLEANFILES = *~\
                       *.tar.gz\
                       aclocal.m4\
                       configure\
                       Makefile.in\
                       moc_*
