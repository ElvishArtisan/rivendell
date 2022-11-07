## Makefile.am
##
## Automake.am for rivendell/rdairplay
##
## (C) 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -I$(top_srcdir)/lib  -Wno-strict-aliasing -std=c++11 -fPIC @QT5_CFLAGS@ @MUSICBRAINZ_CFLAGS@ @IMAGEMAGICK_CFLAGS@
LIBS = -L$(top_srcdir)/lib
MOC = @QT_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

# I18N Stuff
install-exec-local:	
	../helpers/rdi18n_helper.sh --install rdairplay $(DESTDIR)$(prefix)/share/rivendell

uninstall-local:	
	../helpers/rdi18n_helper.sh --uninstall rdairplay $(DESTDIR)$(prefix)/share/rivendell

all:	
	../helpers/rdi18n_helper.sh --update rdairplay $(DESTDIR)$(prefix)/share/rivendell

bin_PROGRAMS = rdairplay

dist_rdairplay_SOURCES = button_log.cpp button_log.h\
                         edit_event.cpp edit_event.h\
                         hourselector.cpp hourselector.h\
                         list_log.cpp list_log.h\
                         list_logs.cpp list_logs.h\
                         local_macros.cpp colors.h\
                         loglinebox.cpp loglinebox.h\
                         logtableview.cpp logtableview.h\
                         mode_display.cpp mode_display.h\
                         pie_counter.cpp pie_counter.h\
                         post_counter.cpp post_counter.h\
                         rdairplay.cpp rdairplay.h\
                         soundpanel.cpp soundpanel.h\
                         start_button.cpp start_button.h\
                         stop_counter.cpp stop_counter.h\
                         topstrip.cpp topstrip.h\
                         voicetracker.cpp voicetracker.h\
                         wall_clock.cpp wall_clock.h


nodist_rdairplay_SOURCES = moc_button_log.cpp\
                           moc_edit_event.cpp\
                           moc_hourselector.cpp\
                           moc_list_log.cpp\
                           moc_list_logs.cpp\
                           moc_loglinebox.cpp\
                           moc_logtableview.cpp\
                           moc_mode_display.cpp\
                           moc_pie_counter.cpp\
                           moc_post_counter.cpp\
                           moc_rdairplay.cpp\
                           moc_soundpanel.cpp\
                           moc_start_button.cpp\
                           moc_stop_counter.cpp\
                           moc_topstrip.cpp\
                           moc_voicetracker.cpp\
                           moc_wall_clock.cpp

rdairplay_LDADD = @LIB_RDLIBS@ @LIBVORBIS@ @QT5_LIBS@ @MUSICBRAINZ_LIBS@ @IMAGEMAGICK_LIBS@
rdairplay_LDFLAGS = -rdynamic

EXTRA_DIST = rdairplay.pro\
             rdairplay_cs.ts\
             rdairplay_de.ts\
             rdairplay_es.ts\
             rdairplay_fr.ts\
             rdairplay_nb.ts\
             rdairplay_nn.ts\
             rdairplay_pt_BR.ts

CLEANFILES = *~\
             *.qm\
             moc_*

MAINTAINERCLEANFILES = *~\
                       *.tar.gz\
                       aclocal.m4\
                       configure\
                       Makefile.in\
                       moc_*
