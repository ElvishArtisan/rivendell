## Makefile.am
##
## Makefile.am for Rivendell apis/cdn/scripts
##
##   (C) Copyright 2023 Fred Gleason <fredg@paravelsystems.com>
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

install-exec-am:	
	mkdir -p $(DESTDIR)/usr/lib/rivendell/cdn
	cp aka_purge.sh $(DESTDIR)/usr/lib/rivendell/cdn/
	cp akamai_purge.cdn $(DESTDIR)/usr/lib/rivendell/cdn/

uninstall-local:	
	rm -f $(DESTDIR)/usr/lib/rivendell/cdn/aka_purge.sh
	rm -f $(DESTDIR)/usr/lib/rivendell/cdn/akamai_purge.cdn

EXTRA_DIST = aka_purge.sh\
             akamai_purge.cdn

CLEANFILES = *~\
             *.idb\
             *ilk\
             *.obj\
             *.pdb\
             *.qm\
             moc_*

MAINTAINERCLEANFILES = *~\
                       *.tar.gz\
                       aclocal.m4\
                       configure\
                       Makefile.in\
                       moc_*
