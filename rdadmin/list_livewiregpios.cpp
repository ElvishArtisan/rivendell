// list_livewiregpios.cpp
//
// List Rivendell Livewire GPIO Slot Associations
//
//   (C) Copyright 2013-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include "globals.h"

#include <rddb.h>
#include <rdescape_string.h>

#include "edit_livewiregpio.h"
#include "list_livewiregpios.h"

ListLiveWireGpios::ListLiveWireGpios(RDMatrix *matrix,int slot_quan,
				     QWidget *parent)
  : RDDialog(parent)
{
  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  list_matrix=matrix;
  list_slot_quan=slot_quan;
  setWindowTitle("RDAdmin - "+tr("Livewire GPIO Source Assignments"));

  //
  // Dialogs
  //
  list_gpio_dialog=new EditLiveWireGpio(this);

  //
  // Matrix List Box
  //
  list_view=new RDTableView(this);
  list_model=new RDGpioSlotsModel(matrix,slot_quan,this);
  list_model->setFont(defaultFont());
  list_model->setPalette(palette());
  list_view->setModel(list_model);
  connect(list_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));
  connect(list_model,SIGNAL(modelReset()),
	  list_view,SLOT(resizeColumnsToContents()));
  list_view->resizeColumnsToContents();

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
  list_close_button->setDefault(true);
  list_close_button->setFont(buttonFont());
  list_close_button->setText(tr("&Close"));
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));
}


ListLiveWireGpios::~ListLiveWireGpios()
{
  delete list_gpio_dialog;
  delete list_view;
  delete list_model;
}


QSize ListLiveWireGpios::sizeHint() const
{
  return QSize(400,300);
} 


QSizePolicy ListLiveWireGpios::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListLiveWireGpios::editData()
{
  QModelIndexList rows=list_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  int id=list_model->slotId(rows.first());
  if(list_gpio_dialog->exec(id)) {
    list_model->refresh(rows.first());
  }
}


void ListLiveWireGpios::doubleClickedData(const QModelIndex &index)
{
  editData();
}


void ListLiveWireGpios::closeData()
{
  done(true);
}


void ListLiveWireGpios::resizeEvent(QResizeEvent *e)
{
  list_view->setGeometry(10,3,size().width()-20,size().height()-73);
  list_edit_button->setGeometry(10,size().height()-60,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
