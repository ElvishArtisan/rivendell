## automake.am
##
## Automake.am for rivendell/rlm
##
##   (C) Copyright 2008,2016-2017 Fred Gleason <fredg@paravelsystems.com>
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

AM_CFLAGS = -fPIC -Wall -I../

RLM_MODULES = rlm_icecast2.rlm\
              rlm_shoutcast1.rlm\
              rlm_tunein.rlm

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
             rlm_icecast2.c\
             rlm_shoutcast1.c\
             rlm_tunein.c

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
