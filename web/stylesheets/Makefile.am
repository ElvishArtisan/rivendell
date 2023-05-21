## Makefile.am
##
## Makefile.am for rivendell/web/stylesheets
##
##   (C) Copyright 2022-2023 Fred Gleason <fredg@paravelsystems.com>
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
	cp rss-item-enclosures.xsl $(DESTDIR)/usr/share/rivendell/
	cp rss-front-item-report.xsl $(DESTDIR)/usr/share/rivendell/
	cp rss-back-item-report.xsl $(DESTDIR)/usr/share/rivendell/

uninstall-local:	
	rm -f $(DESTDIR)/usr/share/rivendell/rss-item-enclosures.xsl
	rm -f $(DESTDIR)/usr/share/rivendell/rss-front-item-report.xsl
	rm -f $(DESTDIR)/usr/share/rivendell/rss-back-item-report.xsl

EXTRA_DIST = rss-front-item-report.xsl\
             rss-back-item-report.xsl\
             rss-item-enclosures.xsl

CLEANFILES = *~
MAINTAINERCLEANFILES = *~\
                       Makefile.in
