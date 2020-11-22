// edit_group.h
//
// Edit a Rivendell Group
//
//   (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef EDIT_GROUP_H
#define EDIT_GROUP_H

#include <q3listbox.h>
#include <q3textedit.h>
#include <qpixmap.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qpushbutton.h>

#include <rddialog.h>
#include <rdgroup.h>
#include <rdlistselector.h>

class EditGroup : public RDDialog
{
 Q_OBJECT
 public:
  EditGroup(QString group,QWidget *parent=0);
  ~EditGroup();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void lowCartChangedData(int value);
  void colorData();
  void cutLifeEnabledData(bool state);
  void purgeEnabledData(bool state);
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);
  
 private:
  void SetButtonColor(const QColor &color);
  bool CheckRange();
  RDGroup *group_group;
  QLabel *group_name_label;
  QLineEdit *group_name_edit;
  QLabel *group_description_label;
  QLineEdit *group_description_edit;
  QLabel *group_notify_email_addrs_label;
  QLineEdit *group_notify_email_addrs_edit;
  QLabel *group_cartrange_label;
  QSpinBox *group_lowcart_box;
  QLabel *group_highcart_label;
  QSpinBox *group_highcart_box;
  QLabel *group_enforcerange_label;
  QCheckBox *group_enforcerange_box;
  RDListSelector *group_svcs_sel;
  QLabel *group_music_label;
  QCheckBox *group_music_check;
  QLabel *group_traffic_label;
  QCheckBox *group_traffic_check;
  QLabel *group_nownext_label;
  QCheckBox *group_nownext_check;
  QLabel *group_carttype_label;
  QComboBox *group_carttype_box;
  QPushButton *group_color_button;
  QLabel *group_title_label;
  QLineEdit *group_title_edit;
  QSpinBox *group_shelflife_spin;
  QLabel *group_cutlife_label;
  QLabel *group_cutlife_unit;
  QCheckBox *group_cutlife_check;
  QSpinBox *group_cutlife_spin;
  QCheckBox *group_shelflife_check;
  QLabel *group_delete_carts_label;
  QCheckBox *group_delete_carts_check;
  QLabel *group_shelflife_label;
  QLabel *group_shelflife_unit;
  QPushButton *group_ok_button;
  QPushButton *group_cancel_button;
};


#endif  // EDIT_GROUP_H

