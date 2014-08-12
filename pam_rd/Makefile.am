## automake.am
##
## Automake.am for rivendell/pam_rd
##
## by Federico Grau <grauf@rfa.org> <donfede@casagrau.org>
## based on work by Fred Gleason <fredg@paravelsystems.com> and from 
## libpam-ldap-178
##
##      $Id: Makefile.am,v 1.9.8.1 2012/11/29 01:37:35 cvs Exp $
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

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -DQTDIR=\"@QT_DIR@\" -fPIC @QT_CXXFLAGS@
INCLUDES = -I$(top_srcdir)/lib
LIBS = @QT_LIBS@ -L$(top_srcdir)/lib
MOC = @QT_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

inst_lib_LTLIBRARIES = pam_rd.la
inst_libdir=/@RD_LIB_PATH@/security
dist_pam_rd_la_SOURCES = pam_rd.cpp
pam_rd_la_LDFLAGS = -module -shared -Xlinker -x -avoid-version -export-dynamic
pam_rd_la_LIBADD = @LIB_RDLIBS@ -lpam @LIBVORBIS@

CLEANFILES = *~\
             *.qm\
             moc_*

DISTCLEANFILES = Makefile.in
