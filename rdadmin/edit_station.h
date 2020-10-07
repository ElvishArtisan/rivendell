// edit_station.h
//
// Edit a Rivendell Workstation
//
//   (C) Copyright 2002-2020 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public
//   License along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#ifndef EDIT_STATION_H
#define EDIT_STATION_H

#include <qcombobox.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qspinbox.h>

#include <rddialog.h>
#include <rdcardselector.h>
#include <rdcatch_connect.h>
#include <rdripc.h>
#include <rdstation.h>

class EditStation : public RDDialog
{
  Q_OBJECT
  public:
   EditStation(QString station,QWidget *parent=0);
   ~EditStation();
   QSize sizeHint() const;
   QSizePolicy sizePolicy() const;

 private slots:
   void selectClicked();
   void heartbeatToggledData(bool state);
   void heartbeatClickedData();
   void caeStationActivatedData(const QString &station_name);
   void okData();
   void okTimerData();
   void cancelData();
   void editLibraryData();
   void editDeckData();
   void editAirPlayData();
   void editPanelData();
   void editLogEditData();
   void editCartSlotsData();
   void viewAdaptersData();
   void editAudioData();
   void editTtyData();
   void editSwitcherData();
   void editHostvarsData();
   void editDropboxesData();
   void jackSettingsData();
   void pypadInstancesData();
   void startCartClickedData();
   void stopCartClickedData();

 protected:
  void resizeEvent(QResizeEvent *e);

  private:
   QString DisplayPart(QString);
   QString HostPart(QString);
   RDStation *station_station;
   RDStation *station_cae_station;
   RDCatchConnect *station_catch_connect;
   QLabel *station_name_label;
   QLineEdit *station_name_edit;
   QLabel *station_short_name_label;
   QLineEdit *station_short_name_edit;
   QLabel *station_description_label;
   QLineEdit *station_description_edit;
   QLabel *station_default_name_label;
   QComboBox *station_default_name_edit;
   QLabel *station_address_label;
   QLineEdit *station_address_edit;
   QLabel *station_audio_editor_label;
   QLineEdit *station_audio_editor_edit;
   QLabel *station_report_editor_label;
   QLineEdit *station_report_editor_edit;
   QLabel *station_web_browser_label;
   QLineEdit *station_web_browser_edit;
   QLabel *station_ssh_identity_file_label;
   QLineEdit *station_ssh_identity_file_edit;
   QLabel *station_timeoffset_label;
   QSpinBox *station_timeoffset_box;
   QLabel *station_startup_cart_label;
   QLineEdit *station_startup_cart_edit;
   QLabel *station_cue_sel_label;
   RDCardSelector *station_cue_sel;
   QLabel *station_start_cart_label;
   QLineEdit *station_start_cart_edit;
   QPushButton *station_startup_select_button;
   QPushButton *station_start_cart_button;
   QLabel *station_stop_cart_label;
   QLineEdit *station_stop_cart_edit;
   QPushButton *station_stop_cart_button;
   QCheckBox *station_heartbeat_box;
   QLabel *station_heartbeat_label;
   QCheckBox *station_filter_box;
   QLabel *station_filter_label;
   QLabel *station_hbcart_label;
   QLineEdit *station_hbcart_edit;
   QPushButton *station_hbcart_button;
   QLabel *station_hbinterval_label;
   QSpinBox *station_hbinterval_spin;
   QLabel *station_hbinterval_unit;
   QString station_cart_filter;
   QString station_cart_group;
   QCheckBox *station_maint_box;
   QLabel *station_maint_label;
   QCheckBox *station_dragdrop_box;
   QLabel *station_dragdrop_label;
   QLabel *station_panel_enforce_label;
   QCheckBox *station_panel_enforce_box;
   QGroupBox *station_systemservices_groupbox;
   QLabel *station_http_station_label;
   QComboBox *station_http_station_box;
   QLabel *station_cae_station_label;
   QComboBox *station_cae_station_box;
   QPushButton *station_rdlibrary_button;
   QPushButton *station_rdcatch_button;
   QPushButton *station_rdairplay_button;
   QPushButton *station_rdpanel_button;
   QPushButton *station_rdlogedit_button;
   QPushButton *station_rdcartslots_button;
   QPushButton *station_dropboxes_button;
   QPushButton *station_switchers_button;
   QPushButton *station_hostvars_button;
   QPushButton *station_audioports_button;
   QPushButton *station_ttys_button;
   QPushButton *station_adapters_button;
   QPushButton *station_jack_button;
   QPushButton *station_pypad_button;
   QPushButton *station_ok_button;
   QPushButton *station_cancel_button;
};


#endif  // EDIT_STATION_H
