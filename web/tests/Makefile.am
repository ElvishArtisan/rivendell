## automake.am
##
## Automake.am for rivendell/web/tests
##
##   (C) Copyright 2010,2013 Fred Gleason <fredg@paravelsystems.com>
##
##      $Id: Makefile.am,v 1.6.6.4 2013/10/14 04:23:55 cvs Exp $
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
	mkdir -p $(DESTDIR)@libexecdir@
	cp addcart.html $(DESTDIR)@libexecdir@
	cp addcut.html $(DESTDIR)@libexecdir@
	cp audioinfo.html $(DESTDIR)@libexecdir@
	cp copyaudio.html $(DESTDIR)@libexecdir@
	cp delete_audio.html $(DESTDIR)@libexecdir@
	cp editcart.html $(DESTDIR)@libexecdir@
	cp export.html $(DESTDIR)@libexecdir@
	cp exportpeaks.html $(DESTDIR)@libexecdir@
	cp import.html $(DESTDIR)@libexecdir@
	cp import.html $(DESTDIR)@libexecdir@
	cp listcart.html $(DESTDIR)@libexecdir@
	cp listcarts.html $(DESTDIR)@libexecdir@
	cp listcut.html $(DESTDIR)@libexecdir@
	cp listcuts.html $(DESTDIR)@libexecdir@
	cp listgroup.html $(DESTDIR)@libexecdir@
	cp listgroups.html $(DESTDIR)@libexecdir@
	cp listlog.html $(DESTDIR)@libexecdir@
	cp listlogs.html $(DESTDIR)@libexecdir@
	cp listservices.html $(DESTDIR)@libexecdir@
	cp removecart.html $(DESTDIR)@libexecdir@
	cp removecut.html $(DESTDIR)@libexecdir@
	cp trimaudio.html $(DESTDIR)@libexecdir@

uninstall:	
	rm -f $(DESTDIR)@libexecdir@/addcart.html
	rm -f $(DESTDIR)@libexecdir@/addcut.html
	rm -f $(DESTDIR)@libexecdir@/audioinfo.html
	rm -f $(DESTDIR)@libexecdir@/copyaudio.html
	rm -f $(DESTDIR)@libexecdir@/delete_audio.html
	rm -f $(DESTDIR)@libexecdir@/editcart.html
	rm -f $(DESTDIR)@libexecdir@/export.html
	rm -f $(DESTDIR)@libexecdir@/exportpeaks.html
	rm -f $(DESTDIR)@libexecdir@/import.html
	rm -f $(DESTDIR)@libexecdir@/listcart.html
	rm -f $(DESTDIR)@libexecdir@/listcarts.html
	rm -f $(DESTDIR)@libexecdir@/listcut.html
	rm -f $(DESTDIR)@libexecdir@/listcuts.html
	rm -f $(DESTDIR)@libexecdir@/listgroup.html
	rm -f $(DESTDIR)@libexecdir@/listgroups.html
	rm -f $(DESTDIR)@libexecdir@/listlog.html
	rm -f $(DESTDIR)@libexecdir@/listlogs.html
	rm -f $(DESTDIR)@libexecdir@/listservices.html
	rm -f $(DESTDIR)@libexecdir@/removecart.html
	rm -f $(DESTDIR)@libexecdir@/removecut.html
	rm -f $(DESTDIR)@libexecdir@/trimaudio.html

EXTRA_DIST = addcart.html\
             addcut.html\
             audioinfo.html\
             copyaudio.html\
             delete_audio.html\
             editcart.html\
             export.html\
             exportpeaks.html\
             import.html\
             listcart.html\
             listcarts.html\
             listcut.html\
             listcuts.html\
             listgroup.html\
             listgroups.html\
             listlog.html\
             listlogs.html\
             listservices.html\
             removecart.html\
             removecut.html\
             trimaudio.html

CLEANFILES = *~
MAINTAINERCLEANFILES = *~\
                       Makefile.in
