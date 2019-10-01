// list_svcs.cpp
//
// List Rivendell Services
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

#include <qdialog.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <q3listbox.h>
#include <q3textedit.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <q3buttongroup.h>

#include <rdapplication.h>
#include <rddb.h>
#include <rdescape_string.h>

#include "add_svc.h"
#include "edit_svc.h"
#include "globals.h"
#include "list_svcs.h"

ListSvcs::ListSvcs(QWidget *parent)
  : RDDialog(parent)
{
  setModal(true);

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  setWindowTitle("RDAdmin - "+tr("Services"));

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
  // Services List Box
  //
  list_box=new Q3ListBox(this);
  list_title_label=new QLabel(list_box,tr("&Services:"),this);
  list_title_label->setFont(labelFont());
  list_title_label->setGeometry(14,11,85,19);
  connect(list_box,SIGNAL(doubleClicked(Q3ListBoxItem *)),
	  this,SLOT(doubleClickedData(Q3ListBoxItem *)));

  RefreshList();
}


ListSvcs::~ListSvcs()
{
  delete list_box;
}


QSize ListSvcs::sizeHint() const
{
  return QSize(500,300);
} 


QSizePolicy ListSvcs::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListSvcs::addData()
{
  QString svcname;

  AddSvc *add_svc=new AddSvc(&svcname,this);
  if(add_svc->exec()<0) {
    delete add_svc;
    return;
  }
  delete add_svc;
  RefreshList(svcname);
}


void ListSvcs::editData()
{
  if(list_box->currentItem()<0) {
    return;
  }
  EditSvc *edit_svc=new EditSvc(list_box->currentText(),this);
  edit_svc->exec();
  delete edit_svc;
}


void ListSvcs::deleteData()
{
  QString sql;
  RDSqlQuery *q;

  if(QMessageBox::warning(this,"RDAdmin- "+tr("Delete Service"),
			  tr("Are you sure you want to delete service")+
			  " \""+list_box->currentText()+"\"?",
			  QMessageBox::Yes,QMessageBox::No)!=QMessageBox::Yes) {
    return;
  }
  sql=QString("select NAME from LOGS where ")+
    "SERVICE=\""+RDEscapeString(list_box->currentText())+"\"";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    if(QMessageBox::warning(this,"RDAdmin - "+tr("Logs Exist"),
			    tr("There are")+QString().sprintf(" %d ",q->size())+
			    tr("logs owned by this service that will also be deleted.")+"\n"+tr("Do you still want to proceed?"),
			    QMessageBox::Yes,QMessageBox::No)!=
       QMessageBox::Yes) {
      delete q;
      return;
    }
  }
  delete q;
  RDSvc *svc=new RDSvc(list_box->currentText(),rda->station(),rda->config());
  svc->remove();
  delete svc;
  list_box->removeItem(list_box->currentItem());
  if(list_box->currentItem()>=0) {
    list_box->setSelected(list_box->currentItem(),true);
  }
}


void ListSvcs::closeData()
{
  done(0);
}


void ListSvcs::doubleClickedData(Q3ListBoxItem *item)
{
  editData();
}


void ListSvcs::resizeEvent(QResizeEvent *e)
{
  list_add_button->setGeometry(size().width()-90,30,80,50);
  list_edit_button->setGeometry(size().width()-90,90,80,50);
  list_delete_button->setGeometry(size().width()-90,150,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
  list_box->setGeometry(10,30,size().width()-110,size().height()-40);
}


void ListSvcs::RefreshList(QString svcname)
{
  QString sql;
  RDSqlQuery *q;

  list_box->clear();
  q=new RDSqlQuery("select NAME from SERVICES");
  while (q->next()) {
    list_box->insertItem(q->value(0).toString());
    if(svcname==list_box->text(list_box->count()-1)) {
      list_box->setCurrentItem(list_box->count()-1);
    }
  }
  delete q;
}
