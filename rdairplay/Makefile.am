## automake.am
##
## Automake.am for rivendell/rdairplay
##
## (C) 2002-2008 Fred Gleason <fredg@paravelsystems.com>
##
##      $Id: Makefile.am,v 1.48.8.5 2013/12/28 00:00:33 cvs Exp $
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

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -DQTDIR=\"@QT_DIR@\" @QT_CXXFLAGS@ -I$(top_srcdir)/lib  -Wno-strict-aliasing
LIBS = @QT_LIBS@ -L$(top_srcdir)/lib
MOC = @QT_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

# I18N Stuff
install-exec-local:	
	mkdir -p $(DESTDIR)$(prefix)/share/rivendell
	cp rdairplay_*.qm $(DESTDIR)$(prefix)/share/rivendell

uninstall:	
	rm -f $(DESTDIR)$(prefix)/share/srlabs/rdairplay_*.qm

all:	
	@QT_BIN@/lupdate rdairplay.pro
	@QT_BIN@/lrelease rdairplay.pro

bin_PROGRAMS = rdairplay

dist_rdairplay_SOURCES = button_log.cpp button_log.h\
                         edit_event.cpp edit_event.h\
                         globals.h\
                         hourselector.cpp hourselector.h\
                         lib_listview.cpp lib_listview.h\
                         list_log.cpp list_log.h\
                         list_logs.cpp list_logs.h\
                         local_macros.cpp colors.h\
                         log_play.cpp log_play.h\
                         log_traffic.cpp log_traffic.h\
                         loglinebox.cpp loglinebox.h\
                         mode_display.cpp mode_display.h\
                         nownext.cpp\
                         pie_counter.cpp pie_counter.h\
                         post_counter.cpp post_counter.h\
                         rdairplay.cpp rdairplay.h\
                         rlmhost.cpp rlmhost.h\
                         start_button.cpp start_button.h\
                         stop_counter.cpp stop_counter.h\
                         wall_clock.cpp wall_clock.h


nodist_rdairplay_SOURCES = moc_button_log.cpp\
                           moc_edit_event.cpp\
                           moc_hourselector.cpp\
                           moc_lib_listview.cpp\
                           moc_list_log.cpp\
                           moc_list_logs.cpp\
                           moc_log_play.cpp\
                           moc_loglinebox.cpp\
                           moc_mode_display.cpp\
                           moc_pie_counter.cpp\
                           moc_post_counter.cpp\
                           moc_rdairplay.cpp\
                           moc_rlmhost.cpp\
                           moc_start_button.cpp\
                           moc_stop_counter.cpp\
                           moc_wall_clock.cpp

rdairplay_LDADD = @LIB_RDLIBS@ @LIBVORBIS@
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
