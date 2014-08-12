// edit_group.h
//
// Edit a Rivendell Group
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_group.h,v 1.16.8.1 2013/01/07 13:50:22 cvs Exp $
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

#include <qdialog.h>
#include <qlistbox.h>
#include <qtextedit.h>
#include <qpixmap.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qsqldatabase.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qpushbutton.h>

#include <rdlistselector.h>

#include <rdgroup.h>


class EditGroup : public QDialog
{
 Q_OBJECT
 public:
  EditGroup(QString group,QWidget *parent=0,const char *name=0);
  ~EditGroup();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void lowCartChangedData(int value);
  void colorData();
  void purgeEnabledData(bool state);
  void okData();
  void cancelData();
  
 private:
  void SetButtonColor(const QColor &color);
  bool CheckRange();
  RDGroup *group_group;
  QLineEdit *group_name_edit;
  QLineEdit *group_description_edit;
  QSpinBox *group_lowcart_box;
  QLabel *group_highcart_label;
  QSpinBox *group_highcart_box;
  QLabel *group_enforcerange_label;
  QCheckBox *group_enforcerange_box;
  RDListSelector *group_svcs_sel;
  QCheckBox *group_music_check;
  QCheckBox *group_traffic_check;
  QCheckBox *group_nownext_check;
  QComboBox *group_carttype_box;
  QPushButton *group_color_button;
  QLineEdit *group_title_edit;
  QSpinBox *group_shelflife_spin;
  QCheckBox *group_shelflife_check;
  QLabel *group_delete_carts_label;
  QCheckBox *group_delete_carts_check;
  QLabel *group_shelflife_label;
  QLabel *group_shelflife_unit;
};


#endif  // EDIT_GROUP_H

