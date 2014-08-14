## automake.am
##
## Automake.am for rivendell/tests
##
## (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
##
##      $Id: Makefile.am,v 1.14.8.4.2.1 2014/05/30 00:26:30 cvs Exp $
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

AM_CPPFLAGS = -Wall @QT_CXXFLAGS@ -I$(top_srcdir)/lib
LIBS = @QT_LIBS@ -L$(top_srcdir)/lib
MOC = @QT_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

noinst_PROGRAMS = audio_convert_test\
                  audio_export_test\
                  audio_import_test\
                  datedecode_test\
                  reserve_carts_test\
                  sas_switch_torture\
                  sas_torture\
                  stringcode_test\
                  test_pam\
                  timer_test\
                  upload_test

dist_audio_convert_test_SOURCES = audio_convert_test.cpp audio_convert_test.h
audio_convert_test_LDADD = @LIB_RDLIBS@ @LIBVORBIS@

dist_audio_export_test_SOURCES = audio_export_test.cpp audio_export_test.h
audio_export_test_LDADD = @LIB_RDLIBS@ @LIBVORBIS@

dist_audio_import_test_SOURCES = audio_import_test.cpp audio_import_test.h
audio_import_test_LDADD = @LIB_RDLIBS@ @LIBVORBIS@

dist_datedecode_test_SOURCES = datedecode_test.cpp datedecode_test.h
datedecode_test_LDADD = @LIB_RDLIBS@ @LIBVORBIS@

dist_reserve_carts_test_SOURCES = reserve_carts_test.cpp reserve_carts_test.h
reserve_carts_test_LDADD = @LIB_RDLIBS@ @LIBVORBIS@

dist_sas_switch_torture_SOURCES = sas_switch_torture.cpp sas_switch_torture.h
nodist_sas_switch_torture_SOURCES = moc_sas_switch_torture.cpp
sas_switch_torture_LDADD = @LIB_RDLIBS@ @LIBVORBIS@

dist_sas_torture_SOURCES = sas_torture.cpp sas_torture.h
nodist_sas_torture_SOURCES = moc_sas_torture.cpp
sas_torture_LDADD = @LIB_RDLIBS@ @LIBVORBIS@

dist_stringcode_test_SOURCES = stringcode_test.cpp stringcode_test.h
stringcode_test_LDADD = @LIB_RDLIBS@ @LIBVORBIS@

dist_test_pam_SOURCES = test_pam.cpp test_pam.h
test_pam_LDADD = @LIB_RDLIBS@ @LIBVORBIS@

dist_timer_test_SOURCES = timer_test.cpp timer_test.h
nodist_timer_test_SOURCES = moc_timer_test.cpp
timer_test_LDADD = -lqui


dist_upload_test_SOURCES = upload_test.cpp upload_test.h
upload_test_LDADD = @LIB_RDLIBS@ @LIBVORBIS@

EXTRA_DIST = rivendell_standard.txt\
             visualtraffic.txt

CLEANFILES = *~\
             moc_*

MAINTAINERCLEANFILES = *~\
                       *.tar.gz\
                       aclocal.m4\
                       configure\
                       Makefile.in\
                       moc_*
