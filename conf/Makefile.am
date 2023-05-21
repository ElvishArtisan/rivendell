## Makefile.am
##
## (C) Copyright 2002-2023 Fred Gleason <fredg@paravelsystems.com>
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
	cp @RIVENDELL_PAM_FILE@ $(DESTDIR)/etc/pam.d/rivendell
	mkdir -p $(DESTDIR)/usr/lib/firewalld/services
	cp rivendell-firewalld.xml $(DESTDIR)/usr/lib/firewalld/services/rivendell.xml
	./firewalld.sh reload

uninstall:	
	rm -f $(DESTDIR)/$(sysconfdir)/rd-bin.conf
	rm -f $(DESTDIR)/etc/pam.d/rivendell
	rm -f $(DESTDIR)/usr/lib/firewalld/services/rivendell.xml
	./firewalld.sh reload

EXTRA_DIST = asound.conf-sample\
             firewalld.sh\
             logrotate-sample\
             rd-bin.conf.in\
             rd.conf-sample\
             rivendell-rhel.pam\
             rivendell-ubuntu.pam\
             rivendell-env.sh\
             rivendell-firewalld.xml\
             rivendell-webapi.conf\
             syslog.conf-sample

CLEANFILES = *~
DISTCLEANFILES = rdfeed.conf
MAINTAINERCLEANFILES = *~\
                       aclocal.m4\
                       configure\
                       Makefile.in
