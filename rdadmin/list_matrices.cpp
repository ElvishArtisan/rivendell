// list_matrices.cpp
//
// List Rivendell Matrices
//
//   (C) Copyright 2002-2003,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <QMessageBox>

#include <rdapplication.h>
#include <rdescape_string.h>

#include "add_matrix.h"
#include "edit_matrix.h"
#include "globals.h"
#include "list_matrices.h"

ListMatrices::ListMatrices(QString station,QWidget *parent)
  : QDialog(parent)
{
  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  list_station=station;
  setWindowTitle("RDAdmin - "+tr("Rivendell Switcher List"));

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);
  QFont small_font=QFont("Helvetica",10,QFont::Bold);
  small_font.setPixelSize(10);

  //
  // Initialize Data Structures
  //
  for(int i=0;i<MAX_MATRICES;i++) {
    list_matrix_modified[i]=false;
  }

  //
  // Matrix List Box
  //
  QLabel *label=new QLabel(tr("Switchers:"),this);
  label->setFont(font);
  label->setGeometry(14,5,85,19);
  list_model=new RDSqlTableModel(this);
  QString sql=QString("select ")+
    "MATRIX,"+
    "NAME,"+
    "TYPE "+
    "from MATRICES where "+
    "STATION_NAME=\""+RDEscapeString(list_station)+"\" "+
    "order by MATRIX";
  list_model->setQuery(sql);
  list_model->setHeaderData(0,Qt::Horizontal,tr("Matrix"));
  list_model->setHeaderData(1,Qt::Horizontal,tr("Description"));
  list_model->setHeaderData(2,Qt::Horizontal,tr("Type"));
  list_model->setFieldType(2,RDSqlTableModel::MatrixType);
  list_view=new RDTableView(this);
  list_view->setGeometry(10,24,sizeHint().width()-20,sizeHint().height()-94);
  list_view->setModel(list_model);
  list_view->resizeColumnsToContents();
  connect(list_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));

  //
  //  Add Button
  //
  QPushButton *add_button=new QPushButton(this);
  add_button->setGeometry(10,sizeHint().height()-60,80,50);
  add_button->setFont(font);
  add_button->setText(tr("&Add"));
  connect(add_button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  //  Edit Button
  //
  QPushButton *edit_button=new QPushButton(this);
  edit_button->setGeometry(100,sizeHint().height()-60,80,50);
  edit_button->setFont(font);
  edit_button->setText(tr("&Edit"));
  connect(edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Delete Button
  //
  QPushButton *delete_button=new QPushButton(this);
  delete_button->setGeometry(190,sizeHint().height()-60,80,50);
  delete_button->setFont(font);
  delete_button->setText(tr("&Delete"));
  connect(delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  //  Close Button
  //
  QPushButton *close_button=new QPushButton(this);
  close_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			    80,50);
  close_button->setDefault(true);
  close_button->setFont(font);
  close_button->setText(tr("&Close"));
  connect(close_button,SIGNAL(clicked()),this,SLOT(closeData()));
}


ListMatrices::~ListMatrices()
{
  delete list_view;
}


QSize ListMatrices::sizeHint() const
{
  return QSize(400,300);
} 


QSizePolicy ListMatrices::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListMatrices::addData()
{
  int matrix_num;

  AddMatrix *add=new AddMatrix(list_station,this);
  if((matrix_num=add->exec())<0) {
    delete add;
    return;
  }
  delete add;
  RDMatrix *matrix=new RDMatrix(list_station,matrix_num);
  EditMatrix *edit=new EditMatrix(matrix,this);
  if(edit->exec()!=0) {
    RDMatrix::remove(list_station,matrix_num);
  }
  else {
    list_matrix_modified[matrix_num]=true;
    list_model->update();
  }
  delete edit;
  delete matrix;
}


void ListMatrices::editData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    RDMatrix *matrix=new RDMatrix(list_station,s->selectedRows()[0].data().toInt());
    EditMatrix *edit=new EditMatrix(matrix,this);
    if(edit->exec()==0) {
      list_model->update();
      list_matrix_modified[s->selectedRows()[0].data().toInt()]=true;
    }
    delete edit;
    delete matrix;
  }
}


void ListMatrices::deleteData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    if(QMessageBox::question(this,"RDAdmin - "+tr("Delete Switcher"),
			     tr("Are you sure you want to delete this switcher?"),
			     QMessageBox::No,QMessageBox::Yes)!=
       QMessageBox::Yes) {
      return;
    }
    RDMatrix::remove(list_station,s->selectedRows()[0].data().toInt());
    list_matrix_modified[s->selectedRows()[0].data().toInt()]=true;
    list_model->update();
  }
}


void ListMatrices::doubleClickedData(const QModelIndex &index)
{
  editData();
}


void ListMatrices::closeData()
{
  RDStation *rmt_station=new RDStation(list_station);
  QHostAddress addr=rmt_station->address();
  RDMacro macro;
  
  macro.setCommand(RDMacro::SZ);
  macro.setRole(RDMacro::Cmd);
  macro.setEchoRequested(false);
  macro.setArgQuantity(1);
  for(int i=0;i<MAX_MATRICES;i++) {
    if(list_matrix_modified[i]) {
      macro.setAddress(rmt_station->address());
      macro.setArg(0,i);
      rda->ripc()->sendRml(&macro);
    }
  }
  delete rmt_station;
  done(0);
}
