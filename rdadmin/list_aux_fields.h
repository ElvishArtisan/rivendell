// list_aux_fields.h
//
// List Auxiliary Fields for an RSS Feed
//
//   (C) Copyright 2002-2007,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef LIST_AUX_FIELDS_H
#define LIST_AUX_FIELDS_H

#include <qdialog.h>
#include <qsqldatabase.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <q3listview.h>

#include <rdlistviewitem.h>

class ListAuxFields : public QDialog
{
 Q_OBJECT
 public:
  ListAuxFields(unsigned feed_id,QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void addData();
  void editData();
  void deleteData();
  void doubleClickedData(Q3ListViewItem *item,const QPoint &pt,int col);
  void closeData();

 private:
  void RefreshList();
  void RefreshItem(RDListViewItem *item);
  Q3ListView *list_list_view;
  unsigned list_feed_id;
};


#endif  // LIST_AUX_FIELDS_H
