// list_sas_resources.h
//
// List SAS Resources
//
//   (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef LIST_SAS_RESOURCES_H
#define LIST_SAS_RESOURCES_H

#include <qlabel.h>
#include <qpushbutton.h>
#include <q3listview.h>

#include <rddialog.h>
#include <rdmatrix.h>
#include <rduser.h>

class ListSasResources : public RDDialog
{
 Q_OBJECT
 public:
 ListSasResources(RDMatrix *matrix,int size,QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void editData();
  void doubleClickedData(Q3ListViewItem *item,const QPoint &pt,int col);
  void okData();
  void cancelData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  void RefreshList();
  RDMatrix *list_matrix;
  QLabel *list_title_label;
  Q3ListView *list_list_view;
  QPushButton *list_edit_button;
  QPushButton *list_ok_button;
  QPushButton *list_cancel_button;
  int list_size;
  QString list_table;
};


#endif  // LIST_SAS_RESOURCES_H
