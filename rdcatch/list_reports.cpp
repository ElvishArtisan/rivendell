// list_reports.cpp
//
// List and Generate RDCatch Reports
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

#include <rdconf.h>
#include <rdescape_string.h>
#include <rdrecording.h>
#include <rdreport.h>
#include <rdtextfile.h>

#include "globals.h"
#include "list_reports.h"

ListReports::ListReports(bool today_only,bool active_only,int dow,
			 QWidget *parent)
  : RDDialog(parent)
{
  list_today_only=today_only;
  list_active_only=active_only;
  list_dow=dow;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setWindowTitle("RDCatch - "+tr("RDLibrary Reports"));

  //
  // Reports List
  //
  list_reports_box=new QComboBox(this);
  list_reports_box->setGeometry(50,10,sizeHint().width()-60,19);
  list_reports_box->insertItem(0,tr("Event Report"));
  list_reports_box->insertItem(1,tr("Upload/Download Report"));
  QLabel *list_reports_label=new QLabel(tr("Type:"),this);
  list_reports_label->setGeometry(10,10,35,19);
  list_reports_label->setFont(labelFont());
  list_reports_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);

  //
  //  Generate Button
  //
  QPushButton *generate_button=new QPushButton(this);
  generate_button->
    setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  generate_button->setDefault(true);
  generate_button->setFont(buttonFont());
  generate_button->setText(tr("Generate"));
  connect(generate_button,SIGNAL(clicked()),this,SLOT(generateData()));

  //
  //  Close Button
  //
  QPushButton *close_button=new QPushButton(this);
  close_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  close_button->setFont(buttonFont());
  close_button->setText(tr("Close"));
  connect(close_button,SIGNAL(clicked()),this,SLOT(closeData()));
}


ListReports::~ListReports()
{
}


QSize ListReports::sizeHint() const
{
  return QSize(350,110);
} 


QSizePolicy ListReports::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListReports::generateData()
{
  QString report;

  switch(list_reports_box->currentIndex()) {
  case 0:  // Event Report
    GenerateEventReport(&report);
    break;

  case 1:  // XLoad Report
    GenerateXloadReport(&report);
    break;

  default:
    return;
  }
  RDTextFile(report);
}


void ListReports::closeData()
{
  done(-1);
}


void ListReports::GenerateEventReport(QString *report)
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;
  QString str;
  bool exists=false;

  //
  // Generate Header
  //
  *report=RDReport::center("Rivendell RDCatch Event Report",132)+"\n";
  *report+=QString("Generated: ")+QDateTime::currentDateTime().toString("MM/dd/yyyy - hh:mm:ss")+"\n";
  *report+="\n";
  *report+="T -Start-------- -End---------- -Days of Week- -Location----- -Source------------- -Destination-------- -Description-----------------\n";

  //
  // Generate Rows
  //
  sql=QString("select ")+
    "TYPE,"+           // 00
    "START_TYPE,"+     // 01
    "START_TIME,"+     // 02
    "END_TYPE,"+       // 03
    "END_TIME,"+       // 04
    "LENGTH,"+         // 05
    "SUN,"+            // 06
    "MON,"+            // 07
    "TUE,"+            // 08
    "WED,"+            // 09
    "THU,"+            // 10
    "FRI,"+            // 11
    "SAT,"+            // 12
    "STATION_NAME,"+   // 13
    "CHANNEL,"+        // 14
    "CUT_NAME,"+       // 15
    "URL,"+            // 16
    "MACRO_CART,"+     // 17
    "SWITCH_INPUT,"+   // 18
    "SWITCH_OUTPUT,"+  // 19
    "DESCRIPTION "+    // 20
    "from RECORDINGS order by START_TIME";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    //
    // Event Type
    //
    switch((RDRecording::Type)q->value(0).toInt()) {
    case RDRecording::Recording:
      *report+="R";
      exists=RDCut::exists(q->value(15).toString());
      break;

    case RDRecording::MacroEvent:
      *report+="M";
      exists=RDCart::exists(q->value(17).toUInt());
      break;

    case RDRecording::SwitchEvent:
      *report+="S";
      exists=true;
      break;

    case RDRecording::Playout:
      *report+="P";
      exists=RDCut::exists(q->value(15).toString());
      break;

    case RDRecording::Download:
      *report+="D";
      exists=RDCut::exists(q->value(15).toString());
      break;

    case RDRecording::Upload:
      *report+="U";
      exists=RDCut::exists(q->value(15).toString());
      break;

    default:
      *report+="?";
      exists=true;
      break;
    }
    if(exists) {
      *report+=" ";
    }
    else {
      *report+="*";
    }

    //
    // Start Time
    //
    switch((RDRecording::StartType)q->value(1).toInt()) {
    case RDRecording::HardStart:
      *report+=QString("Hard: ")+
	RDReport::leftJustify(q->value(2).toTime().toString("hh:mm:ss"),8)+" ";
      break;

    case RDRecording::GpiStart:
      *report+=QString("Gpi: ")+
	RDReport::leftJustify(q->value(2).toTime().toString("hh:mm:ss"),8)+" ";
      break;
    }

    //
    // End Time
    //
    switch((RDRecording::Type)q->value(0).toInt()) {
    case RDRecording::Recording:
      switch((RDRecording::EndType)q->value(3).toInt()) {
      case RDRecording::HardEnd:
	*report+=QString("Hard: ")+
	  RDReport::leftJustify(q->value(4).toTime().toString("hh:mm:ss"),8)+" ";
	break;
		
      case RDRecording::GpiEnd:
	*report+=QString("Gpi: ")+
	  RDReport::leftJustify(q->value(4).toTime().toString("hh:mm:ss"),8)+" ";
	break;
		
      case RDRecording::LengthEnd:
	*report+=QString("Len: ")+
	  RDReport::leftJustify(RDGetTimeLength(q->value(5).toInt(),false,false),8)+"  ";
	break;
      }
      break;

    default:
      *report+="               ";
      break;
    }

    //
    // Days of the Week
    //
    if(q->value(6).toString()=="Y") {
      *report+="Su";
    }
    else {
      *report+="  ";
    }
    if(q->value(7).toString()=="Y") {
      *report+="Mo";
    }
    else {
      *report+="  ";
    }
    if(q->value(8).toString()=="Y") {
      *report+="Tu";
    }
    else {
      *report+="  ";
    }
    if(q->value(9).toString()=="Y") {
      *report+="We";
    }
    else {
      *report+="  ";
    }
    if(q->value(10).toString()=="Y") {
      *report+="Th";
    }
    else {
      *report+="  ";
    }
    if(q->value(11).toString()=="Y") {
      *report+="Fr";
    }
    else {
      *report+="  ";
    }
    if(q->value(12).toString()=="Y") {
      *report+="Sa ";
    }
    else {
      *report+="   ";
    }

    //
    // Location
    //
    switch((RDRecording::Type)q->value(0).toInt()) {
    case RDRecording::Recording:
      str=q->value(13).toString().left(12)+
	QString().sprintf(":%d",q->value(14).toInt());
      break;

    case RDRecording::Playout:
      str=q->value(13).toString().left(12)+
	QString().sprintf(":%d",q->value(14).toInt()-128);
      break;

    default:
      str=q->value(13).toString().left(14);
      break;
    }
    *report+=RDReport::leftJustify(str,14)+" ";

    //
    // Source and Destination
    //
    switch((RDRecording::Type)q->value(0).toInt()) {
    case RDRecording::Recording:
      sql=QString("select ")+
	"SWITCH_STATION,"+  // 00
	"SWITCH_MATRIX "+   // 01
	"from DECKS where "+
	"(STATION_NAME=\""+RDEscapeString(q->value(13).toString())+"\")&&"+
	QString().sprintf("(CHANNEL=%d)",q->value(14).toInt());
      q1=new RDSqlQuery(sql);
      if(q1->first()) {
	*report+=
	  RDReport::leftJustify(GetSourceName(q1->value(0).toString(),
					      q1->value(1).toInt(),
					      q->value(18).toInt()).left(20),20)+" ";
      }
      else {
	*report+="                     ";
      }
      delete q1;
      *report+=QString("Cut ")+
	RDReport::leftJustify(q->value(15).toString(),10)+"       ";
      break;

    case RDRecording::MacroEvent:
      *report+=QString("Cart ")+
	QString().sprintf("%06u          ",q->value(17).toUInt());
      *report+="                     ";
      break;

    case RDRecording::SwitchEvent:
      *report+=
	RDReport::leftJustify(GetSourceName(q->value(13).toString(),
					    q->value(14).toInt(),
					    q->value(18).toInt()),20)+" ";
      *report+=
	RDReport::leftJustify(GetDestinationName(q->value(13).toString(),
						 q->value(14).toInt(),
						 q->value(19).toInt()),20)+" ";
      break;

    case RDRecording::Playout:
      *report+=QString("Cut ")+
	RDReport::leftJustify(q->value(15).toString(),10)+"       ";
      *report+="                     ";
      break;

    case RDRecording::Download:
      *report+=RDReport::leftJustify(q->value(16).toString().left(20),20)+" ";
      *report+=QString("Cut ")+
	RDReport::leftJustify(q->value(15).toString(),10)+"       ";
      break;

    case RDRecording::Upload:
      *report+=QString("Cut ")+
	RDReport::leftJustify(q->value(15).toString(),10)+"       ";
      *report+=RDReport::leftJustify(q->value(16).toString(),20)+" ";
      break;

    case RDRecording::LastType:
      break;
    }

    //
    // Description
    //
    *report+=q->value(20).toString().left(29);

    //
    // End of Line
    //
    *report+="\n";
  }
  delete q;
}


void ListReports::GenerateXloadReport(QString *report)
{
  QString sql;
  RDSqlQuery *q;
  bool exists=false;

  //
  // Generate Header
  //
  *report=RDReport::center("Rivendell RDCatch Upload/Download Report",132)+"\n";
  *report+=QString("Generated: ")+
    QDateTime::currentDateTime().toString("MM/dd/yyyy")+"\n";
  *report+="\n";
  *report+="T -Start-- -Days of Week- -Location----- -Cut------- -URL------------------------------------- -Username---- -Description------------\n";

  //
  // Generate Rows
  //
  sql=QString("select ")+
    "TYPE,"+          // 00
    "START_TIME,"+    // 01
    "SUN,"+           // 02
    "MON,"+           // 03
    "TUE,"+           // 04
    "WED,"+           // 05
    "THU,"+           // 06
    "FRI,"+           // 07
    "SAT,"+           // 08
    "STATION_NAME,"+  // 09
    "CUT_NAME,"+      // 10
    "URL,"+           // 11
    "URL_USERNAME,"+  // 12
    "DESCRIPTION "+   // 13
    "from RECORDINGS where "+
    QString().sprintf("(TYPE=%d)||",RDRecording::Upload)+
    QString().sprintf("(TYPE=%d) ",RDRecording::Download)+
    "order by START_TIME";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    //
    // Event Type
    //
    switch((RDRecording::Type)q->value(0).toInt()) {
    case RDRecording::Download:
      *report+="D";
      exists=RDCut::exists(q->value(10).toString());
      break;

    case RDRecording::Upload:
      *report+="U";
      exists=RDCut::exists(q->value(10).toString());
      break;

    default:
      *report+="?";
      exists=true;
      break;
    }
    if(exists) {
      *report+=" ";
    }
    else {
      *report+="*";
    }
    
    //
    // Start Time
    //
    *report+=RDReport::leftJustify(q->value(1).toTime().toString("hh:mm:ss"),8)+" ";

    //
    // Days of the Week
    //
    if(q->value(2).toString()=="Y") {
      *report+="Su";
    }
    else {
      *report+="  ";
    }
    if(q->value(3).toString()=="Y") {
      *report+="Mo";
    }
    else {
      *report+="  ";
    }
    if(q->value(4).toString()=="Y") {
      *report+="Tu";
    }
    else {
      *report+="  ";
    }
    if(q->value(5).toString()=="Y") {
      *report+="We";
    }
    else {
      *report+="  ";
    }
    if(q->value(6).toString()=="Y") {
      *report+="Th";
    }
    else {
      *report+="  ";
    }
    if(q->value(7).toString()=="Y") {
      *report+="Fr";
    }
    else {
      *report+="  ";
    }
    if(q->value(8).toString()=="Y") {
      *report+="Sa ";
    }
    else {
      *report+="   ";
    }

    //
    // Location
    //
    *report+=RDReport::leftJustify(q->value(9).toString(),14)+" ";

    //
    // Cut
    //
    *report+=RDReport::leftJustify(q->value(10).toString(),11)+" ";

    //
    // URL
    //
    *report+=RDReport::leftJustify(q->value(11).toString(),41)+" ";

    //
    // URL Username
    //
    *report+=RDReport::leftJustify(q->value(12).toString(),13)+" ";

    //
    // Description
    //
    *report+=RDReport::leftJustify(q->value(13).toString(),24);

    //
    // End of Line
    //
    *report+="\n";
  }

  delete q;
}


QString ListReports::GetSourceName(const QString &station,int matrix,int input)
{
  QString input_name;
  QString sql=QString("select NAME from INPUTS where ")+
    "(STATION_NAME=\""+RDEscapeString(station)+"\")&&"+
    QString().sprintf("(MATRIX=%d)&&",matrix)+
    QString().sprintf("(NUMBER=%d)",input);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    input_name=q->value(0).toString();
  }
  delete q;
  return input_name;
}


QString ListReports::GetDestinationName(const QString &station,int matrix,
					int output)
{
  QString output_name;
  QString sql=QString("select NAME from OUTPUTS where ")+
    "(STATION_NAME=\""+RDEscapeString(station)+"\")&&"+
    QString().sprintf("(MATRIX=%d)&&",matrix)+
    QString().sprintf("(NUMBER=%d)",output);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    output_name=q->value(0).toString();
  }
  delete q;
  return output_name;
}
