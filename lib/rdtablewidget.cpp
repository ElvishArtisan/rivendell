// rdtablewidget.cpp
//
// Table Widget for Rivendell
//
//   (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
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

#include "rdtablewidget.h"

RDTableWidget::RDTableWidget(QWidget *parent)
  : QTableWidget(parent)
{
  Initialize();
}


RDTableWidget::RDTableWidget(int rows,int cols,QWidget *parent)
  : QTableWidget(rows,cols,parent)
{
  Initialize();
}


bool RDTableWidget::select(int column,unsigned value)
{
  selectionModel()->select(QModelIndex(),QItemSelectionModel::Clear);
  for(int i=0;i<model()->rowCount();i++) {
    if(model()->data(model()->index(i,column),Qt::DisplayRole).toUInt()==
       value) {
      selectionModel()->select(model()->index(i,column),
		      QItemSelectionModel::Select|QItemSelectionModel::Rows);
    }
  }
  return false;
}


bool RDTableWidget::select(int column,const QString &value,bool case_sensitive)
{
  selectionModel()->select(QModelIndex(),QItemSelectionModel::Clear);
  if(case_sensitive) {
    for(int i=0;i<model()->rowCount();i++) {
      if(model()->data(model()->index(i,column),
		       Qt::DisplayRole).toString()==value) {
	selectionModel()->select(model()->index(i,column),
		    QItemSelectionModel::Select|QItemSelectionModel::Rows);
      }
    }
  }
  else {
    for(int i=0;i<model()->rowCount();i++) {
      if(model()->data(model()->index(i,column),
		       Qt::DisplayRole).toString().toLower()==value.toLower()) {
	selectionModel()->select(model()->index(i,column),
		    QItemSelectionModel::Select|QItemSelectionModel::Rows);
      }
    }
  }
  return false;
}


void RDTableWidget::Initialize()
{
  setShowGrid(false);
  setWordWrap(false);
  setSelectionMode(QAbstractItemView::SingleSelection);
  setSelectionBehavior(QAbstractItemView::SelectRows);
}
