// list_events.h
//
// List Rivendell Log Events
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

#ifndef LIST_EVENTS_H
#define LIST_EVENTS_H

#include <QComboBox>

#include <rddb.h>
#include <rddialog.h>
#include <rdtableview.h>

#include "eventlistmodel.h"

class ListEvents : public RDDialog
{
 Q_OBJECT
 public:
  ListEvents(QString *eventname,QWidget *parent=0);
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
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);
  void closeEvent(QCloseEvent *e);

 private:
  int ActiveEvents(QString event_name,QString *clock_list);
  void DeleteEvent(QString event_name);
  QString GetEventFilter(QString svc_name);
  QString GetNoneFilter();
  RDTableView *edit_events_view;
  EventListModel *edit_events_model;
  QString *edit_eventname;
  QLabel *edit_filter_label;
  QComboBox *edit_filter_box;
  QPushButton *edit_add_button;
  QPushButton *edit_edit_button;
  QPushButton *edit_delete_button;
  QPushButton *edit_rename_button;
  QPushButton *edit_close_button;
  QPushButton *edit_ok_button;
  QPushButton *edit_cancel_button;
};


#endif  // LIST_EVENTS_H
