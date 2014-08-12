// list_reports.cpp
//
// List and Generate RDCatch Reports
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_reports.cpp,v 1.8 2010/07/29 19:32:36 cvs Exp $
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

#include <rddb.h>
#include <rdconf.h>
#include <rdtextfile.h>
#include <rdcart_search_text.h>
#include <rdcart.h>
#include <rdrecording.h>

#include <globals.h>
#include <list_reports.h>


ListReports::ListReports(bool today_only,bool active_only,int dow,
			 QWidget *parent,const char *name)
  : QDialog(parent,name,true)
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

  setCaption(tr("RDLibrary Reports"));

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);

  //
  // Reports List
  //
  list_reports_box=new QComboBox(this,"list_reports_box");
  list_reports_box->setGeometry(50,10,sizeHint().width()-60,19);
  list_reports_box->insertItem(tr("Event Report"));
  list_reports_box->insertItem(tr("Upload/Download Report"));
  QLabel *list_reports_label=
    new QLabel(list_reports_box,tr("Type:"),
	       this,"list_reports_label");
  list_reports_label->setGeometry(10,10,35,19);
  list_reports_label->setFont(font);
  list_reports_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  //  Generate Button
  //
  QPushButton *generate_button=new QPushButton(this,"generate_button");
  generate_button->
    setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  generate_button->setDefault(true);
  generate_button->setFont(font);
  generate_button->setText(tr("&Generate"));
  connect(generate_button,SIGNAL(clicked()),this,SLOT(generateData()));

  //
  //  Close Button
  //
  QPushButton *close_button=new QPushButton(this,"close_button");
  close_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  close_button->setFont(font);
  close_button->setText(tr("&Close"));
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

  switch(list_reports_box->currentItem()) {
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
  *report="                                                  Rivendell RDCatch Event Report\n";
  *report+=QString().
    sprintf("Generated: %s\n",
	    (const char *)QDateTime(QDate::currentDate(),QTime::currentTime()).
	    toString("MM/dd/yyyy - hh:mm:ss"));
  *report+="\n";
  *report+="T -Start-------- -End---------- -Days of Week- -Location----- -Source------------- -Destination-------- -Description-----------------\n";

  //
  // Generate Rows
  //
  sql="select TYPE,START_TYPE,START_TIME,END_TYPE,END_TIME,LENGTH,SUN,MON,\
       TUE,WED,THU,FRI,SAT,STATION_NAME,CHANNEL,CUT_NAME,URL,MACRO_CART,\
       SWITCH_INPUT,SWITCH_OUTPUT,DESCRIPTION \
       from RECORDINGS order by START_TIME";
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
	  *report+=QString().
	    sprintf("Hard: %-8s ",(const char *)q->value(2).toTime().
		    toString("hh:mm:ss"));
	  break;

	case RDRecording::GpiStart:
	  *report+=QString().
	    sprintf("Gpi: %-8s  ",(const char *)q->value(2).toTime().
		    toString("hh:mm:ss"));
	  break;
    }

    //
    // End Time
    //
    switch((RDRecording::Type)q->value(0).toInt()) {
	case RDRecording::Recording:
	  switch((RDRecording::EndType)q->value(3).toInt()) {
	      case RDRecording::HardEnd:
		*report+=QString().
		  sprintf("Hard: %-8s ",(const char *)q->value(4).toTime().
			  toString("hh:mm:ss"));
		break;
		
	      case RDRecording::GpiEnd:
		*report+=QString().
		  sprintf("Gpi: %-8s  ",(const char *)q->value(4).toTime().
			  toString("hh:mm:ss"));
		break;
		
	      case RDRecording::LengthEnd:
		*report+=QString().sprintf("Len: %-8s  ",
		  (const char *)RDGetTimeLength(q->value(5).toInt(),false,false));
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
	  str=QString().sprintf("%s:%d",
				 (const char *)q->value(13).toString().left(12),
				 q->value(14).toInt());
	  break;

	case RDRecording::Playout:
	  str=QString().sprintf("%s:%d",
				 (const char *)q->value(13).toString().left(12),
				 q->value(14).toInt()-128);
	  break;

	default:
	  str=q->value(13).toString().left(14);
	  break;
    }
    *report+=QString().sprintf("%-14s ",(const char *)str);

    //
    // Source and Destination
    //
    switch((RDRecording::Type)q->value(0).toInt()) {
	case RDRecording::Recording:
	  sql=QString().sprintf("select SWITCH_STATION,SWITCH_MATRIX\
                                 from DECKS where \
                                 (STATION_NAME=\"%s\")&&(CHANNEL=%d)",
				(const char *)q->value(13).toString(),
				q->value(14).toInt());
	  q1=new RDSqlQuery(sql);
	  if(q1->first()) {
	    *report+=QString().sprintf("%-20s ",
		    (const char *)GetSourceName(q1->value(0).toString(),
						q1->value(1).toInt(),
						q->value(18).toInt()).left(20));
	  }
	  else {
	    *report+="                     ";
	  }
	  delete q1;
	  *report+=QString().sprintf("Cut %10s       ",
			   (const char *)q->value(15).toString().left(20));
	  break;

	case RDRecording::MacroEvent:
	  *report+=QString().sprintf("Cart %06u          ",
				     q->value(17).toUInt());
	  *report+="                     ";
	  break;

	case RDRecording::SwitchEvent:
	  *report+=QString().sprintf("%-20s ",
	      (const char *)GetSourceName(q->value(13).toString(),
					  q->value(14).toInt(),
					  q->value(18).toInt()).left(20));
	  *report+=QString().sprintf("%-20s ",
	      (const char *)GetDestinationName(q->value(13).toString(),
					       q->value(14).toInt(),
					       q->value(19).toInt()).left(20));
	  break;

	case RDRecording::Playout:
	  *report+=QString().sprintf("Cut %10s       ",
			   (const char *)q->value(15).toString().left(20));
	  *report+="                     ";
	  break;

	case RDRecording::Download:
	  *report+=QString().sprintf("%-20s ",
			   (const char *)q->value(16).toString().left(20));
	  *report+=QString().sprintf("Cut %10s       ",
			   (const char *)q->value(15).toString().left(20));
	  break;

	case RDRecording::Upload:
	  *report+=QString().sprintf("Cut %10s       ",
			   (const char *)q->value(15).toString().left(20));
	  *report+=QString().sprintf("%-20s ",
			   (const char *)q->value(16).toString().left(20));
	  break;
    }

    //
    // Description
    //
    *report+=QString().sprintf("%s",
			       (const char *)q->value(20).toString().left(29));

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
  *report="                                             Rivendell RDCatch Upload/Download Report\n";
  *report+=QString().
    sprintf("Generated: %s\n",
	    (const char *)QDateTime(QDate::currentDate(),QTime::currentTime()).
	    toString("MM/dd/yyyy - hh:mm:ss"));
  *report+="\n";
  *report+="T -Start-- -Days of Week- -Location----- -Cut------- -URL------------------------------------- -Username---- -Description------------\n";

  //
  // Generate Rows
  //
  sql=QString().sprintf("select TYPE,START_TIME,SUN,MON,TUE,WED,THU,FRI,SAT,\
       STATION_NAME,CUT_NAME,URL,URL_USERNAME,DESCRIPTION \
       from RECORDINGS where (TYPE=%d)||(TYPE=%d) order by START_TIME",
			RDRecording::Upload,RDRecording::Download);
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
    *report+=QString().sprintf("%8s ",
		    (const char *)q->value(1).toTime().toString("hh:mm:ss"));

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
    *report+=QString().sprintf("%-14s ",
			       (const char *)q->value(9).toString().left(14));

    //
    // Cut
    //
    *report+=QString().sprintf("%11s ",(const char *)q->value(10).toString());

    //
    // URL
    //
    *report+=QString().sprintf("%-41s ",
			       (const char *)q->value(11).toString().left(41));

    //
    // URL Username
    //
    *report+=QString().sprintf("%-13s ",
			       (const char *)q->value(12).toString().left(13));

    //
    // Description
    //
    *report+=QString().sprintf("%s",
			       (const char *)q->value(13).toString().left(24));

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
  QString sql=QString().sprintf("select NAME from INPUTS where \
                                 (STATION_NAME=\"%s\")&&\
                                 (MATRIX=%d)&&(NUMBER=%d)",
				(const char *)station,
				matrix,input);
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
  QString sql=QString().sprintf("select NAME from OUTPUTS where \
                                 (STATION_NAME=\"%s\")&&\
                                 (MATRIX=%d)&&(NUMBER=%d)",
				(const char *)station,
				matrix,output);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    output_name=q->value(0).toString();
  }
  delete q;
  return output_name;
}
