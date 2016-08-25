// list_reports.cpp
//
// List Rivendell Reports
//
//   (C) Copyright 2002-2004,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <QDialog>
#include <QLabel>
#include <QMessageBox>

#include "add_report.h"
#include "edit_report.h"
#include "list_reports.h"

ListReports::ListReports(QWidget *parent)
  : QDialog(parent)
{
  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

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
  // Report View
  //
  list_model=new RDTableModel(this);
  list_model->setQuery("select NAME from REPORTS order by NAME");
  list_model->setHeaderData(0,Qt::Horizontal,tr("Name"));
  list_view=new QListView(this);
  list_view->setModel(list_model);
  list_view->show();
  connect(list_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));
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
  QString report_name;

  AddReport *add_report=new AddReport(&report_name,this);
  if(add_report->exec()<0) {
    delete add_report;
    return;
  }
  delete add_report;
  EditReport *edit_report=new EditReport(report_name,this);
  if(edit_report->exec()<0) {
    RDReport::remove(report_name);
    delete edit_report;
    return;
  }
  delete edit_report;
  list_model->update();
}


void ListReports::editData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    doubleClickedData(s->selectedRows()[0]);
  }
}


void ListReports::deleteData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  QString rptname=s->selectedRows()[0].data().toString();
  if(s->hasSelection()) {
    if(QMessageBox::warning(this,"RDAdmin - "+tr("Delete Report"),
			    tr("Are you sure you want to delete report")+
			    " \""+rptname+"\"?",
			    QMessageBox::Yes,QMessageBox::No)==
       QMessageBox::Yes) {
      RDReport::remove(rptname);
      list_model->update();
    }
  }
}


void ListReports::closeData()
{
  done(0);
}


void ListReports::doubleClickedData(const QModelIndex &index)
{
  EditReport *edit_report=new EditReport(index.data().toString(),this);
  edit_report->exec();
  delete edit_report;
}


void ListReports::resizeEvent(QResizeEvent *e)
{
  list_add_button->setGeometry(size().width()-90,30,80,50);
  list_edit_button->setGeometry(size().width()-90,90,80,50);
  list_delete_button->setGeometry(size().width()-90,150,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
  list_view->setGeometry(10,10,size().width()-110,size().height()-20);
}
