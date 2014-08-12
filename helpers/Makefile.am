## automake.am
##
## helper/ Automake.am for Rivendell
##
## (C) Copyright 2003-2005 Fred Gleason <fredg@paravelsystems.com>
##
##      $Id: Makefile.am,v 1.9.6.1 2012/11/29 01:37:35 cvs Exp $
##      $Date: 2012/11/29 01:37:35 $
##
## Use automake to process this into a Makefile.in
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


AM_CFLAGS = -Wall

noinst_PROGRAMS = cwrap\
                  jsmin

noinst_SCRIPTS = rdpack.sh\
                 rdtrans.sh

dist_cwrap_SOURCES = cwrap.cpp cwrap.h

dist_jsmin_SOURCES = jsmin.c

EXTRA_DIST = rdpack.sh\
             rdtrans.sh\
             rdtransgui.sh\
             setenvvar.sh\
             win32_frag1.txt\
             win32_frag2.txt

CLEANFILES = *~\
             *.tar.gz\
             moc_*

MAINTAINERCLEANFILES = *~\
                       *.tar.gz\
                       aclocal.m4\
                       configure\
                       Makefile.in\
                       moc_*
