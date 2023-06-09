// edit_system.h
//
// Edit Rivendell System-wide Settings.
//
//   (C) Copyright 2009-2023 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef EDIT_SYSTEM_H
#define EDIT_SYSTEM_H

#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>

#include <rdaudiosettings.h>
#include <rdcombobox.h>
#include <rddialog.h>
#include <rdgrouplistmodel.h>
#include <rdlibrarymodel.h>
#include <rdstationlistmodel.h>
#include <rdsystem.h>
#include <rdtableview.h>

#include "list_encoders.h"
#include "test_datetimes.h"

class EditSystem : public RDDialog
{
  Q_OBJECT
 public:
  EditSystem(QWidget *parent=0);
  ~EditSystem();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void BuildDuplicatesList(std::map<unsigned,QString> *dups);
  void duplicatesCheckedData(bool state);
  void saveData();
  void encodersData();
  void datetimeTestData();
  void datetimeDefaultsData();
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  QLabel *edit_realm_name_label;
  QLineEdit *edit_realm_name_edit;
  QLabel *edit_sample_rate_label;
  QComboBox *edit_sample_rate_box;
  QLabel *edit_sample_rate_unit_label;
  QLabel *edit_duplicate_label;
  QLabel *edit_duplicate_hidden_label;
  QCheckBox *edit_duplicate_carts_box;
  QCheckBox *edit_fix_duplicate_carts_box;
  QLabel *edit_fix_duplicate_carts_label;
  QLabel *edit_isci_path_label;
  QLineEdit *edit_isci_path_edit;
  QLabel *edit_origin_email_addr_label;
  QLineEdit *edit_origin_email_addr_edit;
  QLabel *edit_temp_cart_group_label;
  RDComboBox *edit_temp_cart_group_box;
  RDGroupListModel *edit_temp_cart_group_model;
  QComboBox *edit_rss_processor_station_box;
  QLabel *edit_show_user_list_label;
  QCheckBox *edit_show_user_list_box;
  RDTableView *edit_duplicate_view;
  RDLibraryModel *edit_duplicate_model;
  QLabel *edit_notification_address_label;
  QLineEdit *edit_notification_address_edit;
  QLabel *edit_rss_processor_label;
  QComboBox *edit_rss_processor_box;
  QPushButton *edit_save_button;
  QPushButton *edit_encoders_button;
  QPushButton *edit_ok_button;
  QPushButton *edit_cancel_button;
  RDSystem *edit_system;
  ListEncoders *edit_encoders_dialog;
  int y_pos;
  RDStationListModel *edit_station_list_model;
  QGroupBox *edit_datetime_group;
  QLabel *edit_long_date_label;
  QLineEdit *edit_long_date_edit;
  QLabel *edit_short_date_label;
  QLineEdit *edit_short_date_edit;
  QLabel *edit_time_label;
  QComboBox *edit_time_box;
  QPushButton *edit_datetime_test_button;
  QPushButton *edit_datetime_defaults_button;
  TestDatetimes *edit_test_datetimes_dialog;
};


#endif  // EDIT_SYSTEM_H
