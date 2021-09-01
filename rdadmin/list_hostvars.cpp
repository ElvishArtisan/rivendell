// list_hostvars.cpp
//
// List Rivendell Host Variables
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

#include <QMessageBox>

#include <rddb.h>
#include <rdescape_string.h>

#include "globals.h"
#include "list_hostvars.h"

ListHostvars::ListHostvars(QString station,QWidget *parent)
  : RDDialog(parent)
{
  QString str;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  list_station=station;
  str=QString(tr("Host Variables for"));
  setWindowTitle("RDAdmin - "+tr("Host Variables for")+" "+station);

  //
  // Dialogs
  //
  list_edit_hostvar_dialog=new EditHostvar(this);

  //
  // Matrix List Box
  //
  list_view=new RDTableView(this);
  list_model=new RDHostvarListModel(station,this);
  list_model->setFont(defaultFont());
  list_model->setPalette(palette());
  list_view->setModel(list_model);
  list_title_label=new QLabel(tr("Host Variables")+":",this);
  list_title_label->setFont(labelFont());
  list_title_label->setGeometry(14,5,sizeHint().width()-28,19);
  connect(list_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));
  connect(list_model,SIGNAL(modelReset()),
	  list_view,SLOT(resizeColumnsToContents()));
  list_view->resizeColumnsToContents();

  //
  //  Add Button
  //
  list_add_button=new QPushButton(this);
  list_add_button->setFont(buttonFont());
  list_add_button->setText(tr("Add"));
  connect(list_add_button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  //  Edit Button
  //
  list_edit_button=new QPushButton(this);
  list_edit_button->setFont(buttonFont());
  list_edit_button->setText(tr("Edit"));
  connect(list_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Delete Button
  //
  list_delete_button=new QPushButton(this);
  list_delete_button->setFont(buttonFont());
  list_delete_button->setText(tr("Delete"));
  connect(list_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  //  Close Button
  //
  list_close_button=new QPushButton(this);
  list_close_button->setDefault(true);
  list_close_button->setFont(buttonFont());
  list_close_button->setText(tr("Close"));
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));
}


ListHostvars::~ListHostvars()
{
  delete list_edit_hostvar_dialog;
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
  QString sql=QString("insert into `HOSTVARS` set ")+
    "`STATION_NAME`='"+RDEscapeString(list_station)+"',"+
    "`NAME`='"+RDEscapeString("%NEW_VAR%")+"'";
  int id=RDSqlQuery::run(sql).toInt();
  if(list_edit_hostvar_dialog->exec(id)) {
    QModelIndex row=list_model->addVar(id);
    if(row.isValid()) {
      list_view->selectRow(row.row());
    }
    else {
      sql=QString("delete from `HOSTVARS` ")+
	QString::asprintf("where `ID`=%d",id);
      RDSqlQuery::apply(sql);
    }
  }
}


void ListHostvars::editData()
{
  QModelIndexList rows=list_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  if(list_edit_hostvar_dialog->exec(list_model->varId(rows.first()))) {
    list_model->refresh(rows.first());
  }
}


void ListHostvars::deleteData()
{
  QModelIndexList rows=list_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  
  if(QMessageBox::question(this,"RDAdmin - "+tr("Delete Host Variable"),
			   tr("Are you sure you want to delete the variable")+
			   " \""+list_model->data(rows.first()).toString()+
			   "\"?",
		      QMessageBox::Yes,QMessageBox::No)!=QMessageBox::Yes) {
    return;
  }
  QString sql=QString("delete from `HOSTVARS` ")+
    QString::asprintf("where `ID`=%d",list_model->varId(rows.first()));
  RDSqlQuery::apply(sql);
  list_model->removeVar(rows.first());
}


void ListHostvars::doubleClickedData(const QModelIndex &index)
{
  editData();
}


void ListHostvars::closeData()
{
  done(true);
}


void ListHostvars::resizeEvent(QResizeEvent *e)
{
  list_view->setGeometry(10,24,size().width()-20,size().height()-114);
  list_add_button->setGeometry(10,size().height()-80,80,50);
  list_edit_button->setGeometry(100,size().height()-80,80,50);
  list_delete_button->setGeometry(190,size().height()-80,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
