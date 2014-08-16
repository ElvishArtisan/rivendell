## automake.am
##
## xdg/automake.am for Rivendell
##
##   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
##
##      $Id: Makefile.am,v 1.11.2.8 2013/12/27 00:04:04 cvs Exp $
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
	mkdir -p $(DESTDIR)@prefix@/share/applications
	cp rivendell-rdadmin.desktop $(DESTDIR)@prefix@/share/applications/
	cp rivendell-rdairplay.desktop $(DESTDIR)@prefix@/share/applications/
	cp rivendell-rdcartslots.desktop $(DESTDIR)@prefix@/share/applications/
	cp rivendell-rdcastmanager.desktop $(DESTDIR)@prefix@/share/applications/
	cp rivendell-rdcatch.desktop $(DESTDIR)@prefix@/share/applications/
	cp rivendell-rddgimport.desktop $(DESTDIR)@prefix@/share/applications/
	cp rivendell-rddiscimport.desktop $(DESTDIR)@prefix@/share/applications/
	cp rivendell-rdgpimon.desktop $(DESTDIR)@prefix@/share/applications/
	cp rivendell-rdhpiinfo-root.desktop $(DESTDIR)@prefix@/share/applications/
	cp rivendell-rdlibrary.desktop $(DESTDIR)@prefix@/share/applications/
	cp rivendell-rdlogedit.desktop $(DESTDIR)@prefix@/share/applications/
	cp rivendell-rdlogin.desktop $(DESTDIR)@prefix@/share/applications/
	cp rivendell-rdlogmanager.desktop $(DESTDIR)@prefix@/share/applications/
	cp rivendell-rdmonitor.desktop $(DESTDIR)@prefix@/share/applications/
	cp rivendell-rdpanel.desktop $(DESTDIR)@prefix@/share/applications/
	cp rivendell-rdsoftkeys.desktop $(DESTDIR)@prefix@/share/applications/
	cp rivendell-rmlsend.desktop $(DESTDIR)@prefix@/share/applications/
	cp rivendell-rdalsaconfig-root.desktop $(DESTDIR)@prefix@/share/applications/

	mkdir -p $(DESTDIR)@prefix@/share/desktop-directories
	cp *.directory $(DESTDIR)@prefix@/share/desktop-directories/
	mkdir -p $(DESTDIR)/etc/xdg/menus/applications-merged
	cp *.menu $(DESTDIR)/etc/xdg/menus/applications-merged/
	mkdir -p $(DESTDIR)/etc/pam.d
	cp rdalsaconfig-root-pam $(DESTDIR)/etc/pam.d/rdalsaconfig-root
	if test -f ../rdhpi/.libs/librdhpi-@VERSION@.so ; then cp rdhpiinfo-root-pam $(DESTDIR)/etc/pam.d/rdhpiinfo-root ; fi
	mkdir -p $(DESTDIR)/etc/security/console.apps
	cp rdalsaconfig-root-consolehelper $(DESTDIR)/etc/security/console.apps/rdalsaconfig-root
	if test -f ../rdhpi/.libs/librdhpi-@VERSION@.so ; then cp rdhpiinfo-root-consolehelper $(DESTDIR)/etc/security/console.apps/rdhpiinfo-root ; fi
	rm -f $(DESTDIR)$(prefix)/bin/rdalsaconfig-root
	ln -s /usr/bin/consolehelper $(DESTDIR)$(prefix)/bin/rdalsaconfig-root
	rm -f $(DESTDIR)$(prefix)/bin/rdhpiinfo-root
	ln -s /usr/bin/consolehelper $(DESTDIR)$(prefix)/bin/rdhpiinfo-root

uninstall:	
	rm -f $(DESTDIR)@prefix@/share/applications/rivendell-*.desktop
	rm -f $(DESTDIR)@prefix@/share/desktop-directories/rivendell-*.directory
	rm -f $(DESTDIR)/etc/xdg/menus/applications-merged/rivendell-*.menu
	rm -f $(DESTDIR)/etc/pam.d/rdalsaconfig-root
	rm -f $(DESTDIR)/etc/pam.d/rdhpiinfo-root
	rm -f $(DESTDIR)/etc/security/console.apps/rdalsaconfig-root
	rm -f $(DESTDIR)/etc/security/console.apps/rdhpiinfo-root
	rm -f $(DESTDIR)$(prefix)/bin/rdalsaconfig-root
	rm -f $(DESTIDR)$(prefix)/bin/rdhpiinfo-root

EXTRA_DIST = rdalsaconfig-root-consolehelper\
             rdalsaconfig-root-pam\
             rdhpiinfo-root-consolehelper\
             rdhpiinfo-root-pam\
             rivendell-configuration.directory\
             rivendell-logtools.directory\
             rivendell-rdadmin.desktop\
             rivendell-rdairplay.desktop\
             rivendell-rdalsaconfig.desktop\
             rivendell-rdalsaconfig-root.desktop\
             rivendell-rdcartslots.desktop\
             rivendell-rdcastmanager.desktop\
             rivendell-rdcatch.desktop\
             rivendell-rddgimport.desktop\
             rivendell-rddiscimport.desktop\
             rivendell-rdgpimon.desktop\
             rivendell-rdhpiinfo.desktop\
             rivendell-rdhpiinfo-root.desktop\
             rivendell-rdlibrary.desktop\
             rivendell-rdlogedit.desktop\
             rivendell-rdlogin.desktop\
             rivendell-rdlogmanager.desktop\
             rivendell-rdmonitor.desktop\
             rivendell-rdpanel.desktop\
             rivendell-rmlsend.desktop\
             rivendell-rdsoftkeys.desktop\
             rivendell-rivendell.directory\
             rivendell-rivendell.menu\
             rivendell-utilities.directory

CLEANFILES = *~

MAINTAINERCLEANFILES = *~\
                       Makefile.in
