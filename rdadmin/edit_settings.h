// edit_settings.h
//
// Edit Rivendell System-wide Settings.
//
//   (C) Copyright 2009-2020 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef EDIT_SETTINGS_H
#define EDIT_SETTINGS_H

#include <q3listview.h>

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qspinbox.h>

#include <rdaudiosettings.h>
#include <rddialog.h>
#include <rdsystem.h>

class EditSettings : public RDDialog
{
  Q_OBJECT
  public:
   EditSettings(QWidget *parent=0);
   ~EditSettings();
   QSize sizeHint() const;
   QSizePolicy sizePolicy() const;

  private slots:
   void BuildDuplicatesList(std::map<unsigned,QString> *dups);
   void duplicatesCheckedData(bool state);
   void saveData();
   void okData();
   void cancelData();

 protected:
   void resizeEvent(QResizeEvent *e);

 private:
  QLabel *edit_sample_rate_label;
  QComboBox *edit_sample_rate_box;
  QLabel *edit_sample_rate_unit_label;
  QLabel *edit_duplicate_label;
  QLabel *edit_duplicate_hidden_label;
  QCheckBox *edit_duplicate_carts_box;
  QCheckBox *edit_fix_duplicate_carts_box;
  QLabel *edit_fix_duplicate_carts_label;
  QLabel *edit_maxpost_label;
  QSpinBox *edit_maxpost_spin;
  QLabel *edit_maxpost_unit_label;
  QLabel *edit_isci_path_label;
  QLineEdit *edit_isci_path_edit;
  QLabel *edit_temp_cart_group_label;
  QComboBox *edit_temp_cart_group_box;
  QComboBox *edit_rss_processor_station_box;
  QLabel *edit_show_user_list_label;
  QCheckBox *edit_show_user_list_box;
  Q3ListView *edit_duplicate_list;
  QLabel *edit_notification_address_label;
  QLineEdit *edit_notification_address_edit;
  QLabel *edit_rss_processor_label;
  QComboBox *edit_rss_processor_box;
  QPushButton *edit_settings_button;
  QPushButton *edit_save_button;
  QPushButton *edit_ok_button;
  QPushButton *edit_cancel_button;
  RDSystem *edit_system;
  int y_pos;
};


#endif  // EDIT_SETTINGS_H
