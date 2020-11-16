// list_encoders.cpp
//
// List Rivendell Encoder Profiles
//
//   (C) Copyright 2020 Fred Gleason <fredg@paravelsystems.com>
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

  c_list=new RDListView(this);
  c_list->setAllColumnsShowFocus(true);
  c_list->setItemMargin(5);

  c_list->addColumn("Name");
  c_list->setColumnAlignment(0,Qt::AlignLeft);
  connect(c_list,
	  SIGNAL(doubleClicked(Q3ListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(doubleClickedData(Q3ListViewItem *,const QPoint &,int)));
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
      RDListViewItem *item=new RDListViewItem(c_list);
      item->setId(id);
      item->setText(0,s->name());
    }
    else {
      s->deletePreset(id);
    }
  }
  delete s;
}


void ListEncoders::editData()
{
  RDListViewItem *item=(RDListViewItem *)c_list->selectedItem();
  if(item!=NULL) {
    RDSettings *s=new RDSettings();
    if(s->loadPreset(item->id())) {
      if(c_settings_dialog->exec(s,item->id())) {
	s->savePreset(item->id());
	item->setText(0,s->name());
      }
    }
    delete s;
  }
}


void ListEncoders::deleteData()
{
  RDListViewItem *item=(RDListViewItem *)c_list->selectedItem();
  if(item!=NULL) {
    RDSettings *s=new RDSettings();
    if(s->loadPreset(item->id())) {
      if(QMessageBox::question(this,"RDAdmin - "+tr("Delete Preset"),
			       tr("Are you sure that you want to delete preset")+
			       "\""+s->name()+"\"?",QMessageBox::Yes,QMessageBox::No)==QMessageBox::Yes) {
	s->deletePreset(item->id());
	delete item;
      }
      delete s;
    }
  }
}


void ListEncoders::doubleClickedData(Q3ListViewItem *item,const QPoint &pt,
				     int col)
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
  RDListViewItem *item;

  c_list->clear();
  sql=QString("select ")+
    "ID,"+    // 00
    "NAME "+  // 01
    "from ENCODER_PRESETS order by NAME";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    item=new RDListViewItem(c_list);
    item->setId(q->value(0).toInt());
    item->setText(0,q->value(1).toString());
  }
  delete q;
}
