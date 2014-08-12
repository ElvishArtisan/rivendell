// list_matrices.h
//
// List Rivendell Matrices
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_matrices.h,v 1.9.8.1 2012/12/10 15:40:15 cvs Exp $
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

#ifndef LIST_MATRICES_H
#define LIST_MATRICES_H

#include <qdialog.h>
#include <qlistview.h>
#include <qtextedit.h>
#include <qpixmap.h>
#include <qradiobutton.h>
#include <qsqldatabase.h>

#include <rd.h>
#include <rdmatrix.h>


class ListMatrices : public QDialog
{
 Q_OBJECT
 public:
  ListMatrices(QString station,QWidget *parent=0,const char *name=0);
  ~ListMatrices();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
  private slots:
   void addData();
   void editData();
   void deleteData();
   void doubleClickedData(QListViewItem *item,const QPoint &pt,int col);
   void closeData();

  private:
   void DeleteMatrix(int matrix);
   void RefreshList();
   void AddList(int matrix_num);
   void RefreshRecord(QListViewItem *item);
   QListView *list_view;
   QString list_station;
   bool list_matrix_modified[MAX_MATRICES];
};


#endif

