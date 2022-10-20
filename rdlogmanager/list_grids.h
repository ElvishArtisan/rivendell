// list_grids.h
//
// List Rivendell Log Grids
//
//   (C) Copyright 2002-2022 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef LIST_GRIDS_H
#define LIST_GRIDS_H

#include <QPushButton>

#include <rddialog.h>
#include <rdservicelistmodel.h>
#include <rdtableview.h>

class ListGrids : public RDDialog
{
 Q_OBJECT
 public:
  ListGrids(QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void editData();
  void selectionChangedData(const QItemSelection &selected,
			    const QItemSelection &previous);
  void doubleClickedData(const QModelIndex &index);
  void closeData();

 private:
  RDTableView *edit_grids_view;
  RDServiceListModel *edit_grids_model;
  QPushButton *edit_edit_button;
  QPushButton *edit_close_button;
};


#endif  // LIST_GRIDS_H
