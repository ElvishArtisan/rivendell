// edit_station.h
//
// Edit a Rivendell Workstation
//
//   (C) Copyright 2002-2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_station.h,v 1.36.4.4 2014/02/11 23:46:27 cvs Exp $
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

#include <qdialog.h>
#include <qlistbox.h>
#include <qtextedit.h>
#include <qpixmap.h>
#include <qcheckbox.h>
#include <qsqldatabase.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <qlabel.h>

#include <rdstation.h>
#include <rdcatch_connect.h>
#include <rdripc.h>
#include <rdcardselector.h>

class EditStation : public QDialog
{
  Q_OBJECT
  public:
   EditStation(QString station,QWidget *parent=0,const char *name=0);
   ~EditStation();
   QSize sizeHint() const;
   QSizePolicy sizePolicy() const;

 protected:
  void paintEvent(QPaintEvent *e);

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
   void editBackupsData();
   void editDropboxesData();
   void jackSettingsData();
   void startCartClickedData();
   void stopCartClickedData();

  private:
   QString DisplayPart(QString);
   QString HostPart(QString);
   RDStation *station_station;
   RDStation *station_cae_station;
   RDCatchConnect *station_catch_connect;
   QLineEdit *station_name_edit;
   QLineEdit *station_description_edit;
   QComboBox *station_default_name_edit;
   QComboBox *station_broadcast_sec_edit;
   QLineEdit *station_address_edit;
   QLineEdit *station_editor_cmd_edit;
   QSpinBox *station_timeoffset_box;
   QLineEdit *station_startup_cart_edit;
   RDCardSelector *station_cue_sel;
   QLineEdit *station_start_cart_edit;
   QPushButton *station_start_cart_button;
   QLineEdit *station_stop_cart_edit;
   QPushButton *station_stop_cart_button;
   QCheckBox *station_heartbeat_box;
   QCheckBox *station_filter_box;
   QLabel *station_hbcart_label;
   QLineEdit *station_hbcart_edit;
   QPushButton *station_hbcart_button;
   QLabel *station_hbinterval_label;
   QSpinBox *station_hbinterval_spin;
   QLabel *station_hbinterval_unit;
   QString station_cart_filter;
   QString station_cart_group;
   QCheckBox *station_maint_box;
   QCheckBox *station_dragdrop_box;
   QLabel *station_panel_enforce_label;
   QCheckBox *station_panel_enforce_box;
   QComboBox *station_http_station_box;
   QComboBox *station_cae_station_box;
};


#endif

