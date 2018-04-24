// list_endpoints.h
//
// List Rivendell Endpoints
//
//   (C) Copyright 2002-2003,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <QDialog>
#include <QLabel>
#include <QPushButton>

#include <rduser.h>
#include <rdmatrix.h>
#include <rdsqltablemodel.h>
#include <rdtableview.h>

class ListEndpoints : public QDialog
{
 Q_OBJECT
 public:
  ListEndpoints(RDMatrix *matrix,RDMatrix::Endpoint endpoint,QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void editData();
  void doubleClickedData(const QModelIndex &index);
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  QString ModelSql(RDMatrix::Type type) const;
  bool SetHeaders(RDSqlTableModel *model,RDMatrix::Type type,
		  RDMatrix::Endpoint end);
  RDMatrix *list_matrix;
  RDMatrix::Endpoint list_endpoint;
  RDSqlTableModel *list_model;
  QLabel *list_label;
  RDTableView *list_view;
  int list_size;
  QString list_table;
  bool list_readonly;
  QPushButton *list_edit_button;
  QPushButton *list_ok_button;
  QPushButton *list_cancel_button;
};


#endif  // LIST_ENDPOINTS_H
