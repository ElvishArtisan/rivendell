// list_reports.cpp
//
// List Rivendell Reports
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_reports.cpp,v 1.12 2010/07/29 19:32:35 cvs Exp $
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

#include <rddb.h>
#include <list_reports.h>
#include <edit_report.h>
#include <add_report.h>


ListReports::ListReports(QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setCaption(tr("Rivendell Report List"));

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
  QPushButton *add_button=new QPushButton(this,"add_button");
  add_button->setGeometry(410,30,80,50);
  add_button->setFont(font);
  add_button->setText(tr("&Add"));
  connect(add_button,SIGNAL(clicked()),this,SLOT(addData()));

  //
  //  Edit Button
  //
  QPushButton *edit_button=new QPushButton(this,"edit_button");
  edit_button->setGeometry(410,90,80,50);
  edit_button->setFont(font);
  edit_button->setText(tr("&Edit"));
  connect(edit_button,SIGNAL(clicked()),this,SLOT(editData()));

  //
  //  Delete Button
  //
  QPushButton *delete_button=new QPushButton(this,"delete_button");
  delete_button->setGeometry(410,150,80,50);
  delete_button->setFont(font);
  delete_button->setText(tr("&Delete"));
  connect(delete_button,SIGNAL(clicked()),this,SLOT(deleteData()));

  //
  //  Close Button
  //
  QPushButton *close_button=new QPushButton(this,"close_button");
  close_button->setGeometry(410,240,80,50);
  close_button->setDefault(true);
  close_button->setFont(font);
  close_button->setText(tr("&Close"));
  connect(close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  //
  // Report List Box
  //
  list_box=new QListBox(this,"list_box");
  list_box->setGeometry(10,30,390,260);
  QLabel *list_box_label=new QLabel(list_box,tr("R&eports:"),
				    this,"list_box_label");
  list_box_label->setFont(font);
  list_box_label->setGeometry(14,10,85,19);
  connect(list_box,SIGNAL(doubleClicked(QListBoxItem *)),
	  this,SLOT(doubleClickedData(QListBoxItem *)));

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

  AddReport *add_report=new AddReport(&rptname,this,"add_report");
  if(add_report->exec()<0) {
    delete add_report;
    return;
  }
  delete add_report;
  EditReport *edit_report=new EditReport(rptname,this,"edit_report");
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
  EditReport *edit_report=
    new EditReport(list_box->currentText(),this,"edit_report");
  edit_report->exec();
  delete edit_report;
}


void ListReports::deleteData()
{
  QString str;

  if(list_box->currentText().isEmpty()) {
    return;
  }
  str=QString(tr("Are you sure you want to delete report"));
  if(QMessageBox::warning(this,tr("Delete Report"),
			  QString().sprintf(
			    "%s \"%s\"?",(const char *)str,
			    (const char *)list_box->currentText()),
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


void ListReports::doubleClickedData(QListBoxItem *item)
{
  editData();
}


void ListReports::DeleteReport(QString rptname)
{
  QString sql;
  RDSqlQuery *q;

  sql=QString().sprintf("delete from REPORTS where NAME=\"%s\"",
			(const char *)rptname);
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("delete from REPORT_SERVICES where \
                         REPORT_NAME=\"%s\"",(const char *)rptname);
  q=new RDSqlQuery(sql);
  delete q;
  sql=QString().sprintf("delete from REPORT_STATIONS where \
                         REPORT_NAME=\"%s\"",(const char *)rptname);
  q=new RDSqlQuery(sql);
  delete q;
}


void ListReports::RefreshList(QString rptname)
{
  QString sql;
  RDSqlQuery *q;

  list_box->clear();
  q=new RDSqlQuery("SELECT NAME FROM REPORTS",0);
  while (q->next()) {
    list_box->insertItem(q->value(0).toString());
    if(rptname==list_box->text(list_box->count()-1)) {
      list_box->setCurrentItem(list_box->count()-1);
    }
  }
  delete q;
}
