## Makefile.am
##
## Top level automake.am for Rivendell
##
##   (C) Copyright 2002-2018 Fred Gleason <fredg@paravelsystems.com>
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

if HPI_RD_AM
  HPI_RD_OPT = rdhpi
endif
if ALSA_RD_AM
  ALSACONFIG_RD_OPT = rdalsaconfig
endif

SUBDIRS = icons\
          helpers\
          lib\
          $(HPI_RD_OPT) apis\
          scripts\
          conf\
          docs\
          xdg\
          cae\
          importers\
          ios\
          rdadmin\
          rdairplay\
          rdcartslots\
          rdcastmanager\
          rdcatch\
          rdcatchd\
          rdlibrary\
          rdlogedit\
          rdlogin\
          rdlogmanager\
          rdmonitor\
          rdpanel\
          rdrepld\
          rdpadd\
          rdpadengined\
          rdrssd\
          rdselect\
          rdservice\
          rdvairplayd\
          ripcd\
          systemd\
          tests\
          utils\
          web

ACLOCAL_AMFLAGS = -I m4

maintainer-clean-local:	
	rm -rf m4

rpm:	dist
	mkdir -p $(HOME)/rpmbuild/SOURCES
	cp rivendell-$(VERSION).tar.gz $(HOME)/rpmbuild/SOURCES/
	$(RPMBUILD) -ba --target $(ARCH)-$(VENDOR)-linux rivendell.spec
	mv $(HOME)/rpmbuild/RPMS/$(ARCH)/rivendell-*.rpm $(top_srcdir)/
	mv $(HOME)/rpmbuild/RPMS/noarch/rivendell-*.rpm $(top_srcdir)/
	mv $(HOME)/rpmbuild/SRPMS/rivendell-*.src.rpm $(top_srcdir)/
	rm $(HOME)/rpmbuild/SOURCES/rivendell-$(VERSION).tar.gz

zip:	dist
	tar -zxf rivendell-@VERSION@.tar.gz
	zip -r rivendell-@VERSION@.zip rivendell-@VERSION@
	rm -rf rivendell-@VERSION@

EXTRA_DIST = autogen.sh\
             CODINGSTYLE\
             get_distro.pl\
             get_target.sh\
             INSTALL-WINDOWS\
             PACKAGE_VERSION\
             rivendell.ism\
             rivendell.pro\
             rivendell.spec.in\
             UPGRADING

CLEANFILES = *~\
             *.mo\
             *.rpm\
             *.tar.gz\
             *.tgz\
             *.zip\
             py-compile

DISTCLEANFILES = config.guess\
                 config.sub\
                 ltmain.sh\
                 rivendell\
                 rivendell.spec

MAINTAINERCLEANFILES = *~\
                       *.tar.gz\
                       aclocal.m4\
                       Makefile.in\
                       config.guess\
                       config.status\
                       config.sub\
                       configure\
                       depcomp\
                       install-sh\
                       libtool\
                       ltmain.sh\
                       missing\
                       mkinstalldirs\
                       rivendell.spec
