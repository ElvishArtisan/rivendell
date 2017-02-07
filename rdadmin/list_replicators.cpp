// list_replicators.cpp
//
// List Rivendell Replication Configuratins
//
//   (C) Copyright 2002-2008,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <math.h>

#include <QAbstractItemView>
#include <QHeaderView>
#include <QMessageBox>
#include <QResizeEvent>

#include <rdcart.h>
#include <rdescape_string.h>
#include <rdreplicator.h>
#include <rdtextfile.h>

#include "add_replicator.h"
#include "edit_replicator.h"
#include "list_replicators.h"
#include "list_replicator_carts.h"

ListReplicators::ListReplicators(QWidget *parent)
  : QDialog(parent)
{
  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  setWindowTitle("RDAdmin - "+tr("Rivendell Replicators"));

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
  //  List Carts Button
  //
  list_list_button=new QPushButton(this);
  list_list_button->setFont(font);
  list_list_button->setText(tr("&List\nCarts"));
  connect(list_list_button,SIGNAL(clicked()),this,SLOT(listData()));

  //
  //  Close Button
  //
  list_close_button=new QPushButton(this);
  list_close_button->setDefault(true);
  list_close_button->setFont(font);
  list_close_button->setText(tr("&Close"));
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  // Replicator List
  //
  list_replicators_view=new RDTableWidget(this);
  list_replicators_view->setFont(list_font);
  list_replicators_view->setSelectionBehavior(QAbstractItemView::SelectRows);
  list_replicators_view->setColumnCount(4);
  QStringList headings={tr("NAME"),tr("TYPE"),tr("DESCRIPTION"),tr("HOST")};
  list_replicators_view->setHorizontalHeaderLabels(headings);
  list_replicators_view->verticalHeader()->setVisible(false);
  list_replicators_view->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  QLabel *list_box_label=
    new QLabel(list_replicators_view,tr("&Replicators:"),this);
  list_box_label->setFont(font);
  list_box_label->setGeometry(14,11,85,19);
  connect(list_replicators_view,
	  SIGNAL(itemDoubleClicked(QTableWidgetItem *)),
	  this,
	  SLOT(doubleClickedData(QTableWidgetItem *)));

  RefreshList();
}


ListReplicators::~ListReplicators()
{
}


QSize ListReplicators::sizeHint() const
{
  return QSize(640,480);
} 


QSizePolicy ListReplicators::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListReplicators::addData()
{
  QString name;

  AddReplicator *d=new AddReplicator(&name,this);
  if(d->exec()<0) {
    delete d;
    return;
  }
  delete d;
  QTableWidgetItem *item=new QTableWidgetItem(name);
  int newRow=list_replicators_view->rowCount();
  list_replicators_view->insertRow(newRow);
  list_replicators_view->setItem(newRow,0,item);
  RefreshItem(item);
  item->setSelected(true);
  list_replicators_view->setCurrentItem(item);
}


void ListReplicators::editData()
{
  if(list_replicators_view->selectedItems().isEmpty()) {
      return;
  }
  QTableWidgetItem *item=list_replicators_view->selectedItems().at(0);
  EditReplicator *d=new EditReplicator(item->text(),this);
  if(d->exec()==0) {
    RefreshItem(item);
  }
  delete d;
}


void ListReplicators::deleteData()
{
  if(list_replicators_view->selectedItems().isEmpty()) {
      return;
  }
  QTableWidgetItem *item=list_replicators_view->selectedItems().at(0);

  QString sql;
  RDSqlQuery *q;
  QString warning;
  QString str;

  QString name=RDEscapeString(item->text());

  str=QString(tr("Are you sure you want to delete replicator"));
  warning+=QString().sprintf("%s \"%s\"?",(const char *)str,
			     (const char *)item->text());
  switch(QMessageBox::warning(this,tr("Delete Replicator"),warning,
			      QMessageBox::Yes,QMessageBox::No)) {
      case QMessageBox::No:
      case Qt::NoButton:
	return;

      default:
	break;
  }

  //
  // Delete Group Assignments
  //
  sql=QString("delete from REPLICATOR_MAP where ")+
    "REPLICATOR_NAME=\""+RDEscapeString(name)+"\"";
  q=new RDSqlQuery(sql);
  delete q;
  
  //
  // Delete State Records
  //
  sql=QString("delete from REPL_CART_STATE where ")+
    "REPLICATOR_NAME=\""+RDEscapeString(name)+"\"";
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString("delete from REPL_CUT_STATE where ")+
    "REPLICATOR_NAME=\""+RDEscapeString(name)+"\"";
  q=new RDSqlQuery(sql);
  delete q;

  //
  // Delete from Replicator List
  //
  sql=QString("delete from REPLICATORS where ")+
    "NAME=\""+RDEscapeString(name)+"\"";
  q=new RDSqlQuery(sql);
  delete q;
  item->setSelected(false);
  list_replicators_view->removeRow(item->row());
}


void ListReplicators::listData()
{
  if(list_replicators_view->selectedItems().isEmpty()) {
      return;
  }
  QTableWidgetItem *item=list_replicators_view->selectedItems().at(0);
  ListReplicatorCarts *d=new ListReplicatorCarts(this);
  d->exec(item->text());
  delete d;
}


void ListReplicators::doubleClickedData(QTableWidgetItem *item)
{
  editData();
}


void ListReplicators::closeData()
{
  done(0);
}


void ListReplicators::resizeEvent(QResizeEvent *e)
{
  list_add_button->setGeometry(size().width()-90,30,80,50);
  list_edit_button->setGeometry(size().width()-90,90,80,50);
  list_delete_button->setGeometry(size().width()-90,150,80,50);
  list_list_button->setGeometry(size().width()-90,250,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
  list_replicators_view->setGeometry(10,30,size().width()-120,size().height()-40);
}


void ListReplicators::RefreshList()
{
  QString sql;
  RDSqlQuery *q;
  QTableWidgetItem *name;
  QTableWidgetItem *type;
  QTableWidgetItem *description;
  QTableWidgetItem *host;

  list_replicators_view->clear();
  QStringList headings={tr("NAME"),tr("TYPE"),tr("DESCRIPTION"),tr("HOST")};
  list_replicators_view->setHorizontalHeaderLabels(headings);
  sql=QString("select ")+
    "NAME,"+
    "TYPE_ID,"+
    "DESCRIPTION,"+
    "STATION_NAME "+
    "from REPLICATORS";
  q=new RDSqlQuery(sql);
  while (q->next()) {
    name=new QTableWidgetItem(q->value(0).toString());
    type=new QTableWidgetItem(
            RDReplicator::typeString((RDReplicator::Type)q->value(1).toUInt()));
    description=new QTableWidgetItem(q->value(2).toString());
    host=new QTableWidgetItem(q->value(3).toString());
    
    int newRow=list_replicators_view->rowCount();
    list_replicators_view->insertRow(newRow);
    list_replicators_view->setItem(newRow,0,name);
    list_replicators_view->setItem(newRow,1,type);
    list_replicators_view->setItem(newRow,2,description);
    list_replicators_view->setItem(newRow,3,host);
  }
  delete q;
}


void ListReplicators::RefreshItem(QTableWidgetItem *name)
{
  int row=name->row();
  QString sql;
  RDSqlQuery *q;
  QTableWidgetItem *type=list_replicators_view->item(row,1);
  QTableWidgetItem *description=list_replicators_view->item(row,2);
  QTableWidgetItem *host=list_replicators_view->item(row,3);

  sql=QString("select ")+
    "TYPE_ID,"+
    "DESCRIPTION,"+
    "STATION_NAME "+
    "from REPLICATORS where "+
    "NAME=\""+RDEscapeString(name->text())+"\"";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    if(!type) {
      type=new QTableWidgetItem(
	      RDReplicator::typeString((RDReplicator::Type)q->value(0).toUInt()));
      list_replicators_view->setItem(row,1,type);
    } else {
      type->setText(
	     RDReplicator::typeString((RDReplicator::Type)q->value(0).toUInt()));
    }
    if(!description) {
      description=new QTableWidgetItem(q->value(1).toString());
      list_replicators_view->setItem(row,2,description);
    } else {
      description->setText(q->value(1).toString());
    }
    if(!host) {
      host=new QTableWidgetItem(q->value(2).toString());
      list_replicators_view->setItem(row,3,host);
    } else {
      host->setText(q->value(2).toString());
    }
  }
  delete q;
}
