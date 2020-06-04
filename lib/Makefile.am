## Makefile.am
##
## Automake.am for rivendell/lib
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

AM_CPPFLAGS = -Wall -DPREFIX=\"$(prefix)\" @QT4_CFLAGS@ @MUSICBRAINZ_CFLAGS@ -Wno-strict-aliasing -DQT3_SUPPORT -I/usr/include/Qt3Support -I/usr/include/taglib
MOC = @QT_MOC@
CWRAP = ../helpers/cwrap

# The dependency for qt's Meta Object Compiler (moc)
moc_%.cpp:	%.h
	$(MOC) $< -o $@

instdir = @LOCAL_PREFIX@/lib

# I18N Stuff
install-exec-local:	
	mkdir -p $(DESTDIR)$(prefix)/share/rivendell
	cp librd_*.qm $(DESTDIR)$(prefix)/share/rivendell

uninstall-local:
	rm -f $(DESTDIR)$(prefix)/share/rivendell/librd_*.qm

all:	
	lupdate-qt4 lib.pro
	lrelease-qt4 lib.pro

lib_LTLIBRARIES = librd.la
dist_librd_la_SOURCES = dbversion.h\
                        gpio.h\
                        export_bmiemr.cpp\
                        export_cutlog.cpp\
                        export_deltaflex.cpp\
                        export_musicclassical.cpp\
                        export_musicplayout.cpp\
                        export_musicsummary.cpp\
                        export_nprsoundex.cpp\
                        export_radiotraffic.cpp\
                        export_resultsrecon.cpp\
                        export_soundex.cpp\
                        export_spincount.cpp\
                        export_technical.cpp\
                        export_textlog.cpp\
                        rdadd_cart.cpp rdadd_cart.h\
                        rdadd_log.cpp rdadd_log.h\
                        rdairplay_conf.cpp rdairplay_conf.h\
                        rdapplication.cpp rdapplication.h\
                        rdaudio_exists.cpp rdaudio_exists.h\
                        rdaudio_port.cpp rdaudio_port.h\
                        rdaudioconvert.cpp rdaudioconvert.h\
                        rdaudioexport.cpp rdaudioexport.h\
                        rdaudioimport.cpp rdaudioimport.h\
                        rdaudioinfo.cpp rdaudioinfo.h\
                        rdaudiosettings.cpp rdaudiosettings.h\
                        rdaudiostore.cpp rdaudiostore.h\
                        rdbusybar.cpp rdbusybar.h\
                        rdbusydialog.cpp rdbusydialog.h\
                        rdbutton_dialog.cpp rdbutton_dialog.h\
                        rdbutton_panel.cpp rdbutton_panel.h\
                        rdcae.cpp rdcae.h\
                        rdcardselector.cpp rdcardselector.h\
                        rdcart.cpp rdcart.h\
                        rdcart_dialog.cpp rdcart_dialog.h\
                        rdcart_search_text.cpp rdcart_search_text.h\
                        rdcartdrag.cpp rdcartdrag.h\
                        rdcartslot.cpp rdcartslot.h\
                        rdcastsearch.cpp rdcastsearch.h\
                        rdcatch_conf.cpp rdcatch_conf.h\
                        rdcatch_connect.cpp rdcatch_connect.h\
                        rdcddblookup.cpp rdcddblookup.h\
                        rdcdplayer.cpp rdcdplayer.h\
                        rdcdripper.cpp rdcdripper.h\
                        rdcheck_version.cpp rdcheck_version.h\
                        rdclock.cpp rdclock.h\
                        rdcmd_cache.cpp rdcmd_cache.h\
                        rdcmd_switch.cpp rdcmd_switch.h\
                        rdcodetrap.cpp rdcodetrap.h\
                        rdcombobox.cpp rdcombobox.h\
                        rdconf.cpp rdconf.h\
                        rdconfig.cpp rdconfig.h\
                        rdcopyaudio.cpp rdcopyaudio.h\
                        rdcueedit.cpp rdcueedit.h\
                        rdcueeditdialog.cpp rdcueeditdialog.h\
                        rdcut.cpp rdcut.h\
                        rdcut_dialog.cpp rdcut_dialog.h\
                        rdcut_path.cpp rdcut_path.h\
                        rddatedecode.cpp rddatedecode.h\
                        rddatedialog.cpp rddatedialog.h\
                        rddatepicker.cpp rddatepicker.h\
		        rddb.h rddb.cpp\
                        rddbheartbeat.cpp rddbheartbeat.h\
                        rddatetime.cpp rddatetime.h\
                        rddebug.cpp rddebug.h\
                        rddeck.cpp rddeck.h\
                        rddelete.cpp rddelete.h\
                        rddialog.cpp rddialog.h\
                        rddisclookup.cpp rddisclookup.h\
                        rddisclookup_factory.cpp rddisclookup_factory.h\
                        rddiscrecord.cpp rddiscrecord.h\
                        rddownload.cpp rddownload.h\
                        rddropbox.cpp rddropbox.h\
                        rddummylookup.cpp rddummylookup.h\
                        rdedit_audio.cpp rdedit_audio.h\
                        rdedit_panel_name.cpp rdedit_panel_name.h\
                        rdemptycart.cpp rdemptycart.h\
                        rdescape_string.cpp rdescape_string.h\
                        rdevent.cpp rdevent.h\
                        rdevent_line.cpp rdevent_line.h\
                        rdevent_player.cpp rdevent_player.h\
                        rdeventimportlist.cpp rdeventimportlist.h\
                        rdexport_settings_dialog.cpp rdexport_settings_dialog.h\
                        rdfeed.cpp rdfeed.h\
                        rdfeedlog.cpp rdfeedlog.h\
                        rdfontengine.cpp rdfontengine.h\
                        rdformpost.cpp rdformpost.h\
                        rdflacdecode.cpp rdflacdecode.h\
                        rdframe.cpp rdframe.h\
                        rdgain_envelope.cpp rdgain_envelope.h\
                        rdget_ath.cpp rdget_ath.h\
                        rdgetpasswd.cpp rdgetpasswd.h\
                        rdgpio.cpp rdgpio.h\
                        rdgpioselector.cpp rdgpioselector.h\
                        rdgrid.cpp rdgrid.h\
                        rdgroup.cpp rdgroup.h\
                        rdgroup_list.cpp rdgroup_list.h\
                        rdhash.cpp rdhash.h\
                        rdhotkeys.cpp rdhotkeys.h\
                        rdhotkeylist.cpp rdhotkeylist.h\
                        rdidvalidator.cpp rdidvalidator.h\
                        rdimport_audio.cpp rdimport_audio.h\
                        rdinstancelock.cpp rdinstancelock.h\
                        rd.h\
                        rdkernelgpio.cpp rdkernelgpio.h\
                        rdlibrary_conf.cpp rdlibrary_conf.h\
                        rdlineedit.cpp rdlineedit.h\
                        rdlistselector.cpp rdlistselector.h\
                        rdlist_groups.cpp rdlist_groups.h\
                        rdlist_logs.cpp rdlist_logs.h\
                        rdlistsvcs.cpp rdlistsvcs.h\
                        rdlistview.cpp rdlistview.h\
                        rdlistviewitem.cpp rdlistviewitem.h\
                        rdlivewire.cpp rdlivewire.h\
                        rdlivewiredestination.cpp rdlivewiredestination.h\
                        rdlivewiresource.cpp rdlivewiresource.h\
                        rdlog.cpp rdlog.h\
                        rdlog_event.cpp rdlog_event.h\
                        rdlog_line.cpp rdlog_line.h\
                        rdlogedit_conf.cpp rdlogedit_conf.h\
                        rdlogfilter.cpp rdlogfilter.h\
                        rdloglock.cpp rdloglock.h\
                        rdlogplay.cpp rdlogplay.h\
                        rdmacro.cpp rdmacro.h\
                        rdmacro_event.cpp rdmacro_event.h\
                        rdmarker_bar.cpp rdmarker_bar.h\
                        rdmarker_button.cpp rdmarker_button.h\
                        rdmarker_edit.cpp rdmarker_edit.h\
                        rdmatrix.cpp rdmatrix.h\
                        rdmblookup.cpp rdmblookup.h\
                        rdmeteraverage.cpp rdmeteraverage.h\
                        rdmixer.cpp rdmixer.h\
                        rdmonitor_config.cpp rdmonitor_config.h\
			rdmp4.cpp rdmp4.h\
                        rdmulticaster.cpp rdmulticaster.h\
                        rdnotification.cpp rdnotification.h\
                        rdoneshot.cpp rdoneshot.h\
                        rdpam.cpp rdpam.h\
                        rdpanel_button.cpp rdpanel_button.h\
                        rdpasswd.cpp rdpasswd.h\
                        rdpaths.h\
                        rdplay_deck.cpp rdplay_deck.h\
                        rdplaymeter.cpp rdplaymeter.h\
                        rdpeaksexport.cpp rdpeaksexport.h\
                        rdpodcast.cpp rdpodcast.h\
                        rdprocess.cpp rdprocess.h\
                        rdprofile.cpp rdprofile.h\
                        rdpushbutton.cpp rdpushbutton.h\
                        rdrecording.cpp rdrecording.h\
                        rdrehash.cpp rdrehash.h\
                        rdrenderer.cpp rdrenderer.h\
                        rdreplicator.cpp rdreplicator.h\
                        rdreport.cpp rdreport.h\
                        rdringbuffer.cpp rdringbuffer.h\
                        rdripc.cpp rdripc.h\
                        rdschedcartlist.cpp rdschedcartlist.h\
                        rdschedcode.cpp rdschedcode.h\
                        rdschedcodes_dialog.cpp rdschedcodes_dialog.h\
                        rdschedruleslist.cpp rdschedruleslist.h\
                        rdsegmeter.cpp rdsegmeter.h\
                        rdsettings.cpp rdsettings.h\
                        rdsimpleplayer.cpp rdsimpleplayer.h\
                        rdslider.cpp rdslider.h\
                        rdslotbox.cpp rdslotbox.h\
                        rdslotdialog.cpp rdslotdialog.h\
                        rdslotoptions.cpp rdslotoptions.h\
                        rdsocket.cpp rdsocket.h\
                        rdsocketstrings.cpp rdsocketstrings.h\
                        rdsound_panel.cpp rdsound_panel.h\
                        rdstation.cpp rdstation.h\
                        rdstatus.cpp rdstatus.h\
                        rdstereometer.cpp rdstereometer.h\
                        rdstringlist.cpp rdstringlist.h\
                        rdsvc.cpp rdsvc.h\
                        rdsystem.cpp rdsystem.h\
                        rdsystemuser.cpp rdsystemuser.h\
                        rdtempdirectory.cpp rdtempdirectory.h\
                        rdtextfile.cpp rdtextfile.h\
                        rdtextvalidator.cpp rdtextvalidator.h\
                        rdtimeedit.cpp rdtimeedit.h\
                        rdtimeengine.cpp rdtimeengine.h\
                        rdtimeevent.cpp rdtimeevent.h\
                        rdtransportbutton.cpp rdtransportbutton.h\
                        rdtrimaudio.cpp rdtrimaudio.h\
                        rdtty.cpp rdtty.h\
                        rdttydevice.cpp rdttydevice.h\
                        rdttyout.cpp rdttyout.h\
                        rdurl.cpp rdurl.h\
                        rduser.cpp rduser.h\
                        rdupload.cpp rdupload.h\
                        rdunixserver.cpp rdunixserver.h\
                        rdunixsocket.cpp rdunixsocket.h\
                        rdversion.cpp rdversion.h\
                        rdwavedata.cpp rdwavedata.h\
                        rdwavedata_dialog.cpp rdwavedata_dialog.h\
                        rdwavefile.cpp rdwavefile.h\
                        rdwavepainter.cpp rdwavepainter.h\
                        rdweb.cpp rdweb.h\
                        rdwebresult.cpp rdwebresult.h\
                        rdwidget.cpp rdwidget.h\
                        rdxport_interface.h


nodist_librd_la_SOURCES = moc_rdadd_cart.cpp\
                          moc_rdadd_log.cpp\
                          moc_rdapplication.cpp\
                          moc_rdaudioconvert.cpp\
                          moc_rdaudioexport.cpp\
                          moc_rdaudioimport.cpp\
                          moc_rdaudioinfo.cpp\
                          moc_rdaudiostore.cpp\
                          moc_rdbusybar.cpp\
                          moc_rdbusydialog.cpp\
                          moc_rdbutton_dialog.cpp\
                          moc_rdcae.cpp\
                          moc_rdcardselector.cpp\
                          moc_rdcart_dialog.cpp\
                          moc_rdcartslot.cpp\
                          moc_rdcatch_connect.cpp\
                          moc_rdcddblookup.cpp\
                          moc_rdcdplayer.cpp\
                          moc_rdcdripper.cpp\
                          moc_rdcodetrap.cpp\
                          moc_rdcombobox.cpp\
                          moc_rdcueedit.cpp\
                          moc_rdcueeditdialog.cpp\
                          moc_rdcut_dialog.cpp\
                          moc_rddatedialog.cpp\
                          moc_rddatepicker.cpp\
                          moc_rddbheartbeat.cpp\
                          moc_rddelete.cpp\
                          moc_rddialog.cpp\
                          moc_rddisclookup.cpp\
                          moc_rddownload.cpp\
                          moc_rddummylookup.cpp\
                          moc_rdedit_audio.cpp\
                          moc_rdedit_panel_name.cpp\
                          moc_rdemptycart.cpp\
                          moc_rdevent_player.cpp\
                          moc_rdexport_settings_dialog.cpp\
                          moc_rdfeed.cpp\
                          moc_rdframe.cpp\
                          moc_rdget_ath.cpp\
                          moc_rdgetpasswd.cpp\
                          moc_rdgpio.cpp\
                          moc_rdgpioselector.cpp\
                          moc_rdhotkeys.cpp\
                          moc_rdhotkeylist.cpp\
                          moc_rdimport_audio.cpp\
                          moc_rdkernelgpio.cpp\
                          moc_rdlineedit.cpp\
                          moc_rdlist_groups.cpp\
                          moc_rdlist_logs.cpp\
                          moc_rdlistselector.cpp\
                          moc_rdlistsvcs.cpp\
                          moc_rdlistview.cpp\
                          moc_rdlivewire.cpp\
                          moc_rdlogfilter.cpp\
                          moc_rdloglock.cpp\
                          moc_rdlogplay.cpp\
                          moc_rdmacro_event.cpp\
                          moc_rdmarker_bar.cpp\
                          moc_rdmarker_edit.cpp\
                          moc_rdmblookup.cpp\
                          moc_rdmulticaster.cpp\
                          moc_rdoneshot.cpp\
                          moc_rdpanel_button.cpp\
                          moc_rdpasswd.cpp\
                          moc_rdplay_deck.cpp\
                          moc_rdplaymeter.cpp\
                          moc_rdprocess.cpp\
                          moc_rdpushbutton.cpp\
                          moc_rdrehash.cpp\
                          moc_rdrenderer.cpp\
                          moc_rdripc.cpp\
                          moc_rdschedcodes_dialog.cpp\
                          moc_rdsegmeter.cpp\
                          moc_rdsimpleplayer.cpp\
                          moc_rdslider.cpp\
                          moc_rdsocket.cpp\
                          moc_rdsound_panel.cpp\
                          moc_rdslotbox.cpp\
                          moc_rdslotdialog.cpp\
                          moc_rdstereometer.cpp\
                          moc_rdsvc.cpp\
                          moc_rdtimeedit.cpp\
                          moc_rdtimeengine.cpp\
                          moc_rdtransportbutton.cpp\
                          moc_rdtrimaudio.cpp\
                          moc_rdttydevice.cpp\
                          moc_rdunixserver.cpp\
                          moc_rdunixsocket.cpp\
                          moc_rdupload.cpp\
                          moc_rdwavedata_dialog.cpp\
                          moc_rdwidget.cpp

librd_la_LDFLAGS = -release $(VERSION)

EXTRA_DIST = lib.pro\
             librd_cs.ts\
             librd_de.ts\
             librd_es.ts\
             librd_fr.ts\
             librd_nb.ts\
             librd_nn.ts\
             librd_pt_BR.ts\
             rdpaths.h.in

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
