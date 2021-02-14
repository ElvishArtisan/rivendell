// edit_schedrules.h
//
// Edit scheduler rules of a clock
//
//   (C) Copyright Stefan Gabriel <stg@st-gabriel.de>
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef EDIT_SCHEDRULES_H
#define EDIT_SCHEDRULES_H

#include <rddialog.h>
#include <rdlistview.h>
#include <rdschedruleslist.h>
#include <rdtableview.h>

#include "edit_schedcoderules.h"
#include "schedrulesmodel.h"

class EditSchedRules : public RDDialog
{
  Q_OBJECT
 public:
  EditSchedRules(QString clock,unsigned *artistsep,
		 RDSchedRulesList *schedruleslist,bool *rules_modified,
		 QWidget *parent=0);
  ~EditSchedRules();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void editData();
  void importData();
  void doubleClickedData(const QModelIndex &index);
  void closeData();

 protected:
  void closeEvent(QCloseEvent *e);
  void resizeEvent(QResizeEvent *e);

 private:
  QLabel *list_box_label;
  RDTableView *edit_schedcodes_view;
  SchedRulesModel *edit_schedcodes_model;
  QString edit_clockname;
  unsigned* edit_artistsep;
  bool* edit_rules_modified;
  RDSchedRulesList* edit_sched_rules_list;
  bool edit_modified;
  QPushButton *list_edit_button;
  QPushButton *list_import_button;
  QPushButton *list_close_button;
  EditSchedCodeRules *list_editrules_dialog;
};


#endif  // EDIT_SCHEDRULES_H
