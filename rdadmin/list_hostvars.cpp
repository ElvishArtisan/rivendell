// list_hostvars.cpp
//
// List Rivendell Host Variables
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_hostvars.cpp,v 1.12 2010/07/29 19:32:35 cvs Exp $
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
#include <qlistbox.h>
#include <qtextedit.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qbuttongroup.h>

#include <rdstation.h>
#include <rddb.h>
#include <globals.h>
#include <list_hostvars.h>
#include <add_hostvar.h>
#include <edit_hostvar.h>


ListHostvars::ListHostvars(QString station,QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  QString str;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  list_station=station;
  str=QString(tr("Host Variables for"));
  setCaption(QString().sprintf("%s %s",(const char *)str,
			       (const char *)station.upper()));

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);
  QFont small_font=QFont("Helvetica",10,QFont::Bold);
  small_font.setPixelSize(10);

  //
  // Matrix List Box
  //
  list_view=new QListView(this,"list_box");
  list_view->setGeometry(10,24,sizeHint().width()-20,sizeHint().height()-114);
  QLabel *label=new QLabel(list_view,tr("Host Variables"),
			   this,"list_view_label");
  label->setFont(font);
  label->setGeometry(14,5,sizeHint().width()-28,19);
  list_view->setAllColumnsShowFocus(true);
  list_view->setItemMargin(5);
  list_view->addColumn(tr("NAME"));
  list_view->setColumnAlignment(0,Qt::AlignHCenter);
  list_view->addColumn(tr("VALUE"));
  list_view->setColumnAlignment(1,Qt::AlignLeft);
  list_view->addColumn(tr("REMARK"));
  list_view->setColumnAlignment(2,Qt::AlignLeft);
  connect(list_view,SIGNAL(doubleClicked(QListViewItem *,const QPoint &,int)),
	  this,SLOT(doubleClickedData(QListViewItem *,const QPoint &,int)));

  RefreshList();

  //
  //  Add Button
  //
  QPushButton *add_button=new QPushButton(this,"add_button");
  add_button->setGeometry(10,sizeHint().height()-80,80,50);
  add_button->setFont(font);
  add_button->setText(tr("&Add"));
  connect(add_button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  //  Edit Button
  //
  QPushButton *edit_button=new QPushButton(this,"edit_button");
  edit_button->setGeometry(100,sizeHint().height()-80,80,50);
  edit_button->setFont(font);
  edit_button->setText(tr("&Edit"));
  connect(edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Delete Button
  //
  QPushButton *delete_button=new QPushButton(this,"delete_button");
  delete_button->setGeometry(190,sizeHint().height()-80,80,50);
  delete_button->setFont(font);
  delete_button->setText(tr("&Delete"));
  connect(delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  //  OK Button
  //
  QPushButton *button=new QPushButton(this,"ok_button");
  button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,
			    80,50);
  button->setDefault(true);
  button->setFont(font);
  button->setText(tr("&OK"));
  connect(button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  button=new QPushButton(this,"cancel_button");
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			    80,50);
  button->setDefault(true);
  button->setFont(font);
  button->setText(tr("&Cancel"));
  connect(button,SIGNAL(clicked()),this,SLOT(cancelData()));
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
    new AddHostvar(list_station,&varname,&varvalue,&varremark,
		   this,"var_dialog");
  if(var_dialog->exec()==0) {
    QListViewItem *item=new QListViewItem(list_view);
    item->setText(0,varname);
    item->setText(1,varvalue);
    item->setText(2,varremark);
  }
  delete var_dialog;
}


void ListHostvars::editData()
{
  QListViewItem *item=list_view->selectedItem();
  if(item==NULL) {
    return;
  }
  QString varvalue=item->text(1);
  QString varremark=item->text(2);
  EditHostvar *var_dialog=
    new EditHostvar(list_station,item->text(0),&varvalue,&varremark,
		   this,"var_dialog");
  if(var_dialog->exec()==0) {
    item->setText(1,varvalue);
    item->setText(2,varremark);
  }
  delete var_dialog;
}


void ListHostvars::deleteData()
{
  QListViewItem *item=list_view->selectedItem();
  if(item==NULL) {
    return;
  }
  if(QMessageBox::question(this,"Delete Host Variable",
    QString().sprintf("Are you sure you want to delete the variable %s?",
		      (const char *)item->text(0)),
		      QMessageBox::Yes,QMessageBox::No)!=QMessageBox::Yes) {
    return;
  }
  delete item;
}


void ListHostvars::doubleClickedData(QListViewItem *,const QPoint &,int)
{
  editData();
}


void ListHostvars::okData()
{
  QString sql;
  RDSqlQuery *q;

  sql=QString().sprintf("delete from HOSTVARS where STATION_NAME=\"%s\"",
			(const char *)list_station);
  q=new RDSqlQuery(sql);
  delete q;
  QListViewItem *item=list_view->firstChild();
  while(item!=NULL) {
    sql=QString().sprintf("insert into HOSTVARS set STATION_NAME=\"%s\",\
                           NAME=\"%s\",VARVALUE=\"%s\",REMARK=\"%s\"",
			  (const char *)list_station,
			  (const char *)item->text(0),
			  (const char *)item->text(1),
			  (const char *)item->text(2));
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


void ListHostvars::RefreshList()
{
  QListViewItem *l;

  list_view->clear();
  QString sql=QString().sprintf("select NAME,VARVALUE,REMARK from HOSTVARS \
                                 where STATION_NAME=\"%s\" order by NAME",
				(const char *)list_station);
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    l=new QListViewItem(list_view);
    l->setText(0,q->value(0).toString());
    l->setText(1,q->value(1).toString());
    l->setText(2,q->value(2).toString());
  }
  delete q;
}
