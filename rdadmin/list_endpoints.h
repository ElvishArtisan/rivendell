// list_endpoints.h
//
// List Rivendell Endpoints
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_endpoints.h,v 1.8 2010/07/29 19:32:34 cvs Exp $
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

#ifndef LIST_ENDPOINTS_H
#define LIST_ENDPOINTS_H

#include <qdialog.h>
#include <qsqldatabase.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlistview.h>

#include <rduser.h>
#include <rdmatrix.h>


class ListEndpoints : public QDialog
{
 Q_OBJECT
 public:
  ListEndpoints(RDMatrix *matrix,RDMatrix::Endpoint endpoint,
		QWidget *parent=0,const char *name=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void editData();
  void doubleClickedData(QListViewItem *item,const QPoint &pt,int col);
  void okData();
  void cancelData();

 private:
  RDMatrix *list_matrix;
  RDMatrix::Endpoint list_endpoint;
  QListView *list_list_view;
  int list_size;
  QString list_table;
  bool list_readonly;
};


#endif

