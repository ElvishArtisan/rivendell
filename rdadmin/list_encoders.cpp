// list_encoders.cpp
//
// List Rivendell Encoder Profiles
//
//   (C) Copyright 2020-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include "list_encoders.h"

ListEncoders::ListEncoders(QWidget *parent)
  : RDDialog(parent)
{
  //
  // Basic Window Attributes
  //
  setMinimumSize(sizeHint());
  setWindowTitle("RDAdmin - "+tr("Encoder Profiles"));

  //
  // Dialogs
  //
  c_settings_dialog=new RDExportSettingsDialog("RDAdmin",this);
  c_settings_dialog->setShowNormalizationLevel(true);
  c_settings_dialog->setShowAutotrimLevel(true);

  c_list_label=new QLabel(tr("Encoder Profiles"),this);
  c_list_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  c_list_label->setFont(bigLabelFont());

  c_list=new QListWidget(this);
  c_list->setSelectionMode(QAbstractItemView::SingleSelection);
  connect(c_list,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));
  RefreshList();

  c_add_button=new QPushButton(tr("Add"),this);
  c_add_button->setFont(buttonFont());
  connect(c_add_button,SIGNAL(clicked()),this,SLOT(addData()));

  c_edit_button=new QPushButton(tr("Edit"),this);
  c_edit_button->setFont(buttonFont());
  connect(c_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  c_delete_button=new QPushButton(tr("Delete"),this);
  c_delete_button->setFont(buttonFont());
  connect(c_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  c_close_button=new QPushButton(tr("Close"),this);
  c_close_button->setFont(buttonFont());
  connect(c_close_button,SIGNAL(clicked()),this,SLOT(closeData()));
}


ListEncoders::~ListEncoders()
{
  delete c_list;
  delete c_list_label;
  delete c_settings_dialog;
}


QSize ListEncoders::sizeHint() const
{
  return QSize(400,300);
}


int ListEncoders::exec()
{
  c_list->clear();
  RefreshList();

  return RDDialog::exec();
}
  

void ListEncoders::addData()
{
  unsigned id=0;
  RDSettings *s=new RDSettings();

  if((id=s->addPreset())>0) {
    if(c_settings_dialog->exec(s,id)) {
      s->savePreset(id);
      QListWidgetItem *item=new QListWidgetItem(c_list);
      item->setData(Qt::UserRole,id);
      item->setData(Qt::DisplayRole,s->name());
    }
    else {
      s->deletePreset(id);
    }
  }
  delete s;
}


void ListEncoders::editData()
{
  QList<QListWidgetItem *> rows=c_list->selectedItems();

  if(rows.size()!=1) {
    return;
  }
  QListWidgetItem *item=rows.first();
  if(item!=NULL) {
    RDSettings *s=new RDSettings();
    if(s->loadPreset(item->data(Qt::UserRole).toInt())) {
      if(c_settings_dialog->exec(s,item->data(Qt::UserRole).toInt())) {
	  s->savePreset(item->data(Qt::UserRole).toInt());
	  item->setData(Qt::DisplayRole,s->name());
      }
    }
    delete s;
  }
}


void ListEncoders::deleteData()
{
  QList<QListWidgetItem *> rows=c_list->selectedItems();

  if(rows.size()!=1) {
    return;
  }
  QListWidgetItem *item=rows.first();
  if(item!=NULL) {
    RDSettings *s=new RDSettings();
    if(s->loadPreset(item->data(Qt::UserRole).toInt())) {
      if(QMessageBox::question(this,"RDAdmin - "+tr("Delete Preset"),
			       tr("Are you sure that you want to delete preset")+
			       "\""+s->name()+"\"?",QMessageBox::Yes,QMessageBox::No)==QMessageBox::Yes) {
	s->deletePreset(item->data(Qt::UserRole).toInt());
	delete item;
      }
      delete s;
    }
  }
}


void ListEncoders::doubleClickedData(const QModelIndex &)
{
  editData();
}


void ListEncoders::closeData()
{
  done(true);
}


void ListEncoders::resizeEvent(QResizeEvent *e)
{
  int w=size().width();
  int h=size().height();

  c_list_label->setGeometry(10,2,w-20,20);
  c_list->setGeometry(10,24,w-20,h-94);

  c_add_button->setGeometry(10,h-60,80,50);
  c_edit_button->setGeometry(100,h-60,80,50);
  c_delete_button->setGeometry(190,h-60,80,50);
  c_close_button->setGeometry(w-90,h-60,80,50);
}


void ListEncoders::RefreshList()
{
  QString sql;
  RDSqlQuery *q;
  QListWidgetItem *item;

  c_list->clear();
  sql=QString("select ")+
    "`ID`,"+    // 00
    "`NAME` "+  // 01
    "from `ENCODER_PRESETS` order by `NAME`";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    item=new QListWidgetItem(c_list);
    item->setData(Qt::UserRole,q->value(0).toInt());
    item->setData(Qt::DisplayRole,q->value(1).toString());
  }
  delete q;
}
