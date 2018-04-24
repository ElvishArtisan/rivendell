// list_vguest_resources.h
//
// List vGuest Resources
//
//   (C) Copyright 2002-2005,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef LIST_VGUEST_RESOURCES_H
#define LIST_VGUEST_RESOURCES_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>

#include <rdmatrix.h>
#include <rdsqltablemodel.h>
#include <rdtableview.h>

class ListVguestResources : public QDialog
{
 Q_OBJECT
 public:
 ListVguestResources(RDMatrix *matrix,RDMatrix::VguestType type,int size,
		     QWidget *parent=0);
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
  QString ModelSql(RDMatrix::VguestType type) const;
  void SetHeaders(RDSqlTableModel *model,RDMatrix::VguestType type);
  RDMatrix *list_matrix;
  RDMatrix::VguestType list_type;
  RDSqlTableModel *list_model;
  QLabel *list_label;
  RDTableView *list_view;
  int list_size;
  QString list_table;
  QPushButton *list_edit_button;
  QPushButton *list_close_button;
};


#endif  // LIST_VGUEST_RESOURCES_H
