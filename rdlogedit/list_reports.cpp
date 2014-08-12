// list_reports.cpp
//
// List and Generate Log Reports
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_reports.cpp,v 1.7 2010/07/29 19:32:37 cvs Exp $
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
#include <qpushbutton.h>
#include <qlabel.h>
#include <qsqldatabase.h>
#include <qmessagebox.h>

#include <rdconf.h>
#include <rdtextfile.h>
#include <rddatedialog.h>

#include <globals.h>
#include <list_reports.h>


ListReports::ListReports(const QString &logname,const QString &description,
			 const QString service_name,const QDate &start_date,
			 const QDate &end_date,bool auto_refresh,
			 RDLogEvent *events,
			 QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  list_log_name=logname;
  list_description=description;
  list_service_name=service_name;
  list_start_date=start_date;
  list_end_date=end_date;
  list_auto_refresh=auto_refresh;
  list_events=events;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setCaption(tr("RDLibrary Reports"));

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);
  QFont select_font=QFont("Helvetica",12,QFont::Normal);
  select_font.setPixelSize(12);

  //
  // Reports List
  //
  list_reports_box=new QComboBox(this,"list_reports_box");
  list_reports_box->setGeometry(50,10,sizeHint().width()-60,19);
  list_reports_box->insertItem(tr("Log Listing"));
  list_reports_box->insertItem(tr("Log Exception Report"));
  QLabel *list_reports_label=
    new QLabel(list_reports_box,tr("Type:"),
	       this,"list_reports_label");
  list_reports_label->setGeometry(10,10,35,19);
  list_reports_label->setFont(font);
  list_reports_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Effective Date
  //
  list_date_edit=new QDateEdit(this,"list_date_edit");
  list_date_edit->setGeometry(110,34,100,19);
  QLabel *list_date_label=
    new QLabel(list_date_edit,tr("Effective Date:"),
	       this,"list_date_label");
  list_date_label->setGeometry(10,34,95,19);
  list_date_label->setFont(font);
  list_date_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  QPushButton *button=new QPushButton(this,"select_button");
  button->setGeometry(215,32,60,24);
  button->setFont(select_font);
  button->setText(tr("&Select"));
  connect(button,SIGNAL(clicked()),this,SLOT(selectDateData()));
  list_date_edit->setDate(QDate::currentDate());

  //
  //  Generate Button
  //
  button=new QPushButton(this,"generate_button");
  button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  button->setDefault(true);
  button->setFont(font);
  button->setText(tr("&Generate"));
  connect(button,SIGNAL(clicked()),this,SLOT(generateData()));

  //
  //  Close Button
  //
  button=new QPushButton(this,"close_button");
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setFont(font);
  button->setText(tr("&Close"));
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

  switch(list_reports_box->currentItem()) {
      case 0:  // Event Report
	GenerateLogReport(&report);
	break;

      case 1:  // XLoad Report
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
    start_date=list_start_date.toString("MM/dd/yyyy");
  }
  QString end_date=tr("[none]");
  if(!list_end_date.isNull()) {
    end_date=list_end_date.toString("MM/dd/yyyy");
  }
  *report="                                                     Rivendell Log Listing\n";
  *report+=QString().
    sprintf("Generated: %s                        Log: %-30s  Description: %s\n",
	    (const char *)QDateTime(QDate::currentDate(),QTime::currentTime()).
	    toString("MM/dd/yyyy - hh:mm:ss"),
	    (const char *)list_log_name.left(30),
	    (const char *)list_description.left(27));
  *report+=QString().
    sprintf("Service: %-10s          AutoRefresh Enabled: %-3s   Start Date: %-10s               End Date: %s\n",
	    (const char *)list_service_name,
	    (const char *)refresh,
	    (const char *)start_date,(const char *)end_date);
  *report+="\n";
  *report+="-Type-- -Time---- Trans -Cart- -Group---- -Length- -Title--------------------------- -Artist----------------------- -Source----- Line\n";

  //
  // Generate Event Listing
  //
  RDLogLine *logline;
  for(int i=0;i<list_events->size();i++) {
    logline=list_events->logLine(i);

    //
    // Type
    //
    *report+=QString().sprintf("%-7s ",
	       (const char *)RDLogLine::typeText(logline->type()));

    //
    // Time
    //
    if(logline->timeType()==RDLogLine::Hard) {
      *report+="H";
    }
    else {
      *report+=" ";
    }
    if(!logline->startTime(RDLogLine::Imported).isNull()) {
      *report+=QString().sprintf("%-8s ",
				 (const char *)logline->
				 startTime(RDLogLine::Logged).
				 toString("hh:mm:ss"));
    }
    else {
      *report+="         ";
    }

    //
    // Transition Type
    //
    *report+=QString().sprintf("%-5s ",
	  (const char *)RDLogLine::transText(logline->transType()).left(5));

    switch(logline->type()) {
	case RDLogLine::Cart:
	case RDLogLine::Macro:
	  *report+=QString().sprintf("%06u ",logline->cartNumber());
	  *report+=QString().sprintf("%-10s ",
				     (const char *)logline->groupName());
	  *report+=QString().sprintf("%8s ",(const char *)
				     RDGetTimeLength(logline->forcedLength(),
						    false,false));
	  *report+=
	    QString().sprintf("%-33s ",
			      (const char *)logline->title().left(33));
	  *report+=
	    QString().sprintf("%-30s ",
			      (const char *)logline->artist().left(30));
	  break;

	case RDLogLine::Marker:
	case RDLogLine::Track:
	  *report+="       ";
	  *report+="           ";
	  *report+="     :00 ";
	  *report+=
	    QString().sprintf("%-30s ",
			      (const char *)logline->markerComment().left(30));
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
	  *report+=
	    QString().sprintf("%-30s ",
			      (const char *)logline->markerLabel().left(30));
	  *report+="                               ";
	  break;
	  break;

	case RDLogLine::OpenBracket:
	case RDLogLine::CloseBracket:
	case RDLogLine::UnknownType:
	  break;
    }

    //
    // Source
    //
    *report+=QString().sprintf("%-12s ",
		(const char *)RDLogLine::sourceText(logline->source()));

    //
    // Line
    //
    *report+=QString().sprintf("%4d",i);

    //
    // End of Line
    //
    *report+="\n";
  }
}


void ListReports::GenerateExceptionReport(QString *report,const QDate &date)
{
  int errs=list_events->validate(report,date);
  if(errs==0) {
    QMessageBox::information(this,tr("Log Check"),tr("No exceptions found."));
    *report="";
  }
}
