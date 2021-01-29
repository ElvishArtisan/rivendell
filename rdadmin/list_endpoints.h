// list_endpoints.h
//
// List Rivendell Endpoints
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

#ifndef LIST_ENDPOINTS_H
#define LIST_ENDPOINTS_H

#include <QLabel>
#include <QPushButton>

#include <rddialog.h>
#include <rdendpointlistmodel.h>
#include <rdmatrix.h>
#include <rdtableview.h>
#include <rduser.h>

class ListEndpoints : public RDDialog
{
 Q_OBJECT
 public:
  ListEndpoints(RDMatrix *matrix,RDMatrix::Endpoint endpoint,QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void editData();
  void doubleClickedData(const QModelIndex &index);
  void closeData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  RDMatrix *list_matrix;
  RDMatrix::Endpoint list_endpoint;
  RDTableView *list_list_view;
  RDEndpointListModel *list_list_model;
  QLabel *list_type_label;
  QPushButton *list_edit_button;
  QPushButton *list_close_button;
  int list_size;
  QString list_table;
  bool list_readonly;
};


#endif  // LIST_ENDPOINTS_H
