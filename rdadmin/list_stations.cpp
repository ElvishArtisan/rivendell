// list_stations.cpp
//
// List Rivendell Workstations
//
//   (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
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

#include <qmessagebox.h>

#include <rdairplay_conf.h>
#include <rddb.h>
#include <rdescape_string.h>

#include "add_station.h"
#include "edit_station.h"
#include "list_stations.h"

ListStations::ListStations(QWidget *parent)
  : RDDialog(parent)
{
  setModal(true);

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  setWindowTitle("RDAdmin - "+tr("Rivendell Host List"));

  //
  //  Add Button
  //
  list_add_button=new QPushButton(this);
  list_add_button->setFont(buttonFont());
  list_add_button->setText(tr("&Add"));
  connect(list_add_button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  //  Edit Button
  //
  list_edit_button=new QPushButton(this);
  list_edit_button->setFont(buttonFont());
  list_edit_button->setText(tr("&Edit"));
  connect(list_edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Delete Button
  //
  list_delete_button=new QPushButton(this);
  list_delete_button->setFont(buttonFont());
  list_delete_button->setText(tr("&Delete"));
  connect(list_delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  //  Close Button
  //
  list_close_button=new QPushButton(this);
  list_close_button->setDefault(true);
  list_close_button->setFont(buttonFont());
  list_close_button->setText(tr("&Close"));
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  // Station List Box
  //
  list_box=new Q3ListBox(this);
  QLabel *list_box_label=new QLabel(list_box,tr("Ho&sts:"),this);
  list_box_label->setFont(labelFont());
  list_box_label->setGeometry(14,10,85,19);
  connect(list_box,SIGNAL(doubleClicked(Q3ListBoxItem *)),
	  this,SLOT(doubleClickedData(Q3ListBoxItem *)));

  RefreshList();
}


ListStations::~ListStations()
{
  delete list_box;
}


QSize ListStations::sizeHint() const
{
  return QSize(500,300);
} 


QSizePolicy ListStations::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListStations::addData()
{
  QString stationname;

  AddStation *add_station=new AddStation(&stationname,this);
  if(add_station->exec()<0) {
    RDStation::remove(stationname);
    delete add_station;
    return;
  }
  delete add_station;
  RefreshList(stationname);
}


void ListStations::editData()
{
  if(list_box->currentItem()<0) {
    return;
  }
  EditStation *edit_station=new EditStation(list_box->currentText(),this);
  edit_station->exec();
  delete edit_station;
}


void ListStations::deleteData()
{
  if(QMessageBox::warning(this,"RDAdmin - "+tr("Delete Station"),
			    tr("Are you sure you want to delete host")+
			    " \""+list_box->currentText()+"\"?",
			  QMessageBox::Yes,QMessageBox::No)==QMessageBox::Yes) {
    RDStation::remove(list_box->currentText());
    list_box->removeItem(list_box->currentItem());
    if(list_box->currentItem()>=0) {
      list_box->setSelected(list_box->currentItem(),true);
    }
  }
}


void ListStations::closeData()
{
  done(0);
}


void ListStations::doubleClickedData(Q3ListBoxItem *item)
{
  editData();
}


void ListStations::resizeEvent(QResizeEvent *e)
{
  list_add_button->setGeometry(size().width()-90,30,80,50);
  list_edit_button->setGeometry(size().width()-90,90,80,50);
  list_delete_button->setGeometry(size().width()-90,150,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
  list_box->setGeometry(10,30,size().width()-110,size().height()-40);
}


void ListStations::RefreshList(QString stationname)
{
  QString sql;
  RDSqlQuery *q;

  list_box->clear();
  q=new RDSqlQuery("select NAME from STATIONS");
  while (q->next()) {
    list_box->insertItem(q->value(0).toString());
    if(stationname==list_box->text(list_box->count()-1)) {
      list_box->setCurrentItem(list_box->count()-1);
    }
  }
  delete q;
}
