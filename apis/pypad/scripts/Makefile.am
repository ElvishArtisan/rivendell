## makefile.am
##
## Makefile.am for Rivendell pypad/examples
##
##   (C) Copyright 2018-2024 Fred Gleason <fredg@paravelsystems.com>
##
##   This program is free software; you can redistribute it and/or modify
##   it under the terms of the GNU General Public License as
##   published by the Free Software Foundation; either version 2 of
##   the License, or (at your option) any later version.
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
	mkdir -p $(DESTDIR)/usr/lib/rivendell/pypad
	../../../helpers/install_python.sh pypad_ando.py $(DESTDIR)/usr/lib/rivendell/pypad/pypad_ando.py
	cp pypad_ando.exemplar $(DESTDIR)/usr/lib/rivendell/pypad/pypad_ando.exemplar
	../../../helpers/install_python.sh pypad_filewrite.py $(DESTDIR)/usr/lib/rivendell/pypad/pypad_filewrite.py
	cp pypad_filewrite.exemplar $(DESTDIR)/usr/lib/rivendell/pypad/pypad_filewrite.exemplar
	../../../helpers/install_python.sh pypad_httpget.py $(DESTDIR)/usr/lib/rivendell/pypad/pypad_httpget.py
	cp pypad_httpget.exemplar $(DESTDIR)/usr/lib/rivendell/pypad/pypad_httpget.exemplar
	../../../helpers/install_python.sh pypad_icecast2.py $(DESTDIR)/usr/lib/rivendell/pypad/pypad_icecast2.py
	cp pypad_icecast2.exemplar $(DESTDIR)/usr/lib/rivendell/pypad/pypad_icecast2.exemplar
	../../../helpers/install_python.sh pypad_inno713.py $(DESTDIR)/usr/lib/rivendell/pypad/pypad_inno713.py
	cp pypad_inno713.exemplar $(DESTDIR)/usr/lib/rivendell/pypad/pypad_inno713.exemplar
	../../../helpers/install_python.sh pypad_ino713_tcp.py $(DESTDIR)/usr/lib/rivendell/pypad/pypad_ino713_tcp.py
	cp pypad_ino713_tcp.exemplar $(DESTDIR)/usr/lib/rivendell/pypad/pypad_ino713_tcp.exemplar
	../../../helpers/install_python.sh pypad_live365.py $(DESTDIR)/usr/lib/rivendell/pypad/pypad_live365.py
	cp pypad_live365.exemplar $(DESTDIR)/usr/lib/rivendell/pypad/pypad_live365.exemplar
	../../../helpers/install_python.sh pypad_liqcomp.py $(DESTDIR)/usr/lib/rivendell/pypad/pypad_liqcomp.py
	cp pypad_liqcomp.exemplar $(DESTDIR)/usr/lib/rivendell/pypad/pypad_liqcomp.exemplar
	../../../helpers/install_python.sh pypad_nautel.py $(DESTDIR)/usr/lib/rivendell/pypad/pypad_nautel.py
	cp pypad_nautel.exemplar $(DESTDIR)/usr/lib/rivendell/pypad/pypad_nautel.exemplar
	../../../helpers/install_python.sh pypad_serial.py $(DESTDIR)/usr/lib/rivendell/pypad/pypad_serial.py
	cp pypad_serial.exemplar $(DESTDIR)/usr/lib/rivendell/pypad/pypad_serial.exemplar
	../../../helpers/install_python.sh pypad_sftpwrite.py $(DESTDIR)/usr/lib/rivendell/pypad/pypad_sftpwrite.py
	cp pypad_sftpwrite.exemplar $(DESTDIR)/usr/lib/rivendell/pypad/pypad_sftpwrite.exemplar
	../../../helpers/install_python.sh pypad_shoutcast1.py $(DESTDIR)/usr/lib/rivendell/pypad/pypad_shoutcast1.py
	cp pypad_shoutcast1.exemplar $(DESTDIR)/usr/lib/rivendell/pypad/pypad_shoutcast1.exemplar
	../../../helpers/install_python.sh pypad_spinitron.py $(DESTDIR)/usr/lib/rivendell/pypad/pypad_spinitron.py
	cp pypad_spinitron.exemplar $(DESTDIR)/usr/lib/rivendell/pypad/pypad_spinitron.exemplar
	../../../helpers/install_python.sh pypad_spottrap.py $(DESTDIR)/usr/lib/rivendell/pypad/pypad_spottrap.py
	cp pypad_spottrap.exemplar $(DESTDIR)/usr/lib/rivendell/pypad/pypad_spottrap.exemplar
	../../../helpers/install_python.sh pypad_tunein.py $(DESTDIR)/usr/lib/rivendell/pypad/pypad_tunein.py
	cp pypad_tunein.exemplar $(DESTDIR)/usr/lib/rivendell/pypad/pypad_tunein.exemplar
	../../../helpers/install_python.sh pypad_udp.py $(DESTDIR)/usr/lib/rivendell/pypad/pypad_udp.py
	cp pypad_udp.exemplar $(DESTDIR)/usr/lib/rivendell/pypad/pypad_udp.exemplar
	../../../helpers/install_python.sh pypad_urlwrite.py $(DESTDIR)/usr/lib/rivendell/pypad/pypad_urlwrite.py
	cp pypad_urlwrite.exemplar $(DESTDIR)/usr/lib/rivendell/pypad/pypad_urlwrite.exemplar
	../../../helpers/install_python.sh pypad_walltime.py $(DESTDIR)/usr/lib/rivendell/pypad/pypad_walltime.py
	cp pypad_walltime.exemplar $(DESTDIR)/usr/lib/rivendell/pypad/pypad_walltime.exemplar
	../../../helpers/install_python.sh pypad_xcmd.py $(DESTDIR)/usr/lib/rivendell/pypad/pypad_xcmd.py
	cp pypad_xcmd.exemplar $(DESTDIR)/usr/lib/rivendell/pypad/pypad_xcmd.exemplar
	../../../helpers/install_python.sh pypad_xds.py $(DESTDIR)/usr/lib/rivendell/pypad/pypad_xds.py
	cp pypad_xds.exemplar $(DESTDIR)/usr/lib/rivendell/pypad/pypad_xds.exemplar
	../../../helpers/install_python.sh pypad_xmpad.py $(DESTDIR)/usr/lib/rivendell/pypad/pypad_xmpad.py
	cp pypad_xmpad.exemplar $(DESTDIR)/usr/lib/rivendell/pypad/pypad_xmpad.exemplar

uninstall-local:	
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_ando.exemplar
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_ando.py
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_filewrite.exemplar
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_filewrite.py
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_httpget.exemplar
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_httpget.py
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_icecast2.exemplar
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_icecast2.py
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_inno713.exemplar
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_inno713.py
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_ino713_tcp.exemplar
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_ino713_tcp.py
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_live365.exemplar
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_live365.py
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_liqcomp.exemplar
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_liqcomp.py
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_nautel.exemplar
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_nautel.py
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_serial.exemplar
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_serial.py
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_sftpwrite.exemplar
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_sftpwrite.py
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_shoutcast1.exemplar
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_shoutcast1.py
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_spinitron.exemplar
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_spinitron.py
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_spottrap.exemplar
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_spottrap.py
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_tunein.exemplar
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_tunein.py
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_udp.exemplar
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_udp.py
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_urlwrite.exemplar
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_urlwrite.py
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_walltime.exemplar
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_walltime.py
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_xcmd.exemplar
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_xcmd.py
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_xds.exemplar
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_xds.py
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_xmpad.exemplar
	rm -f $(DESTDIR)/usr/lib/rivendell/pypad/pypad_xmpad.py

EXTRA_DIST = pypad_ando.exemplar\
             pypad_ando.py\
             pypad_filewrite.exemplar\
             pypad_filewrite.py\
             pypad_httpget.exemplar\
             pypad_httpget.py\
             pypad_icecast2.exemplar\
             pypad_icecast2.py\
             pypad_inno713.exemplar\
             pypad_inno713.py\
             pypad_ino713_tcp.exemplar\
             pypad_ino713_tcp.py\
             pypad_liqcomp.exemplar\
             pypad_liqcomp.py\
             pypad_live365.exemplar\
             pypad_live365.py\
             pypad_nautel.exemplar\
             pypad_nautel.py\
             pypad_serial.exemplar\
             pypad_serial.py\
             pypad_sftpwrite.exemplar\
             pypad_sftpwrite.py\
             pypad_shoutcast1.exemplar\
             pypad_shoutcast1.py\
             pypad_spinitron.exemplar\
             pypad_spinitron.py\
             pypad_spottrap.exemplar\
             pypad_spottrap.py\
             pypad_tunein.exemplar\
             pypad_tunein.py\
             pypad_udp.exemplar\
             pypad_udp.py\
             pypad_urlwrite.exemplar\
             pypad_urlwrite.py\
             pypad_walltime.exemplar\
             pypad_walltime.py\
             pypad_xcmd.exemplar\
             pypad_xcmd.py\
             pypad_xds.exemplar\
             pypad_xds.py\
             pypad_xmpad.exemplar\
             pypad_xmpad.py

CLEANFILES = *~\
             *.idb\
             *ilk\
             *.obj\
             *.pdb\
             *.qm\
             moc_*

MAINTAINERCLEANFILES = *~\
                       *.tar.gz\
                       aclocal.m4\
                       configure\
                       Makefile.in\
                       moc_*
