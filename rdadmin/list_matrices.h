// list_matrices.h
//
// List Rivendell Matrices
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

#ifndef LIST_MATRICES_H
#define LIST_MATRICES_H

#include <QLabel>
#include <QPushButton>

#include <rd.h>
#include <rddialog.h>
#include <rdmatrix.h>
#include <rdmatrixlistmodel.h>
#include <rdtableview.h>

class ListMatrices : public RDDialog
{
 Q_OBJECT
 public:
  ListMatrices(QString station,QWidget *parent=0);
  ~ListMatrices();
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;
  
  private slots:
   void addData();
   void editData();
   void deleteData();
   void doubleClickedData(const QModelIndex &);
   void closeData();

 protected:
  void resizeEvent(QResizeEvent *e);

  private:
   void DeleteMatrix(RDMatrix *mtx);
   void AddList(int matrix_num);
   QLabel *list_title_label;
   RDTableView *list_view;
   RDMatrixListModel *list_model;
   QPushButton *list_add_button;
   QPushButton *list_edit_button;
   QPushButton *list_delete_button;
   QPushButton *list_close_button;
   QString list_station;
   bool list_matrix_modified[MAX_MATRICES];
};


#endif  // LIST_MATRICES_H
