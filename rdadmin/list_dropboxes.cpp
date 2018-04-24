// list_dropboxes.cpp
//
// List Rivendell Dropboxes
//
//   (C) Copyright 2002,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include "edit_dropbox.h"
#include "list_dropboxes.h"

ListDropboxes::ListDropboxes(const QString &stationname,QWidget *parent)
  : QDialog(parent)
{
  list_stationname=stationname;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  setWindowTitle("RDAdmin - "+tr("Rivendell Dropbox Configurations on")+" "+
		 stationname);

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);
  QFont list_font=QFont("Helvetica",12,QFont::Normal);
  list_font.setPixelSize(12);
  QFont small_font=QFont("Helvetica",10,QFont::Normal);
  small_font.setPixelSize(10);

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

  //
  // Group List
  //
  list_model=new RDSqlTableModel(this);
  QString sql=QString("select ")+
    "DROPBOXES.ID,"+                    // 00
    "DROPBOXES.GROUP_NAME,"+            // 01
    "DROPBOXES.PATH,"+                  // 02
    "DROPBOXES.NORMALIZATION_LEVEL,"+   // 03
    "DROPBOXES.AUTOTRIM_LEVEL,"+        // 04
    "DROPBOXES.TO_CART,"+               // 05
    "DROPBOXES.USE_CARTCHUNK_ID,"+      // 06
    "DROPBOXES.DELETE_CUTS,"+           // 07
    "DROPBOXES.METADATA_PATTERN,"+      // 08
    "DROPBOXES.FIX_BROKEN_FORMATS,"+    // 09
    "DROPBOXES.SET_USER_DEFINED,"+      // 10
    "GROUPS.COLOR "+                    // 11
    "from DROPBOXES left join GROUPS "+
    "on DROPBOXES.GROUP_NAME=GROUPS.NAME where "+
    "DROPBOXES.STATION_NAME=\""+RDEscapeString(list_stationname)+"\"";
  list_model->setQuery(sql);
  list_model->setHeaderData(1,Qt::Horizontal,tr("Group"));
  list_model->setFieldType(1,RDSqlTableModel::ColorTextType,11);
  list_model->setHeaderData(2,Qt::Horizontal,tr("Path"));
  list_model->setHeaderData(3,Qt::Horizontal,tr("Normalization Level"));
  list_model->setFieldType(3,RDSqlTableModel::AudioLevelType);
  list_model->setHeaderData(4,Qt::Horizontal,tr("Autotrim Level"));
  list_model->setFieldType(4,RDSqlTableModel::AudioLevelType);
  list_model->setHeaderData(5,Qt::Horizontal,tr("To Cart"));
  list_model->setFieldType(5,RDSqlTableModel::CartNumberType);
  list_model->setHeaderData(6,Qt::Horizontal,tr("Use CartChunk ID"));
  list_model->setFieldType(6,RDSqlTableModel::BooleanType);
  list_model->setHeaderData(7,Qt::Horizontal,tr("Delete Cuts"));
  list_model->setFieldType(7,RDSqlTableModel::BooleanType);
  list_model->setHeaderData(8,Qt::Horizontal,tr("Metadata Pattern"));
  list_model->setHeaderData(9,Qt::Horizontal,tr("Fix Broken Formats"));
  list_model->setFieldType(9,RDSqlTableModel::BooleanType);
  list_model->setHeaderData(10,Qt::Horizontal,tr("User Defined"));
  list_view=new RDTableView(this);
  list_view->setModel(list_model);
  list_view->hideColumn(0);
  list_view->hideColumn(11);
  list_view->resizeColumnsToContents();
  connect(list_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));
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
  int id;

  if((id=RDDropbox::create(list_stationname))<0) {
    QMessageBox::warning(this,"RDAdmin - "+tr("Error"),
			 tr("Unable to create dropbox!"));
    return;
  }
  EditDropbox *edit_dropbox=new EditDropbox(id,this);
  if(edit_dropbox->exec()<0) {
    QString sql=QString().sprintf("delete from DROPBOXES where ID=%d",id);
    RDSqlQuery *q=new RDSqlQuery(sql);
    delete q;
    delete edit_dropbox;
    return;
  }
  list_model->update();
}


void ListDropboxes::editData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    EditDropbox *edit_dropbox=
      new EditDropbox(s->selectedRows()[0].data().toInt(),this);
    if(edit_dropbox->exec()==0) {
      list_model->update();
    }
    delete edit_dropbox;
  }
}


void ListDropboxes::deleteData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    if(QMessageBox::question(this,"RDAdmin - "+tr("Delete Dropbox"),
			     tr("Are you sure you want to delete the dropbox?"),
			     QMessageBox::Yes,QMessageBox::No)!=
       QMessageBox::Yes) {
      return;
    }
    RDDropbox::remove(s->selectedRows()[0].data().toInt());
    list_model->update();
  }
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
  list_delete_button->setGeometry(size().width()-90,130,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
  list_view->setGeometry(10,10,size().width()-120,size().height()-40);
}
