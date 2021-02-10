// list_clocks.h
//
// List Rivendell Log Clocks
//
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

#ifndef LIST_CLOCKS_H
#define LIST_CLOCKS_H

#include <QComboBox>

#include <rddialog.h>
#include <rddb.h>
#include <rdtableview.h>

#include "clocklistmodel.h"

class ListClocks : public RDDialog
{
 Q_OBJECT
 public:
  ListClocks(QString *clockname=NULL,QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void addData();
  void editData();
  void deleteData();
  void renameData();
  void doubleClickedData(const QModelIndex &index);
  void filterActivatedData(int id);
  void closeData();
  void clearData();
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);
  void closeEvent(QCloseEvent *e);

 private:
  int ActiveClocks(QString clockname,QString *svc_list);
  void DeleteClock(QString clockname);
  QString GetClockFilter(QString svc_name);
  QString GetNoneFilter();
  RDTableView *edit_clocks_view;
  ClockListModel *edit_clocks_model;
  QString *edit_clockname;
  QLabel *edit_filter_label;
  QComboBox *edit_filter_box;
  QPushButton *edit_add_button;
  QPushButton *edit_edit_button;
  QPushButton *edit_delete_button;
  QPushButton *edit_rename_button;
  QPushButton *edit_close_button;
  QPushButton *edit_clear_button;
  QPushButton *edit_ok_button;
  QPushButton *edit_cancel_button;
};


#endif  // LIST_CLOCKS_H
