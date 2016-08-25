// list_svcs.cpp
//
// List Rivendell Services
//
//   (C) Copyright 2002-2003,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <QLabel>
#include <QMessageBox>
#include <QPushButton>

#include <rdapplication.h>
#include <rdescape_string.h>

#include "add_svc.h"
#include "edit_svc.h"
#include "list_svcs.h"

ListSvcs::ListSvcs(QWidget *parent)
  : QDialog(parent)
{
  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  setCaption(tr("Services"));

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
  // Services List Box
  //
  list_model=new RDSqlTableModel(this);
  list_model->setQuery("select NAME from SERVICES order by NAME");
  list_model->setHeaderData(0,Qt::Horizontal,tr("Name"));
  list_view=new QListView(this);
  list_view->setModel(list_model);
  list_view->show();
  connect(list_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));
  QLabel *list_box_label=new QLabel(tr("Services:"),this);
  list_box_label->setFont(font);
  list_box_label->setGeometry(14,11,85,19);
}


ListSvcs::~ListSvcs()
{
  //  delete list_box;
}


QSize ListSvcs::sizeHint() const
{
  return QSize(400,300);
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
  list_model->update();
}


void ListSvcs::editData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    doubleClickedData(s->selectedRows()[0]);
  }
}


void ListSvcs::deleteData()
{
  QString sql;
  RDSqlQuery *q;

  QItemSelectionModel *s=list_view->selectionModel();
  QString svcname=s->selectedRows()[0].data().toString();
  if(s->hasSelection()) {
    if(QMessageBox::warning(this,tr("Delete Service"),
			    tr("Are you sure you want to delete service")+
			    " \""+svcname+"\"?",
			    QMessageBox::Yes,QMessageBox::No)!=
       QMessageBox::Yes) {
      return;
    }
    sql=QString("select NAME from LOGS where ")+
      "SERVICE=\""+RDEscapeString(svcname)+"\"";
    q=new RDSqlQuery(sql);
    if(q->first()) {
      if(QMessageBox::
	 warning(this,tr("Logs Exist"),
		 tr("There are")+
		 QString().sprintf(" %d ",q->size())+
		 tr("logs owned by this service that will also be deleted.")+
		 "\n"+tr("Do you still want to proceed?"),
		 QMessageBox::Yes,QMessageBox::No)!=
	 QMessageBox::Yes) {
	delete q;
	return;
      }
    }
    delete q;
    RDSvc *svc=new RDSvc(svcname);
    svc->remove();
    delete svc;
    list_model->update();
  }
}


void ListSvcs::closeData()
{
  done(0);
}


void ListSvcs::resizeEvent(QResizeEvent *e)
{
  list_view->setGeometry(10,30,size().width()-110,size().height()-40);
  list_add_button->setGeometry(size().width()-90,30,80,50);
  list_edit_button->setGeometry(size().width()-90,90,80,50);
  list_delete_button->setGeometry(size().width()-90,150,80,50);
  list_close_button->setGeometry(size().width()-90,240,80,50);
}


void ListSvcs::doubleClickedData(const QModelIndex &index)
{
  EditSvc *edit_svc=new EditSvc(index.data().toString(),this);
  edit_svc->exec();
  delete edit_svc;
}
