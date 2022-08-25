## Makefile.am
##
## Automake.am for rivendell/web/stylesheets
##
##   (C) Copyright 2022 Fred Gleason <fredg@paravelsystems.com>
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

install-exec-am:	
	mkdir -p $(DESTDIR)/usr/share/rivendell
	cp rdcastmanager-report.xsl $(DESTDIR)/usr/share/rivendell/

uninstall-local:	
	rm -f $(DESTDIR)/usr/share/rivendell/rdcastmanager-report.xsl

EXTRA_DIST = rdcastmanager-report.xsl

CLEANFILES = *~
MAINTAINERCLEANFILES = *~\
                       Makefile.in
