// edit_schedcoderules.h
//
// Change rules for scheduler codes dialog
//
//   (C) Copyright Stefan Gabriel <stg@st-gabriel.de>
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

#ifndef EDIT_SCHEDCODERULES_H
#define EDIT_SCHEDCODERULES_H

#include <qcombobox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qspinbox.h>

#include <rddialog.h>
#include <rdschedruleslist.h>

#include "clock_listview.h"

class EditSchedCodeRules : public RDDialog
{
  Q_OBJECT
 public:
  EditSchedCodeRules(Q3ListViewItem *item, RDSchedRulesList *sched_rules_list,
		     QWidget *parent=0);
  ~EditSchedCodeRules();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void okData();
  void cancelData();

 private:
  QPushButton* edit_ok_button;
  QPushButton* edit_cancel_button;
  QLabel* edit_code_name_label;
  QLabel* edit_max_row_label;
  QLabel* edit_min_wait_label;
  QLabel* edit_not_after_label;
  QLabel* edit_or_after_label;
  QLabel* edit_or_after_label_II;
  QSpinBox* edit_max_row_spin;
  QSpinBox* edit_min_wait_spin;
  QLabel* edit_code_label;
  QComboBox* edit_not_after_box;
  QComboBox* edit_or_after_box;
  QComboBox* edit_or_after_box_II;
  QLabel* edit_description_label;
  Q3ListViewItem *edit_edit_item; 
 
 protected:
  void closeEvent(QCloseEvent *e);
};

#endif  // EDIT_SCHEDCODERULES_H
