// list_users.h
//
// List Rivendell Users
//
//   (C) Copyright 2002-2020 Fred Gleason <fredg@paravelsystems.com>
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

#include <qpixmap.h>
#include <qpushbutton.h>

#include <rddb.h>
#include <rddialog.h>
#include <rdlistviewitem.h>

class ListUsers : public RDDialog
{
  Q_OBJECT
 public:
  ListUsers(const QString &admin_name,QWidget *parent=0);
  ~ListUsers();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void addData();
  void editData();
  void deleteData();
  void doubleClickedData(Q3ListViewItem *item,const QPoint &pt,int col);
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
  QPixmap *list_localuser_map;
  QPixmap *list_rss_map;
  QPixmap *list_user_map;
};


#endif  // LIST_USERS_H

