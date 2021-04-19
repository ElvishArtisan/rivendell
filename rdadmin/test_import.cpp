// test_import.cpp
//
// Test a Rivendell Log Import
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
#include <QPainter>

#include <rdapplication.h>
#include <rdconf.h>
#include <rddatedecode.h>
#include <rddatedialog.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdevent_line.h>
#include <rdpasswd.h>

#include "globals.h"
#include "test_import.h"

TestImport::TestImport(RDSvc *svc,RDSvc::ImportSource src,QWidget *parent)
  : RDDialog(parent)
{
  QString sql;
  QDate current_date=QDate::currentDate();

  test_svc=svc;
  test_src=src;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  switch(test_src) {
  case RDSvc::Traffic:
    setWindowTitle("RDAdmin - "+tr("Test Traffic Import"));
    break;

  case RDSvc::Music:
    setWindowTitle("RDAdmin - "+tr("Test Music Import"));
    break;
  }

  //
  // Date Selector
  //
  test_date_edit=new QDateEdit(this);
  test_date_edit->setDisplayFormat("MM/dd/yyyy");
  test_date_label=new QLabel(tr("Test Date:"),this);
  test_date_label->setFont(labelFont());
  test_date_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  test_date_edit->setDate(current_date);
  connect(test_date_edit,SIGNAL(dateChanged(const QDate &)),
	  this,SLOT(dateChangedData(const QDate &)));

  //
  // Select Date Button
  //
  QPushButton *button=new QPushButton(this);
  button->setGeometry(200,5,60,30);
  button->setFont(subButtonFont());
  button->setText(tr("Select"));
  connect(button,SIGNAL(clicked()),this,SLOT(selectData()));

  //
  // Import Button
  //
  test_import_button=new QPushButton(this);
  test_import_button->setFont(buttonFont());
  test_import_button->setText(tr("Import"));
  connect(test_import_button,SIGNAL(clicked()),this,SLOT(importData()));

  //
  // Import Filename
  //
  test_filename_edit=new QLineEdit(this);
  test_filename_edit->setReadOnly(true);
  test_filename_label=new QLabel(tr("Using source file:"),this);
  test_filename_label->setGeometry(15,115,sizeHint().width()-30,18);
  test_filename_label->setFont(labelFont());

  //
  // Events List
  //
  test_events_view=new RDTableView(this);
  test_events_model=new RDLogImportModel(rda->station()->name(),getpid(),this);
  test_events_view->setModel(test_events_model);
  test_events_label=new QLabel(tr("Imported Events"),this);
  test_events_label->setGeometry(15,160,sizeHint().width()-30,18);
  test_events_label->setFont(labelFont());
  connect(test_events_model,SIGNAL(modelReset()),
	  test_events_view,SLOT(resizeColumnsToContents()));
  test_events_view->resizeColumnsToContents();

  //
  //  Close Button
  //
  test_close_button=new QPushButton(this);
  test_close_button->setFont(buttonFont());
  test_close_button->setText(tr("Close"));
  connect(test_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  dateChangedData(current_date);
}


TestImport::~TestImport()
{
}


QSize TestImport::sizeHint() const
{
  return QSize(1000,600);
} 


QSizePolicy TestImport::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
}


void TestImport::selectData()
{
  RDDateDialog *datedialog=
    new RDDateDialog(QDate::currentDate().year(),QDate::currentDate().year()+1,
		     this);
  QDate date=test_date_edit->date();
  if(datedialog->exec(&date)<0) {
    delete datedialog;
    return;
  }
  test_date_edit->setDate(date);
  delete datedialog;
}


void TestImport::importData()
{
  if(!test_svc->import(test_src,test_date_edit->date(),test_svc->breakString(),
		       test_svc->trackString(test_src),true)) {
    QMessageBox::information(this,tr("Import Error"),
			     tr("There was an error during import\nplease check your settings and try again."));
    return;
  }
  test_events_model->refresh();

  QString sql=QString("delete from `IMPORTER_LINES` where ")+
    "`STATION_NAME`='"+RDEscapeString(rda->station()->name())+"' && "+
    QString().sprintf("`PROCESS_ID`=%u",getpid());
  //  printf("IMPORTER_LINES cleanup SQL: %s\n",(const char *)sql);
  RDSqlQuery::apply(sql);
}


void TestImport::dateChangedData(const QDate &date)
{
  test_filename_edit->
    setText(RDDateDecode(test_svc->importPath(test_src),date,
			 rda->station(),rda->config(),test_svc->name()));
}


void TestImport::closeData()
{
  done(0);
}


void TestImport::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);
  p->setPen(Qt::black);
  p->drawLine(10,105,size().width()-20,105);
  p->end();
  delete p;
}


void TestImport::resizeEvent(QResizeEvent *e)
{
  test_date_edit->setGeometry(95,10,95,20);
  test_date_label->setGeometry(5,10,85,20);
  test_filename_edit->setGeometry(10,133,size().width()-20,18);
  test_import_button->setGeometry(30,45,size().width()-60,50);
  test_events_view->
    setGeometry(10,178,size().width()-20,size().height()-248);
  test_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
