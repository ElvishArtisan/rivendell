// list_feeds.h
//
// List Rivendell RSS Feeds
//
//   (C) Copyright 2002-2023 Fred Gleason <fredg@paravelsystems.com>
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

#include <QPushButton>

#include <rddialog.h>
#include <rdfeedlistmodel.h>
#include <rdtableview.h>

#include <feedlistview.h>

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
  void doubleClickedData(const QModelIndex &index);
  void feedSelectionChangedData(const QItemSelection &selected,
				const QItemSelection &unselected);
  void repostData();
  void unpostData();
  void closeData();
  void resetModelData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  QLabel *list_box_label;
  FeedListView *list_feeds_view;
  RDFeedListModel *list_feeds_model;
  QPushButton *list_add_button;
  QPushButton *list_edit_button;
  QPushButton *list_delete_button;
  QPushButton *list_repost_button;
  QPushButton *list_unpost_button;
  QPushButton *list_close_button;
};


#endif  // LIST_FEEDS_H


