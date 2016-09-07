// list_gpis.h
//
// List Rivendell GPIs
//
//   (C) Copyright 2002-2004,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <QDialog>
#include <QLabel>
#include <QPushButton>

#include <rdmatrix.h>
#include <rdtablewidget.h>

class ListGpis : public QDialog
{
 Q_OBJECT
 public:
  ListGpis(RDMatrix *matrix,RDMatrix::GpioType type,QWidget *parent=0);
  QSize sizeHint() const;
  QSizePolicy sizePolicy() const;

 private slots:
  void editData();
  void doubleClickedData(const QModelIndex &index);
  void closeData();

 protected:
  void resizeEvent(QResizeEvent *e);

 private:
  void RefreshList();
  void UpdateRow(int row);
  RDMatrix *list_matrix;
  RDMatrix::GpioType list_type;
  QString list_tablename;
  QLabel *list_label;
  RDTableWidget *list_widget;
  int list_size;
  QPushButton *list_edit_button;
  QPushButton *list_close_button;
};


#endif

