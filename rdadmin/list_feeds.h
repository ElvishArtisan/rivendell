// list_feeds.h
//
// List Rivendell Feeds
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

#ifndef LIST_FEEDS_H
#define LIST_FEEDS_H

#include <qpushbutton.h>

#include <rddialog.h>
#include <rdlistviewitem.h>

class ListFeeds : public RDDialog
{
  Q_OBJECT
 public:
  ListFeeds(QWidget *parent=0);
  ~ListFeeds();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void addData();
  void editData();
  void deleteData();
  void doubleClickedData(Q3ListViewItem *item,const QPoint &pt,int col);
  void repostData();
  void unpostData();
  void closeData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  void RefreshList();
  void RefreshItem(RDListViewItem *item);
  QPixmap *list_rdcastmanager_32x32_map;
  QLabel *list_box_label;
  RDListView *list_feeds_view;
  QPushButton *list_add_button;
  QPushButton *list_edit_button;
  QPushButton *list_delete_button;
  QPushButton *list_repost_button;
  QPushButton *list_unpost_button;
  QPushButton *list_close_button;
};


#endif  // LIST_FEEDS_H


