// list_nodes.h
//
// List Rivendell LiveWire Nodes
//
//   (C) Copyright 2002-2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_nodes.h,v 1.3 2010/07/29 19:32:35 cvs Exp $
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

#ifndef LIST_NODES_H
#define LIST_NODES_H

#include <qdialog.h>
#include <qsqldatabase.h>
#include <qlabel.h>
#include <qpushbutton.h>

#include <rduser.h>
#include <rdmatrix.h>
#include <rdlistview.h>
#include <rdlistviewitem.h>


class ListNodes : public QDialog
{
 Q_OBJECT
 public:
  ListNodes(RDMatrix *matrix,QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void addData();
  void editData();
  void deleteData();
  void doubleClickedData(QListViewItem *item,const QPoint &pt,int col);
  void closeData();

 private:
  void RefreshList();
  void RefreshItem(RDListViewItem *item);
  void PurgeEndpoints(const QString &tablename);
  RDMatrix *list_matrix;
  RDListView *list_list_view;
};


#endif

