// list_reports.cpp
//
// List Rivendell Reports
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

#include "add_report.h"
#include "edit_report.h"
#include "list_reports.h"

ListReports::ListReports(QWidget *parent)
  : RDDialog(parent)
{
  setModal(true);

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  setWindowTitle("RDAdmin - "+tr("Rivendell Report List"));

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
  // Report List Box
  //
  list_box=new Q3ListBox(this);
  QLabel *list_box_label=new QLabel(list_box,tr("R&eports:"),this);
  list_box_label->setFont(labelFont());
  list_box_label->setGeometry(14,10,85,19);
  connect(list_box,SIGNAL(doubleClicked(Q3ListBoxItem *)),
	  this,SLOT(doubleClickedData(Q3ListBoxItem *)));

  RefreshList();
}


ListReports::~ListReports()
{
}


QSize ListReports::sizeHint() const
{
  return QSize(500,300);
} 


QSizePolicy ListReports::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListReports::addData()
{
  QString rptname;

  AddReport *add_report=new AddReport(&rptname,this);
  if(add_report->exec()<0) {
    delete add_report;
    return;
  }
  delete add_report;
  EditReport *edit_report=new EditReport(rptname,this);
  if(edit_report->exec()<0) {
    DeleteReport(rptname);
    delete edit_report;
    return;
  }
  delete edit_report;
  RefreshList(rptname);
}


void ListReports::editData()
{
  if(list_box->currentItem()<0) {
    return;
  }
  EditReport *edit_report=new EditReport(list_box->currentText(),this);
  edit_report->exec();
  delete edit_report;
}


void ListReports::deleteData()
{
  if(list_box->currentText().isEmpty()) {
    return;
  }
  if(QMessageBox::warning(this,"RDAdmin - "+tr("Delete Report"),
			  tr("Are you sure you want to delete report")+
			  " \""+list_box->currentText()+"\"?",
			  QMessageBox::Yes,QMessageBox::No)==
     QMessageBox::Yes) {
    DeleteReport(list_box->currentText());

    list_box->removeItem(list_box->currentItem());
    if(list_box->currentItem()>=0) {
      list_box->setSelected(list_box->currentItem(),true);
    }
  }
}


void ListReports::closeData()
{
  done(0);
}


void ListReports::doubleClickedData(Q3ListBoxItem *item)
{
  editData();
}


void ListReports::resizeEvent(QResizeEvent *e)
{
  list_add_button->setGeometry(size().width()-90,30,80,50);
  list_edit_button->setGeometry(size().width()-90,90,80,50);
  list_delete_button->setGeometry(size().width()-90,150,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
  list_box->setGeometry(10,30,size().width()-110,size().height()-40);
}


void ListReports::DeleteReport(QString rptname)
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("delete from REPORTS where ")+
    "NAME=\""+RDEscapeString(rptname)+"\"";
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString("delete from REPORT_SERVICES where ")+
    "REPORT_NAME=\""+RDEscapeString(rptname)+"\"";
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString("delete from REPORT_STATIONS where ")+
    "REPORT_NAME=\""+RDEscapeString(rptname)+"\"";
  q=new RDSqlQuery(sql);
  delete q;
}


void ListReports::RefreshList(QString rptname)
{
  QString sql;
  RDSqlQuery *q;

  list_box->clear();
  q=new RDSqlQuery("select NAME from REPORTS");
  while (q->next()) {
    list_box->insertItem(q->value(0).toString());
    if(rptname==list_box->text(list_box->count()-1)) {
      list_box->setCurrentItem(list_box->count()-1);
    }
  }
  delete q;
}
