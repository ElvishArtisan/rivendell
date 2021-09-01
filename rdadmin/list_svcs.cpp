// list_svcs.cpp
//
// List Rivendell Services
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

#include <QEvent>
#include <QMessageBox>

#include <rdapplication.h>
#include <rddb.h>
#include <rdescape_string.h>

#include "add_svc.h"
#include "edit_svc.h"
#include "globals.h"
#include "list_svcs.h"

ListSvcs::ListSvcs(QWidget *parent)
  : RDDialog(parent)
{
  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  setWindowTitle("RDAdmin - "+tr("Services"));

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

  //
  // Services List Box
  //
  list_services_view=new RDTableView(this);
  list_title_label=new QLabel(tr("Services:"),this);
  list_title_label->setFont(labelFont());
  list_title_label->setGeometry(14,11,85,19);
  list_services_model=new RDServiceListModel(false,this);
  list_services_model->setFont(defaultFont());
  list_services_model->setPalette(palette());
  list_services_view->setModel(list_services_model);
  connect(list_services_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));
  connect(list_services_model,SIGNAL(modelReset()),
	  list_services_view,SLOT(resizeColumnsToContents()));
  list_services_view->resizeColumnsToContents();
}


ListSvcs::~ListSvcs()
{
  delete list_services_model;
  delete list_services_view;
}


QSize ListSvcs::sizeHint() const
{
  return QSize(500,300);
} 


QSizePolicy ListSvcs::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListSvcs::addData()
{
  QString svcname;

  AddSvc *d=new AddSvc(&svcname,this);
  if(d->exec()) {
    QModelIndex index=list_services_model->addService(svcname);
    list_services_view->selectRow(index.row());
  }
  delete d;
}


void ListSvcs::editData()
{
  QModelIndexList rows=list_services_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  EditSvc *d=new EditSvc(list_services_model->serviceName(rows.first()),this);
  if(d->exec()) {
    list_services_model->refresh(rows.first());
  }
  delete d;
}


void ListSvcs::deleteData()
{
  QString sql;
  RDSqlQuery *q;
  QModelIndexList rows=list_services_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  QString svcname=list_services_model->serviceName(rows.first());
  if(QMessageBox::warning(this,"RDAdmin- "+tr("Delete Service"),
			  tr("Are you sure you want to delete service")+
			  " \""+svcname+"\"?",
			  QMessageBox::Yes,QMessageBox::No)!=QMessageBox::Yes) {
    return;
  }

  sql=QString("select `NAME` from `LOGS` where ")+
    "`SERVICE`='"+RDEscapeString(svcname)+"'";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    if(QMessageBox::warning(this,"RDAdmin - "+tr("Logs Exist"),
			    tr("There are")+QString::asprintf(" %d ",q->size())+
			    tr("logs owned by this service that will also be deleted.")+"\n"+tr("Do you still want to proceed?"),
			    QMessageBox::Yes,QMessageBox::No)!=
       QMessageBox::Yes) {
      delete q;
      return;
    }
  }
  delete q;
  RDSvc *svc=new RDSvc(svcname,rda->station(),rda->config());
  svc->remove();
  delete svc;
  list_services_model->removeService(svcname);
}


void ListSvcs::closeData()
{
  done(true);
}


void ListSvcs::doubleClickedData(const QModelIndex &index)
{
  editData();
}


void ListSvcs::resizeEvent(QResizeEvent *e)
{
  list_add_button->setGeometry(size().width()-90,30,80,50);
  list_edit_button->setGeometry(size().width()-90,90,80,50);
  list_delete_button->setGeometry(size().width()-90,150,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
  list_services_view->setGeometry(10,30,size().width()-110,size().height()-40);
}
