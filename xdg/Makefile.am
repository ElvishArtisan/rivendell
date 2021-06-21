## automake.am
##
## xdg/automake.am for Rivendell
##
##   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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
	./install_usermode.sh install $(DESTDIR) $(prefix)
	mkdir -p $(DESTDIR)@prefix@/share/applications
	cp rivendell-rdadmin.desktop $(DESTDIR)@prefix@/share/applications/
	cp rivendell-rdairplay.desktop $(DESTDIR)@prefix@/share/applications/
	cp rivendell-rdcartslots.desktop $(DESTDIR)@prefix@/share/applications/
	cp rivendell-rdcastmanager.desktop $(DESTDIR)@prefix@/share/applications/
	cp rivendell-rdcatch.desktop $(DESTDIR)@prefix@/share/applications/
	cp rivendell-rdgpimon.desktop $(DESTDIR)@prefix@/share/applications/
	cp rivendell-rdlibrary.desktop $(DESTDIR)@prefix@/share/applications/
	cp rivendell-rdlogedit.desktop $(DESTDIR)@prefix@/share/applications/
	cp rivendell-rdlogin.desktop $(DESTDIR)@prefix@/share/applications/
	cp rivendell-rdlogmanager.desktop $(DESTDIR)@prefix@/share/applications/
	cp rivendell-rdmonitor.desktop $(DESTDIR)@prefix@/share/applications/
	cp rivendell-rdpanel.desktop $(DESTDIR)@prefix@/share/applications/
	cp rivendell-rdsoftkeys.desktop $(DESTDIR)@prefix@/share/applications/
	cp rivendell-rmlsend.desktop $(DESTDIR)@prefix@/share/applications/
	mkdir -p $(DESTDIR)@prefix@/share/desktop-directories
	cp rivendell-configuration.directory $(DESTDIR)@prefix@/share/desktop-directories/
	cp rivendell-documentation.directory $(DESTDIR)@prefix@/share/desktop-directories/
	cp rivendell-logtools.directory $(DESTDIR)@prefix@/share/desktop-directories/
	cp rivendell-rivendell.directory $(DESTDIR)@prefix@/share/desktop-directories/
	cp rivendell-utilities.directory $(DESTDIR)@prefix@/share/desktop-directories/
	mkdir -p $(DESTDIR)/etc/xdg/menus/applications-merged
	cp rivendell-rivendell.menu $(DESTDIR)/etc/xdg/menus/applications-merged/
	cp @RDALSACONFIG_DESKTOP_FILE@ $(DESTDIR)@prefix@/share/applications/rivendell-rdalsaconfig.desktop
	cp @RDDBCONFIG_DESKTOP_FILE@ $(DESTDIR)@prefix@/share/applications/rivendell-rddbconfig.desktop

uninstall-local:	
	./install_usermode.sh uninstall $(DESTDIR) $(prefix)
	rm -f $(DESTDIR)@prefix@/share/applications/rivendell-*
	rm -f $(DESTDIR)@prefix@/share/desktop-directories/rivendell-configuration.directory
	rm -f $(DESTDIR)@prefix@/share/desktop-directories/rivendell-documentation.directory
	rm -f $(DESTDIR)@prefix@/share/desktop-directories/rivendell-logtools.directory
	rm -f $(DESTDIR)@prefix@/share/desktop-directories/rivendell-rivendell.directory
	rm -f $(DESTDIR)@prefix@/share/desktop-directories/rivendell-utilities.directory
	rm -f $(DESTDIR)/etc/xdg/menus/applications-merged/rivendell-rivendell.menu

EXTRA_DIST = install_usermode.sh.in\
             rdalsaconfig-root-consolehelper.in\
             rdalsaconfig-root-pam\
             rddbconfig-root-consolehelper.in\
             rddbconfig-root-pam\
             rivendell-configuration.directory\
             rivendell-documentation.directory\
             rivendell-logtools.directory\
             rivendell-rdadmin.desktop\
             rivendell-rdairplay.desktop\
             rivendell-rdalsaconfig.desktop\
             rivendell-rdalsaconfig-root.desktop\
             rivendell-rdalsaconfig-sudo.desktop\
             rivendell-rdcartslots.desktop\
             rivendell-rdcastmanager.desktop\
             rivendell-rdcatch.desktop\
             rivendell-rddbconfig.desktop\
             rivendell-rddbconfig-root.desktop\
             rivendell-rddbconfig-sudo.desktop\
             rivendell-rdgpimon.desktop\
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
DISTCLEANFILES = install_usermod.sh\
                 rdalsaconfig-root-consolehelper\
                 rddbconfig-root-consolehelper
MAINTAINERCLEANFILES = *~\
                       Makefile.in
