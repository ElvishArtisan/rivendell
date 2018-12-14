## automake.am
##
## Automake.am for Rivendell PyPAD/examples
##
##   (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
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
	mkdir -p $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD
	../../../helpers/install_python.sh now_and_next.py $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/now_and_next.py
	../../../helpers/install_python.sh pypad_filewrite.py $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_filewrite.py
	../../../helpers/install_python.sh pypad_icecast2.py $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_icecast2.py
	../../../helpers/install_python.sh pypad_tunein.py $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_tunein.py
	../../../helpers/install_python.sh pypad_udp.py $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_udp.py

uninstall-local:	
	rm -f $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/now_and_next.py
	rm -f $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_filewrite.py
	rm -f $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_icecast2.py
	rm -f $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_tunein.py
	rm -f $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_udp.py

EXTRA_DIST = now_and_next.py\
             pypad_filewrite.py\
             pypad_icecast2.py\
             pypad_tunein.py\
             pypad_udp.py

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
