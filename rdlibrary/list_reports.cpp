// list_reports.cpp
//
// List RDLibrary Reports
//
//   (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
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
#include <qstringlist.h>

#include <rdapplication.h>
#include <rdcart.h>
#include <rdcart_search_text.h>
#include <rdconf.h>
#include <rddb.h>
#include <rdreport.h>
#include <rdtextfile.h>

#include "globals.h"
#include "list_reports.h"

ListReports::ListReports(const QString &filter,const QString &type_filter,
			 const QString &group,const QString &schedcode,
			 QWidget *parent)
  : QDialog(parent)
{
  setModal(true);

  list_filter=filter;
  list_type_filter=type_filter;
  list_group=group;
  list_schedcode=schedcode;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  setWindowTitle("RDLibrary - "+tr("Select Report"));

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);

  //
  // Reports List
  //
  list_reports_box=new QComboBox(this);
  list_reports_box->setGeometry(50,10,sizeHint().width()-60,19);
  list_reports_box->insertItem(tr("Cart Report"));
  list_reports_box->insertItem(tr("Cut Report"));
  list_reports_box->insertItem(tr("Cart Data Dump (fixed width)"));
  list_reports_box->insertItem(tr("Cart Data Dump (CSV)"));
  list_reports_label=new QLabel(list_reports_box,tr("Type:"),this);
  list_reports_label->setGeometry(10,10,35,19);
  list_reports_label->setFont(font);
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
  list_fieldnames_label=
    new QLabel(list_fieldnames_check,tr("Prepend Field Names"),this);
  list_fieldnames_label->setGeometry(75,32,sizeHint().width()-75,19);
  list_fieldnames_label->setFont(font);
  list_fieldnames_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  list_fieldnames_label->setDisabled(true);

  //
  //  Generate Button
  //
  QPushButton *generate_button=new QPushButton(this);
  generate_button->
    setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  generate_button->setDefault(true);
  generate_button->setFont(font);
  generate_button->setText(tr("&Generate"));
  connect(generate_button,SIGNAL(clicked()),this,SLOT(generateData()));

  //
  //  Close Button
  //
  QPushButton *close_button=new QPushButton(this);
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

  switch(list_reports_box->currentItem()) {
  case 0:  // Cart Report
    GenerateCartReport(&report);
    break;

  case 1:  // Cut Report
    GenerateCutReport(&report);
    break;

  case 2:  // Cart Data Dump (fixed)
    GenerateCartDumpFixed(&report,list_fieldnames_check->isChecked());
    break;

  case 3:  // Cart Data Dump (CSV)
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
  if(list_type_filter.isEmpty()) {
    return;
  }
  sql=QString("select ")+
    "CART.TYPE,"+              // 00
    "CART.NUMBER,"+            // 01
    "CART.GROUP_NAME,"+        // 02
    "CART.FORCED_LENGTH,"+     // 03
    "CART.TITLE,"+             // 04
    "CART.ARTIST,"+            // 05
    "CART.CUT_QUANTITY,"+      // 06
    "CART.USE_WEIGHTING,"+     // 07
    "CART.ENFORCE_LENGTH,"+    // 08
    "CART.LENGTH_DEVIATION,"+  // 09
    "CART.OWNER "+             // 10
    "from CART left join CUTS on "+
    "CART.NUMBER=CUTS.CART_NUMBER ";
  if(list_group==QString("ALL")) {
    sql+=RDAllCartSearchText(list_filter,schedcode,rda->user()->name(),true)+" && "+
      list_type_filter+" order by NUMBER";
  }
  else {
    sql+=RDCartSearchText(list_filter,list_group,schedcode,true)+" && "+
      list_type_filter+" order by NUMBER";
  }
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
      *report+=QString().sprintf("%06u ",q->value(1).toUInt());

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
      *report+=QString().sprintf("%4d ",q->value(6).toInt());

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
  *report=RDReport::center("Rivendell Cut Report",132)+"\n";
  *report+=RDReport::center(QString("Generated: ")+
			    QDateTime::currentDateTime().toString("MM/dd/yyyy - hh:mm:ss")+"     Group: "+list_group+"     Filter: "+filter,132)+"\n";
  *report+="-Cart- Cut Wht -Cart Title-------------- -Description--- -Len- Last Play Plays Start Date End Date -Days of Week- -Daypart-----------\n";

  //
  // Generate Rows
  //
  if(list_type_filter.isEmpty()) {
    return;
  }
  sql=QString("select ")+
    "CART.NUMBER,"+              // 00
    "CUTS.CUT_NAME,"+            // 01
    "CUTS.WEIGHT,"+              // 02
    "CART.TITLE,"+               // 03
    "CUTS.DESCRIPTION,"+         // 04
    "CUTS.LENGTH,"+              // 05
    "CUTS.LAST_PLAY_DATETIME,"+  // 06
    "CUTS.PLAY_COUNTER,"+        // 07
    "CUTS.START_DATETIME,"+      // 08
    "CUTS.END_DATETIME,"+        // 09
    "CUTS.SUN,"+                 // 10
    "CUTS.MON,"+                 // 11
    "CUTS.TUE,"+                 // 12
    "CUTS.WED,"+                 // 13
    "CUTS.THU,"+                 // 14
    "CUTS.FRI,"+                 // 15
    "CUTS.SAT,"+                 // 16
    "CUTS.START_DAYPART,"+       // 17
    "CUTS.END_DAYPART "+         // 18
    "from CART join CUTS "+
    "on CART.NUMBER=CUTS.CART_NUMBER ";
  if(list_group==QString("ALL")) {
    sql+=RDAllCartSearchText(list_filter,schedcode,rda->user()->name(),true)+" && "+
      list_type_filter+" order by CART.NUMBER";
  }
  else {
    sql+=RDCartSearchText(list_filter,list_group,schedcode,true)+" && "+
      list_type_filter+" order by CART.NUMBER";
  }
  q=new RDSqlQuery(sql);
  while(q->next()) {
    //
    // Cart Number
    //
    if(q->value(0).toUInt()!=current_cart) {
      *report+=QString().sprintf("%06u ",q->value(0).toUInt());
    }
    else {
      *report+="       ";
    }

    //
    // Cut Number
    //
    *report+=
      QString().sprintf("%03d ",q->value(1).toString().right(3).toInt());

    //
    // Weight
    //
    *report+=QString().sprintf("%3d ",q->value(2).toInt());

    //
    // Title
    //
    if(q->value(0).toUInt()!=current_cart) {
      *report+=RDReport::leftJustify(q->value(3).toString(),25)+" ";
    }
    else {
      *report+="                          ";
    }

    //
    // Description
    //
    *report+=RDReport::leftJustify(q->value(4).toString(),15)+" ";

    //
    // Length
    //
    *report+=RDReport::rightJustify(RDGetTimeLength(q->value(5).toInt(),
						    false,false),5)+" ";

    //
    // Last Play
    //
    if(q->value(6).toDateTime().isNull()) {
      *report+="  [none]   ";
    }
    else {
      *report+=RDReport::center(q->value(6).toDate().toString("MM/dd/yy"),10)+"  ";
    }

    //
    // Plays
    //
    *report+=QString().sprintf("%4d ",q->value(7).toInt());

    //
    // Start Date
    //
    if(q->value(8).toDateTime().isNull()) {
      *report+="  [none]  ";
    }
    else {
      *report+=RDReport::center(q->value(8).toDateTime().toString("MM/dd/yy"),10)+" ";
    }

    //
    // End Date
    //
    if(q->value(9).toDateTime().isNull()) {
      *report+="   TFN    ";
    }
    else {
      *report+=RDReport::center(q->value(9).toDateTime().toString("MM/dd/yy"),10)+" ";
    }

    //
    // Days of the Week
    //
    if(q->value(10).toString()=="Y") {
      *report+="Su";
    }
    else {
      *report+="  ";
    }
    if(q->value(11).toString()=="Y") {
      *report+="Mo";
    }
    else {
      *report+="  ";
    }
    if(q->value(12).toString()=="Y") {
      *report+="Tu";
    }
    else {
      *report+="  ";
    }
    if(q->value(13).toString()=="Y") {
      *report+="We";
    }
    else {
      *report+="  ";
    }
    if(q->value(14).toString()=="Y") {
      *report+="Th";
    }
    else {
      *report+="  ";
    }
    if(q->value(15).toString()=="Y") {
      *report+="Fr";
    }
    else {
      *report+="  ";
    }
    if(q->value(16).toString()=="Y") {
      *report+="Sa ";
    }
    else {
      *report+="   ";
    }

    //
    // Dayparts
    //
    if(q->value(18).toTime().isNull()) {
      *report+="[none]";
    }
    else {
      *report+=q->value(17).toTime().toString("hh:mm:ss")+" - "+
	q->value(18).toTime().toString("hh:mm:ss");
    }

    //
    // End of Line
    //
    *report+="\n";

    current_cart=q->value(0).toUInt();
  }
  delete q;
}


void ListReports::GenerateCartDumpFixed(QString *report,bool prepend_names)
{
  QString sql;
  RDSqlQuery *q;
  QString schedcode="";

  if(list_schedcode!=tr("ALL")) {
    schedcode=list_schedcode;
  }

  //
  // Prepend Field Names
  //
  if(prepend_names) {
    *report="CART  |";
    *report+="CUT|";
    *report+="GROUP_NAME|";
    *report+="TITLE                                                                                                                                                                                                                                                          |";
    *report+="ARTIST                                                                                                                                                                                                                                                         |";
    *report+="ALBUM                                                                                                                                                                                                                                                          |";
    *report+="YEAR|";
    *report+="ISRC        |";
    *report+="LABEL                                                           |";
    *report+="CLIENT                                                          |";
    *report+="AGENCY                                                          |";
    *report+="PUBLISHER                                                       |";
    *report+="COMPOSER                                                        |";
    *report+="USER_DEFINED                                                                                                                                                                                                                                                   |";
    *report+="LENGTH   |\n";
  }

  //
  // Generate Rows
  //
  if(list_type_filter.isEmpty()) {
    return;
  }
  sql=QString("select ")+
    "CUTS.CUT_NAME,"+      // 00
    "CART.GROUP_NAME,"+    // 01
    "CART.TITLE,"+         // 02
    "CART.ARTIST,"+        // 03
    "CART.ALBUM,"+         // 04
    "CART.YEAR,"+          // 05
    "CUTS.ISRC,"+          // 06
    "CART.LABEL,"+         // 07
    "CART.CLIENT,"+        // 08
    "CART.AGENCY,"+        // 09
    "CART.PUBLISHER,"+     // 10
    "CART.COMPOSER,"+      // 11
    "CART.USER_DEFINED,"+  // 12
    "CUTS.LENGTH "+        // 13
    "from CART join CUTS "+
    "on CART.NUMBER=CUTS.CART_NUMBER ";
  if(list_group==QString("ALL")) {
    sql+=RDAllCartSearchText(list_filter,schedcode,rda->user()->name(),true)+" && "+
      list_type_filter+" order by CUTS.CUT_NAME";
  }
  else {
    sql+=RDCartSearchText(list_filter,list_group,schedcode,true)+" && "+
      list_type_filter+" order by CUTS.CUT_NAME";
  }
  q=new RDSqlQuery(sql);
  while(q->next()) {
    //
    // Cart Number
    //
    *report+=QString().sprintf("%-6s|",(const char *)q->value(0).toString().
			       utf8().left(6));

    //
    // Cut Number
    //
    *report+=
      QString().sprintf("%-3s|",(const char *)q->value(0).toString().right(3));

    //
    // Group Name
    //
    *report+=QString().sprintf("%-10s|",(const char *)q->value(1).toString().
			       utf8());

    //
    // Title
    //
    *report+=QString().sprintf("%-255s|",(const char *)q->value(2).toString().
			       utf8());

    //
    // Artist
    //
    *report+=QString().sprintf("%-255s|",(const char *)q->value(3).toString().
			       utf8());

    //
    // Album
    //
    *report+=QString().sprintf("%-255s|",(const char *)q->value(4).toString().
			       utf8());

    //
    // Year
    //
    if(q->value(5).toDate().isNull()) {
      *report+="    |";
    }
    else {
      *report+=QString().sprintf("%4d|",q->value(5).toDate().year());
    }

    //
    // ISRC
    //
    *report+=QString().sprintf("%-12s|",(const char *)q->value(6).toString().
			       utf8());

    //
    // Label
    //
    *report+=QString().sprintf("%-64s|",(const char *)q->value(7).toString().
			       utf8());

    //
    // Client
    //
    *report+=QString().sprintf("%-64s|",(const char *)q->value(8).toString().
			       utf8());

    //
    // Agency
    //
    *report+=QString().sprintf("%-64s|",(const char *)q->value(9).toString().
			       utf8());

    //
    // Publisher
    //
    *report+=QString().sprintf("%-64s|",(const char *)q->value(10).toString().
			       utf8());

    //
    // Composer
    //
    *report+=QString().sprintf("%-64s|",(const char *)q->value(11).toString().
			       utf8());

    //
    // User Defined
    //
    *report+=QString().sprintf("%-255s|",(const char *)q->value(12).toString().
			       utf8());

    //
    // Length
    //
    *report+=QString().sprintf("%9s|",
	     (const char *)RDGetTimeLength(q->value(13).toInt(),true,true));

    //
    // End of Line
    //
    *report+="\n";
  }

  delete q;
}


void ListReports::GenerateCartDumpCsv(QString *report,bool prepend_names)
{
  QString sql;
  RDSqlQuery *q;
  QString schedcode="";
  QStringList f0;
  int code_quan=0;

  if(list_schedcode!=tr("ALL")) {
    schedcode=list_schedcode;
  }

  //
  // Generate Rows
  //
  if(list_type_filter.isEmpty()) {
    return;
  }
  sql=QString("select ")+
    "CART.NUMBER,"+             // 00
    "CART.TYPE,"+               // 01
    "CUTS.CUT_NAME,"+           // 02
    "CART.GROUP_NAME,"+         // 03
    "CART.TITLE,CART.ARTIST,"+  // 04
    "CART.ALBUM,"+              // 05
    "CART.YEAR,"+               // 06
    "CUTS.ISRC,"+               // 07
    "CUTS.ISCI,"+               // 08
    "CART.LABEL,"+              // 09
    "CART.CLIENT,"+             // 10
    "CART.AGENCY,"+             // 11
    "CART.PUBLISHER,"+          // 12
    "CART.COMPOSER,"+           // 13
    "CART.CONDUCTOR,"+          // 14
    "CART.SONG_ID,"+            // 15
    "CART.USER_DEFINED,"+       // 16
    "CUTS.DESCRIPTION,"+        // 17
    "CUTS.OUTCUE,"+             // 18
    "CUTS.LENGTH,"+             // 19
    "CUTS.START_POINT,"+        // 20
    "CUTS.END_POINT,"+          // 21
    "CUTS.SEGUE_START_POINT,"+  // 22
    "CUTS.SEGUE_END_POINT,"+    // 23
    "CUTS.HOOK_START_POINT,"+   // 24
    "CUTS.HOOK_END_POINT,"+     // 25
    "CUTS.TALK_START_POINT,"+   // 26
    "CUTS.TALK_END_POINT,"+     // 27
    "CUTS.FADEUP_POINT,"+       // 28
    "CUTS.FADEDOWN_POINT,"+     // 29
    "SCHED_CODES "+             // 30
    "from CART left join CUTS "+
    "on CART.NUMBER=CUTS.CART_NUMBER ";
  if(list_group==QString("ALL")) {
    sql+=RDAllCartSearchText(list_filter,schedcode,rda->user()->name(),true)+" && "+
      list_type_filter+" order by CART.NUMBER,CUTS.CUT_NAME";
  }
  else {
    sql+=RDCartSearchText(list_filter,list_group,schedcode,true)+" && "+
      list_type_filter+" order by CART.NUMBER,CUTS.CUT_NAME";
  }
  q=new RDSqlQuery(sql);

  //
  // Get max number of scheduler codes
  //
  while(q->next()) {
    f0=q->value(17).toString().split(" ");
    if((int)f0.size()>code_quan) {
      code_quan=f0.size();
    }
  }
  code_quan--;

  //
  // Prepend Field Names
  //
  if(prepend_names) {
    *report=QString("CART,")+
      "CUT,"+                // 00
      "TYPE,"+               // 01
      "GROUP_NAME,"+         // 02
      "TITLE,"+              // 03
      "ARTIST,"+             // 04
      "ALBUM,"+              // 05
      "YEAR,"+               // 06
      "ISRC,"+               // 07
      "ISCI,"+               // 08
      "LABEL,"+              // 09
      "CLIENT,"+             // 10
      "AGENCY,"+             // 11
      "PUBLISHER,"+          // 12
      "COMPOSER,"+           // 13
      "CONDUCTOR,"+          // 14
      "SONG_ID,"+            // 15
      "USER_DEFINED,"+       // 16
      "DESCRIPTION,"+        // 17
      "OUTCUE,"+             // 18
      "FILENAME,LENGTH,"+    // 19
      "START_POINT,"+        // 20
      "END_POINT,"+          // 21
      "SEGUE_START_POINT,"+  // 22
      "SEGUE_END_POINT,"+    // 23
      "HOOK_START_POINT,"+   // 24
      "HOOK_END_POINT,"+     // 25
      "TALK_START_POINT,"+   // 26
      "TALK_END_POINT,"+     // 27
      "FADEUP_POINT,"+       // 28
      "FADEDOWN_POINT,";     // 29
    for(int i=0;i<code_quan;i++) {
      *report+=QString().sprintf("SCHED_CODE%u,",i+1);  // 30
    } 
    *report=report->left(report->length()-1);
    *report+="\n";
  }

  //
  // Generate Rows
  //
  q->seek(-1);
  while(q->next()) {
    RDCart::Type type=(RDCart::Type)q->value(1).toInt();
    *report+=QString().sprintf("%u,",q->value(0).toUInt());
    if(type==RDCart::Macro) {
      *report+="0,\"macro\",";
    }
    else {
      *report+=QString().sprintf("%u,",RDCut::cutNumber(q->value(2).toString()));
      *report+="\"audio\",";
    }
    *report+="\""+q->value(3).toString()+"\",";
    *report+="\""+q->value(4).toString()+"\",";
    *report+="\""+q->value(5).toString()+"\",";
    *report+="\""+q->value(6).toString()+"\",";
    *report+="\""+q->value(7).toDate().toString("yyyy")+"\",";
    *report+="\""+q->value(8).toString()+"\",";
    *report+="\""+q->value(9).toString()+"\",";
    *report+="\""+q->value(10).toString()+"\",";
    *report+="\""+q->value(11).toString()+"\",";
    *report+="\""+q->value(12).toString()+"\",";
    *report+="\""+q->value(13).toString()+"\",";
    *report+="\""+q->value(14).toString()+"\",";
    *report+="\""+q->value(15).toString()+"\",";
    *report+="\""+q->value(16).toString()+"\",";
    *report+="\""+q->value(17).toString()+"\",";
    *report+="\""+q->value(18).toString()+"\",";
    *report+="\""+q->value(19).toString()+"\",";
    if(type==RDCart::Macro) {
      *report+="\"\",";
    }
    else {
      *report+="\""+q->value(2).toString()+".wav\",";
    }
    *report+="\""+
      RDGetTimeLength(q->value(20).toInt(),false,false).stripWhiteSpace()+"\",";
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
      *report+=QString().sprintf("%d,",q->value(21).toInt());
      *report+=QString().sprintf("%d,",q->value(22).toInt());
      *report+=QString().sprintf("%d,",q->value(23).toInt());
      *report+=QString().sprintf("%d,",q->value(24).toInt());
      *report+=QString().sprintf("%d,",q->value(25).toInt());
      *report+=QString().sprintf("%d,",q->value(26).toInt());
      *report+=QString().sprintf("%d,",q->value(27).toInt());
      *report+=QString().sprintf("%d,",q->value(28).toInt());
      *report+=QString().sprintf("%d,",q->value(29).toInt());
      *report+=QString().sprintf("%d,",q->value(30).toInt());
    }

    f0=q->value(31).toString().split(" ");
    for(int i=0;i<code_quan;i++) {
      if(((int)f0.size()>i)&&(f0[i]!=".")) {
	*report+="\""+f0[i].stripWhiteSpace()+"\",";
      }
      else {
	*report+="\"\",";
      }
    }
    *report=report->left(report->length()-1);
    *report+="\n";
  }
}
