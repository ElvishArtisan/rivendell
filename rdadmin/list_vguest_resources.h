// list_vguest_resources.h
//
// List vGuest Resources
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

#ifndef LIST_VGUEST_RESOURCES_H
#define LIST_VGUEST_RESOURCES_H

//#include <q3listview.h>
#include <QLabel>
#include <QPushButton>

#include <rddialog.h>
#include <rdmatrix.h>
#include <rdtableview.h>
#include <rduser.h>
#include <rdvguestresourcelistmodel.h>

#include "edit_vguest_resource.h"

class ListVguestResources : public RDDialog
{
 Q_OBJECT
 public:
  ListVguestResources(RDMatrix *matrix,RDMatrix::VguestType type,int size,
		      QWidget *parent=0);
  ~ListVguestResources();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void editData();
  void doubleClickedData(const QModelIndex &);
  void closeData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  EditVguestResource *list_edit_resource_dialog;
  RDMatrix *list_matrix;
  RDMatrix::VguestType list_type;
  QLabel *list_title_label;
  RDTableView *list_list_view;
  RDVguestResourceListModel *list_list_model;
  QPushButton *list_edit_button;
  QPushButton *list_close_button;
  int list_size;
  QString list_table;
};


#endif  // LIST_VGUEST_RESOURCES_H
