// pick_report_date.cpp
//
// Select a Set of Dates for a Rivendell Report
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: pick_report_dates.cpp,v 1.8.8.2 2013/01/22 20:59:39 cvs Exp $
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

#include <qpushbutton.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <qfile.h>

#include <rddatedialog.h>
#include <rdreport.h>
#include <rddatedecode.h>
#include <rddb.h>
#include <globals.h>
#include <pick_report_dates.h>


PickReportDates::PickReportDates(const QString &svcname,
				   QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  QString sql;
  RDSqlQuery *q;
  QDate yesterday_date=QDate::currentDate().addDays(-1);

  edit_svcname=svcname;
  setCaption(tr("Select Report Dates"));
	     
  //
  // Fix the Window Size
  //
  setMaximumWidth(sizeHint().width());
  setMaximumHeight(sizeHint().height());
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  //
  // Create Fonts
  //
  QFont bold_font=QFont("Helvetica",12,QFont::Bold);
  bold_font.setPixelSize(12);
  QFont font=QFont("Helvetica",12,QFont::Normal);
  font.setPixelSize(12);

  //
  // Report List
  //
  edit_report_box=new QComboBox(this,"edit_report_box");
  edit_report_box->setGeometry(75,11,sizeHint().width()-85,19);
  QLabel *label=new QLabel(edit_report_box,tr("&Report:"),
				      this,"edit_report_label");
  label->setGeometry(10,11,60,19);
  label->setFont(bold_font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  sql=QString().sprintf("select REPORT_NAME from REPORT_SERVICES\
                         where SERVICE_NAME=\"%s\" order by REPORT_NAME",
			(const char *)svcname);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    edit_report_box->insertItem(q->value(0).toString());
  }
  delete q;

  //
  // Start Date
  //
  edit_startdate_edit=new QDateEdit(this,"edit_startdate_edit");
  edit_startdate_edit->setGeometry(150,35,100,22);
  edit_startdate_edit->setDate(yesterday_date);
  label=new QLabel(edit_startdate_edit,tr("&Start Date:"),
				      this,"edit_startdate_label");
  label->setGeometry(75,36,70,19);
  label->setFont(bold_font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  QPushButton *button=new QPushButton(this,"startdate_button");
  button->setGeometry(260,33,50,27);
  button->setFont(font);
  button->setText(tr("&Select"));
  connect(button,SIGNAL(clicked()),this,SLOT(selectStartDateData()));

  //
  // End Date
  //
  edit_enddate_edit=new QDateEdit(this,"edit_enddate_edit");
  edit_enddate_edit->setGeometry(150,65,100,22);
  edit_enddate_edit->setDate(yesterday_date);
  label=new QLabel(edit_enddate_edit,tr("&End Date:"),
				      this,"edit_enddate_label");
  label->setGeometry(75,66,70,19);
  label->setFont(bold_font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  button=new QPushButton(this,"enddate_button");
  button->setGeometry(260,63,50,27);
  button->setFont(font);
  button->setText(tr("&Select"));
  connect(button,SIGNAL(clicked()),this,SLOT(selectEndDateData()));

  //
  //  Generate Button
  //
  button=new QPushButton(this,"list_purge_button");
  button->setGeometry(10,sizeHint().height()-60,80,50);
  button->setFont(bold_font);
  button->setText(tr("&Generate\nReport"));
  connect(button,SIGNAL(clicked()),this,SLOT(generateData()));

  //
  //  Close Button
  //
  button=new QPushButton(this,"close_button");
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(bold_font);
  button->setText(tr("C&lose"));
  connect(button,SIGNAL(clicked()),this,SLOT(closeData()));
}


PickReportDates::~PickReportDates()
{
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
    QMessageBox::warning(this,tr("Invalid Date Range"),
		 tr("The end date cannot be earlier than the start date!"));
    return;
  }
  RDReport *report=new RDReport(edit_report_box->currentText(),this);
  if((edit_startdate_edit->date()!=edit_enddate_edit->date())&&
     (!RDReport::multipleDaysAllowed(report->filter()))) {
    QMessageBox::warning(this,tr("Invalid Date Range"),
			 tr("This report type cannot span multiple dates!"));
    delete report;
    return;
  }
  if((edit_startdate_edit->date().month()!=edit_enddate_edit->date().month())&&
     (!RDReport::multipleMonthsAllowed(report->filter()))) {
    QMessageBox::warning(this,tr("Invalid Date Range"),
			 tr("This report type cannot span multiple months!"));
    delete report;
    return;
  }
#ifdef WIN32
  QString filename=RDDateDecode(report->exportPath(RDReport::Windows),
				edit_startdate_edit->date());
#else
  QString filename=RDDateDecode(report->exportPath(RDReport::Linux),
				edit_startdate_edit->date());
#endif
  QFile file(filename);
  if(file.exists()) {
    if(QMessageBox::question(this,"File Exists",
			     "The report file already exists.  Overwrite?",
			     QMessageBox::Yes,QMessageBox::No)!=
       QMessageBox::Yes) {
      delete report;
      return;
    }
  }
  QString out_path;
  report->generateReport(edit_startdate_edit->date(),
			 edit_enddate_edit->date(),rdstation_conf,&out_path);
  switch(report->errorCode()) {
      case RDReport::ErrorOk:
	QMessageBox::information(this,tr("Report Complete"),
				 tr("Report generated in")+" \""+out_path+
				 "\".");
	break;

      case RDReport::ErrorCantOpen:
	QMessageBox::warning(this,"File Error","Unable to open report file!");
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
