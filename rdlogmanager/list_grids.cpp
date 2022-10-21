// list_grids.cpp
//
// List Rivendell Log Grids
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

#include "edit_grid.h"
#include "globals.h"
#include "list_grids.h"

ListGrids::ListGrids(QWidget *parent)
  : RDDialog(parent)
{
  setWindowTitle("RDLogManager - "+tr("Log Grids"));

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  //
  // Grids List
  //
  edit_grids_view=new RDTableView(this);
  edit_grids_view->
    setGeometry(10,10,sizeHint().width()-20,sizeHint().height()-80);
  edit_grids_model=new RDServiceListModel(false,false,this);
  edit_grids_model->setFont(font());
  edit_grids_model->setPalette(palette());
  edit_grids_view->setModel(edit_grids_model);
  for(int i=2;i<(edit_grids_model->columnCount()-1);i++) {
    edit_grids_view->hideColumn(i);
  }
  connect(edit_grids_view->selectionModel(),
	  SIGNAL(selectionChanged(const QItemSelection &,
				  const QItemSelection &)),
	  this,
	  SLOT(selectionChangedData(const QItemSelection &,
				    const QItemSelection &)));
  connect(edit_grids_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));
  connect(edit_grids_model,SIGNAL(modelReset()),
	  edit_grids_view,SLOT(resizeColumnsToContents()));
  edit_grids_view->resizeColumnsToContents();

  //
  //  Edit Button
  //
  edit_edit_button=new QPushButton(this);
  edit_edit_button->setGeometry(10,sizeHint().height()-60,80,50);
  edit_edit_button->setFont(buttonFont());
  edit_edit_button->setText(tr("Edit"));
  edit_edit_button->setDisabled(true);
  connect(edit_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Close Button
  //
  edit_close_button=new QPushButton(this);
  edit_close_button->
    setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  edit_close_button->setDefault(true);
  edit_close_button->setFont(buttonFont());
  edit_close_button->setText(tr("Close"));
  connect(edit_close_button,SIGNAL(clicked()),this,SLOT(closeData()));
}


QSize ListGrids::sizeHint() const
{
  return QSize(400,300);
} 


QSizePolicy ListGrids::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListGrids::editData()
{
  QModelIndexList rows=edit_grids_view->selectionModel()->selectedRows();

  if((rows.size()!=1)||(!edit_grids_model->hasGrid(rows.at(0)))) {
    return;
  }
  EditGrid *d=new EditGrid(edit_grids_model->serviceName(rows.first()),this);
  d->exec();
  delete d;
}


void ListGrids::selectionChangedData(const QItemSelection &selected,
				     const QItemSelection &previous)
{
  QModelIndexList rows=edit_grids_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    edit_edit_button->setDisabled(true);
    return;
  }
  edit_edit_button->setEnabled(edit_grids_model->hasGrid(rows.at(0)));
}


void ListGrids::doubleClickedData(const QModelIndex &index)
{
  editData();
}


void ListGrids::closeData()
{
  done(true);
}
