## automake.am
##
## Automake.am for rivendell/rlm
##
##   (C) Copyright 2008 Fred Gleason <fredg@paravelsystems.com>
##
##      $Id: Makefile.am,v 1.7.4.5 2013/09/12 23:26:11 cvs Exp $
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

AM_CFLAGS = -fPIC -Wall

RLM_MODULES=rlm_ando.rlm\
            rlm_facebook.rlm\
            rlm_filewrite.rlm\
            rlm_icecast2.rlm\
            rlm_inno713.rlm\
            rlm_liqcomp.rlm\
            rlm_padpoint.rlm\
            rlm_serial.rlm\
            rlm_shoutcast1.rlm\
            rlm_spinitron_plus.rlm\
            rlm_spottrap.rlm\
            rlm_test.rlm\
            rlm_twitter.rlm\
            rlm_udp.rlm\
            rlm_xds.rlm\
            rlm_xmpad.rlm

all:	$(RLM_MODULES)

%.rlm:	%.c
	$(CC) $(AM_CFLAGS) -I$(top_srcdir) -shared $< -o $@

install:	all
	mkdir -p $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell
	cp $(RLM_MODULES) $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/
	mkdir -p $(DESTDIR)$(headerdir)
	cp rlm.h $(DESTDIR)$(headerdir)
	mkdir -p $(DESTDIR)$(prefix)/src/rlm
	cp *.c $(DESTDIR)$(prefix)/src/rlm/
	cp Makefile-example $(DESTDIR)$(prefix)/src/rlm/Makefile

headerdir = $(includedir)/rlm
header_HEADERS = rlm.h

EXTRA_DIST = Makefile-example\
             rlm.h\
             rlm_ando.c\
             rlm_facebook.c\
             rlm_filewrite.c\
             rlm_icecast2.c\
             rlm_inno713.c\
             rlm_liqcomp.c\
             rlm_padpoint.c\
             rlm_serial.c\
             rlm_shoutcast1.c\
             rlm_spinitron_plus.c\
             rlm_spottrap.c\
             rlm_test.c\
             rlm_twitter.c\
             rlm_udp.c\
             rlm_xds.c\
             rlm_xmpad.c

CLEANFILES = *~\
             *.idb\
             *ilk\
             *.obj\
             *.pdb\
             *.qm\
             *.rlm\
             moc_*

MAINTAINERCLEANFILES = *~\
                       *.tar.gz\
                       aclocal.m4\
                       configure\
                       Makefile.in\
                       moc_*
