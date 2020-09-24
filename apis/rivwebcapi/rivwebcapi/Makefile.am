## Makefile.am
##
## (C) Copyright 2015-2020 Fred Gleason <fredg@paravelsystems.com>
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

AM_CFLAGS = -I ..
lib_LTLIBRARIES = librivwebcapi.la 
dist_librivwebcapi_la_SOURCES = rd_addcart.c rd_addcart.h \
				rd_addcut.c rd_addcut.h \
				rd_addlog.c rd_addlog.h \
				rd_assignschedcode.c rd_assignschedcode.h\
				rd_audioinfo.c rd_audioinfo.h \
				rd_audiostore.c rd_audiostore.h \
				rd_common.c rd_common.h \
				rd_copyaudio.c rd_copyaudio.h \
				rd_createticket.c rd_createticket.h \
				rd_deleteaudio.c rd_deleteaudio.h \
				rd_deletelog.c rd_deletelog.h \
				rd_deletepodcast.c rd_deletepodcast.h \
				rd_editcart.c rd_editcart.h \
				rd_editcut.c rd_editcut.h \
				rd_export.c rd_export.h \
				rd_exportpeaks.c rd_exportpeaks.h \
				rd_getuseragent.c rd_getuseragent.h \
				rd_getversion.c rd_getversion.h \
				rd_import.c rd_import.h \
				rd_listcartschedcodes.c rd_listcartschedcodes.h\
				rd_listcart.c rd_listcart.h \
				rd_listcartcuts.c rd_listcartcuts.h \
				rd_listcarts.c rd_listcarts.h \
				rd_listcartscuts.c rd_listcartscuts.h \
				rd_listcut.c rd_listcut.h \
				rd_listcuts.c rd_listcuts.h \
				rd_listgroups.c rd_listgroups.h \
				rd_listgroup.c rd_listgroup.h \
				rd_listlog.c rd_listlog.h \
				rd_listlogs.c rd_listlogs.h \
				rd_listservices.c rd_listservices.h \
				rd_listschedcodes.c rd_listschedcodes.h \
				rd_listsystemsettings.c rd_listsystemsettings.h \
                                rd_postimage.c rd_postimage.h \
                                rd_postpodcast.c rd_postpodcast.h \
                                rd_postrss.c rd_postrss.h \
				rd_removecart.c rd_removecart.h \
				rd_removecut.c rd_removecut.h \
                                rd_removeimage.c rd_removeimage.h \
                                rd_removepodcast.c rd_removepodcast.h \
                                rd_removerss.c rd_removerss.h \
				rd_savelog.c rd_savelog.h \
				rd_savepodcast.c rd_savepodcast.h \
				rd_trimaudio.c rd_trimaudio.h \
				rd_unassignschedcode.c rd_unassignschedcode.h 

librivwebcapi_la_LDFLAGS = -version-info $(INTERFACE_RIVWEBCAPI_CURRENT):$(INTERFACE_RIVWEBCAPI_REVISION):$(INTERFACE_RIVWEBCAPI_AGE)

includedir = $(prefix)/include/rivwebcapi
include_HEADERS = rd_addcart.h\
                  rd_addcut.h\
                  rd_addlog.h\
                  rd_assignschedcode.h\
                  rd_audioinfo.h\
                  rd_audiostore.h\
                  rd_cart.h\
                  rd_common.h\
                  rd_copyaudio.h\
                  rd_createticket.h\
                  rd_cut.h\
                  rd_deleteaudio.h\
                  rd_deletelog.h\
                  rd_deletepodcast.h\
                  rd_editcart.h\
                  rd_editcut.h\
                  rd_export.h\
                  rd_exportpeaks.h\
                  rd_getuseragent.h\
                  rd_getversion.h\
                  rd_group.h\
                  rd_import.h\
                  rd_listcart.h\
                  rd_listcartcuts.h\
                  rd_listcartschedcodes.h\
                  rd_listcarts.h\
                  rd_listcartscuts.h\
                  rd_listcut.h\
                  rd_listcuts.h\
                  rd_listgroup.h\
                  rd_listgroups.h\
                  rd_listlog.h\
                  rd_listlogs.h\
                  rd_listschedcodes.h\
                  rd_listservices.h\
                  rd_listsystemsettings.h \
                  rd_postimage.h \
                  rd_postpodcast.h \
                  rd_postrss.h \
                  rd_savelog.h\
                  rd_savepodcast.h\
                  rd_schedcodes.h\
                  rd_removecart.h\
                  rd_removecut.h\
                  rd_removeimage.h\
                  rd_removepodcast.h\
                  rd_removerss.h\
                  rd_trimaudio.h\
                  rd_unassignschedcode.h

CLEANFILES = *~\
             moc_*\
             *.lib\
             *.obj\
             *.qm

DISTCLEANFILES = rdpaths.h

MAINTAINERCLEANFILES = *~\
                       *.tar.gz\
                       aclocal.m4\
                       configure\
                       Makefile.in\
                       moc_*
