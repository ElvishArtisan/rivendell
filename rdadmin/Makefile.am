## Makefile.am
##
## Automake.am for rivendell/rdadmin
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

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" -I$(top_srcdir)/lib @QT4_CFLAGS@ @MUSICBRAINZ_CFLAGS@ -DQT3_SUPPORT -I/usr/include/Qt3Support
LIBS = -L$(top_srcdir)/lib
MOC = @QT_MOC@
CWRAP = ../helpers/cwrap

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

# The cwrap dependencies
global_gpl2.c:	
	$(CWRAP) -o global_gpl2.c gpl2.html
global_credits.c:	
	$(CWRAP) -o global_credits.c ../AUTHORS

# I18N Stuff
install-exec-local:	
	mkdir -p $(DESTDIR)$(prefix)/share/rivendell
	cp rdadmin_*.qm $(DESTDIR)$(prefix)/share/rivendell

uninstall-local:
	rm -f $(DESTDIR)$(prefix)/share/rivendell/rdadmin_*.qm

all:	
	lupdate-qt4 rdadmin.pro
	lrelease-qt4 rdadmin.pro

bin_PROGRAMS = rdadmin

dist_rdadmin_SOURCES = add_feed.cpp add_feed.h\
                       add_group.cpp add_group.h\
                       add_hostvar.cpp add_hostvar.h\
                       add_matrix.cpp add_matrix.h\
                       add_replicator.cpp add_replicator.h\
                       add_report.cpp add_report.h\
                       add_schedcodes.cpp add_schedcodes.h\
                       add_station.cpp add_station.h\
                       add_svc.cpp add_svc.h\
                       add_user.cpp add_user.h\
                       autofill_carts.cpp autofill_carts.h\
                       edit_audios.cpp edit_audios.h\
                       edit_cartslots.cpp edit_cartslots.h\
                       edit_channelgpios.cpp edit_channelgpios.h\
                       edit_decks.cpp edit_decks.h\
                       edit_dropbox.cpp edit_dropbox.h\
                       edit_endpoint.cpp edit_endpoint.h\
                       edit_feed.cpp edit_feed.h\
                       edit_feed_perms.cpp edit_feed_perms.h\
                       edit_gpi.cpp edit_gpi.h\
                       edit_group.cpp edit_group.h\
                       edit_hostvar.cpp edit_hostvar.h\
                       edit_hotkeys.cpp edit_hotkeys.h\
                       edit_image.cpp edit_image.h\
                       edit_jack.cpp edit_jack.h\
                       edit_jack_client.cpp edit_jack_client.h\
                       edit_livewiregpio.cpp edit_livewiregpio.h\
                       edit_matrix.cpp edit_matrix.h\
                       edit_node.cpp edit_node.h\
                       edit_pypad.cpp edit_pypad.h\
                       edit_rdairplay.cpp edit_rdairplay.h\
                       edit_rdlibrary.cpp edit_rdlibrary.h\
                       edit_rdlogedit.cpp edit_rdlogedit.h\
                       edit_rdpanel.cpp edit_rdpanel.h\
                       edit_replicator.cpp edit_replicator.h\
                       edit_report.cpp edit_report.h\
                       edit_sas_resource.cpp edit_sas_resource.h\
                       edit_schedcodes.cpp edit_schedcodes.h\
                       edit_station.cpp edit_station.h\
                       edit_superfeed.cpp edit_superfeed.h\
                       edit_svc.cpp edit_svc.h\
                       edit_svc_perms.cpp edit_svc_perms.h\
                       edit_system.cpp edit_system.h\
                       edit_ttys.cpp edit_ttys.h\
                       edit_user.cpp edit_user.h\
                       edit_user_perms.cpp edit_user_perms.h\
                       edit_user_service_perms.cpp edit_user_service_perms.h\
                       edit_vguest_resource.cpp edit_vguest_resource.h\
                       globals.h\
                       help_audios.cpp help_audios.h\
                       importfields.cpp importfields.h\
                       info_dialog.cpp info_dialog.h\
                       license.cpp license.h\
                       list_dropboxes.cpp list_dropboxes.h\
                       list_encoders.cpp list_encoders.h\
                       list_endpoints.cpp list_endpoints.h\
                       list_feeds.cpp list_feeds.h\
                       list_gpis.cpp list_gpis.h\
                       list_groups.cpp list_groups.h\
                       list_hostvars.cpp list_hostvars.h\
                       list_images.cpp list_images.h\
                       list_livewiregpios.cpp list_livewiregpios.h\
                       list_matrices.cpp list_matrices.h\
                       list_nodes.cpp list_nodes.h\
                       list_pypads.cpp list_pypads.h\
                       list_replicator_carts.cpp list_replicator_carts.h\
                       list_replicators.cpp list_replicators.h\
                       list_reports.cpp list_reports.h\
                       list_sas_resources.cpp list_sas_resources.h\
                       list_schedcodes.cpp list_schedcodes.h\
                       list_stations.cpp list_stations.h\
                       list_svcs.cpp list_svcs.h\
                       list_users.cpp list_users.h\
                       list_vguest_resources.cpp list_vguest_resources.h\
                       login.cpp login.h\
                       rdadmin.cpp rdadmin.h\
                       rename_group.cpp rename_group.h\
                       test_import.cpp test_import.h\
                       view_adapters.cpp view_adapters.h\
                       view_node_info.cpp view_node_info.h\
                       view_pypad_errors.cpp view_pypad_errors.h

nodist_rdadmin_SOURCES = global_credits.c\
                         global_gpl2.c\
                         moc_add_feed.cpp\
                         moc_add_group.cpp\
                         moc_add_hostvar.cpp\
                         moc_add_matrix.cpp\
                         moc_add_replicator.cpp\
                         moc_add_report.cpp\
                         moc_add_schedcodes.cpp\
                         moc_add_station.cpp\
                         moc_add_svc.cpp\
                         moc_add_user.cpp\
                         moc_autofill_carts.cpp\
                         moc_edit_audios.cpp\
                         moc_edit_cartslots.cpp\
                         moc_edit_channelgpios.cpp\
                         moc_edit_decks.cpp\
                         moc_edit_dropbox.cpp\
                         moc_edit_endpoint.cpp\
                         moc_edit_feed.cpp\
                         moc_edit_feed_perms.cpp\
                         moc_edit_gpi.cpp\
                         moc_edit_group.cpp\
                         moc_edit_hostvar.cpp\
                         moc_edit_hotkeys.cpp\
                         moc_edit_image.cpp\
                         moc_edit_jack.cpp\
                         moc_edit_jack_client.cpp\
                         moc_edit_livewiregpio.cpp\
                         moc_edit_matrix.cpp\
                         moc_edit_node.cpp\
                         moc_edit_pypad.cpp\
                         moc_edit_rdairplay.cpp\
                         moc_edit_rdlibrary.cpp\
                         moc_edit_rdlogedit.cpp\
                         moc_edit_rdpanel.cpp\
                         moc_edit_replicator.cpp\
                         moc_edit_report.cpp\
                         moc_edit_sas_resource.cpp\
                         moc_edit_schedcodes.cpp\
                         moc_edit_station.cpp\
                         moc_edit_superfeed.cpp\
                         moc_edit_svc.cpp\
                         moc_edit_svc_perms.cpp\
                         moc_edit_system.cpp\
                         moc_edit_ttys.cpp\
                         moc_edit_user.cpp\
                         moc_edit_user_perms.cpp\
                         moc_edit_user_service_perms.cpp\
                         moc_edit_vguest_resource.cpp\
                         moc_help_audios.cpp\
                         moc_importfields.cpp\
                         moc_info_dialog.cpp\
                         moc_license.cpp\
                         moc_list_dropboxes.cpp\
                         moc_list_encoders.cpp\
                         moc_list_endpoints.cpp\
                         moc_list_feeds.cpp\
                         moc_list_gpis.cpp\
                         moc_list_groups.cpp\
                         moc_list_hostvars.cpp\
                         moc_list_images.cpp\
                         moc_list_livewiregpios.cpp\
                         moc_list_matrices.cpp\
                         moc_list_nodes.cpp\
                         moc_list_pypads.cpp\
                         moc_list_replicator_carts.cpp\
                         moc_list_replicators.cpp\
                         moc_list_reports.cpp\
                         moc_list_sas_resources.cpp\
                         moc_list_schedcodes.cpp\
                         moc_list_stations.cpp\
                         moc_list_svcs.cpp\
                         moc_list_users.cpp\
                         moc_list_vguest_resources.cpp\
                         moc_login.cpp\
                         moc_rdadmin.cpp\
                         moc_rename_group.cpp\
                         moc_test_import.cpp\
                         moc_view_adapters.cpp\
                         moc_view_node_info.cpp\
                         moc_view_pypad_errors.cpp

rdadmin_LDADD = @LIB_RDLIBS@ -lsamplerate @LIBVORBIS@ @QT4_LIBS@ @MUSICBRAINZ_LIBS@ -lQt3Support

EXTRA_DIST = gpl2.html\
             info_banner1.xpm\
             info_banner2.xpm\
             rdadmin.pro\
             rdadmin_cs.ts\
             rdadmin_de.ts\
             rdadmin_es.ts\
             rdadmin_fr.ts\
             rdadmin_nb.ts\
             rdadmin_nn.ts\
             rdadmin_pt_BR.ts\
             xpm_info_banner1.cpp\
             xpm_info_banner2.cpp

CLEANFILES = *~\
             global_credits.c\
             global_gpl2.c\
             *.qm\
             moc_*

MAINTAINERCLEANFILES = *~\
                       *.tar.gz\
                       aclocal.m4\
                       configure\
                       Makefile.in\
                       moc_*
