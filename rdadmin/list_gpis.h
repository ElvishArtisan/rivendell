// list_gpis.h
//
// List Rivendell GPIs
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

#ifndef LIST_GPIS_H
#define LIST_GPIS_H

#include <QPushButton>

#include <rddialog.h>
#include <rdgpiolistmodel.h>
#include <rdmatrix.h>
#include <rdtableview.h>

#include "edit_gpi.h"

class ListGpis : public RDDialog
{
 Q_OBJECT
 public:
  ListGpis(RDMatrix *matrix,RDMatrix::GpioType type,QWidget *parent=0);
  ~ListGpis();
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
  RDMatrix::GpioType list_type;
  EditGpi *list_edit_gpi_dialog;
  RDTableView *list_list_view;
  RDGpioListModel *list_list_model;
  QLabel *list_list_label;
  QPushButton *list_edit_button;
  QPushButton *list_close_button;
};


#endif  // LIST_GPIS_H
