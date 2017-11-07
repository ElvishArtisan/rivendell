## Makefile.am
##
## docs/stylesheets/Makefile.am for Rivendell
##
## (C) Copyright 2017 Fred Gleason <fredg@paravelsystems.com>
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

all-local:	book-fo-titlepages.xsl

book-fo-titlepages.xsl:	book-fo-titlepages-spec.xml
	xsltproc -o book-fo-titlepages.xsl ../../helpers/docbook/template/titlepage.xsl book-fo-titlepages-spec.xml

EXTRA_DIST = book-common.xml\
             book-fo-titlepages.xsl\
             book-fo.xml\
             book-fo-titlepages-spec.xml\
             book-xhtml.xml

CLEANFILES = *~\
             *.xsl
MAINTAINERCLEANFILES = *~\
                       aclocal.m4\
                       configure\
                       Makefile.in
