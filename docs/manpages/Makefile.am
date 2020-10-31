## Makefile.am
##
## docs/manpages/automake.am for Rivendell
##
## (C) Copyright 2015-2019 Fred Gleason <fredg@paravelsystems.com>
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
%.8:	%.xml
	xsltproc $(DOCBOOK_STYLESHEETS)/manpages/docbook.xsl $<

all-local:	rdairplay.1\
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
                rdrender.1\
                rmlsend.1\
                rdservice.8\
                rdsoftkeys.1

man_MANS = rdairplay.1\
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
           rdrender.1\
           rmlsend.1\
           rdservice.8\
           rdsoftkeys.1

EXTRA_DIST = exitcodes.xml\
             rdairplay.1\
             rdairplay.xml\
             rdalsaconfig.1\
             rdalsaconfig.xml\
             rdclilogedit.1\
             rdclilogedit.xml\
             rd.conf.5\
             rd.conf.xml\
             rdconvert.1\
             rdconvert.xml\
             rddbmgr.8\
             rddbmgr.xml\
             rdexport.1\
             rdexport.xml\
             rdimport.1\
             rdimport.xml\
             rdlogmanager.1\
             rdlogmanager.xml\
             rdmarkerset.8\
             rdmarkerset.xml\
             rdmetadata.1\
             rdmetadata.xml\
             rdrender.1\
             rdrender.xml\
             rmlsend.1\
             rmlsend.xml\
             rdservice.8\
             rdservice.xml\
             rdsoftkeys.1\
             rdsoftkeys.xml

CLEANFILES = *~
MAINTAINERCLEANFILES = *~\
                       *.1\
                       *.5\
                       *.8\
                       *.html\
                       *.pdf\
                       aclocal.m4\
                       configure\
                       Makefile.in
