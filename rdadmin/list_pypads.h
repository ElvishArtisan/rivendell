// list_pypads.h
//
// List PyPAD Instances
//
//   (C) Copyright 2018-2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef LIST_PYPADS_H
#define LIST_PYPADS_H

#include <QPushButton>

#include <rddialog.h>
#include <rdpypadlistmodel.h>
#include <rdstation.h>
#include <rdtableview.h>

class ListPypads : public RDDialog
{
 Q_OBJECT
 public:
  ListPypads(RDStation *station,QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void addData();
  void editData();
  void deleteData();
  void errorData();
  void doubleClickedData(const QModelIndex &index);
  void closeData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  RDTableView *list_list_view;
  RDPypadListModel *list_list_model;
  QPushButton *list_add_button;
  QPushButton *list_edit_button;
  QPushButton *list_delete_button;
  QPushButton *list_error_button;
  QPushButton *list_close_button;
  RDStation *list_station;
};


#endif  // LIST_PYPADS_H
