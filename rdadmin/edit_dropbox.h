// edit_dropbox.h
//
// Edit a Rivendell Dropbox Configuration
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_dropbox.h,v 1.5.8.1.2.1 2014/06/03 18:23:36 cvs Exp $
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

#ifndef EDIT_DROPBOX_H
#define EDIT_DROPBOX_H

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
#include <qlabel.h>
#include <qstringlist.h>

#include <rddropbox.h>
#include <rdschedcodes_dialog.h>


class EditDropbox : public QDialog
{
 Q_OBJECT
 public:
  EditDropbox(int id,QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void selectPathData();
  void selectCartData();
  void toCartChangedData(const QString &str);
  void selectLogPathData();
  void schedcodesData();
  void normalizationToggledData(bool state);
  void autotrimToggledData(bool state);
  void createDatesToggledData(bool state);
  void okData();
  void cancelData();
  
 private:
  RDDropbox *box_dropbox;
  QComboBox *box_group_name_box;
  QLineEdit *box_path_edit;
  QLineEdit *box_to_cart_edit;
  QPushButton *box_schedcodes_button;
  QCheckBox *box_delete_cuts_box;
  QLabel *box_delete_cuts_label;
  QLineEdit *box_metadata_pattern_edit;
  QLineEdit *box_user_defined_edit;
  QLineEdit *box_log_path_edit;
  QCheckBox *box_delete_source_box;
  QCheckBox *box_normalization_box;
  QLabel *box_normalization_level_label;
  QSpinBox *box_normalization_level_spin;
  QLabel *box_normalization_level_unit;
  QCheckBox *box_autotrim_box;
  QLabel *box_autotrim_level_label;
  QSpinBox *box_autotrim_level_spin;
  QLabel *box_autotrim_level_unit;
  QCheckBox *box_use_cartchunk_id_box;
  QCheckBox *box_title_from_cartchunk_id_box;
  QCheckBox *box_fix_broken_formats_box;
  QPushButton *box_select_cart_button;
  QSpinBox *box_startoffset_spin;
  QSpinBox *box_endoffset_spin;
  QCheckBox *box_create_dates_box;
  QSpinBox *box_create_startdate_offset_spin;
  QLabel *box_create_startdate_label;
  QLabel *box_create_startdate_unit;
  QSpinBox *box_create_enddate_offset_spin;
  QLabel *box_create_enddate_label;
  QLabel *box_create_enddate_unit;
  RDSchedCodesDialog *box_schedcodes_dialog;
  QStringList box_schedcodes;
};


#endif  // EDIT_DROPBOX_H
