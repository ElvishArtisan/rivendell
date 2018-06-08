## Makefile.am
##
## docs/manpages/automake.am for Rivendell
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

all-local:	rdclilogedit.1\
                rdconvert.1\
                rddbcheck.8\
                rddbmgr.8\
                rdexport.1\
                rdimport.1\
                rdmarkerset.8\
                rdrender.1\
                rdrevert.8\
                rmlsend.1

man_MANS = rdclilogedit.1\
           rdconvert.1\
           rddbcheck.8\
           rddbmgr.8\
           rdexport.1\
           rdimport.1\
           rdmarkerset.8\
           rdrender.1\
           rdrevert.8\
           rmlsend.1

EXTRA_DIST = rdclilogedit.1\
             rdclilogedit.xml\
             rdconvert.1\
             rdconvert.xml\
             rddbcheck.8\
             rddbcheck.xml\
             rddbmgr.8\
             rddbmgr.xml\
             rdexport.1\
             rdexport.xml\
             rdimport.1\
             rdimport.xml\
             rdmarkerset.8\
             rdmarkerset.xml\
             rdrender.1\
             rdrender.xml\
             rdrevert.8\
             rdrevert.xml\
             rmlsend.1\
             rmlsend.xml

CLEANFILES = *~
MAINTAINERCLEANFILES = *~\
                       *.1\
                       *.8\
                       *.html\
                       *.pdf\
                       aclocal.m4\
                       configure\
                       Makefile.in
