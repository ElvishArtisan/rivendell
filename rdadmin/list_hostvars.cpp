// list_hostvars.cpp
//
// List Rivendell Host Variables
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

#include <QMessageBox>

#include <rddb.h>
#include <rdescape_string.h>

#include "add_hostvar.h"
#include "edit_hostvar.h"
#include "globals.h"
#include "list_hostvars.h"

ListHostvars::ListHostvars(QString station,QWidget *parent)
  : QDialog(parent)
{
  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  list_station=station;
  setWindowTitle("RDAdmin - "+tr("Host variables for")+" "+station);

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);
  QFont small_font=QFont("Helvetica",10,QFont::Bold);
  small_font.setPixelSize(10);

  //
  // Matrix List Box
  //
  list_label=new QLabel(tr("Host Variables"),this);
  list_label->setFont(font);
  list_model=new RDSqlTableModel(this);
  QString sql=QString("select ")+
    "ID,"+
    "NAME,"+
    "VARVALUE,"+
    "REMARK "+
    "from HOSTVARS where "+
    "STATION_NAME=\""+RDEscapeString(list_station)+"\" "+
    "order by NAME";
  list_model->setQuery(sql);
  list_model->setHeaderData(1,Qt::Horizontal,tr("Name"));
  list_model->setHeaderData(2,Qt::Horizontal,tr("Value"));
  list_model->setHeaderData(3,Qt::Horizontal,tr("Remarks"));
  list_view=new RDTableView(this);
  list_view->setModel(list_model);
  list_view->hideColumn(0);
  list_view->resizeColumnsToContents();
  connect(list_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));

  //
  //  Add Button
  //
  list_add_button=new QPushButton(this);
  list_add_button->setFont(font);
  list_add_button->setText(tr("&Add"));
  connect(list_add_button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  //  Edit Button
  //
  list_edit_button=new QPushButton(this);
  list_edit_button->setFont(font);
  list_edit_button->setText(tr("&Edit"));
  connect(list_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Delete Button
  //
  list_delete_button=new QPushButton(this);
  list_delete_button->setFont(font);
  list_delete_button->setText(tr("&Delete"));
  connect(list_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  //  Close Button
  //
  list_close_button=new QPushButton(this);
  list_close_button->setDefault(true);
  list_close_button->setFont(font);
  list_close_button->setText(tr("&Close"));
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));
}


ListHostvars::~ListHostvars()
{
  delete list_view;
}


QSize ListHostvars::sizeHint() const
{
  return QSize(490,320);
} 


QSizePolicy ListHostvars::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListHostvars::addData()
{
  int id;

  AddHostvar *var_dialog=new AddHostvar(list_station,&id,this);
  if(var_dialog->exec()==0) {
    EditHostvar *var_dialog=
      new EditHostvar(id,this);
    if(var_dialog->exec()==0) {
      list_model->update();
    }
  }
}


void ListHostvars::editData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    EditHostvar *var_dialog=
      new EditHostvar(s->selectedRows()[0].data().toInt(),this);
    if(var_dialog->exec()==0) {
      list_model->update();
    }
  }
}


void ListHostvars::deleteData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    if(QMessageBox::question(this,"RDAdmin - "+tr("Delete Host Variable"),
			     tr("Are you sure you want to delete the host variable?"),
			     QMessageBox::No,QMessageBox::Yes)!=
       QMessageBox::Yes) {
      return;
    }
    RDHostVariable::remove(s->selectedRows()[0].data().toInt());
    list_model->update();
  }
}


void ListHostvars::doubleClickedData(const QModelIndex &index)
{
  editData();
}


void ListHostvars::closeData()
{
  done(0);
}


void ListHostvars::resizeEvent(QResizeEvent *e)
{
  list_label->setGeometry(14,5,size().width()-28,19);
  list_view->setGeometry(10,24,size().width()-20,size().height()-94);
  list_add_button->setGeometry(10,size().height()-60,80,50);
  list_edit_button->setGeometry(100,size().height()-60,80,50);
  list_delete_button->setGeometry(190,size().height()-60,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
