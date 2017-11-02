## automake.am
##
## docs/opsguide/automake.am for Rivendell
##
## (C) Copyright 2017 Fred Gleason <fredg@paravelsystems.com>
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

##
## Build Dependencies
##
%.html:	%.xml
	xsltproc -o $@ $(DOCBOOK_STYLESHEETS)/xhtml/docbook.xsl $<
%.pdf:	%.xml
	xsltproc $(DOCBOOK_STYLESHEETS)/fo/docbook.xsl $< | fop - -pdf $@
%.1:	%.xml
	xsltproc $(DOCBOOK_STYLESHEETS)/manpages/docbook.xsl $<
%.8:	%.xml
	xsltproc $(DOCBOOK_STYLESHEETS)/manpages/docbook.xsl $<

all-local:	opsguide.html\
                opsguide.pdf

EXTRA_DIST = opsguide.html\
             opsguide.pdf\
             opsguide.xml\
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
             rdcatch.add_event_dialog.png\
             rdcatch.edit_cart_event_dialog.png\
             rdcatch.edit_download_dialog.png\
             rdcatch.edit_playout_dialog.png\
             rdcatch.edit_recording_dialog.png\
             rdcatch.edit_switcher_event_dialog.png\
             rdcatch.edit_upload_dialog.png\
             rdcatch.rdcatch_screenshot.png\
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
             voicetracking.voice_tracker_dialog_track_selected.png

CLEANFILES = *~
MAINTAINERCLEANFILES = *~\
                       *.1\
                       *.8\
                       *.html\
                       *.pdf\
                       aclocal.m4\
                       configure\
                       Makefile.in
