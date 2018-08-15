// edit_settings.h
//
// Edit Rivendell System-wide Settings.
//
//   (C) Copyright 2009-2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <map>

#include <qdialog.h>
#include <qcheckbox.h>
#include <q3listview.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qcombobox.h>

#include <rdlabel.h>
#include <rdsystem.h>
#include <rdaudiosettings.h>

class EditSettings : public QDialog
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

 private:
   QComboBox *edit_sample_rate_box;
   QCheckBox *edit_duplicate_carts_box;
   RDLabel *edit_duplicate_label;
   QCheckBox *edit_fix_duplicate_carts_box;
   QLabel *edit_fix_duplicate_carts_label;
   QSpinBox *edit_maxpost_spin;
   QLineEdit *edit_isci_path_edit;
   QComboBox *edit_temp_cart_group_box;
   QCheckBox *edit_show_user_list_box;
   Q3ListView *edit_duplicate_list;
   QLineEdit *edit_notification_address_edit;
   QPushButton *edit_settings_button;
   QPushButton *edit_save_button;
   QPushButton *edit_ok_button;
   QPushButton *edit_cancel_button;
   RDSystem *edit_system;
   int y_pos;
};


#endif  // EDIT_SETTINGS_H
