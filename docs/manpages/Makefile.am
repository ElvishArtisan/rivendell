## Makefile.am
##
## docs/manpages/automake.am for Rivendell
##
## (C) Copyright 2015-2023 Fred Gleason <fredg@paravelsystems.com>
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
%.5:	%.xml
	xsltproc $(DOCBOOK_STYLESHEETS)/manpages/docbook.xsl $<
%.7:	%.xml
	xsltproc $(DOCBOOK_STYLESHEETS)/manpages/docbook.xsl $<
%.8:	%.xml
	xsltproc $(DOCBOOK_STYLESHEETS)/manpages/docbook.xsl $<

install-exec-local:	
	mkdir -p $(DESTDIR)$(prefix)/share/rivendell/manpages
	cp *.pdf $(DESTDIR)$(prefix)/share/rivendell/manpages/
	mkdir -p $(DESTDIR)$(prefix)/share/rivendell/manpages/html
	cp *.html $(DESTDIR)$(prefix)/share/rivendell/manpages/html/

uninstall-local:	
	rm -rf $(DESTDIR)$(prefix)/share/rivendell/manpages


all-local:	rdadmin.1\
                rdadmin.html\
                rdadmin.pdf\
                rdairplay.1\
                rdairplay.html\
                rdairplay.pdf\
                rdalsaconfig.1\
                rdalsaconfig.html\
                rdalsaconfig.pdf\
                rdclilogedit.1\
                rdclilogedit.html\
                rdclilogedit.pdf\
                rd.conf.5\
                rd.conf.html\
                rd.conf.pdf\
                rdconvert.1\
                rdconvert.html\
                rdconvert.pdf\
                rddbmgr.8\
                rddbmgr.html\
                rddbmgr.pdf\
                rdexport.1\
                rdexport.html\
                rdexport.pdf\
                rdimport.1\
                rdimport.html\
                rdimport.pdf\
                rdlogmanager.1\
                rdlogmanager.html\
                rdlogmanager.pdf\
                rdmarkerset.8\
                rdmarkerset.html\
                rdmarkerset.pdf\
                rdmetadata.1\
                rdmetadata.html\
                rdmetadata.pdf\
                rdpanel.1\
                rdpanel.html\
                rdpanel.pdf\
                rdrender.1\
                rdrender.html\
                rdrender.pdf\
                rmlsend.1\
                rmlsend.html\
                rmlsend.pdf\
                rdservice.8\
                rdservice.html\
                rdservice.pdf\
                rdsinglestart.1\
                rdsinglestart.html\
                rdsinglestart.pdf\
                rdsoftkeys.1\
                rdsoftkeys.html\
                rdsoftkeys.pdf\
                rivendell.7\
                rivendell.html\
                rivendell.pdf

man_MANS = rdadmin.1\
           rdairplay.1\
           rdalsaconfig.1\
           rdclilogedit.1\
           rd.conf.5\
           rdconvert.1\
           rddbmgr.8\
           rdexport.1\
           rdimport.1\
           rdlogmanager.1\
           rdmarkerset.8\
           rdmetadata.1\
           rdpanel.1\
           rdrender.1\
           rmlsend.1\
           rdservice.8\
           rdsinglestart.1\
           rdsoftkeys.1\
           rivendell.7

EXTRA_DIST = exitcodes.xml\
             rdadmin.1\
             rdadmin.html\
             rdadmin.pdf\
             rdadmin.xml\
             rdairplay.1\
             rdairplay.html\
             rdairplay.pdf\
             rdairplay.xml\
             rdalsaconfig.1\
             rdalsaconfig.html\
             rdalsaconfig.pdf\
             rdalsaconfig.xml\
             rdclilogedit.1\
             rdclilogedit.html\
             rdclilogedit.pdf\
             rdclilogedit.xml\
             rd.conf.5\
             rd.conf.html\
             rd.conf.pdf\
             rd.conf.xml\
             rdconvert.1\
             rdconvert.html\
             rdconvert.pdf\
             rdconvert.xml\
             rddbmgr.8\
             rddbmgr.html\
             rddbmgr.pdf\
             rddbmgr.xml\
             rdexport.1\
             rdexport.html\
             rdexport.pdf\
             rdexport.xml\
             rdimport.1\
             rdimport.html\
             rdimport.pdf\
             rdimport.xml\
             rdlogmanager.1\
             rdlogmanager.html\
             rdlogmanager.pdf\
             rdlogmanager.xml\
             rdmarkerset.8\
             rdmarkerset.html\
             rdmarkerset.pdf\
             rdmarkerset.xml\
             rdmetadata.1\
             rdmetadata.html\
             rdmetadata.pdf\
             rdmetadata.xml\
             rdpanel.1\
             rdpanel.html\
             rdpanel.pdf\
             rdpanel.xml\
             rdrender.1\
             rdrender.html\
             rdrender.pdf\
             rdrender.xml\
             rmlsend.1\
             rmlsend.html\
             rmlsend.pdf\
             rmlsend.xml\
             rdservice.8\
             rdservice.html\
             rdservice.pdf\
             rdservice.xml\
             rdsinglestart.1\
             rdsinglestart.html\
             rdsinglestart.pdf\
             rdsinglestart.xml\
             rdsoftkeys.1\
             rdsoftkeys.html\
             rdsoftkeys.pdf\
             rdsoftkeys.xml\
             rivendell.7\
             rivendell.html\
             rivendell.pdf\
             rivendell.xml

CLEANFILES = *~
MAINTAINERCLEANFILES = *~\
                       *.1\
                       *.5\
                       *.5\
                       *.7\
                       *.html\
                       *.pdf\
                       aclocal.m4\
                       configure\
                       Makefile.in
