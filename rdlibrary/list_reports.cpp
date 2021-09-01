// list_reports.cpp
//
// List RDLibrary Reports
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

#include <QPushButton>

#include <rdconf.h>
#include <rdreport.h>
#include <rdtextfile.h>

#include "globals.h"
#include "list_reports.h"

ListReports::ListReports(const QString &filter,const QString &grpname,
			 const QString &filter_sql,QWidget *parent)
{
  list_filter=filter;
  list_group=grpname;
  list_filter_sql=filter_sql;

  setWindowTitle("RDLibrary - "+tr("Select Report"));

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
  list_reports_box->insertItem(0,tr("Cart Report"));
  list_reports_box->insertItem(1,tr("Cut Report"));
  list_reports_box->insertItem(2,tr("Cart Data Dump (CSV)"));
  list_reports_label=new QLabel(tr("Type:"),this);
  list_reports_label->setGeometry(10,10,35,19);
  list_reports_label->setFont(labelFont());
  list_reports_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  connect(list_reports_box,SIGNAL(activated(int)),
	  this,SLOT(typeActivatedData(int)));

  //
  // Field Names Checkbox
  //
  list_fieldnames_check=new QCheckBox(this);
  list_fieldnames_check->setGeometry(55,34,15,15);
  list_fieldnames_check->setChecked(true);
  list_fieldnames_check->setDisabled(true);
  list_fieldnames_label=new QLabel(tr("Prepend Field Names"),this);
  list_fieldnames_label->setGeometry(75,32,sizeHint().width()-75,19);
  list_fieldnames_label->setFont(labelFont());
  list_fieldnames_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  list_fieldnames_label->setDisabled(true);

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
  return QSize(350,130);
} 


QSizePolicy ListReports::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListReports::typeActivatedData(int index)
{
  list_fieldnames_check->setEnabled((index==2)||(index==3));
  list_fieldnames_label->setEnabled((index==2)||(index==3));
}


void ListReports::generateData()
{
  QString report;

  switch(list_reports_box->currentIndex()) {
  case 0:  // Cart Report
    GenerateCartReport(&report);
    break;

  case 1:  // Cut Report
    GenerateCutReport(&report);
    break;

  case 2:  // Cart Data Dump (CSV)
    GenerateCartDumpCsv(&report,list_fieldnames_check->isChecked());
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


void ListReports::GenerateCartReport(QString *report)
{
  QString sql;
  RDSqlQuery *q;
  QString schedcode="";

  if(list_schedcode!=tr("ALL")) {
    schedcode=list_schedcode;
  }

  //
  // Generate Header
  //
  QString filter=list_filter;
  if(list_filter.isEmpty()) {
    filter="[none]";
  }
  *report=RDReport::center("Rivendell Cart Report",132)+"\n";
  *report+=RDReport::center(QString("Generated: ")+
			    QDateTime::currentDateTime().toString("MM/dd/yyyy - hh:mm:ss")+"     Group: "+list_group+"     Filter: "+filter,132)+"\n";
  *report+="Type -Cart- -Group---- -Len- -Title------------------------- -Artist----------------------- Cuts Rot Enf -LenDev -Owner--------------\n";

  //
  // Generate Rows
  //
  sql=QString("select ")+
    "`CART`.`TYPE`,"+              // 00
    "`CART`.`NUMBER`,"+            // 01
    "`CART`.`GROUP_NAME`,"+        // 02
    "`CART`.`FORCED_LENGTH`,"+     // 03
    "`CART`.`TITLE`,"+             // 04
    "`CART`.`ARTIST`,"+            // 05
    "`CART`.`CUT_QUANTITY`,"+      // 06
    "`CART`.`USE_WEIGHTING`,"+     // 07
    "`CART`.`ENFORCE_LENGTH`,"+    // 08
    "`CART`.`LENGTH_DEVIATION`,"+  // 09
    "`CART`.`OWNER` "+             // 10
    "from `CART` "+
    "left join `GROUPS` on `CART`.`GROUP_NAME`=`GROUPS`.`NAME` "+
    "left join `CUTS` on `CART`.`NUMBER`=`CUTS`.`CART_NUMBER` ";
  sql+=list_filter_sql;
  unsigned prev_cartnum=0;
  q=new RDSqlQuery(sql);
  while(q->next()) {
    if(q->value(1).toUInt()!=prev_cartnum) { // So we don't show duplicates 
      prev_cartnum=q->value(1).toUInt();
      
      //
      // Cart Type
      //
      switch((RDCart::Type)q->value(0).toInt()) {
      case RDCart::Audio:
	*report+="  A  ";
	break;

      case RDCart::Macro:
	*report+="  M  ";
	break;

      default:
	*report+="  ?  ";
	break;
      }

      //
      // Cart Number
      //
      *report+=QString::asprintf("%06u ",q->value(1).toUInt());

      //
      // Group
      //
      *report+=RDReport::leftJustify(q->value(2).toString(),10)+" ";

      //
      // Length
      //
      *report+=RDReport::rightJustify(RDGetTimeLength(q->value(3).toInt(),false,false),5)+" ";

      //
      // Title
      //
      *report+=RDReport::leftJustify(q->value(4).toString(),31)+" ";

      //
      // Artist
      //
      *report+=RDReport::leftJustify(q->value(5).toString(),30)+" ";

      //
      // Cut Quantity
      //
      *report+=QString::asprintf("%4d ",q->value(6).toInt());

      //
      // Play Order
      //
      if(q->value(7).toString()=="Y") {
	*report+="WTD ";
      }
      else {
	*report+="ORD ";
      }

      //
      // Enforce Length
      //
      if(q->value(8).toString()=="Y") {
	*report+="Yes ";
      }
      else {
	*report+="No  ";
      }

      //
      // Length Deviation
      //
      *report+=RDReport::rightJustify(RDGetTimeLength(q->value(9).toInt(),false,true),7)+" ";

      //
      // Owner
      //
      if(q->value(10).toString().isEmpty()) {
	*report+="[none]    ";
      }
      else {
	*report+=RDReport::leftJustify(q->value(10).toString(),20);
      }

      //
      // End of Line
      //
      *report+="\n";
    }
  }
  delete q;
}


void ListReports::GenerateCutReport(QString *report)
{
  QString sql;
  RDSqlQuery *q;
  unsigned current_cart=0;
  QString schedcode="";

  if(list_schedcode!=tr("ALL")) {
    schedcode=list_schedcode;
  }

  //
  // Generate Header
  //
  QString filter=list_filter;
  if(list_filter.isEmpty()) {
    filter="[none]";
  }
  *report=RDReport::center("Rivendell Cut Report",136)+"\n";
  *report+=RDReport::center(QString("Generated: ")+
			    QDateTime::currentDateTime().toString("MM/dd/yyyy - hh:mm:ss")+"     Group: "+list_group+"     Filter: "+filter,136)+"\n";
  *report+="-Cart- Cut W/O- -Cart Title-------------- -Description-- -Len- Last Play-- Plays Start Date- End Date--- -Days of Week- -Daypart----------------\n";

  //
  // Generate Rows
  //
  sql=QString("select ")+
    "`CART`.`NUMBER`,"+              // 00
    "`CUTS`.`CUT_NAME`,"+            // 01
    "`CART`.`USE_WEIGHTING`,"+       // 02
    "`CUTS`.`PLAY_ORDER`,"+          // 03
    "`CUTS`.`WEIGHT`,"+              // 04
    "`CART`.`TITLE`,"+               // 05
    "`CUTS`.`DESCRIPTION`,"+         // 06
    "`CUTS`.`LENGTH`,"+              // 07
    "`CUTS`.`LAST_PLAY_DATETIME`,"+  // 08
    "`CUTS`.`PLAY_COUNTER`,"+        // 09
    "`CUTS`.`START_DATETIME`,"+      // 10
    "`CUTS`.`END_DATETIME`,"+        // 11
    "`CUTS`.`SUN`,"+                 // 12
    "`CUTS`.`MON`,"+                 // 13
    "`CUTS`.`TUE`,"+                 // 14
    "`CUTS`.`WED`,"+                 // 15
    "`CUTS`.`THU`,"+                 // 16
    "`CUTS`.`FRI`,"+                 // 17
    "`CUTS`.`SAT`,"+                 // 18
    "`CUTS`.`START_DAYPART`,"+       // 19
    "`CUTS`.`END_DAYPART` "+         // 20
    "from `CART` "+
    "left join `GROUPS` on `CART`.`GROUP_NAME`=`GROUPS`.`NAME` "+
    "left join `CUTS` on `CART`.`NUMBER`=`CUTS`.`CART_NUMBER` ";
  sql+=list_filter_sql;
  q=new RDSqlQuery(sql);
  while(q->next()) {
    //
    // Cart Number
    //
    if(q->value(0).toUInt()!=current_cart) {
      *report+=QString::asprintf("%06u ",q->value(0).toUInt());
    }
    else {
      *report+="       ";
    }

    //
    // Cut Number
    //
    *report+=
      QString::asprintf("%03d ",q->value(1).toString().right(3).toInt());

    //
    // Weight /  Play Order
    //
    if(q->value(2).toString()=="Y") {
      *report+=QString::asprintf("W%03d ",q->value(4).toInt());
    }
    else {
      *report+=QString::asprintf("O%03d ",q->value(3).toInt());
    }

    //
    // Title
    //
    if(q->value(0).toUInt()!=current_cart) {
      *report+=RDReport::leftJustify(q->value(5).toString(),25)+" ";
    }
    else {
      *report+="                          ";
    }

    //
    // Description
    //
    *report+=RDReport::leftJustify(q->value(6).toString(),14)+" ";

    //
    // Length
    //
    *report+=RDReport::rightJustify(RDGetTimeLength(q->value(7).toInt(),
						    false,false),5)+" ";

    //
    // Last Play
    //
    if(q->value(8).toDateTime().isNull()) {
      *report+="   [none]    ";
    }
    else {
      *report+=RDReport::center(rda->shortDateString(q->value(8).toDate()),12)+"  ";
    }

    //
    // Plays
    //
    *report+=QString::asprintf("%4d ",q->value(9).toInt());

    //
    // Start Date
    //
    if(q->value(10).toDateTime().isNull()) {
      *report+="  [none]   ";
    }
    else {
      *report+=RDReport::center(rda->shortDateString(q->value(10).toDateTime().date()),12)+" ";
    }

    //
    // End Date
    //
    if(q->value(11).toDateTime().isNull()) {
      *report+="    TFN      ";
    }
    else {
      *report+=RDReport::center(rda->shortDateString(q->value(11).toDateTime().date()),12)+" ";
    }

    //
    // Days of the Week
    //
    if(q->value(12).toString()=="Y") {
      *report+="Su";
    }
    else {
      *report+="  ";
    }
    if(q->value(13).toString()=="Y") {
      *report+="Mo";
    }
    else {
      *report+="  ";
    }
    if(q->value(14).toString()=="Y") {
      *report+="Tu";
    }
    else {
      *report+="  ";
    }
    if(q->value(15).toString()=="Y") {
      *report+="We";
    }
    else {
      *report+="  ";
    }
    if(q->value(16).toString()=="Y") {
      *report+="Th";
    }
    else {
      *report+="  ";
    }
    if(q->value(17).toString()=="Y") {
      *report+="Fr";
    }
    else {
      *report+="  ";
    }
    if(q->value(18).toString()=="Y") {
      *report+="Sa ";
    }
    else {
      *report+="   ";
    }

    //
    // Dayparts
    //
    if(q->value(20).toTime().isNull()) {
      *report+="[none]";
    }
    else {
      *report+=rda->timeString(q->value(19).toTime())+" - "+
	rda->timeString(q->value(20).toTime());
    }

    //
    // End of Line
    //
    *report+="\n";

    current_cart=q->value(0).toUInt();
  }
  delete q;
}


void ListReports::GenerateCartDumpCsv(QString *report,bool prepend_names)
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;
  QString schedcode="";
  QStringList f0;

  if(list_schedcode!=tr("ALL")) {
    schedcode=list_schedcode;
  }

  //
  // Generate Rows
  //
  sql=QString("select ")+
    "`CART`.`NUMBER`,"+             // 00
    "`CART`.`TYPE`,"+               // 01
    "`CUTS`.`CUT_NAME`,"+           // 02
    "`CART`.`GROUP_NAME`,"+         // 03
    "`CART`.`TITLE`,"+              // 04
    "`CART`.`ARTIST`,"+             // 05
    "`CART`.`ALBUM`,"+              // 06
    "`CART`.`YEAR`,"+               // 07
    "`CUTS`.`ISRC`,"+               // 08
    "`CUTS`.`ISCI`,"+               // 09
    "`CART`.`LABEL`,"+              // 10
    "`CART`.`CLIENT`,"+             // 11
    "`CART`.`AGENCY`,"+             // 12
    "`CART`.`PUBLISHER`,"+          // 13
    "`CART`.`COMPOSER`,"+           // 14
    "`CART`.`CONDUCTOR`,"+          // 15
    "`CART`.`SONG_ID`,"+            // 16
    "`CART`.`USER_DEFINED`,"+       // 17
    "`CUTS`.`DESCRIPTION`,"+        // 18
    "`CUTS`.`OUTCUE`,"+             // 19
    "`CUTS`.`LENGTH`,"+             // 20
    "`CUTS`.`START_POINT`,"+        // 21
    "`CUTS`.`END_POINT`,"+          // 22
    "`CUTS`.`SEGUE_START_POINT`,"+  // 23
    "`CUTS`.`SEGUE_END_POINT`,"+    // 24
    "`CUTS`.`HOOK_START_POINT`,"+   // 25
    "`CUTS`.`HOOK_END_POINT`,"+     // 26
    "`CUTS`.`TALK_START_POINT`,"+   // 27
    "`CUTS`.`TALK_END_POINT`,"+     // 28
    "`CUTS`.`FADEUP_POINT`,"+       // 29
    "`CUTS`.`FADEDOWN_POINT` "+     // 30
    "from `CART` "+
    "left join `GROUPS` on `CART`.`GROUP_NAME`=`GROUPS`.`NAME` "+
    "left join `CUTS` on `CART`.`NUMBER`=`CUTS`.`CART_NUMBER` ";
  sql+=list_filter_sql;
  q=new RDSqlQuery(sql);

  //
  // Prepend Field Names
  //
  if(prepend_names) {
    *report=QString("CART_NUMBER,")+  // 00
      "CUT_NUMBER,"+                  // 01
      "TYPE,"+                        // 02
      "GROUP_NAME,"+                  // 03
      "TITLE,"+                       // 04
      "ARTIST,"+                      // 05
      "ALBUM,"+                       // 06
      "YEAR,"+                        // 07
      "ISRC,"+                        // 08
      "ISCI,"+                        // 09
      "LABEL,"+                       // 10
      "CLIENT,"+                      // 11
      "AGENCY,"+                      // 12
      "PUBLISHER,"+                   // 13
      "COMPOSER,"+                    // 14
      "CONDUCTOR,"+                   // 15
      "SONG_ID,"+                     // 16
      "USER_DEFINED,"+                // 17
      "DESCRIPTION,"+                 // 18
      "OUTCUE,"+                      // 19
      "FILENAME,LENGTH,"+             // 20
      "START_POINT,"+                 // 21
      "END_POINT,"+                   // 22
      "SEGUE_START_POINT,"+           // 23
      "SEGUE_END_POINT,"+             // 24
      "HOOK_START_POINT,"+            // 25
      "HOOK_END_POINT,"+              // 26
      "TALK_START_POINT,"+            // 27
      "TALK_END_POINT,"+              // 28
      "FADEUP_POINT,"+                // 29
      "FADEDOWN_POINT,"+              // 30
      "SCHED_CODES";                  // 31
    *report+="\n";
  }

  //
  // Generate Rows
  //
  while(q->next()) {
    RDCart::Type type=(RDCart::Type)q->value(1).toInt();
    *report+=QString::asprintf("%u,",q->value(0).toUInt());
    if(type==RDCart::Macro) {
      *report+="0,macro,";
    }
    else {
      *report+=QString::asprintf("%u,",RDCut::cutNumber(q->value(2).toString()));
      *report+="audio,";
    }
    *report+=CsvField(q->value(3).toString())+",";
    *report+=CsvField(q->value(4).toString())+",";
    *report+=CsvField(q->value(5).toString())+",";
    *report+=CsvField(q->value(6).toString())+",";
    *report+=CsvField(q->value(7).toDate().toString("yyyy"))+",";
    *report+=CsvField(q->value(8).toString())+",";
    *report+=CsvField(q->value(9).toString())+",";
    *report+=CsvField(q->value(10).toString())+",";
    *report+=CsvField(q->value(11).toString())+",";
    *report+=CsvField(q->value(12).toString())+",";
    *report+=CsvField(q->value(13).toString())+",";
    *report+=CsvField(q->value(14).toString())+",";
    *report+=CsvField(q->value(15).toString())+",";
    *report+=CsvField(q->value(16).toString())+",";
    *report+=CsvField(q->value(17).toString())+",";
    *report+=CsvField(q->value(18).toString())+",";
    *report+=CsvField(q->value(19).toString())+",";
    if(type==RDCart::Macro) {
      *report+=",";
    }
    else {
      *report+=CsvField(q->value(2).toString()+".wav")+",";
    }
    *report+=
      RDGetTimeLength(q->value(20).toInt(),false,false).trimmed()+",";
    if(type==RDCart::Macro) {
      *report+="-1,";
      *report+="-1,";
      *report+="-1,";
      *report+="-1,";
      *report+="-1,";
      *report+="-1,";
      *report+="-1,";
      *report+="-1,";
      *report+="-1,";
      *report+="-1,";
    }
    else {
      *report+=QString::asprintf("%d,",q->value(21).toInt());
      *report+=QString::asprintf("%d,",q->value(22).toInt());
      *report+=QString::asprintf("%d,",q->value(23).toInt());
      *report+=QString::asprintf("%d,",q->value(24).toInt());
      *report+=QString::asprintf("%d,",q->value(25).toInt());
      *report+=QString::asprintf("%d,",q->value(26).toInt());
      *report+=QString::asprintf("%d,",q->value(27).toInt());
      *report+=QString::asprintf("%d,",q->value(28).toInt());
      *report+=QString::asprintf("%d,",q->value(29).toInt());
      *report+=QString::asprintf("%d,",q->value(30).toInt());
    }
    sql=QString("select SCHED_CODE from CART_SCHED_CODES where ")+
      QString::asprintf("CART_NUMBER=%u",q->value(0).toUInt());
    QString schedcodes="";
    q1=new RDSqlQuery(sql);
    while(q1->next()) {
      schedcodes+=q1->value(0).toString()+"|";
    }
    if(schedcodes.right(1)=="|") {
      schedcodes=schedcodes.left(schedcodes.length()-1);
    }
    *report+=CsvField(schedcodes);
    delete q1;
    *report+="\n";
  }
}


QString ListReports::CsvField(QString str) const
{
  return "\""+str.replace("\"","\"\"")+"\"";
}
