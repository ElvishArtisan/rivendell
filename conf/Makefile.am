## Makefile.am
##
## (C) Copyright 2002-2020 Fred Gleason <fredg@paravelsystems.com>
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

install-exec-am:	
	mkdir -p $(DESTDIR)/$(sysconfdir)
	cp rd-bin.conf $(DESTDIR)/$(sysconfdir)/
	mkdir -p $(DESTDIR)/etc/pam.d
	cp rivendell.pam $(DESTDIR)/etc/pam.d/rivendell

uninstall:	
	rm -f $(DESTDIR)/$(sysconfdir)/rd-bin.conf
	rm -f $(DESTDIR)/etc/pam.d/rivendell

EXTRA_DIST = asound.conf-sample\
             logrotate-sample\
             rd-bin.conf.in\
             rd.conf-sample\
             rivendell.pam\
             rivendell-webapi.conf\
             syslog.conf-sample

CLEANFILES = *~
DISTCLEANFILES = rdfeed.conf
MAINTAINERCLEANFILES = *~\
                       aclocal.m4\
                       configure\
                       Makefile.in
