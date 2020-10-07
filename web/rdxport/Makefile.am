## Makefile.am
##
##   (C) Copyright 2010-2020 Fred Gleason <fredg@paravelsystems.com>
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

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

libexec_PROGRAMS = rdxport.cgi

install-exec-hook:	
	if test -z $(DESTDIR) ; then chown root $(DESTDIR)$(libexecdir)/rdxport.cgi ; chmod 4755 $(DESTDIR)$(libexecdir)/rdxport.cgi ; fi

dist_rdxport_cgi_SOURCES = audioinfo.cpp\
                           audiostore.cpp\
                           carts.cpp\
                           copyaudio.cpp\
                           deleteaudio.cpp\
                           groups.cpp\
                           export.cpp\
                           exportpeaks.cpp\
                           import.cpp\
                           logs.cpp\
                           podcasts.cpp\
                           rdxport.cpp rdxport.h\
                           rehash.cpp\
                           tests.cpp\
                           schedcodes.cpp\
                           services.cpp\
                           systemsettings.cpp\
                           trimaudio.cpp

nodist_rdxport_cgi_SOURCES = moc_rdxport.cpp

rdxport_cgi_LDADD = @LIB_RDLIBS@ -lsndfile @LIBVORBIS@ @QT4_LIBS@ @MUSICBRAINZ_LIBS@ -lQt3Support

EXTRA_DIST = rdxport.pro

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
