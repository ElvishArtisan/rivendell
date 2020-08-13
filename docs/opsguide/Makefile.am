## automake.am
##
## docs/opsguide/automake.am for Rivendell
##
## (C) Copyright 2017-2019 Fred Gleason <fredg@paravelsystems.com>
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
## Use automake to process this into a Makefile.in

OPSGUIDE_HTML = appendix.filepath_wildcards.html\
                appendix.gfdl.html\
                appendix.gpio_switcher_devices.html\
                appendix.gplv2.html\
                appendix.metadata_wildcards.html\
                appendix.rivendell_filter.html\
                appendix.wings_filter.html\
                chapter.overview.html\
                chapter.pad.html\
                chapter.rdadmin.html\
                chapter.rdairplay.html\
                chapter.rdcartslots.html\
                chapter.rdcatch.html\
                chapter.rddbconfig.html\
                chapter.rdlibrary.html\
                chapter.rdlogedit.html\
                chapter.rdlogin.html\
                chapter.rdlogmanager.html\
                chapter.rdcastmanager.html\
                chapter.rml.html\
                chapter.utilities.html\
                chapter.voicetracking.html\
                chapter.webget.html\
                index.html\
                part.appendices.html\
                part.rivendell_administration.html\
                part.rivendell_operations.html\
                rivendell_filter.procedure.html\
                rivendell_filter.usage.html\
                sect.filepath_wildcards.examples.html\
                sect.gpio_switcher_devices.8x2.html\
                sect.gpio_switcher_devices.acu1p.html\
                sect.gpio_switcher_devices.bt10x1.html\
                sect.gpio_switcher_devices.bt16x1.html\
                sect.gpio_switcher_devices.bt16x2.html\
                sect.gpio_switcher_devices.btacs82.html\
                sect.gpio_switcher_devices.btadms4422.html\
                sect.gpio_switcher_devices.btgpi16.html\
                sect.gpio_switcher_devices.btsentinel4web.html\
                sect.gpio_switcher_devices.btsrc16.html\
                sect.gpio_switcher_devices.btss124.html\
                sect.gpio_switcher_devices.btss164.html\
                sect.gpio_switcher_devices.btss41mlr.html\
                sect.gpio_switcher_devices.btss42.html\
                sect.gpio_switcher_devices.btss44.html\
                sect.gpio_switcher_devices.btss82.html\
                sect.gpio_switcher_devices.btu41mlrweb.html\
                sect.gpio_switcher_devices.gvc7000.html\
                sect.gpio_switcher_devices.harlond.html\
                sect.gpio_switcher_devices.kernelgpio.html\
                sect.gpio_switcher_devices.livewire_lwrpaudio.html\
                sect.gpio_switcher_devices.livewire_lwrpgpio.html\
                sect.gpio_switcher_devices.livewire_mcastgpio.html\
                sect.gpio_switcher_devices.local_audio.html\
                sect.gpio_switcher_devices.local_gpio.html\
                sect.gpio_switcher_devices.modbus.html\
                sect.gpio_switcher_devices.modemlines.html\
                sect.gpio_switcher_devices.quartz1.html\
                sect.gpio_switcher_devices.rossnkscp.html\
                sect.gpio_switcher_devices.sas16000.html\
                sect.gpio_switcher_devices.sas32000.html\
                sect.gpio_switcher_devices.sas64000gpi.html\
                sect.gpio_switcher_devices.sas64000.html\
                sect.gpio_switcher_devices.sasusi.html\
                sect.gpio_switcher_devices.starguide3.html\
                sect.gpio_switcher_devices.swauthority.html\
                sect.gpio_switcher_devices.unity4000.html\
                sect.gpio_switcher_devices.vguest.html\
                sect.gpio_switcher_devices.wheatnet_lio.html\
                sect.gpio_switcher_devices.wheatnet_slio.html\
                sect.gplv2.how_to_apply_these_terms_to_your_new_programs.html\
                sect.rdadmin.manage_groups.html\
                sect.rdadmin.manage_hosts.html\
                sect.rdadmin.manage_replicators.html\
                sect.rdadmin.manage_scheduler_codes.html\
                sect.rdadmin.manage_services.html\
                sect.rdadmin.manage_system_settings.html\
                sect.rdadmin.managing_users.html\
                sect.rdairplay.editing_a_log.html\
                sect.rdairplay.layout.html\
                sect.rdairplay.log_machines.html\
                sect.rdcartslots.operation_in_breakaway_mode.html\
                sect.rdcartslots.operation_in_cart_deck_mode.html\
                sect.rdcartslots.setting_the_slot_options.html\
                sect.rdcastmanager.deleting_an_item.html\
                sect.rdcastmanager.editing_an_items_metadata.html\
                sect.rdcastmanager.posting_creating_a_new_item.html\
                sect.rdcatch.adding_new_events.html\
                sect.rdcatch_automating_macro_execution.html\
                sect.rdcatch.automating_playouts.html\
                sect.rdcatch.automating_recordings.html\
                sect.rdcatch.automating_switcher_operations.html\
                sect.rdcatch.automating_uploads_downloads.html\
                sect.rdcatch.the_rdcatch_main_window.html\
                sect.rddbconfig.backing_up_the_database.html\
                sect.rddbconfig.creating_a_new_database.html\
                sect.rddbconfig.restoring_the_database_from_a_backup.html\
                sect.rdlibrary.alternative_methods_of_audio_ingestion.html\
                sect.rdlibrary.generating_library_reports.html\
                sect.rdlibrary.library_maintenance.html\
                sect.rdlibrary.macro_carts.html\
                sect.rdlibrary.navigating_the_audio_library.html\
                sect.rdlogedit.auditioning_audio.html\
                sect.rdlogedit.editing_log_event_parameters.html\
                sect.rdlogedit.event_transitions.html\
                sect.rdlogedit.generating_log_reports.html\
                sect.rdlogedit.time_and_time_types.html\
                sect.rdlogmanager.clocks.html\
                sect.rdlogmanager.events.html\
                sect.rdlogmanager.generating_logs.html\
                sect.rdlogmanager.generating_reports.html\
                sect.rdlogmanager.grids.html\
                sect.rml.binary_data.html\
                sect.rml.command_delivery.html\
                sect.rml.command_reply.html\
                sect.rml.commands.html\
                sect.rml.protocol.html\
                sect.rml.specifying_color.html\
                sect.utilities.mlsend_1_.html\
                sect.utilities.rdconvert.html\
                sect.utilities.rddbcheck.html\
                sect.utilities.rdexport.html\
                sect.utilities.rdimport.html\
                sect.utilities.rdmarkerset.html\
                sect.utilities.rdrevert.html\
                sect.wings_filter.procedure.html\
                sect.wings_filter.usage.html\
                sect.voicetracking.using_the_voicetracker_dialog.html

OPSGUIDE_IMAGES = rdadmin.audio_resource_information_dialog.png\
                  rdadmin.autofill_carts_dialog.png\
                  rdadmin.configure_rdairplay_dialog.png\
                  rdadmin.configure_rdcartslots_dialog.png\
                  rdadmin.configure_rdcatch_dialog.png\
                  rdadmin.configure_rdlibrary_dialog.png\
                  rdadmin.configure_rdlogedit_dialog.png\
                  rdadmin.configure_rdpanel_dialog.png\
                  rdadmin.dropbox_configuration_dialog.png\
                  rdadmin.edit_audio_ports_dialog.png\
                  rdadmin.edit_channel_gpios_dialog.png\
                  rdadmin.edit_gpi_dialog.png\
                  rdadmin.edit_host_variable_dialog.png\
                  rdadmin.edit_pypad_instance_dialog.png\
                  rdadmin.edit_serial_ports_dialog.png\
                  rdadmin.edit_service_dialog.png\
                  rdadmin.edit_switcher_dialog.png\
                  rdadmin.group_dialog.png\
                  rdadmin.host_dialog.png\
                  rdadmin.host_variables_dialog.png\
                  rdadmin.hosts_dialog.png\
                  rdadmin.import_parser_fields.png\
                  rdadmin.jack_configuration_dialog.png\
                  rdadmin.list_gpis_dialog.png\
                  rdadmin.log_importation_string_fields.png\
                  rdadmin.login_dialog.png\
                  rdadmin.pypad_instances_dialog.png\
                  rdadmin.rdadmin_screenshot.png\
                  rdadmin.rdairplay_channel_assignments.png\
                  rdadmin.rdairplay_display_settings.png\
                  rdadmin.rdairplay_edit_now___next_data_dialog.png\
                  rdadmin.rdairplay_edit_plugin_dialog.png\
                  rdadmin.rdairplay_hotkey_configuration_dialog.png\
                  rdadmin.rdairplay_log_mode_control.png\
                  rdadmin.rdairplay_log_settings.png\
                  rdadmin.rdairplay_miscellaneous_settings.png\
                  rdadmin.rdairplay_sound_panel_settings.png\
                  rdadmin.rdairplay_start_stop_settings.png\
                  rdadmin.rename_group_dialog.png\
                  rdadmin.replicator_dialog.png\
                  rdadmin.rivendell_dropbox_configurations_dialog.png\
                  rdadmin.rivendell_group_list_dialog.png\
                  rdadmin.rivendell_host_list_dialog.png\
                  rdadmin.rivendell_replicators_dialog.png\
                  rdadmin.rivendell_scheduler_code_list_dialog.png\
                  rdadmin.rivendell_switcher_list_dialog.png\
                  rdadmin.rivendell_user_list_dialog.png\
                  rdadmin.scheduler_code_dialog.png\
                  rdadmin.select_pypad_script_dialog.png\
                  rdadmin.select_scheduler_codes_dialog.png\
                  rdadmin.services_dialog.png\
                  rdadmin.system_wide_settings_dialog.png\
                  rdadmin.test_import_dialog.png\
                  rdadmin.user_dialog.png\
                  rdairplay.audio_meter.png\
                  rdairplay.button_log_widget.png\
                  rdairplay.delete_from.png\
                  rdairplay.edit_button_dialog.png\
                  rdairplay.edit_buttons.png\
                  rdairplay.edit_event_dialog.png\
                  rdairplay.full_log_widget.png\
                  rdairplay.label_widget.png\
                  rdairplay.mode_widget.png\
                  rdairplay.next_stop_widget.png\
                  rdairplay.piewedge_widget.png\
                  rdairplay.postpoint_early_widget.png\
                  rdairplay.postpoint_late_widget.png\
                  rdairplay.postpoint_ontime_widget.png\
                  rdairplay.rdairplay_screenshot.png\
                  rdairplay.select_a_log_dialog.png\
                  rdairplay.soundpanel_widget.png\
                  rdairplay.wallclock_widget.png\
                  rdairplay.where_to.png\
                  rdcastmanager.delete_item_dialog.png\
                  rdcastmanager.editing_item_dialog.png\
                  rdcastmanager.list_log_events_dialog.png\
                  rdcastmanager.list_log_select_select_audio_file_dialog.png\
                  rdcastmanager.log_render_options_dialog.png\
                  rdcastmanager.podcast_item_list_dialog.png\
                  rdcastmanager.rdcastmanager_screenshot.png\
                  rdcastmanager.select_cut_dialog.png\
                  rdcastmanager.select_log_dialog.png\
                  rdcartslots.rdcartslots.edit_slot_options_dialog.png\
                  rdcartslots.rdcartslots_screenshot.png\
                  rdcartslots.rivendell_services_dialog.png\
                  rdcartslots.select_cart_dialog.png\
                  rdcatch.add_event_dialog.png\
                  rdcatch.edit_cart_event_dialog.png\
                  rdcatch.edit_download_dialog.png\
                  rdcatch.edit_playout_dialog.png\
                  rdcatch.edit_recording_dialog.png\
                  rdcatch.edit_switcher_event_dialog.png\
                  rdcatch.edit_upload_dialog.png\
                  rdcatch.rdcatch_screenshot.png\
                  rddbconfig.login_dialog.png\
                  rddbconfig.rddbconfig_screenshot.png\
                  rdlibrary.cd_ripper_dialog.png\
                  rdlibrary.edit_cart_dialog.png\
                  rdlibrary.edit_cut_info_dialog.png\
                  rdlibrary.edit_macro_cart_dialog.png\
                  rdlibrary.edit_marker_dialog.png\
                  rdlibrary.import_export_dialog.png\
                  rdlibrary.rdlibrary_screenshot.png\
                  rdlibrary.rip_disk_dialog.png\
                  rdlibrary.set_cart_cut_dialog.png\
                  rdlogedit.edit_log_chain_dialog.png\
                  rdlogedit.edit_log_dialog.png\
                  rdlogedit.edit_log_entry_dialog.png\
                  rdlogedit.edit_log_marker_dialog.png\
                  rdlogedit.edit_voice_track_marker_dialog.png\
                  rdlogedit.rdlogedit_screenshot.png\
                  rdlogedit.select_cart_dialog.png\
                  rdlogin.rdlogin_screenshot.png\
                  rdlogmanager.edit_clock_dialog.png\
                  rdlogmanager.edit_event_assignment_dialog.png\
                  rdlogmanager.edit_grid_dialog.png\
                  rdlogmanager.editing_event.png\
                  rdlogmanager.generate_log_dialog.png\
                  rdlogmanager.generate_log_dialog_fully_merged.png\
                  rdlogmanager.generate_log_dialog_music_merge_required.png\
                  rdlogmanager.generate_log_dialog_traffic_merge_required.png\
                  rdlogmanager.log_clock_list.png\
                  rdlogmanager.log_clocks.png\
                  rdlogmanager.log_events_dialog.png\
                  rdlogmanager.log_grids_dialog.png\
                  rdlogmanager.rdlogmanager_screenshot.png\
                  rdlogmanager.rivendell_services_dialog.png\
                  rdlogmanager.select_date_dialog.png\
                  rdlogmanager.select_report_dates_dialog.png\
                  rdlogmanager.service_association_dialog.png\
                  utilities.rmlsend_screenshot.png\
                  voicetracking.voice_tracker_dialog.png\
                  voicetracking.voice_tracker_dialog_recording.png\
                  voicetracking.voice_tracker_dialog_track_1_playing.png\
                  voicetracking.voice_tracker_dialog_track_2_playing.png\
                  voicetracking.voice_tracker_dialog_track_completed.png\
                  voicetracking.voice_tracker_dialog_track_selected.png\
                  webget.webget_screen.png

OPSGUIDE_ICONS = blueball.png\
                 chain.png\
                 greenball.png\
                 greencheckmark.png\
                 mic16.png\
                 music.png\
                 notemarker.png\
                 play.png\
                 redball.png\
                 redx.png\
                 rml5.png\
                 track_cart.png\
                 traffic.png\
                 trashcan-32x32.png\
                 whiteball.png

OPSGUIDE_DEPS = $(OPSGUIDE_IMAGES) \
                $(OPSGUIDE_ICONS) \
                filepath_wildcards.xml\
                gpio_switcher_devices.xml\
                gfdl.xml\
                gplv2.xml\
                opsguide.xml\
                overview.xml\
                metadata_wildcards.xml\
                pad.xml\
                rdadmin.xml\
                rdairplay.xml\
                rdcartslots.xml\
                rdcastmanager.xml\
                rdcatch.xml\
                rddbconfig.xml\
                rdlibrary.xml\
                rdlogedit.xml\
                rdlogin.xml\
                rdlogmanager.xml\
                rivendell_filter.xml\
                wings_filter.xml\
                rml.xml\
                utilities.xml\
                voicetracking.xml\
                webget.xml

%.html:	$(OPSGUIDE_DEPS)\
                ../../PACKAGE_VERSION
	xsltproc ../stylesheets/book-xhtml.xml opsguide.xml

opsguide.pdf:	$(OPSGUIDE_DEPS) ../../PACKAGE_VERSION\
                ../stylesheets/book-fo.xml\
                ../stylesheets/book-fo-titlepages.xsl
	xsltproc ../stylesheets/book-fo.xml opsguide.xml | fop - -pdf opsguide.pdf

install-exec-local:	
	mkdir -p $(DESTDIR)$(prefix)/share/rivendell/html
	cp opsguide.pdf $(DESTDIR)$(prefix)/share/rivendell/
	cp $(OPSGUIDE_HTML) $(DESTDIR)$(prefix)/share/rivendell/html/
	cp $(OPSGUIDE_IMAGES) $(DESTDIR)$(prefix)/share/rivendell/html/
	cp $(OPSGUIDE_ICONS) $(DESTDIR)$(prefix)/share/rivendell/html/

uninstall-local:
	rm -rf $(DESTDIR)$(prefix)/share/rivendell/html
	rm -rf $(DESTDIR)$(prefix)/share/rivendell/opsguide.pdf

all-local:	$(OPSGUIDE_HTML) \
                $(OPSGUIDE_IMAGES) \
                opsguide.pdf

EXTRA_DIST = $(OPSGUIDE_DEPS)\
             $(OPSGUIDE_HTML)\
             frontspiece.xml\
             frontspiece.xml.in\
             opsguide.pdf

CLEANFILES = *~\
             *.html\
             *.pdf

DISTCLEANFILES = chain.png \
                 blueball.png\
                 greenball.png\
                 greencheckmark.png\
                 mic16.png \
                 music.png \
                 notemarker.png \
                 play.png \
                 redball.png\
                 redx.png\
                 rml5.png \
                 track_cart.png \
                 traffic.png \
                 trashcan-32x32.png\
                 whiteball.png

MAINTAINERCLEANFILES = *~\
                       *.1\
                       *.8\
                       *.html\
                       *.pdf\
                       aclocal.m4\
                       configure\
                       frontspiece.xml\
                       Makefile.in
