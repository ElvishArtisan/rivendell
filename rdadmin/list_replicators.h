// list_replicators.h
//
// List Rivendell Replication Configurations
//
//   (C) Copyright 2010-2018 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef LIST_REPLICATORS_H
#define LIST_REPLICATORS_H

#include <qdialog.h>
#include <qpixmap.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qpixmap.h>

#include <rdlistviewitem.h>
#include <rddb.h>

class ListReplicators : public QDialog
{
  Q_OBJECT
 public:
  ListReplicators(QWidget *parent=0);
  ~ListReplicators();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void addData();
  void editData();
  void deleteData();
  void listData();
  void doubleClickedData(Q3ListViewItem *item,const QPoint &pt,int col);
  void closeData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  void RefreshList();
  void RefreshItem(RDListViewItem *item);
  RDListView *list_replicators_view;
  QPushButton *list_add_button;
  QPushButton *list_edit_button;
  QPushButton *list_delete_button;
  QPushButton *list_list_button;
  QPushButton *list_close_button;
};


#endif


