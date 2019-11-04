// list_hostvars.cpp
//
// List Rivendell Host Variables
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

#include <rddb.h>
#include <rdescape_string.h>

#include "add_hostvar.h"
#include "edit_hostvar.h"
#include "globals.h"
#include "list_hostvars.h"

ListHostvars::ListHostvars(QString station,QWidget *parent)
  : RDDialog(parent)
{
  setModal(true);

  QString str;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  list_station=station;
  str=QString(tr("Host Variables for"));
  setWindowTitle("RDAdmin - "+tr("Host Variables for")+" "+station);

  //
  // Matrix List Box
  //
  list_view=new Q3ListView(this);
  list_title_label=new QLabel(list_view,tr("Host Variables"),this);
  list_title_label->setFont(labelFont());
  list_title_label->setGeometry(14,5,sizeHint().width()-28,19);
  list_view->setAllColumnsShowFocus(true);
  list_view->setItemMargin(5);
  list_view->addColumn(tr("NAME"));
  list_view->setColumnAlignment(0,Qt::AlignHCenter);
  list_view->addColumn(tr("VALUE"));
  list_view->setColumnAlignment(1,Qt::AlignLeft);
  list_view->addColumn(tr("REMARK"));
  list_view->setColumnAlignment(2,Qt::AlignLeft);
  connect(list_view,SIGNAL(doubleClicked(Q3ListViewItem *,const QPoint &,int)),
	  this,SLOT(doubleClickedData(Q3ListViewItem *,const QPoint &,int)));

  RefreshList();

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
  //  OK Button
  //
  list_ok_button=new QPushButton(this);
  list_ok_button->setDefault(true);
  list_ok_button->setFont(buttonFont());
  list_ok_button->setText(tr("&OK"));
  connect(list_ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  list_cancel_button=new QPushButton(this);
  list_cancel_button->setDefault(true);
  list_cancel_button->setFont(buttonFont());
  list_cancel_button->setText(tr("&Cancel"));
  connect(list_cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));
}


ListHostvars::~ListHostvars()
{
  delete list_view;
}


QSize ListHostvars::sizeHint() const
{
  return QSize(490,320);
} 


QSizePolicy ListHostvars::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListHostvars::addData()
{
  QString varname;
  QString varvalue;
  QString varremark;
  AddHostvar *var_dialog=
    new AddHostvar(list_station,&varname,&varvalue,&varremark,this);
  if(var_dialog->exec()==0) {
    Q3ListViewItem *item=new Q3ListViewItem(list_view);
    item->setText(0,varname);
    item->setText(1,varvalue);
    item->setText(2,varremark);
  }
  delete var_dialog;
}


void ListHostvars::editData()
{
  Q3ListViewItem *item=list_view->selectedItem();
  if(item==NULL) {
    return;
  }
  QString varvalue=item->text(1);
  QString varremark=item->text(2);
  EditHostvar *var_dialog=
    new EditHostvar(list_station,item->text(0),&varvalue,&varremark,this);
  if(var_dialog->exec()==0) {
    item->setText(1,varvalue);
    item->setText(2,varremark);
  }
  delete var_dialog;
}


void ListHostvars::deleteData()
{
  Q3ListViewItem *item=list_view->selectedItem();
  if(item==NULL) {
    return;
  }
  if(QMessageBox::question(this,"RDAdmin - "+tr("Delete Host Variable"),
			   tr("Are you sure you want to delete the variable")+
			   " \""+item->text(0)+"\"?",
		      QMessageBox::Yes,QMessageBox::No)!=QMessageBox::Yes) {
    return;
  }
  delete item;
}


void ListHostvars::doubleClickedData(Q3ListViewItem *,const QPoint &,int)
{
  editData();
}


void ListHostvars::okData()
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("delete from HOSTVARS where ")+
    "STATION_NAME=\""+RDEscapeString(list_station)+"\"";
  q=new RDSqlQuery(sql);
  delete q;
  Q3ListViewItem *item=list_view->firstChild();
  while(item!=NULL) {
    sql=QString("insert into HOSTVARS set ")+
      "STATION_NAME=\""+RDEscapeString(list_station)+"\","+
      "NAME=\""+RDEscapeString(item->text(0))+"\","+
      "VARVALUE=\""+RDEscapeString(item->text(1))+"\","+
      "REMARK=\""+RDEscapeString(item->text(2))+"\"";
    q=new RDSqlQuery(sql);
    delete q;
    item=item->nextSibling();
  }
  done(0);
}


void ListHostvars::cancelData()
{
  done(-1);
}


void ListHostvars::resizeEvent(QResizeEvent *e)
{
  list_view->setGeometry(10,24,size().width()-20,size().height()-114);
  list_add_button->setGeometry(10,size().height()-80,80,50);
  list_edit_button->setGeometry(100,size().height()-80,80,50);
  list_delete_button->setGeometry(190,size().height()-80,80,50);
  list_ok_button->setGeometry(size().width()-180,size().height()-60,80,50);
  list_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void ListHostvars::RefreshList()
{
  Q3ListViewItem *l;

  list_view->clear();
  QString sql=QString("select ")+
    "NAME,"+      // 00
    "VARVALUE,"+  // 01
    "REMARK "     // 02
    "from HOSTVARS where "+
    "STATION_NAME=\""+RDEscapeString(list_station)+"\" "+
    "order by NAME";
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    l=new Q3ListViewItem(list_view);
    l->setText(0,q->value(0).toString());
    l->setText(1,q->value(1).toString());
    l->setText(2,q->value(2).toString());
  }
  delete q;
}
