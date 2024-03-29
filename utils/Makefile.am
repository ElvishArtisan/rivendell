## automake.am
##
## Automake.am for rivendell/utils
##
##   (C) Copyright 2002-2022 Fred Gleason <fredg@paravelsystems.com>
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

if ALSA_RD_AM
  ALSACONFIG_RD_OPT = rdalsaconfig
endif
SUBDIRS = $(ALSACONFIG_RD_OPT)\
          rdcheckcuts\
          rdcleandirs\
          rdclilogedit\
          rdcollect\
          rdconvert\
          rddbconfig\
          rddbmgr\
          rddelete\
          rdexport\
          rdgen\
          rdgpimon\
          rdimport\
          rdmaint\
          rdmarkerset\
          rdmetadata\
          rdpopup\
          rdrender\
          rdselect_helper\
          rdsinglestart\
          rdsoftkeys\
          rmlsend

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -DQTDIR=\"@QT_DIR@\" @QT_CXXFLAGS@ -I$(top_srcdir)/lib
LIBS = @QT_LIBS@ -L$(top_srcdir)/lib
MOC = @QT_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

EXTRA_DIST = utils.pro

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
