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
	../../../helpers/install_python.sh pypad_ando.py $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_ando.py
	cp pypad_ando.exemplar $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_ando.exemplar
	../../../helpers/install_python.sh pypad_filewrite.py $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_filewrite.py
	cp pypad_filewrite.exemplar $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_filewrite.exemplar
	../../../helpers/install_python.sh pypad_live365.py $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_live365.py
	cp pypad_live365.exemplar $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_live365.exemplar
	../../../helpers/install_python.sh pypad_liqcomp.py $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_liqcomp.py
	cp pypad_liqcomp.exemplar $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_liqcomp.exemplar
	../../../helpers/install_python.sh pypad_spinitron.py $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_spinitron.py
	cp pypad_spinitron.exemplar $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_spinitron.exemplar
	../../../helpers/install_python.sh pypad_udp.py $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_udp.py
	cp pypad_udp.exemplar $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_udp.exemplar
	../../../helpers/install_python.sh pypad_urlwrite.py $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_urlwrite.py
	cp pypad_urlwrite.exemplar $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_urlwrite.exemplar
	../../../helpers/install_python.sh pypad_walltime.py $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_walltime.py
	cp pypad_walltime.exemplar $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_walltime.exemplar

uninstall-local:	
	rm -f $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_ando.exemplar
	rm -f $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_ando.py
	rm -f $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_filewrite.exemplar
	rm -f $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_filewrite.py
	rm -f $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_live365.exemplar
	rm -f $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_live365.py
	rm -f $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_liqcomp.exemplar
	rm -f $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_liqcomp.py
	rm -f $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_spinitron.exemplar
	rm -f $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_spinitron.py
	rm -f $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_udp.exemplar
	rm -f $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_udp.py
	rm -f $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_urlwrite.exemplar
	rm -f $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_urlwrite.py
	rm -f $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_walltime.exemplar
	rm -f $(DESTDIR)$(prefix)/@RD_LIB_PATH@/rivendell/PyPAD/pypad_walltime.py

EXTRA_DIST = pypad_ando.exemplar\
             pypad_ando.py\
             pypad_filewrite.exemplar\
             pypad_filewrite.py\
             pypad_liqcomp.exemplar\
             pypad_liqcomp.py\
             pypad_live365.exemplar\
             pypad_live365.py\
             pypad_spinitron.exemplar\
             pypad_spinitron.py\
             pypad_udp.exemplar\
             pypad_udp.py\
             pypad_urlwrite.exemplar\
             pypad_urlwrite.py\
             pypad_walltime.exemplar\
             pypad_walltime.py

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
