// list_reports.cpp
//
// List and Generate Log Reports
//
//   (C) Copyright 2002-2022 Fred Gleason <fredg@paravelsystems.com>
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
#include <QPushButton>

#include <rdconf.h>
#include <rdcsv.h>
#include <rddatedialog.h>
#include <rdreport.h>
#include <rdtextfile.h>

#include "globals.h"
#include "list_reports.h"

ListReports::ListReports(const QString &logname,const QString &description,
			 const QString service_name,const QDate &start_date,
			 const QDate &end_date,bool auto_refresh,
			 RDLogModel *model,QWidget *parent)
  : RDDialog(parent)
{
  list_log_name=logname;
  list_description=description;
  list_service_name=service_name;
  list_start_date=start_date;
  list_end_date=end_date;
  list_auto_refresh=auto_refresh;
  list_model=model;

  setWindowTitle("RDLogEdit - "+tr("Reports"));

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());

  //
  // Reports List
  //
  list_reports_box=new QComboBox(this);
  list_reports_box->setGeometry(50,10,sizeHint().width()-60,19);
  list_reports_box->insertItem(0,tr("Log Listing"));
  list_reports_box->insertItem(1,tr("Log Listing (CSV)"));
  list_reports_box->insertItem(2,tr("Log Exception Report"));
  QLabel *list_reports_label=new QLabel(tr("Type:"),this);
  list_reports_label->setGeometry(10,10,35,19);
  list_reports_label->setFont(labelFont());
  list_reports_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  // Effective Date
  //
  list_date_edit=new RDDateEdit(this);
  list_date_edit->setGeometry(110,34,100,19);
  QLabel *list_date_label=new QLabel(tr("Effective Date:"),this);
  list_date_label->setGeometry(10,34,95,19);
  list_date_label->setFont(labelFont());
  list_date_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  QPushButton *button=new QPushButton(this);
  button->setGeometry(215,32,60,24);
  button->setFont(subButtonFont());
  button->setText(tr("Select"));
  connect(button,SIGNAL(clicked()),this,SLOT(selectDateData()));
  list_date_edit->setDate(QDate::currentDate());

  //
  //  Generate Button
  //
  button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(buttonFont());
  button->setText(tr("Generate"));
  connect(button,SIGNAL(clicked()),this,SLOT(generateData()));

  //
  //  Close Button
  //
  button=new QPushButton(this);
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setFont(buttonFont());
  button->setText(tr("Close"));
  connect(button,SIGNAL(clicked()),this,SLOT(closeData()));
}


ListReports::~ListReports()
{
}


QSize ListReports::sizeHint() const
{
  return QSize(350,132);
} 


QSizePolicy ListReports::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListReports::selectDateData()
{
  QDate today=QDate::currentDate();
  QDate date=list_date_edit->date();
  RDDateDialog *d=new RDDateDialog(today.year()-1,today.year()+1,this);
  if(d->exec(&date)==0) {
    list_date_edit->setDate(date);
  }
  delete d;
}


void ListReports::generateData()
{
  QString report;

  switch(list_reports_box->currentIndex()) {
  case 0:  // Event Report
    GenerateLogReport(&report);
    break;

  case 1:  // Event Report
    GenerateLogCsvReport(&report);
    break;

  case 2:  // XLoad Report
    GenerateExceptionReport(&report,list_date_edit->date());
    break;

  default:
    return;
  }
  if(!report.isEmpty()) {
    RDTextFile(report);
  }
}


void ListReports::closeData()
{
  done(-1);
}


void ListReports::GenerateLogReport(QString *report)
{
  //
  // Generate Header
  //
  QString refresh="No ";
  if(list_auto_refresh) {
    refresh="Yes";
  }
  QString start_date=tr("[none]");
  if(!list_start_date.isNull()) {
    start_date=rda->shortDateString(list_start_date);
  }
  QString end_date=tr("[none]");
  if(!list_end_date.isNull()) {
    end_date=rda->shortDateString(list_end_date);
  }
  *report=RDReport::center("Rivendell Log Listing",132)+"\n";
  *report+=QString("Generated: ")+
    rda->shortDateString(QDate::currentDate())+
    "                        Log: "+
    RDReport::leftJustify(list_log_name,30)+
    "  Description: "+RDReport::leftJustify(list_description,27)+"\n";
  *report+=QString("Service: ")+RDReport::leftJustify(list_service_name,10)+
    "          AutoRefresh Enabled: "+RDReport::leftJustify(refresh,3)+"   "+
    RDReport::leftJustify(start_date,10)+"               "+end_date+"\n";

  *report+="\n";
  *report+="-Type-- -Start Time--- Trans -Cart- -Group---- -Length- -Title--------------------------- -Artist----------------------- -Source----- Line\n";

  //
  // Generate Event Listing
  //
  RDLogLine *logline;
  for(int i=0;i<list_model->lineCount();i++) {
    logline=list_model->logLine(i);

    //
    // Type
    //
    *report+=RDReport::leftJustify(RDLogLine::typeText(logline->type()),7)+" ";

    //
    // Time
    //
    if(logline->timeType()==RDLogLine::Hard) {
      *report+="T";
    }
    else {
      *report+=" ";
    }
    if(!logline->startTime(RDLogLine::Imported).isNull()) {
      *report+=RDReport::leftJustify(rda->tenthsTimeString(logline->startTime(RDLogLine::Logged),"0"),13)+" ";
    }
    else {
      *report+="         ";
    }

    //
    // Transition Type
    //
    *report+=RDReport::leftJustify(RDLogLine::transText(logline->transType()).left(5),5)+" ";

    switch(logline->type()) {
    case RDLogLine::Cart:
    case RDLogLine::Macro:
      *report+=QString::asprintf("%06u ",logline->cartNumber());
      *report+=RDReport::leftJustify(logline->groupName(),10)+" ";
      *report+=RDReport::rightJustify(RDGetTimeLength(logline->forcedLength(),false,false),8)+" ";
      *report+=RDReport::leftJustify(logline->title(),33)+" ";
      *report+=RDReport::leftJustify(logline->artist(),30)+" ";
      break;

    case RDLogLine::Marker:
    case RDLogLine::Track:
      *report+="       ";
      *report+="           ";
      *report+="     :00 ";
      *report+=RDReport::leftJustify(logline->markerComment(),33)+" ";
      *report+="                               ";
      break;

    case RDLogLine::TrafficLink:
      *report+="       ";
      *report+="           ";
      *report+="     :00 ";
      *report+="Traffic Import                 ";
      *report+="                               ";
      break;

    case RDLogLine::MusicLink:
      *report+="       ";
      *report+="           ";
      *report+="     :00 ";
      *report+="Music Import                 ";
      *report+="                               ";
      break;


    case RDLogLine::Chain:
      *report+="       ";
      *report+="           ";
      *report+="         ";
      *report+=RDReport::leftJustify(logline->markerLabel(),33)+" ";
      *report+="                               "; 
      break;

    case RDLogLine::OpenBracket:
    case RDLogLine::CloseBracket:
    case RDLogLine::UnknownType:
      break;
    }

    //
    // Source
    //
    *report+=RDReport::leftJustify(RDLogLine::sourceText(logline->source()),12)+" ";

    //
    // Line
    //
    *report+=QString::asprintf("%4d",i);

    //
    // End of Line
    //
    *report+="\n";
  }
}


void ListReports::GenerateLogCsvReport(QString *report)
{
  RDLogLine *logline;

  //
  // Column Names
  //
  *report+=RDCsvField("TYPE");
  *report+=RDCsvField("START_TIME");
  *report+=RDCsvField("TIME_TYPE");
  *report+=RDCsvField("TRANS_TYPE");
  *report+=RDCsvField("CART_NUMBER");
  *report+=RDCsvField("GROUP_NAME");
  *report+=RDCsvField("LENGTH");
  *report+=RDCsvField("TITLE");
  *report+=RDCsvField("ARTIST");
  *report+=RDCsvField("CLIENT");
  *report+=RDCsvField("AGENCY");
  *report+=RDCsvField("ALBUM");
  *report+=RDCsvField("LABEL");
  *report+=RDCsvField("CONDUCTOR");
  *report+=RDCsvField("COMPOSER");
  *report+=RDCsvField("PUBLISHER");
  *report+=RDCsvField("USER_DEFINED");
  *report+=RDCsvField("SONG_ID");
  *report+=RDCsvField("USAGE_CODE");
  *report+=RDCsvField("SOURCE");
  *report+=RDCsvField("EXT_DATA");
  *report+=RDCsvField("EXT_EVENT_ID");
  *report+=RDCsvField("EXT_ANNC_TYPE");
  *report+=RDCsvField("LINE_ID");
  *report+=RDCsvField("LINE",true);

  for(int i=0;i<list_model->lineCount();i++) {
    logline=list_model->logLine(i);

    //
    // Event Type
    //
    *report+=RDCsvField(RDLogLine::typeText(logline->type()));

    //
    // Time
    //
    if(logline->startTime(RDLogLine::Imported).isNull()||
       (logline->startTime(RDLogLine::Imported)==QTime(0,0,0,0))) {
      *report+=RDCsvField();
    }
    else {
      *report+=RDCsvField(logline->startTime(RDLogLine::Imported).
			  toString("hh:mm:ss"));
    }
    if(logline->timeType()==RDLogLine::Hard) {
      *report+=RDCsvField("Hard");
    }
    else {
      *report+=RDCsvField("Relative");
    }

    //
    // Transition Type
    //
    *report+=RDCsvField(RDLogLine::transText(logline->transType()));

    switch(logline->type()) {
    case RDLogLine::Cart:
    case RDLogLine::Macro:
      *report+=RDCsvField(logline->cartNumber());
      *report+=RDCsvField(logline->groupName());
      *report+=RDCsvField(RDGetTimeLength(logline->forcedLength(),false,false));
      *report+=RDCsvField(logline->title());
      *report+=RDCsvField(logline->artist());
      break;

    case RDLogLine::Marker:
    case RDLogLine::Track:
      *report+=RDCsvField();
      *report+=RDCsvField();
      *report+=RDCsvField(":00");
      *report+=RDCsvField(logline->markerComment());
      *report+=RDCsvField();
      break;

    case RDLogLine::TrafficLink:
      *report+=RDCsvField();
      *report+=RDCsvField();
      *report+=RDCsvField(":00");
      *report+=RDCsvField(tr("Traffic Import"));
      *report+=RDCsvField();
      break;

    case RDLogLine::MusicLink:
      *report+=RDCsvField();
      *report+=RDCsvField();
      *report+=RDCsvField(":00");
      *report+=RDCsvField(tr("Music Import"));
      *report+=RDCsvField();
      break;

    case RDLogLine::Chain:
      *report+=RDCsvField();
      *report+=RDCsvField();
      *report+=RDCsvField();
      *report+=RDCsvField(logline->markerLabel());
      *report+=RDCsvField();
      break;

    case RDLogLine::OpenBracket:
    case RDLogLine::CloseBracket:
    case RDLogLine::UnknownType:
      *report+=RDCsvField();
      *report+=RDCsvField();
      *report+=RDCsvField();
      *report+=RDCsvField();
      *report+=RDCsvField();
      break;
    }
    *report+=RDCsvField(logline->client());
    *report+=RDCsvField(logline->agency());
    *report+=RDCsvField(logline->album());
    *report+=RDCsvField(logline->label());
    *report+=RDCsvField(logline->conductor());
    *report+=RDCsvField(logline->composer());
    *report+=RDCsvField(logline->publisher());
    *report+=RDCsvField(logline->userDefined());
    *report+=RDCsvField(logline->songId());
    *report+=RDCsvField(RDCart::usageText(logline->usageCode()));
    *report+=RDCsvField(RDLogLine::sourceText(logline->source()));
    *report+=RDCsvField(logline->extData());
    *report+=RDCsvField(logline->extEventId());
    *report+=RDCsvField(logline->extAnncType());
    *report+=RDCsvField(logline->id());
    *report+=RDCsvField(i,true);
  }
}


void ListReports::GenerateExceptionReport(QString *report,const QDate &date)
{
  int errs=list_model->validate(report,date);
  if(errs==0) {
    QMessageBox::information(this,tr("Log Check"),tr("No exceptions found."));
    *report="";
  }
}
