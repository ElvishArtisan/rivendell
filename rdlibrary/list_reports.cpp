// list_reports.cpp
//
// Add a Rivendell Service
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_reports.cpp,v 1.11.4.4.2.1 2014/03/19 22:12:59 cvs Exp $
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

#include <rddb.h>
#include <rdconf.h>
#include <rdtextfile.h>
#include <rdcart_search_text.h>
#include <rdcart.h>

#include <globals.h>
#include <list_reports.h>


ListReports::ListReports(const QString &filter,const QString &type_filter,
			 const QString &group,const QString &schedcode,
			 QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
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
  list_reports_box->insertItem(tr("Cart Report"));
  list_reports_box->insertItem(tr("Cut Report"));
  list_reports_box->insertItem(tr("Cart Data Dump (fixed width)"));
  list_reports_box->insertItem(tr("Cart Data Dump (CSV)"));
  list_reports_label=new QLabel(list_reports_box,tr("Type:"),this);
  list_reports_label->setGeometry(10,10,35,19);
  list_reports_label->setFont(font);
  list_reports_label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
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
  list_fieldnames_label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);
  list_fieldnames_label->setDisabled(true);

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
  *report="                                                        Rivendell Cart Report\n";
  *report+=QString().
    sprintf("Generated: %s     Group: %-10s      Filter: %s\n",
	    (const char *)QDateTime(QDate::currentDate(),QTime::currentTime()).
	    toString("MM/dd/yyyy - hh:mm:ss"),
	    (const char *)list_group.utf8(),(const char *)filter.utf8());
  *report+="\n";
  *report+="Type -Cart- -Group---- -Len- -Title------------------------- -Artist----------------------- Cuts Rot Enf -LenDev -Owner--------------\n";

  //
  // Generate Rows
  //
  if(list_type_filter.isEmpty()) {
    return;
  }
  sql=QString("select CART.TYPE,CART.NUMBER,CART.GROUP_NAME,")+
    "CART.FORCED_LENGTH,CART.TITLE,CART.ARTIST,CART.CUT_QUANTITY,"+
    "CART.PLAY_ORDER,CART.ENFORCE_LENGTH,CART.LENGTH_DEVIATION,CART.OWNER "+
    "from CART left join CUTS on CART.NUMBER=CUTS.CART_NUMBER";
  if(list_group==QString("ALL")) {
    sql+=QString(" where ")+
      RDAllCartSearchText(list_filter,schedcode,lib_user->name(),true)+" && "+
      list_type_filter+" order by NUMBER";
  }
  else {
    sql+=QString(" where ")+
      RDCartSearchText(list_filter,list_group,schedcode,true)+" && "+
      list_type_filter+" order by NUMBER";
  }
  q=new RDSqlQuery(sql);
  while(q->next()) {
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
    *report+=
      QString().sprintf("%-10s ",(const char *)q->value(2).toString().utf8());

    //
    // Length
    //
    *report+=
      QString().sprintf("%5s ",
	       (const char *)RDGetTimeLength(q->value(3).toInt(),false,false));

    //
    // Title
    //
    *report+=QString().sprintf("%-31s ",(const char *)q->value(4).toString().
			       utf8().left(31));

    //
    // Artist
    //
    *report+=QString().sprintf("%-30s ",(const char *)q->value(5).toString().
			       utf8().left(30));

    //
    // Cut Quantity
    //
    *report+=QString().sprintf("%4d ",q->value(6).toInt());

    //
    // Play Order
    //
    switch((RDCart::PlayOrder)q->value(7).toInt()) {
	case RDCart::Sequence:
	  *report+="SEQ ";
	  break;

	case RDCart::Random:
	  *report+="RND ";
	  break;

	default:
	  *report+="???";
	  break;
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
    *report+=
      QString().sprintf("%7s ",
	       (const char *)RDGetTimeLength(q->value(9).toInt(),false,true));

    //
    // Owner
    //
    if(q->value(10).toString().isEmpty()) {
      *report+="[none]";
    }
    else {
      *report+=QString().sprintf("%s",(const char *)q->value(10).toString().
				 utf8().left(20));
    }

    //
    // End of Line
    //
    *report+="\n";
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
  *report="                                                        Rivendell Cut Report\n";
  *report+=QString().
    sprintf("Generated: %s     Group: %-10s      Filter: %s\n",
	    (const char *)QDateTime(QDate::currentDate(),QTime::currentTime()).
	    toString("MM/dd/yyyy - hh:mm:ss"),
	    (const char *)list_group.utf8(),(const char *)filter.utf8());
  *report+="\n";
  *report+="-Cart- Cut Wht -Cart Title-------------- -Description--- -Len- Last Play Plays Start Date End Date -Days of Week- -Daypart-----------\n";

  //
  // Generate Rows
  //
  if(list_type_filter.isEmpty()) {
    return;
  }
  sql="select CART.NUMBER,CUTS.CUT_NAME,CUTS.WEIGHT,CART.TITLE,\
       CUTS.DESCRIPTION,CUTS.LENGTH,CUTS.LAST_PLAY_DATETIME,CUTS.PLAY_COUNTER,\
       CUTS.START_DATETIME,CUTS.END_DATETIME,SUN,MON,TUE,WED,THU,FRI,SAT,\
       CUTS.START_DAYPART,CUTS.END_DAYPART from CART join CUTS \
       on CART.NUMBER=CUTS.CART_NUMBER";
  if(list_group==QString("ALL")) {
    sql+=QString(" where ")+
      RDAllCartSearchText(list_filter,schedcode,lib_user->name(),true)+" && "+
      list_type_filter+" order by CART.NUMBER";
  }
  else {
    sql+=QString(" where ")+
      RDCartSearchText(list_filter,list_group,schedcode,true)+" && "+
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
      *report+=QString().sprintf("%-25s ",(const char *)q->value(3).toString().
				 utf8().left(25));
    }
    else {
      *report+="                          ";
    }

    //
    // Description
    //
    *report+=QString().sprintf("%-15s ",(const char *)q->value(4).toString().
			       utf8().left(15));

    //
    // Length
    //
    *report+=
      QString().sprintf("%5s ",
	       (const char *)RDGetTimeLength(q->value(5).toInt(),false,false));

    //
    // Last Play
    //
    if(q->value(6).toDateTime().isNull()) {
      *report+="  [none]   ";
    }
    else {
      *report+=QString().sprintf(" %8s  ",
		    (const char *)q->value(6).toDate().toString("MM/dd/yy"));
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
      *report+=QString().sprintf(" %8s ",
	       (const char *)q->value(8).toDateTime().toString("MM/dd/yy"));
    }

    //
    // End Date
    //
    if(q->value(9).toDateTime().isNull()) {
      *report+="   TFN    ";
    }
    else {
      *report+=QString().sprintf(" %8s ",
		(const char *)q->value(9).toDateTime().toString("MM/dd/yy"));
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
      *report+=QString().sprintf("%8s - %8s",
	       (const char *)q->value(17).toTime().toString("hh:mm:ss"),
	       (const char *)q->value(18).toTime().toString("hh:mm:ss"));
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
  sql="select CUTS.CUT_NAME,CART.GROUP_NAME,CART.TITLE,CART.ARTIST,CART.ALBUM,\
       CART.YEAR,CUTS.ISRC,CART.LABEL,CART.CLIENT,CART.AGENCY,CART.PUBLISHER,\
       CART.COMPOSER,CART.USER_DEFINED,CUTS.LENGTH from CART \
       join CUTS on CART.NUMBER=CUTS.CART_NUMBER";
  if(list_group==QString("ALL")) {
    sql+=QString(" where ")+
      RDAllCartSearchText(list_filter,schedcode,lib_user->name(),true)+" && "+
      list_type_filter+" order by CUTS.CUT_NAME";
  }
  else {
    sql+=QString(" where ")+
      RDCartSearchText(list_filter,list_group,schedcode,true)+" && "+
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

  if(list_schedcode!=tr("ALL")) {
    schedcode=list_schedcode;
  }

  //
  // Prepend Field Names
  //
  if(prepend_names) {
    *report="CART,CUT,GROUP_NAME,TITLE,ARTIST,ALBUM,YEAR,ISRC,ISCI,LABEL,";
    *report+="CLIENT,AGENCY,PUBLISHER,COMPOSER,USER_DEFINED,LENGTH\n";
  }

  //
  // Generate Rows
  //
  if(list_type_filter.isEmpty()) {
    return;
  }
  sql="select CUTS.CUT_NAME,CART.GROUP_NAME,CART.TITLE,CART.ARTIST,CART.ALBUM,\
       CART.YEAR,CUTS.ISRC,CUTS.ISCI,CART.LABEL,CART.CLIENT,CART.AGENCY,\
       CART.PUBLISHER,CART.COMPOSER,CART.USER_DEFINED,CUTS.LENGTH from CART \
       join CUTS on CART.NUMBER=CUTS.CART_NUMBER";
  if(list_group==QString("ALL")) {
    sql+=QString(" where ")+
      RDAllCartSearchText(list_filter,schedcode,lib_user->name(),true)+" && "+
      list_type_filter+" order by CUTS.CUT_NAME";
  }
  else {
    sql=QString(" where ")+
      RDCartSearchText(list_filter,list_group,schedcode,true)+" && "+
      list_type_filter+" order by CUTS.CUT_NAME";
  }
  q=new RDSqlQuery(sql);
  while(q->next()) {
    *report+=QString().sprintf("%u,",RDCut::cartNumber(q->value(0).toString()));
    *report+=QString().sprintf("%u,",RDCut::cutNumber(q->value(0).toString()));
    *report+="\""+q->value(1).toString()+"\",";
    *report+="\""+q->value(2).toString()+"\",";
    *report+="\""+q->value(3).toString()+"\",";
    *report+="\""+q->value(4).toString()+"\",";
    *report+="\""+q->value(5).toString()+"\",";
    *report+="\""+q->value(6).toString()+"\",";
    *report+="\""+q->value(7).toString()+"\",";
    *report+="\""+q->value(8).toString()+"\",";
    *report+="\""+q->value(9).toString()+"\",";
    *report+="\""+q->value(10).toString()+"\",";
    *report+="\""+q->value(11).toString()+"\",";
    *report+="\""+q->value(12).toString()+"\",";
    *report+="\""+q->value(13).toString()+"\",";
    *report+="\""+RDGetTimeLength(q->value(14).toInt(),false,false)+"\",";
    *report+="\n";
  }
}
