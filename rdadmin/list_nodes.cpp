// list_nodes.cpp
//
// List Rivendell Livewire Nodes
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

#include <rd.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdpasswd.h>

#include "edit_node.h"
#include "list_nodes.h"
#include "edit_endpoint.h"

ListNodes::ListNodes(RDMatrix *matrix,QWidget *parent)
  : RDDialog(parent)
{
  list_matrix=matrix;
  setWindowTitle("RDAdmin - "+tr("Livewire Node List"));

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  //
  // Nodes List Box
  //
  list_list_view=new RDTableView(this);
  list_list_model=new RDNodeListModel(list_matrix,this);
  list_list_model->setFont(defaultFont());
  list_list_model->setPalette(palette());
  list_list_view->setModel(list_list_model);
  connect(list_list_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));
  connect(list_list_model,SIGNAL(modelReset()),
	  list_list_view,SLOT(resizeColumnsToContents()));
  list_list_view->resizeColumnsToContents();

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


QSize ListNodes::sizeHint() const
{
  return QSize(400,250);
} 


QSizePolicy ListNodes::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListNodes::addData()
{
  int id=-1;
  EditNode *d=new EditNode(&id,list_matrix,this);
  if(d->exec()) {
    QModelIndex row=list_list_model->addNode(id);
    if(row.isValid()) {
      list_list_view->selectRow(row.row());
    }
  }
  delete d;
                              }


void ListNodes::editData()
{
  QModelIndexList rows=list_list_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  int id=list_list_model->nodeId(rows.first());
  EditNode *d=new EditNode(&id,list_matrix,this);
  if(d->exec()) {
    list_list_model->refresh(rows.first());
  }
  delete d;
}


void ListNodes::deleteData()
{
  QModelIndexList rows=list_list_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  int id=list_list_model->nodeId(rows.first());
  if(QMessageBox::question(this,tr("Delete Node"),
			   tr("Are your sure you want to delete this node?"),
			   QMessageBox::Yes,QMessageBox::No)!=
     QMessageBox::Yes) {
    return;
  }
  QString sql=QString().sprintf("delete from SWITCHER_NODES where ID=%d",id);
  RDSqlQuery::apply(sql);
  list_list_model->removeNode(id);
}


void ListNodes::doubleClickedData(const QModelIndex &index)
{
  editData();
}


void ListNodes::closeData()
{
  PurgeEndpoints("INPUTS");
  PurgeEndpoints("OUTPUTS");
  done(0);
}


void ListNodes::resizeEvent(QResizeEvent *e)
{
  list_list_view->setGeometry(10,10,size().width()-20,size().height()-80);
  list_add_button->setGeometry(10,size().height()-60,80,50);
  list_edit_button->setGeometry(100,size().height()-60,80,50);
  list_delete_button->setGeometry(190,size().height()-60,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void ListNodes::PurgeEndpoints(const QString &tablename)
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("select ")+
    "HOSTNAME,"+       // 00
    "TCP_PORT "+  // 01
    "from SWITCHER_NODES where "+
    "(STATION_NAME=\""+RDEscapeString(list_matrix->station())+"\")&&"+
    QString().sprintf("(MATRIX=%d)",list_matrix->matrix());
  q=new RDSqlQuery(sql);
  sql=QString("delete from ")+tablename+" where "+
    "(STATION_NAME=\""+RDEscapeString(list_matrix->station())+"\")&&"+
    QString().sprintf("(MATRIX=%d)&&",list_matrix->matrix());
  while(q->next()) {
    sql+=QString("((NODE_HOSTNAME!=\"")+
      RDEscapeString(q->value(0).toString())+"\")&&"+
      QString().sprintf("(NODE_TCP_PORT!=%d))&&",q->value(1).toInt());
  }
  sql=sql.left(sql.length()-2);
  delete q;
  RDSqlQuery::apply(sql);
}
