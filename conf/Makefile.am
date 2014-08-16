## automake.am
##
## Top level automake.am for Rivendell
##
## (C) Copyright 2002-2014 Fred Gleason <fredg@paravelsystems.com>
##
##      $Id: Makefile.am,v 1.16.4.5 2013/09/12 23:26:09 cvs Exp $
##      $Date: 2013/09/12 23:26:09 $
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
	mkdir -p $(DESTDIR)/$(sysconfdir)
	cp rd-bin.conf $(DESTDIR)/$(sysconfdir)/

uninstall:	
	rm -f $(DESTDIR)/$(sysconfdir)/rd-bin.conf

EXTRA_DIST = my.cnf-master\
             my.cnf-standby\
             rd-bin.conf.in\
             rd.conf-sample\
             rd.conf-slax\
             rd-sample.ini\
             rlm_ando.conf\
             rlm_facebook.conf\
             rlm_filewrite.conf\
             rlm_icecast2.conf\
             rlm_inno713.conf\
             rlm_liqcomp.conf\
             rlm_padpoint.conf\
             rlm_serial.conf\
             rlm_shoutcast1.conf\
             rlm_spinitron_plus.conf\
             rlm_spottrap.conf\
             rlm_twitter.conf\
             rlm_udp.conf\
             rlm_xds.conf\
             rlm_xmpad.conf

CLEANFILES = *~
DISTCLEANFILES = rdfeed.conf
MAINTAINERCLEANFILES = *~\
                       aclocal.m4\
                       configure\
                       Makefile.in
