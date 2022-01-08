## Makefile.am
##
## Makefile.am for rivendell/apis
##
##   (C) Copyright 2018-2022 Fred Gleason <fredg@paravelsystems.com>
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

SUBDIRS = pypad\
          rivwebcapi\
          rivwebpyapi

pypi:	
	rm -rf pypi
	mkdir -p pypi/src/rivendellaudio
	touch pypi/src/rivendellaudio/__init__.py
	cp pypad/api/pypad.py pypi/src/rivendellaudio/
	cp rivwebpyapi/api/rivwebpyapi.py pypi/src/rivendellaudio/
	cp ../LICENSES/LGPLv2.txt pypi/LICENSE
	cp pyproject.toml pypi/
	cp setup.cfg pypi/
	mkdir -p pypi/tests
	python3 -m build pypi
	cp pypi/dist/* .
	cp pypi/src/rivendellaudio.egg-info/* .
	rm -rf pypi

post-pypi:	
	python3 -m twine upload --repository pypi *.tar.gz *.whl

EXTRA_DIST = pyproject.toml\
             setup.cfg.in

CLEANFILES = *~\
             *.idb\
             *ilk\
             *.obj\
             *.pdb\
             *.qm\
             *.tar.gz\
             *.whl\
             PKG-INFO\
             SOURCES.txt\
             dependency_links.txt\
             moc_*\
             not-zip-safe\
             requires.txt\
             top_level.txt

MAINTAINERCLEANFILES = *~\
                       *.tar.gz\
                       aclocal.m4\
                       configure\
                       Makefile.in\
                       moc_*
