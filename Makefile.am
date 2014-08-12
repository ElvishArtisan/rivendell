## automake.am
##
## Top level automake.am for Rivendell
##
## Use automake to process this into a Makefile.in
##
##   (C) Copyright 2002-2010 Fred Gleason <fredg@paravelsystems.com>
##
##      $Id: Makefile.am,v 1.65.6.6 2013/01/14 16:35:58 cvs Exp $
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

if PAM_RD_AM
  PAM_RD_OPT = pam_rd
endif
if HPI_RD_AM
  HPI_RD_OPT = rdhpi
endif
if ALSA_RD_AM
  ALSACONFIG_RD_OPT = rdalsaconfig
endif
SUBDIRS = icons\
          helpers\
          lib\
          $(HPI_RD_OPT) $(PAM_RD_OPT) rlm\
          scripts\
          conf\
          debian\
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
          rdselect\
          ripcd\
          tests\
          utils\
          web

install-exec-local:	
	./install-init.sh $(DESTDIR)$(prefix)

install-debian:	
	touch ./building-debian
	make install
	rm -f ./building-debian

uninstall-exec:
	rm -f /etc/init.d/rdrepld
	rm -f /etc/init.d/rivendell
	rm -f /etc/sysconfig/rivendell

rpm:	dist
	cp rivendell-$(VERSION).tar.gz $(RPM_ROOT)/SOURCES/
	$(RPMBUILD) -ba --target $(ARCH)-$(VENDOR)-linux rivendell.spec
	mv $(RPM_ROOT)/RPMS/$(ARCH)/rivendell-*.rpm $(top_srcdir)/
	mv $(RPM_ROOT)/SRPMS/rivendell-*.src.rpm $(top_srcdir)/
	rm $(RPM_ROOT)/SOURCES/rivendell-$(VERSION).tar.gz
	rm -rf $(RPM_ROOT)/BUILD/rivendell-$(VERSION)

slack:	all
	./make_slack

slax:	all
	rm -f rivendell-$(VERSION)-$(ARCH)-$(RPM_RELEASE).mo
	./make_slack
	tgz2mo rivendell-$(VERSION)-$(ARCH)-$(RPM_RELEASE).tgz rivendell-$(VERSION)-$(ARCH)-$(RPM_RELEASE).mo

zip:	dist
	tar -zxf rivendell-@VERSION@.tar.gz
	zip -r rivendell-@VERSION@.zip rivendell-@VERSION@
	rm -rf rivendell-@VERSION@

doxygen: 
	cd lib && doxygen doxygen.conf 
	cd cae && doxygen doxygen.conf &
	cd rdairplay && doxygen doxygen.conf &
	cd rdadmin && doxygen doxygen.conf &
	cd rdcatch && doxygen doxygen.conf &
	cd rdcatchd && doxygen doxygen.conf &
	cd rdlibrary && doxygen doxygen.conf &
	cd rdlogedit && doxygen doxygen.conf &
	cd rdlogin && doxygen doxygen.conf &
	cd rdlogmanager && doxygen doxygen.conf &
	cd ripcd && doxygen doxygen.conf &

EXTRA_DIST = autogen.sh\
             build_win32.bat\
             CODINGSTYLE\
             get_distro.sh\
             get_target.sh\
             install-init.sh\
             make_slack.in\
             PACKAGE_VERSION\
             rdrepld-suse.in\
             rivendell-gentoo\
             rivendell.ism\
             rivendell.pro\
             rivendell.spec.in\
             rivendell-suse.in\
             rivendell.sys\
             slack-desc.in\
             slack_doinst.sh\
             UPGRADING

CLEANFILES = *~\
             *.mo\
             *.rpm\
             *.tar.gz\
             *.tgz\
             *.zip

DISTCLEANFILES = config.guess\
                 config.sub\
                 ltmain.sh\
                 make_slack\
                 rdrepld-suse\
                 rivendell\
                 rivendell.spec\
                 slack-desc

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
                       rivendell.spec\
                       rivendell-suse
