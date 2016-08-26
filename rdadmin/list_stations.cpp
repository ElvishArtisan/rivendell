// list_stations.cpp
//
// List Rivendell Workstations
//
//   (C) Copyright 2002-2016 Fred Gleason <fredg@paravelsystems.com>
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

#include "add_station.h"
#include "edit_station.h"
#include "list_stations.h"

ListStations::ListStations(QWidget *parent)
  : QDialog(parent)
{
  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  setWindowTitle("RDAdmin - "+tr("Rivendell Workstation List"));

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);
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
  // Station List
  //
  list_model=new RDSqlTableModel(this);
  list_model->setQuery("select NAME from STATIONS order by NAME");
  list_model->setHeaderData(0,Qt::Horizontal,tr("Name"));
  list_view=new QListView(this);
  list_view->setModel(list_model);
  list_view->show();
  connect(list_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));
  list_view_label=new QLabel(tr("Ho&sts:"),this);
  list_view_label->setFont(font);
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
  list_model->update();
}


void ListStations::editData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    doubleClickedData(s->selectedRows()[0]);
  }
}


void ListStations::deleteData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    if(QMessageBox::question(this,"RDAdmin - "+tr("Delete Host"),
			     tr("Are you sure you want to delete host")+
			     " \""+
			     s->selectedRows()[0].data().toString()+
			     "\"?",QMessageBox::Yes,QMessageBox::No)!=
       QMessageBox::Yes) {
      return;
    }
    RDStation::remove(s->selectedRows()[0].data().toString());
    list_model->update();
  }
}


void ListStations::closeData()
{
  done(0);
}


void ListStations::resizeEvent(QResizeEvent *e)
{
  list_view->setGeometry(10,30,size().width()-110,size().height()-40);
  list_view_label->setGeometry(14,10,85,22);
  list_add_button->setGeometry(size().width()-90,30,80,50);
  list_edit_button->setGeometry(size().width()-90,90,80,50);
  list_delete_button->setGeometry(size().width()-90,150,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void ListStations::doubleClickedData(const QModelIndex &index)
{
  EditStation *edit_station=new EditStation(index.data().toString(),this);
  edit_station->exec();
  delete edit_station;
}
