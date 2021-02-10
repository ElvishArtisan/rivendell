// list_svcs.h
//
// List Rivendell Services and Report Ages
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

#ifndef LIST_SVCS_H
#define LIST_SVCS_H

#include <QPushButton>

#include <rddialog.h>
#include <rdservicelistmodel.h>
#include <rdtableview.h>

class ListSvcs : public RDDialog
{
 Q_OBJECT
 public:
  ListSvcs(QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void generateData();
  void purgeData();
  void listDoubleClickedData(const QModelIndex &);
  void closeData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  RDTableView *list_log_view;
  RDServiceListModel *list_log_model;
  QPushButton *list_generate_button;
  QPushButton *list_purge_button;
  QPushButton *list_close_button;
};


#endif  // LIST_SVCS_H
