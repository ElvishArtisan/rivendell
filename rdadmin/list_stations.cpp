// list_stations.cpp
//
// List Rivendell Workstations
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

#include <rdairplay_conf.h>
#include <rddb.h>
#include <rdescape_string.h>

#include "add_station.h"
#include "edit_station.h"
#include "list_stations.h"

ListStations::ListStations(QWidget *parent)
  : RDDialog(parent)
{
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
  list_stations_view=new QTableView(this);
  list_title_label=new QLabel(list_stations_view,tr("H&osts:"),this);
  list_title_label->setFont(labelFont());
  list_title_label->setGeometry(14,11,85,19);
  list_stations_view->setSelectionBehavior(QAbstractItemView::SelectRows);
  list_stations_view->setSelectionMode(QAbstractItemView::SingleSelection);
  list_stations_view->setShowGrid(false);
  list_stations_view->setSortingEnabled(false);
  list_stations_view->setWordWrap(false);
  list_stations_model=new RDStationListModel(this);
  list_stations_model->setFont(defaultFont());
  list_stations_model->setPalette(palette());
  list_stations_view->setModel(list_stations_model);
  connect(list_stations_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));
  connect(list_stations_model,SIGNAL(modelReset()),
	  list_stations_view,SLOT(resizeColumnsToContents()));
  list_stations_view->resizeColumnsToContents();
  /*
  list_box=new Q3ListBox(this);
  QLabel *list_box_label=new QLabel(list_box,tr("Ho&sts:"),this);
  list_box_label->setFont(labelFont());
  list_box_label->setGeometry(14,10,85,19);
  connect(list_box,SIGNAL(doubleClicked(Q3ListBoxItem *)),
	  this,SLOT(doubleClickedData(Q3ListBoxItem *)));

  RefreshList();
  */
}


ListStations::~ListStations()
{
  delete list_stations_view;
  delete list_stations_model;
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
  QString hostname;

  AddStation *d=new AddStation(&hostname,this);
  if(d->exec()) {
    QModelIndex row=list_stations_model->addStation(hostname);
    if(row.isValid()) {
      list_stations_view->selectRow(row.row());
    }
  }
  else {
    RDStation::remove(hostname);
  }
  delete d;
}


void ListStations::editData()
{
  QModelIndexList rows=list_stations_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }

  EditStation *d=
    new EditStation(list_stations_model->stationName(rows.first()),this);
  if(d->exec()) {
    list_stations_model->refresh(rows.first());
  }
  delete d;
}


void ListStations::deleteData()
{
  QModelIndexList rows=list_stations_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  QString hostname=list_stations_model->stationName(rows.first());
  if(QMessageBox::warning(this,"RDAdmin - "+tr("Delete Station"),
			  tr("Are you sure you want to delete host")+
			  " \""+hostname+"\"?",
			  QMessageBox::Yes,QMessageBox::No)==
     QMessageBox::Yes) {
    RDStation::remove(hostname);
    list_stations_model->removeStation(hostname);
  }
}


void ListStations::closeData()
{
  done(true);
}


void ListStations::doubleClickedData(const QModelIndex &index)
{
  editData();
}


void ListStations::resizeEvent(QResizeEvent *e)
{
  list_add_button->setGeometry(size().width()-90,30,80,50);
  list_edit_button->setGeometry(size().width()-90,90,80,50);
  list_delete_button->setGeometry(size().width()-90,150,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
  list_stations_view->setGeometry(10,30,size().width()-110,size().height()-40);
}

/*
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
*/
