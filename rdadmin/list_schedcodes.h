// list_schedcodes.h
//
// The scheduler codes dialog for rdadmin
//
//   Based on original code by Stefan Gabriel <stg@st-gabriel.de>
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

#ifndef LIST_SCHEDCODES_H
#define LIST_SCHEDCODES_H

#include <QLabel>
#include <QPushButton>

#include <rddb.h>
#include <rddialog.h>
#include <rdschedcodelistmodel.h>
#include <rdtableview.h>

#include "add_schedcodes.h"
#include "edit_schedcodes.h"

class ListSchedCodes : public RDDialog
{
  Q_OBJECT
 public:
  ListSchedCodes(QWidget *parent=0);
  ~ListSchedCodes();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void addData();
  void editData();
  void deleteData();
  void doubleClickedData(const QModelIndex &index);
  void closeData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  RDTableView *list_schedcodes_view;
  QLabel *list_schedcodes_label;
  RDSchedCodeListModel *list_schedcodes_model;
  QPushButton *list_add_button;
  QPushButton *list_edit_button;
  QPushButton *list_delete_button;
  QPushButton *list_close_button;
  AddSchedCode *list_add_schedcode_dialog;
  EditSchedCode *list_edit_schedcode_dialog;
};



#endif  // LIST_SCHEDCODES_H
