// pick_report_date.cpp
//
// Select a Set of Dates for a Rivendell Report
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

#include <QFile>
#include <QMessageBox>

#include <rddatedialog.h>
#include <rddatedecode.h>
#include <rdescape_string.h>
#include <rdreport.h>

#include "globals.h"
#include "pick_report_dates.h"

PickReportDates::PickReportDates(const QString &svcname,QWidget *parent)
  : RDDialog(parent)
{
  QString sql;
  RDSqlQuery *q;
  QDate yesterday_date=QDate::currentDate().addDays(-1);
  edit_svcname=svcname;

  setWindowTitle("RDLogManager - "+tr("Select Report Dates"));
	     
  //
  // Fix the Window Size
  //
  setMaximumSize(sizeHint());
  setMinimumSize(sizeHint());

  //
  // Dialogs
  //
  edit_viewreport_dialog=new ViewReportDialog(this);

  //
  // Report List
  //
  edit_report_box=new QComboBox(this);
  edit_report_box->setGeometry(75,11,sizeHint().width()-85,19);
  QLabel *label=new QLabel(tr("&Report:"),this);
  label->setGeometry(10,11,60,19);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  sql=QString("select `REPORT_NAME` from `REPORT_SERVICES` where ")+
    "`SERVICE_NAME`='"+RDEscapeString(svcname)+"' "+
    "order by `REPORT_NAME`";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    edit_report_box->
      insertItem(edit_report_box->count(),q->value(0).toString());
  }
  delete q;

  //
  // Start Date
  //
  edit_startdate_edit=new QDateEdit(this);
  edit_startdate_edit->setGeometry(150,35,100,22);
  edit_startdate_edit->setDisplayFormat("MM/dd/yyyy");
  edit_startdate_edit->setDate(yesterday_date);
  label=new QLabel(tr("&Start Date:"),this);
  label->setGeometry(75,36,70,19);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  QPushButton *button=new QPushButton(this);
  button->setGeometry(260,33,50,27);
  button->setFont(subButtonFont());
  button->setText(tr("&Select"));
  connect(button,SIGNAL(clicked()),this,SLOT(selectStartDateData()));

  //
  // End Date
  //
  edit_enddate_edit=new QDateEdit(this);
  edit_enddate_edit->setGeometry(150,65,100,22);
  edit_enddate_edit->setDisplayFormat("MM/dd/yyyy");
  edit_enddate_edit->setDate(yesterday_date);
  label=new QLabel(tr("&End Date:"),this);
  label->setGeometry(75,66,70,19);
  label->setFont(labelFont());
  label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  button=new QPushButton(this);
  button->setGeometry(260,63,50,27);
  button->setFont(subButtonFont());
  button->setText(tr("&Select"));
  connect(button,SIGNAL(clicked()),this,SLOT(selectEndDateData()));

  //
  //  Generate Button
  //
  button=new QPushButton(this);
  button->setGeometry(10,sizeHint().height()-60,80,50);
  button->setFont(buttonFont());
  button->setText(tr("&Generate\nReport"));
  connect(button,SIGNAL(clicked()),this,SLOT(generateData()));

  //
  //  Close Button
  //
  button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(buttonFont());
  button->setText(tr("C&lose"));
  connect(button,SIGNAL(clicked()),this,SLOT(closeData()));
}


PickReportDates::~PickReportDates()
{
  delete edit_viewreport_dialog;
}


QSize PickReportDates::sizeHint() const
{
  return QSize(400,160);
}


QSizePolicy PickReportDates::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}

  
void PickReportDates::selectStartDateData()
{
  RDDateDialog *dialog=new RDDateDialog(2002,QDate::currentDate().year(),this);
  QDate date=edit_startdate_edit->date();
  if(dialog->exec(&date)<0) {
    delete dialog;
    return;
  }
  edit_startdate_edit->setDate(date);
  edit_enddate_edit->setDate(date);
  delete dialog;
}


void PickReportDates::selectEndDateData()
{
  RDDateDialog *dialog=new RDDateDialog(2002,QDate::currentDate().year(),this);
  QDate date=edit_enddate_edit->date();
  if(dialog->exec(&date)<0) {
    delete dialog;
    return;
  }
  edit_enddate_edit->setDate(date);
  delete dialog;
}


void PickReportDates::generateData()
{
  if(edit_startdate_edit->date()>edit_enddate_edit->date()) {
    QMessageBox::warning(this,"RDLogManager - "+tr("Invalid Date Range"),
		 tr("The end date cannot be earlier than the start date!"));
    return;
  }
  RDReport *report=
    new RDReport(edit_report_box->currentText(),rda->station(),rda->config(),this);
  if((edit_startdate_edit->date()!=edit_enddate_edit->date())&&
     (!RDReport::multipleDaysAllowed(report->filter()))) {
    QMessageBox::warning(this,"RDLogManager - "+tr("Invalid Date Range"),
			 tr("This report type cannot span multiple dates!"));
    delete report;
    return;
  }
  if((edit_startdate_edit->date().month()!=edit_enddate_edit->date().month())&&
     (!RDReport::multipleMonthsAllowed(report->filter()))) {
    QMessageBox::warning(this,"RDLogManager - "+tr("Invalid Date Range"),
			 tr("This report type cannot span multiple months!"));
    delete report;
    return;
  }
  QString filename=
    RDDateDecode(report->exportPath(RDReport::Linux),
		 edit_startdate_edit->date(),rda->station(),rda->config(),
		 edit_svcname);
  QFile file(filename);
  if(file.exists()) {
    if(QMessageBox::question(this,"RDLogManager - "+tr("File Exists"),
			     tr("The report file already exists.  Overwrite?"),
			     QMessageBox::Yes,QMessageBox::No)!=
       QMessageBox::Yes) {
      delete report;
      return;
    }
  }
  QString out_path;
  report->generateReport(edit_startdate_edit->date(),
			 edit_enddate_edit->date(),rda->station(),&out_path);
  switch(report->errorCode()) {
  case RDReport::ErrorOk:
    edit_viewreport_dialog->exec(out_path);
    break;

  case RDReport::ErrorCantOpen:
    QMessageBox::warning(this,"RDLogManager - "+tr("File Error"),
			 tr("Unable to open report file at")+
			 QString("\"")+out_path+"\"!");
    break;

  case RDReport::ErrorCanceled:
    break;
  }
  delete report;
}


void PickReportDates::closeData()
{
  done(0);
}
