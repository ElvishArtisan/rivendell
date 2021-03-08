// list_endpoints.cpp
//
// List a Rivendell Endpoints
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

#include "edit_user.h"
#include "list_endpoints.h"
#include "edit_endpoint.h"

ListEndpoints::ListEndpoints(RDMatrix *matrix,RDMatrix::Endpoint endpoint,
			     QWidget *parent)
  : RDDialog(parent)
{
  QString str;

  list_matrix=matrix;
  list_endpoint=endpoint;
  switch(list_endpoint) {
  case RDMatrix::Input:
    list_size=list_matrix->inputs();
    list_table="INPUTS";
    setWindowTitle("RDAdmin - "+tr("List Inputs"));
    break;

  case RDMatrix::Output:
    list_size=list_matrix->outputs();
    list_table="OUTPUTS";
    setWindowTitle("RDAdmin - "+tr("List Outputs"));
    break;
  }

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  //
  // Endpoints List Box
  //
  list_list_view=new RDTableView(this);
  list_list_model=new RDEndpointListModel(matrix,endpoint,false,this);
  list_list_model->setFont(defaultFont());
  list_list_model->setPalette(palette());
  list_list_view->setModel(list_list_model);
  list_type_label=new QLabel(list_table,this);
  list_type_label->setFont(labelFont());
  list_type_label->setGeometry(14,5,85,19);
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
  list_edit_button->setText(tr("Edit"));
  connect(list_edit_button,SIGNAL(clicked()),this,SLOT(editData()));
  list_edit_button->setDisabled(list_readonly);

  //
  //  Cancel Button
  //
  list_close_button=new QPushButton(this);
  list_close_button->setFont(buttonFont());
  list_close_button->setText(tr("Close"));
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));
}


QSize ListEndpoints::sizeHint() const
{
  return QSize(400,250);
} 


QSizePolicy ListEndpoints::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListEndpoints::editData()
{
  QModelIndexList rows=list_list_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }

  int pointnum=list_list_model->endpointNumber(rows.first())-1;
  QString pointname=list_list_model->endpointName(rows.first());
  int enginenum=list_list_model->engineNumber(rows.first());
  int devicenum=list_list_model->deviceNumber(rows.first());
  EditEndpoint *edit=new EditEndpoint(list_matrix->type(),list_endpoint,
				      pointnum,&pointname,
				      &enginenum,&devicenum,this);
  if(edit->exec(list_matrix,list_endpoint,
		list_list_model->endpointId(rows.first()))) {
    list_list_model->refresh(rows.first());
  }
  delete edit;
}


void ListEndpoints::doubleClickedData(const QModelIndex &index)
{
  editData();
}


void ListEndpoints::closeData()
{
  done(true);
}


void ListEndpoints::resizeEvent(QResizeEvent *e)
{
  list_list_view->
    setGeometry(10,24,size().width()-20,size().height()-94);
  list_edit_button->setGeometry(10,size().height()-60,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
