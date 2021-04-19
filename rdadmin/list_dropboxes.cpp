// list_dropboxes.cpp
//
// List Rivendell Dropboxes
//
//   (C) Copyright 2002-2021 Gleason <fredg@paravelsystems.com>
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

#include <rdapplication.h>
#include <rdescape_string.h>
#include <rdnotification.h>

#include "edit_dropbox.h"
#include "list_dropboxes.h"

ListDropboxes::ListDropboxes(const QString &stationname,QWidget *parent)
  : RDDialog(parent)
{
  list_stationname=stationname;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  setWindowTitle("RDAdmin - "+tr("Rivendell Dropbox Configurations on")+" "+
		 stationname);

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
  //  Duplicate Button
  //
  list_duplicate_button=new QPushButton(this);
  list_duplicate_button->setFont(buttonFont());
  list_duplicate_button->setText(tr("Duplicate"));
  connect(list_duplicate_button,SIGNAL(clicked()),this,SLOT(duplicateData()));

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
  // Dropbox List
  //
  list_dropboxes_view=new RDTableView(this);
  list_dropboxes_model=new RDDropboxListModel(stationname,this);
  list_dropboxes_model->setFont(defaultFont());
  list_dropboxes_model->setPalette(palette());
  list_dropboxes_view->setModel(list_dropboxes_model);
  connect(list_dropboxes_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));
  connect(list_dropboxes_model,SIGNAL(modelReset()),
	  list_dropboxes_view,SLOT(resizeColumnsToContents()));
  list_dropboxes_view->resizeColumnsToContents();
}


ListDropboxes::~ListDropboxes()
{
}


QSize ListDropboxes::sizeHint() const
{
  return QSize(640,420);
} 


QSizePolicy ListDropboxes::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListDropboxes::addData()
{
  RDDropbox *box=new RDDropbox(-1,list_stationname);
  int id=box->id();
  delete box;
  EditDropbox *edit_dropbox=new EditDropbox(id,false,this);
  if(edit_dropbox->exec()) {
    QModelIndex row=list_dropboxes_model->addDropbox(id);
    if(row.isValid()) {
      list_dropboxes_view->selectRow(row.row());
    }
    RDNotification *notify=new RDNotification(RDNotification::DropboxType,
					      RDNotification::AddAction,
					      list_stationname);
    rda->ripc()->sendNotification(*notify);
    delete notify;
  }
  else {
    QString sql=QString().sprintf("delete from `DROPBOXES` where `ID`=%d",id);
    RDSqlQuery *q=new RDSqlQuery(sql);
    delete q;
    delete edit_dropbox;
    return;
  }
}


void ListDropboxes::editData()
{
  QModelIndexList rows=list_dropboxes_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  EditDropbox *d=
    new EditDropbox(list_dropboxes_model->dropboxId(rows.first()),false,this);
  if(d->exec()) {
    list_dropboxes_model->refresh(rows.first());
    RDNotification *notify=new RDNotification(RDNotification::DropboxType,
					      RDNotification::ModifyAction,
					      list_stationname);
    rda->ripc()->sendNotification(*notify);
    delete notify;
  }
  delete d;
}


void ListDropboxes::duplicateData()
{
  QModelIndexList rows=list_dropboxes_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  int box_id=list_dropboxes_model->dropboxId(rows.first());
  RDDropbox *src_box=new RDDropbox(box_id,list_stationname);
  int new_box_id=src_box->duplicate();
  delete src_box;

  EditDropbox *d=new EditDropbox(new_box_id,true,this);
  if(d->exec()) {
    QModelIndex index=list_dropboxes_model->addDropbox(new_box_id);
    if(index.isValid()) {
      list_dropboxes_view->selectRow(index.row());
    }
    RDNotification *notify=new RDNotification(RDNotification::DropboxType,
					      RDNotification::AddAction,
					      list_stationname);
    rda->ripc()->sendNotification(*notify);
    delete notify;
  }
  else {
    QString sql=QString().sprintf("delete from `DROPBOXES` where `ID`=%d",
				  new_box_id);
    RDSqlQuery::apply(sql);
  }
  delete d;  
}


void ListDropboxes::deleteData()
{
  QString sql;
  QModelIndexList rows=list_dropboxes_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  int box_id=list_dropboxes_model->dropboxId(rows.first());
  sql=QString().sprintf("delete from `DROPBOX_PATHS` where `DROPBOX_ID`=%d",
			box_id);
  RDSqlQuery::apply(sql);

  sql=QString().sprintf("delete from `DROPBOXES` where `ID`=%d",box_id);
  RDSqlQuery::apply(sql);

  list_dropboxes_model->removeDropbox(box_id);

  RDNotification *notify=new RDNotification(RDNotification::DropboxType,
					    RDNotification::DeleteAction,
					    list_stationname);
  rda->ripc()->sendNotification(*notify);
  delete notify;
}


void ListDropboxes::doubleClickedData(const QModelIndex &index)
{
  editData();
}


void ListDropboxes::closeData()
{
  done(0);
}


void ListDropboxes::resizeEvent(QResizeEvent *e)
{
  list_add_button->setGeometry(size().width()-90,10,80,50);
  list_edit_button->setGeometry(size().width()-90,70,80,50);
  list_duplicate_button->setGeometry(size().width()-90,130,80,50);
  list_delete_button->setGeometry(size().width()-90,190,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
  list_dropboxes_view->
    setGeometry(10,10,size().width()-120,size().height()-40);
}
