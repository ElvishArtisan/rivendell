// list_users.h
//
// List Rivendell Users
//
//   (C) Copyright 2002-2008 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_users.h,v 1.11 2010/07/29 19:32:35 cvs Exp $
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

#ifndef LIST_USERS_H
#define LIST_USERS_H

#include <qdialog.h>
#include <qpixmap.h>
#include <qradiobutton.h>
#include <qsqldatabase.h>
#include <qpushbutton.h>
#include <qpixmap.h>

#include <rdlistviewitem.h>
#include <rddb.h>


class ListUsers : public QDialog
{
  Q_OBJECT
 public:
  ListUsers(const QString &admin_name,QWidget *parent=0,const char *name=0);
  ~ListUsers();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void addData();
  void editData();
  void deleteData();
  void doubleClickedData(QListViewItem *item,const QPoint &pt,int col);
  void closeData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  void RefreshList();
  void RefreshItem(RDListViewItem *item);
  RDListView *list_users_view;
  QPushButton *list_add_button;
  QPushButton *list_edit_button;
  QPushButton *list_delete_button;
  QPushButton *list_close_button;
  QString list_admin_name;
  QPixmap *list_admin_map;
  QPixmap *list_user_map;
};


#endif


