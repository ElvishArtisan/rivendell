// list_replicator_carts.h
//
// List Rivendell Replicator Carts
//
//   (C) Copyright 2012-2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef LIST_REPLICATOR_CARTS_H
#define LIST_REPLICATOR_CARTS_H

#include <QPushButton>

#include <rddialog.h>
#include <rdreplcartlistmodel.h>
#include <rdtableview.h>

class ListReplicatorCarts : public RDDialog
{
  Q_OBJECT
 public:
  ListReplicatorCarts(QWidget *parent=0);
  ~ListReplicatorCarts();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 public slots:
  int exec(const QString &replname);

 private slots:
  void repostData();
  void repostAllData();
  void closeData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  RDTableView *list_view;
  RDReplCartListModel *list_model;
  QPushButton *list_repost_button;
  QPushButton *list_repost_all_button;
  QPushButton *list_close_button;
};


#endif  // LIST_REPLICATOR_CARTS_H
