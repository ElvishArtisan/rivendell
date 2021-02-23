// list_sas_resources.cpp
//
// List SAS Resources.
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

#include <rd.h>
#include <rddb.h>
#include <rdescape_string.h>

#include "edit_sas_resource.h"
#include "edit_user.h"
#include "list_sas_resources.h"

ListSasResources::ListSasResources(RDMatrix *matrix,int size,QWidget *parent)
  : RDDialog(parent)
{
  QString sql;
  QString str;

  list_matrix=matrix;
  list_size=size;
  setWindowTitle("RDAdmin - "+tr("SAS Switches"));

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  //
  // Dialogs
  //
  list_edit_resource_dialog=new EditSasResource(this);

  //
  // Resources List Box
  //
  //
  // Resources List Box
  //
  list_list_view=new RDTableView(this);
  list_list_model=
    new RDResourceListModel(list_matrix,RDMatrix::VguestTypeNone,this);
  list_list_model->setFont(defaultFont());
  list_list_model->setPalette(palette());
  list_list_view->setModel(list_list_model);
  list_title_label=new QLabel(tr("SAS Switches"),this);
  list_title_label->setFont(labelFont());
  connect(list_list_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));
  connect(list_list_model,SIGNAL(modelReset()),
	  list_list_view,SLOT(resizeColumnsToContents()));
  list_list_view->resizeColumnsToContents();

  //
  //  Edit Button
  //
  list_edit_button=new QPushButton(this);
  list_edit_button->setFont(buttonFont());
  list_edit_button->setText(tr("&Edit"));
  connect(list_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Close Button
  //
  list_close_button=new QPushButton(this);
  list_close_button->setFont(buttonFont());
  list_close_button->setText(tr("&Close"));
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));
}


ListSasResources::~ListSasResources()
{
  delete list_edit_resource_dialog;
}


QSize ListSasResources::sizeHint() const
{
  return QSize(400,250);
} 


QSizePolicy ListSasResources::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListSasResources::editData()
{
  QModelIndexList rows=list_list_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }

  if(list_edit_resource_dialog->
     exec(list_list_model->resourceId(rows.first()))) {
    list_list_model->refresh(rows.first());
  }
}


void ListSasResources::doubleClickedData(const QModelIndex &index)
{
  editData();
}


void ListSasResources::closeData()
{
  done(true);
}


void ListSasResources::resizeEvent(QResizeEvent *e)
{
  list_list_view->setGeometry(10,24,size().width()-20,size().height()-94);
  list_title_label->setGeometry(14,5,85,19);
  list_edit_button->setGeometry(10,size().height()-60,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
