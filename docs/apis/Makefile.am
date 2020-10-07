## automake.am
##
## docs/docbook/automake.am for Rivendell
##
## (C) Copyright 2015-2017 Fred Gleason <fredg@paravelsystems.com>
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

##
## Build Dependencies
##
%.html:	%.xml
	xsltproc -o $@ $(DOCBOOK_STYLESHEETS)/xhtml/docbook.xsl $<
%.pdf:	%.xml
	xsltproc $(DOCBOOK_STYLESHEETS)/fo/docbook.xsl $< | fop - -pdf $@
%.1:	%.xml
	xsltproc $(DOCBOOK_STYLESHEETS)/manpages/docbook.xsl $<
%.8:	%.xml
	xsltproc $(DOCBOOK_STYLESHEETS)/manpages/docbook.xsl $<

all-local:	cae.html\
                cae.pdf\
                catchd.html\
                catchd.pdf\
                fonts.html\
                fonts.rdcastmanager.list_carts.png\
                fonts.pdf\
                fonts.rdadmin.configure_rdairplay_screenshot.png\
                fonts.rdairplay_screenshot.png\
                fonts.rdcartslots_screenshot.png\
                fonts.rdcastmanager.list_carts.png\
                fonts.rdcastmanager.uploading_audio_screenshot.png\
                fonts.rdlibrary.edit_audio_screenshot.png\
                notification.html\
                notification.pdf\
                ripc.html\
                ripc.pdf\
                web_api.html\
                web_api.pdf

EXTRA_DIST = cae.html\
             cae.pdf\
             cae.xml\
             catchd.html\
             catchd.pdf\
             catchd.xml\
             fonts.html\
             fonts.pdf\
             fonts.xml\
             fonts.rdadmin.configure_rdairplay_screenshot.png\
             fonts.rdadmin.configure_rdairplay_screenshot.xcf\
             fonts.rdairplay_screenshot.png\
             fonts.rdairplay_screenshot.xcf\
             fonts.rdcartslots_screenshot.png\
             fonts.rdcartslots_screenshot.xcf\
             fonts.rdcastmanager.list_carts.png\
             fonts.rdcastmanager.list_carts.xcf\
             fonts.rdcastmanager.uploading_audio_screenshot.png\
             fonts.rdcastmanager.uploading_audio_screenshot.xcf\
             fonts.rdlibrary.edit_audio_screenshot.png\
             fonts.rdlibrary.edit_audio_screenshot.xcf\
             notification.html\
             notification.pdf\
             notification.xml\
             ripc.html\
             ripc.pdf\
             ripc.xml\
             web_api.html\
             web_api.pdf\
             web_api.xml

CLEANFILES = *~
MAINTAINERCLEANFILES = *~\
                       *.1\
                       *.8\
                       *.html\
                       *.pdf\
                       aclocal.m4\
                       configure\
                       Makefile.in
