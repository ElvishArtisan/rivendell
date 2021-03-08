// list_pypads.cpp
//
// List PyPAD Instances
//
//   (C) Copyright 2018-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <QFileDialog>
#include <QHeaderView>
#include <QMessageBox>

#include <rd.h>
#include <rdapplication.h>
#include <rdescape_string.h>
#include <rdpaths.h>

#include "edit_pypad.h"
#include "list_pypads.h"
#include "view_pypad_errors.h"

ListPypads::ListPypads(RDStation *station,QWidget *parent)
  : RDDialog(parent)
{
  list_station=station;

  setMinimumSize(sizeHint());

  setWindowTitle("RDAdmin - "+tr("PyPAD Instances on")+" "+
		 rda->station()->name());

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  //
  // Instances List Box
  //
  list_list_view=new RDTableView(this);
  list_list_model=new RDPypadListModel(station->name(),this);
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
  //  Error Button
  //
  list_error_button=new QPushButton(this);
  list_error_button->setFont(buttonFont());
  list_error_button->setText(tr("Error\nLog"));
  connect(list_error_button,SIGNAL(clicked()),this,SLOT(errorData()));

  //
  //  Close Button
  //
  list_close_button=new QPushButton(this);
  list_close_button->setDefault(true);
  list_close_button->setFont(buttonFont());
  list_close_button->setText(tr("Close"));
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));
}


QSize ListPypads::sizeHint() const
{
  return QSize(600,400);
} 


QSizePolicy ListPypads::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListPypads::addData()
{
  //
  // Get Script Name
  //
  QString script=
    QFileDialog::getOpenFileName(this,tr("Select PyPAD Script"),
				 RD_PYPAD_SCRIPT_DIR,
				 "Python Scripts (*.py)");
  if(script.isNull()) {
    return;
  }

  //
  // Get Exemplar
  //
  QString exemplar="";
  QStringList f0=script.split(".");
  f0.last()="exemplar";
  QFile *file=new QFile(f0.join("."));
  if(file->open(QIODevice::ReadOnly)) {
    exemplar=file->readAll();
    file->close();
  }
  delete file;

  QString sql=QString("insert into PYPAD_INSTANCES set ")+
    "STATION_NAME=\""+RDEscapeString(list_station->name())+"\","+
    "SCRIPT_PATH=\""+RDEscapeString(script)+"\","+
    "DESCRIPTION=\""+
    RDEscapeString("new "+script.split("/").last()+" instance")+"\","+
    "CONFIG=\""+RDEscapeString(exemplar)+"\"";
  int id=RDSqlQuery::run(sql).toInt();
  EditPypad *d=new EditPypad(id,this);
  if(d->exec()) {
    QModelIndex row=list_list_model->addInstance(id);
    if(row.isValid()) {
      list_list_view->selectRow(row.row());
    }
    RDNotification notify=RDNotification(RDNotification::PypadType,
					 RDNotification::AddAction,id);
    rda->ripc()->sendNotification(notify);
  }
  else {
    sql=QString("delete from PYPAD_INSTANCES where ")+
      QString().sprintf("ID=%u",id);
    RDSqlQuery::apply(sql);
  }
  delete d;
}


void ListPypads::editData()
{
  QModelIndexList rows=list_list_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  EditPypad *d=new EditPypad(list_list_model->instanceId(rows.first()),this);
  if(d->exec()) {
    list_list_model->refresh(rows.first());
    RDNotification notify=
      RDNotification(RDNotification::PypadType,RDNotification::ModifyAction,
		     list_list_model->instanceId(rows.first()));
    rda->ripc()->sendNotification(notify);
  }
  delete d;
}


void ListPypads::deleteData()
{
  QString sql;
  QModelIndexList rows=list_list_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  int id=list_list_model->instanceId(rows.first());
  if(QMessageBox::question(this,tr("Delete Instance"),
       		   tr("Are your sure you want to delete this instance?"),
			   QMessageBox::Yes,QMessageBox::No)==
     QMessageBox::No) {
    return;
  }
  sql=QString("delete from PYPAD_INSTANCES where ")+
    QString().sprintf("ID=%d",id);
  RDSqlQuery::apply(sql);
  list_list_model->removeInstance(id);
  RDNotification notify=
    RDNotification(RDNotification::PypadType,RDNotification::DeleteAction,id);
  rda->ripc()->sendNotification(notify);
}


void ListPypads::errorData()
{
  QModelIndexList rows=list_list_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  ViewPypadErrors *d=
    new ViewPypadErrors(list_list_model->instanceId(rows.first()),this);
  d->exec();
  delete d;
}


void ListPypads::doubleClickedData(const QModelIndex &index)
{
  editData();
}


void ListPypads::closeData()
{
  done(true);
}


void ListPypads::resizeEvent(QResizeEvent *e)
{
  list_list_view->setGeometry(10,10,size().width()-20,size().height()-80);

  list_add_button->setGeometry(10,size().height()-60,80,50);
  list_edit_button->setGeometry(100,size().height()-60,80,50);
  list_delete_button->setGeometry(190,size().height()-60,80,50);

  list_error_button->setGeometry(300,size().height()-60,80,50);

  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
