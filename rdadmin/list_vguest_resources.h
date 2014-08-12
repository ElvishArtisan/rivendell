// list_vguest_resources.h
//
// List vGuest Resources
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_vguest_resources.h,v 1.5 2010/07/29 19:32:35 cvs Exp $
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

#include <qdialog.h>
#include <qsqldatabase.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlistview.h>

#include <rduser.h>
#include <rdmatrix.h>


class ListVguestResources : public QDialog
{
 Q_OBJECT
 public:
 ListVguestResources(RDMatrix *matrix,RDMatrix::VguestType type,int size,
		      QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void editData();
  void doubleClickedData(QListViewItem *item,const QPoint &pt,int col);
  void okData();
  void cancelData();

 private:
  void RefreshList();
  RDMatrix *list_matrix;
  RDMatrix::VguestType list_type;
  QListView *list_list_view;
  int list_size;
  QString list_table;
};


#endif  // LIST_VGUEST_RESOURCES_H
