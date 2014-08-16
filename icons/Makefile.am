## automake.am
##
## icons/automake.am for Rivendell
##
## (C) Copyright 2002-2006 Fred Gleason <fredg@salemradiolabs.com>
##
##      $Id: Makefile.am,v 1.19.2.3 2013/12/30 19:56:11 cvs Exp $
##      $Date: 2013/12/30 19:56:11 $
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
	mkdir -p $(DESTDIR)@libexecdir@
	cp greencheckmark.png $(DESTDIR)@libexecdir@
	cp redx.png $(DESTDIR)@libexecdir@
	cp greenball.png $(DESTDIR)@libexecdir@
	cp redball.png $(DESTDIR)@libexecdir@
	cp whiteball.png $(DESTDIR)@libexecdir@
	cp progressbar.gif $(DESTDIR)@libexecdir@
	mkdir -p $(DESTDIR)@prefix@/share/icons/hicolor/16x16/apps
	cp rivendell-16x16.png $(DESTDIR)@prefix@/share/icons/hicolor/16x16/apps/rivendell.png
	mkdir -p $(DESTDIR)@prefix@/share/icons/hicolor/22x22/apps
	cp rivendell-22x22.png $(DESTDIR)@prefix@/share/icons/hicolor/22x22/apps/rivendell.png
	mkdir -p $(DESTDIR)@prefix@/share/icons/hicolor/32x32/apps
	cp rivendell-32x32.png $(DESTDIR)@prefix@/share/icons/hicolor/32x32/apps/rivendell.png
	mkdir -p $(DESTDIR)@prefix@/share/icons/hicolor/48x48/apps
	cp rivendell-48x48.png $(DESTDIR)@prefix@/share/icons/hicolor/48x48/apps/rivendell.png
	mkdir -p $(DESTDIR)@prefix@/share/icons/hicolor/64x64/apps
	cp rivendell-64x64.png $(DESTDIR)@prefix@/share/icons/hicolor/64x64/apps/rivendell.png
	mkdir -p $(DESTDIR)@prefix@/share/icons/hicolor/128x128/apps
	cp rivendell-128x128.png $(DESTDIR)@prefix@/share/icons/hicolor/128x128/apps/rivendell.png
	mkdir -p $(DESTDIR)@prefix@/share/X11/fvwm2/pixmaps
	cp rivendell-16x16.xpm $(DESTDIR)@prefix@/share/X11/fvwm2/pixmaps/mini.rivendell.xpm
	cp rivendell-32x32.xpm $(DESTDIR)@prefix@/share/X11/fvwm2/pixmaps/rivendell.xpm


uninstall:	
	rm -f $(DESTDIR)@libexecdir@/greencheckmark.png
	rm -f $(DESTDIR)@libexecdir@/redx.png
	rm -f $(DESTDIR)@libexecdir@/greenball.png
	rm -f $(DESTDIR)@libexecdir@/redball.png
	rm -f $(DESTDIR)@libexecdir@/whiteball.png
	rm -f $(DESTDIR)@libexecdir@/progressbar.gif
	rm -f $(DESTDIR)@prefix@/share/icons/hicolor/16x16/apps/rivendell.png
	rm -f $(DESTDIR)@prefix@/share/icons/hicolor/22x22/apps/rivendell.png
	rm -f $(DESTDIR)@prefix@/share/icons/hicolor/32x32/apps/rivendell.png
	rm -f $(DESTDIR)@prefix@/share/icons/hicolor/48x48/apps/rivendell.png
	rm -f $(DESTDIR)@prefix@/share/icons/hicolor/64x64/apps/rivendell.png
	rm -f $(DESTDIR)@prefix@/share/icons/hicolor/128x128/apps/rivendell-128x128.png

EXTRA_DIST = admin.xpm\
             blueball.xpm\
             chain.xpm\
             checkmark.xpm\
             download.xpm\
             ex.xpm\
             fillstart.xpm\
             fillstop.xpm\
             greenball.png\
             greencheckmark.png\
             greenball.xpm\
             greencheckmark.xpm\
             marker.xpm\
             mic16.xpm\
             music.xpm\
             note.xpm\
             note2.xpm\
             notemarker.xpm\
             play.xpm macro.xpm\
             post.xpm\
             post2.xpm\
             post3.xpm\
             progressbar.gif\
             progressbar.xcf\
             record.xpm\
             record2.xpm\
             record3.xpm\
             redball.png\
             redball.xpm\
             redcheckmark.xpm\
             redx.png\
             redx.xpm\
             rivendell.ico\
             rivendell.res\
             rivendell-16x16.png\
             rivendell-16x16.xpm\
             rivendell-22x22.png\
             rivendell-22x22.xpm\
             rivendell-32x32.png\
             rivendell-32x32.xpm\
             rivendell-48x48.png\
             rivendell-48x48.xpm\
             rivendell-64x64.png\
             rivendell-64x64.xpm\
             rivendell-128x128.png\
             rivendell-128x128.xpm\
             rml.xpm\
             rml2.xpm\
             rml3.xpm\
             rml5.xpm\
             split.xpm\
             switch.xpm\
             switch2.xpm\
             switch3.xpm\
             track.xpm\
             track_cart.xpm\
             traffic.xpm\
             trashcan-16x16.xpm\
             trashcan-32x32.xpm\
             upload.xpm\
             user.xpm\
             whiteball.png\
             whiteball.xpm

CLEANFILES = *~

MAINTAINERCLEANFILES = *~\
                       aclocal.m4\
                       configure\
                       Makefile.in
