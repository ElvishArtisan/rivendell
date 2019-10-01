// list_schedcodes.h
//
// The scheduler codes dialog for rdadmin
//
//   Stefan Gabriel <stg@st-gabriel.de>
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

#ifndef LIST_SCHEDCODES_H
#define LIST_SCHEDCODES_H

#include <qpixmap.h>
#include <qpushbutton.h>

#include <rddb.h>
#include <rddialog.h>
#include <rdlistviewitem.h>

class ListSchedCodes : public RDDialog
{
  Q_OBJECT
 public:
  ListSchedCodes(QWidget *parent=0);
  ~ListSchedCodes();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
 private slots:
  void addData();
  void editData();
  void deleteData();
  void doubleClickedData(Q3ListViewItem *item,const QPoint &pt,int col);
  void closeData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  void RefreshList();
  void RefreshItem(Q3ListViewItem *item);
  void WriteItem(Q3ListViewItem *item,RDSqlQuery *q);
  Q3ListView *list_schedCodes_view;
  QPushButton *list_add_button;
  QPushButton *list_edit_button;
  QPushButton *list_delete_button;
  QPushButton *list_close_button;
};



#endif  // LIST_SCHEDCODES_H
