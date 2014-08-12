
// edit_replicator.h
//
// Edit a Rivendell Replicator
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: edit_replicator.h,v 1.2 2010/07/29 19:32:34 cvs Exp $
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

#ifndef EDIT_REPLICATOR_H
#define EDIT_REPLICATOR_H

#include <qdialog.h>
#include <qtextedit.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qsqldatabase.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qpushbutton.h>

#include <rdreplicator.h>
#include <rdsettings.h>
#include <rdstation.h>
#include <rdlistselector.h>

class EditReplicator : public QDialog
{
 Q_OBJECT
 public:
  EditReplicator(const QString &repl_name,QWidget *parent=0,const char *name=0);
  ~EditReplicator();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void setFormatData();
  void normalizeCheckData(bool state);
  void okData();
  void cancelData();
  
 private:
  RDReplicator *repl_replicator;
  QLineEdit *repl_name_edit;
  QLineEdit *repl_description_edit;
  QComboBox *repl_type_box;
  QComboBox *repl_station_box;
  QLineEdit *repl_url_edit;
  QLabel *repl_username_label;
  QLineEdit *repl_username_edit;
  QLabel *repl_password_label;
  QLineEdit *repl_password_edit;
  QLineEdit *repl_format_edit;
  QCheckBox *repl_normalize_box;
  QLabel *repl_normalize_label;
  QSpinBox *repl_normalize_spin;
  QPushButton *repl_format_button;
  QPushButton *repl_metadata_button;
  RDSettings *repl_settings;

  QLabel *repl_url_label;
  QLabel *repl_format_label;
  QLabel *repl_normalize_check_label;
  QLabel *repl_normalize_unit_label;
  RDListSelector *repl_groups_sel;
};


#endif  // EDIT_REPLICATOR_H

