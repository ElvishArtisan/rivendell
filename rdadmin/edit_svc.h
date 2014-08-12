// edit_svc.h
//
// Edit a Rivendell Service
//
//   (C) Copyright 2002-2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_svc.h,v 1.24.8.2 2014/01/10 15:40:15 cvs Exp $
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

#ifndef EDIT_SVC_H
#define EDIT_SVC_H

#include <qdialog.h>
#include <qsqldatabase.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qcombobox.h>

#include <rdsvc.h>
#include <importfields.h>

class EditSvc : public QDialog
{
 Q_OBJECT
 public:
  EditSvc(QString svc,QWidget *parent=0,const char *name=0);
  ~EditSvc();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void autofillData();
  void enableHostsData();
  void trafficData();
  void musicData();
  void textChangedData(const QString &);
  void tfcTemplateActivatedData(int index);
  void musTemplateActivatedData(int index);
  void okData();
  void cancelData();

 private:
  void TestDataImport(RDSvc::ImportSource src);
  void Save();
  RDSvc *svc_svc;
  QLineEdit *svc_name_edit;
  QLineEdit *svc_description_edit;
  QLineEdit *svc_program_code_edit;
  QLineEdit *svc_name_template_edit;
  QLineEdit *svc_description_template_edit;
  QComboBox *svc_voice_group_box;
  QComboBox *svc_autospot_group_box;
  QLineEdit *svc_tfc_path_edit;
  QLineEdit *svc_tfc_preimport_cmd_edit;
  QLineEdit *svc_tfc_win_path_edit;
  QLineEdit *svc_tfc_win_preimport_cmd_edit;
  QComboBox *svc_tfc_import_template_box;
  QLineEdit *svc_tfc_label_cart_edit;
  QLineEdit *svc_tfc_track_edit;
  ImportFields *svc_tfc_fields;
  ImportFields *svc_mus_fields;
  QLineEdit *svc_mus_path_edit;
  QLineEdit *svc_mus_preimport_cmd_edit;
  QLineEdit *svc_mus_win_path_edit;
  QLineEdit *svc_mus_win_preimport_cmd_edit;
  QComboBox *svc_mus_import_template_box;
  QLineEdit *svc_mus_label_cart_edit;
  QLineEdit *svc_mus_break_edit;
  QLineEdit *svc_mus_track_edit;
  QCheckBox *svc_chain_box;
  QCheckBox *svc_autorefresh_box;
  QCheckBox *svc_loglife_box;
  QSpinBox *svc_loglife_spin;
  QCheckBox *svc_shelflife_box;
  QSpinBox *svc_shelflife_spin;
  bool import_changed;
};


#endif

