## Makefile.am
##
## Rivendell Interprocess Communication Daemon Makefile.am
##
## (C) Copyright 2002-2020 Fred Gleason <fredg@paravelsystems.com>
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

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -Wno-strict-aliasing -std=c++11 -fPIC -I$(top_srcdir)/lib @QT5_CFLAGS@ @MUSICBRAINZ_CFLAGS@ @IMAGEMAGICK_CFLAGS@ @LIBJACK_CFLAGS@
LIBS = -L$(top_srcdir)/lib -L$(top_srcdir)/rdhpi
MOC = @QT_MOC@

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@


sbin_PROGRAMS = ripcd

dist_ripcd_SOURCES = acu1p.cpp acu1p.h\
                     am16.cpp am16.h\
                     bt10x1.cpp bt10x1.h\
                     bt16x1.cpp bt16x1.h\
                     bt16x2.cpp bt16x2.h\
                     bt8x2.cpp bt8x2.h\
                     btacs82.cpp btacs82.h\
                     btadms4422.cpp btadms4422.h\
                     btgpi16.cpp btgpi16.h\
                     btsentinel4web.cpp btsentinel4web.h\
                     btss124.cpp btss124.h\
                     btss21.cpp btss21.h\
                     btss164.cpp btss164.h\
                     btss41mlr.cpp btss41mlr.h\
                     btss42.cpp btss42.h\
                     btss44.cpp btss44.h\
                     btss82.cpp btss82.h\
                     btsrc16.cpp btsrc16.h\
                     btsrc8iii.cpp btsrc8iii.h\
                     btu41mlrweb.cpp btu41mlrweb.h\
                     gvc7000.cpp gvc7000.h\
                     harlond.cpp harlond.h\
                     kernelgpio.cpp kernelgpio.h\
                     livewire_lwrpaudio.cpp livewire_lwrpaudio.h\
                     livewire_lwrpgpio.cpp livewire_lwrpgpio.h\
                     livewire_mcastgpio.cpp livewire_mcastgpio.h\
                     local_audio.cpp local_audio.h\
                     local_gpio.cpp local_gpio.h\
                     local_macros.cpp\
                     local_notifications.cpp\
                     loaddrivers.cpp\
                     modbus.cpp modbus.h\
                     modemlines.cpp modemlines.h\
                     quartz1.cpp quartz1.h\
                     ripcd.cpp ripcd.h globals.h\
                     ripcd_connection.cpp ripcd_connection.h\
                     rossnkscp.cpp rossnkscp.h\
                     sas32000.cpp sas32000.h\
                     sas64000.cpp sas64000.h\
                     sas64000gpi.cpp sas64000gpi.h\
                     sas16000.cpp sas16000.h\
                     sasusi.cpp sasusi.h\
                     starguide3.cpp starguide3.h\
                     starguide_feed.cpp starguide_feed.h\
                     swauthority.cpp swauthority.h\
                     switcher.cpp switcher.h\
                     unity4000.cpp unity4000.h\
                     unity_feed.cpp unity_feed.h\
                     vguest.cpp vguest.h\
                     wheatnet_lio.cpp wheatnet_lio.h\
                     wheatnet_slio.cpp wheatnet_slio.h

nodist_ripcd_SOURCES = moc_am16.cpp\
                       moc_acu1p.cpp\
                       moc_bt10x1.cpp\
                       moc_bt16x1.cpp\
                       moc_bt16x2.cpp\
                       moc_bt8x2.cpp\
                       moc_btacs82.cpp\
                       moc_btadms4422.cpp\
                       moc_btgpi16.cpp\
                       moc_btsentinel4web.cpp\
                       moc_btsrc16.cpp\
                       moc_btsrc8iii.cpp\
                       moc_btss124.cpp\
                       moc_btss164.cpp\
                       moc_btss21.cpp\
                       moc_btss41mlr.cpp\
                       moc_btss42.cpp\
                       moc_btss44.cpp\
                       moc_btss82.cpp\
                       moc_btu41mlrweb.cpp\
                       moc_gvc7000.cpp\
                       moc_harlond.cpp\
                       moc_kernelgpio.cpp\
                       moc_livewire_lwrpaudio.cpp\
                       moc_livewire_lwrpgpio.cpp\
                       moc_livewire_mcastgpio.cpp\
                       moc_local_audio.cpp\
                       moc_local_gpio.cpp\
                       moc_modbus.cpp\
                       moc_modemlines.cpp\
                       moc_quartz1.cpp\
                       moc_ripcd.cpp\
                       moc_rossnkscp.cpp\
                       moc_sas32000.cpp\
                       moc_sas64000.cpp\
                       moc_sas64000gpi.cpp\
                       moc_sas16000.cpp\
                       moc_sasusi.cpp\
                       moc_starguide3.cpp\
                       moc_swauthority.cpp\
                       moc_switcher.cpp\
                       moc_unity4000.cpp\
                       moc_vguest.cpp\
                       moc_wheatnet_lio.cpp\
                       moc_wheatnet_slio.cpp

ripcd_LDADD = @LIB_RDLIBS@\
              @LIBVORBIS@\
              @QT5_LIBS@\
              @LIBHPI@\
              @LIBJACK_LIBS@\
              @MUSICBRAINZ_LIBS@\
              @IMAGEMAGICK_LIBS@

CLEANFILES = *~\
             *.idb\
             *ilk\
             *.obj\
             *.pdb\
             moc_*

MAINTAINERCLEANFILES = *~\
                       Makefile.in
