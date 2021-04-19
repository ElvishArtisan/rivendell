// list_replicators.cpp
//
// List Rivendell Replication Configurations
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

#include <rdescape_string.h>
#include <rdreplicator.h>

#include "add_replicator.h"
#include "edit_replicator.h"
#include "list_replicators.h"
#include "list_replicator_carts.h"

ListReplicators::ListReplicators(QWidget *parent)
  : RDDialog(parent)
{
  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  setWindowTitle("RDAdmin - "+tr("Rivendell Replicators"));

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
  //  List Carts Button
  //
  list_list_button=new QPushButton(this);
  list_list_button->setFont(buttonFont());
  list_list_button->setText(tr("List\nCarts"));
  connect(list_list_button,SIGNAL(clicked()),this,SLOT(listData()));

  //
  //  Close Button
  //
  list_close_button=new QPushButton(this);
  list_close_button->setDefault(true);
  list_close_button->setFont(buttonFont());
  list_close_button->setText(tr("Close"));
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  // Replicator List
  //
  list_replicators_view=new RDTableView(this);
  list_replicators_model=new RDReplicatorListModel(this);
  list_replicators_model->setFont(defaultFont());
  list_replicators_model->setPalette(palette());
  list_replicators_view->setModel(list_replicators_model);
  list_replicators_label=new QLabel(tr("Replicators:"),this);
  list_replicators_label->setFont(labelFont());
  connect(list_replicators_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));
  connect(list_replicators_model,SIGNAL(modelReset()),
	  list_replicators_view,SLOT(resizeColumnsToContents()));
  list_replicators_view->resizeColumnsToContents();
}


ListReplicators::~ListReplicators()
{
}


QSize ListReplicators::sizeHint() const
{
  return QSize(640,480);
} 


QSizePolicy ListReplicators::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListReplicators::addData()
{
  QString name;

  AddReplicator *d=new AddReplicator(&name,this);
  if(d->exec()) {
    QModelIndex row=list_replicators_model->addReplicator(name);
    if(row.isValid()) {
      list_replicators_view->selectRow(row.row());
    }
  }
  delete d;
}


void ListReplicators::editData()
{
  QModelIndexList rows=list_replicators_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  EditReplicator *d=new EditReplicator(list_replicators_model->
				       replicatorName(rows.first()),this);
  if(d->exec()) {
    list_replicators_model->refresh(rows.first());
  }
  delete d;
}


void ListReplicators::deleteData()
{
  QString sql;
  QString warning;
  QModelIndexList rows=list_replicators_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  QString name=list_replicators_model->replicatorName(rows.first());

  warning+=tr("Are you sure you want to delete replicator")+
    " \""+name+"\"?";
  switch(QMessageBox::warning(this,tr("Delete Replicator"),warning,
			      QMessageBox::Yes,QMessageBox::No)) {
  case QMessageBox::No:
  case Qt::NoButton:
    return;

  default:
    break;
  }

  //
  // Delete Group Assignments
  //
  sql=QString("delete from `REPLICATOR_MAP` where ")+
    "`REPLICATOR_NAME`='"+RDEscapeString(name)+"'";
  RDSqlQuery::apply(sql);
  
  //
  // Delete State Records
  //
  sql=QString("delete from `REPL_CART_STATE` where ")+
    "`REPLICATOR_NAME`='"+RDEscapeString(name)+"'";
  RDSqlQuery::apply(sql);
  sql=QString("delete from `REPL_CUT_STATE` where ")+
    "`REPLICATOR_NAME`='"+RDEscapeString(name)+"'";
  RDSqlQuery::apply(sql);

  //
  // Delete from Replicator List
  //
  sql=QString("delete from `REPLICATORS` where ")+
    "`NAME`='"+RDEscapeString(name)+"'";
  RDSqlQuery::apply(sql);

  list_replicators_model->removeReplicator(name);
}


void ListReplicators::listData()
{
  QModelIndexList rows=list_replicators_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  QString name=list_replicators_model->replicatorName(rows.first());
  ListReplicatorCarts *d=new ListReplicatorCarts(this);
  d->exec(name);
  delete d;
}


void ListReplicators::doubleClickedData(const QModelIndex &index)
{
  editData();
}


void ListReplicators::closeData()
{
  done(true);
}


void ListReplicators::resizeEvent(QResizeEvent *e)
{
  list_add_button->setGeometry(size().width()-90,30,80,50);
  list_edit_button->setGeometry(size().width()-90,90,80,50);
  list_delete_button->setGeometry(size().width()-90,150,80,50);
  list_list_button->setGeometry(size().width()-90,250,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
  list_replicators_label->setGeometry(14,11,85,19);
  list_replicators_view->setGeometry(10,30,size().width()-120,size().height()-40);
}
