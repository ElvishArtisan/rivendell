## Makefile.am
##
## Makefile.am for rivendell/apis/rivwebcapi
##
##   (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
##
##   This program is free software; you can redistribute it and/or modify
##   it under the terms of the GNU General Public License as
##   published by the Free Software Foundation; either version 2 of
##   the License, or (at your option) any later version.
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

SUBDIRS = rivwebcapi \
          tests

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = rivwebcapi.pc

EXTRA_DIST = acinclude.m4\
             autogen.sh\
             configure.ac\
             get_distro.pl\
             get_target.sh

CLEANFILES = *~\
             *.idb\
             *ilk\
             *.obj\
             *.pdb\
             *.qm\
             moc_*

DISTCLEANFILES = rivwebcapi.pc

MAINTAINERCLEANFILES = *~\
                       *.tar.gz\
                       aclocal.m4\
                       config.guess\
                       config.sub\
                       configure\
                       depcomp\
                       install-sh\
                       ltmain.sh\
                       Makefile.in\
                       missing\
                       moc_*
