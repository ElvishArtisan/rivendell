// test_import.cpp
//
// Test a Rivendell Log Import
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: test_import.cpp,v 1.22 2010/07/29 19:32:35 cvs Exp $
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
#include <qpainter.h>
#include <qmessagebox.h>

#include <rddatedialog.h>
#include <rdconf.h>
#include <rddb.h>
#include <test_import.h>
#include <rduser.h>
#include <rdpasswd.h>
#include <rddatedecode.h>
#include <rdescape_string.h>
#include <rdlistviewitem.h>
#include <rdevent_line.h>


TestImport::TestImport(RDSvc *svc,RDSvc::ImportSource src,QWidget *parent,
		       const char *name)
  : QDialog(parent,name,true)
{
  QString sql;
  QDate current_date=QDate::currentDate();

  test_svc=svc;
  test_src=src;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  switch(test_src) {
      case RDSvc::Traffic:
	setCaption(tr("Test Traffic Import"));
	break;

      case RDSvc::Music:
	setCaption(tr("Test Music Import"));
	break;
  }

  //
  // Create Fonts
  //
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);
  QFont section_font=QFont("Helvetica",14,QFont::Bold);
  section_font.setPixelSize(14);

  //
  // Date Selector
  //
  test_date_edit=new QDateEdit(this,"test_date_edit");
  test_date_label=new QLabel(test_date_edit,tr("Test Date:"),this);
  test_date_label->setFont(font);
  test_date_label->setAlignment(AlignVCenter|AlignRight);
  test_date_edit->setDate(current_date);
  connect(test_date_edit,SIGNAL(valueChanged(const QDate &)),
	  this,SLOT(dateChangedData(const QDate &)));

  //
  // Select Date Button
  //
  test_select_button=new QPushButton(this);
  test_select_button->setFont(font);
  test_select_button->setText(tr("&Select"));
  connect(test_select_button,SIGNAL(clicked()),this,SLOT(selectData()));

  //
  // Import Button
  //
  test_import_button=new QPushButton(this);
  test_import_button->setFont(font);
  test_import_button->setText(tr("&Import"));
  connect(test_import_button,SIGNAL(clicked()),this,SLOT(importData()));

  //
  // Import Filename
  //
  test_filename_edit=new QLineEdit(this);
  test_filename_edit->setReadOnly(true);
  test_filename_label=
    new QLabel(test_filename_edit,tr("Using source file:"),this);
  test_filename_label->setFont(font);

  //
  // Events List
  //
  test_events_list=new RDListView(this);
  test_events_list->setItemMargin(2);
  test_events_list->addColumn(tr("Start Time"));
  test_events_list->setColumnAlignment(0,AlignCenter);
  test_events_list->addColumn(tr("Cart"));
  test_events_list->setColumnAlignment(1,AlignCenter);
  test_events_list->addColumn(tr("Len"));
  test_events_list->setColumnAlignment(2,AlignRight);
  test_events_list->addColumn(tr("Title"));
  test_events_list->setColumnAlignment(3,AlignLeft);
  test_events_list->addColumn(tr("Trans"));
  test_events_list->setColumnAlignment(4,AlignCenter);
  test_events_list->addColumn(tr("Time Type"));
  test_events_list->setColumnAlignment(5,AlignCenter);
  test_events_list->addColumn(tr("Wait Time"));
  test_events_list->setColumnAlignment(6,AlignCenter);
  test_events_list->addColumn(tr("Contract #"));
  test_events_list->setColumnAlignment(7,AlignCenter);
  test_events_list->addColumn(tr("Event ID"));
  test_events_list->setColumnAlignment(8,AlignCenter);
  test_events_list->addColumn(tr("Announcement Type"));
  test_events_list->setColumnAlignment(9,AlignCenter);
  test_events_list->setColumnSortType(0,RDListView::LineSort);
  test_events_label=new QLabel(test_events_list,tr("Imported Events"),this);
  test_events_label->setFont(font);

  //
  //  Close Button
  //
  test_close_button=new QPushButton(this);
  test_close_button->setFont(font);
  test_close_button->setText(tr("&Close"));
  connect(test_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  dateChangedData(current_date);
}


TestImport::~TestImport()
{
}


QSize TestImport::sizeHint() const
{
  return QSize(270,400);
} 


QSizePolicy TestImport::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void TestImport::selectData()
{
  RDDateDialog *datedialog=new RDDateDialog(QDate::currentDate().year(),
					    QDate::currentDate().year()+1,
					    this,"timedialog");
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
  RDListViewItem *item;
  int next_line=0;

  test_events_list->clear();
  if(!test_svc->import(test_src,test_date_edit->date(),test_svc->breakString(),
		       test_svc->trackString(test_src),QString().
		       sprintf("%s_TEST_IMP",
			       (const char *)test_svc->name()))) {
    QMessageBox::information(this,tr("Import Error"),
			     tr("There was an error during import\nplease check your settings and try again."));
    return;
  }
  QString sql=QString("select ")+
    "TYPE,"+           // 00
    "START_HOUR,"+     // 01
    "START_SECS,"+     // 02
    "EXT_CART_NAME,"+  // 03
    "LENGTH,"+         // 04
    "EXT_DATA,"+       // 05
    "EXT_EVENT_ID,"+   // 06
    "EXT_ANNC_TYPE,"+  // 07
    "TITLE,"+          // 08
    "TRANS_TYPE,"+     // 09
    "TIME_TYPE,"+      // 10
    "GRACE_TIME "+     // 11
    "from `"+RDEscapeString(test_svc->name())+"_TEST_IMP`";
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    item=new RDListViewItem(test_events_list);
    item->setLine(next_line++);
    item->setText(0,RDSvc::timeString(q->value(1).toInt(),
                                     q->value(2).toInt()));
    item->setText(3,q->value(8).toString());
    switch((RDSvc::ImportType)q->value(0).toInt()) {
    case RDSvc::Cart:
    case RDSvc::Label:
      item->setText(1,q->value(3).toString());
      if(q->value(4).toInt()>=0) {
       item->setText(2,RDGetTimeLength(q->value(4).toInt(),false,false));
      }
      item->
       setText(4,RDLogLine::transText((RDLogLine::TransType)q->value(9).toInt()));
      if(q->value(10).toInt()==RDLogLine::Hard) {
       if(q->value(11).toInt()<0) {
         item->setText(5,tr("Make Next"));
       }
       else {
         item->setText(5,tr("Start Immediately"));
         if(q->value(11).toInt()>0) {
           item->setText(6,RDGetTimeLength(q->value(11).toInt(),true,false));
         }
       }
      }
      item->setText(7,q->value(5).toString());
      item->setText(8,q->value(6).toString());
      item->setText(9,q->value(7).toString());
      break;

    case RDSvc::Break:
      item->setText(2,RDGetTimeLength(q->value(4).toInt(),false,false));
      break;

    case RDSvc::Track:
      break;
    }
  }
  delete q;
  // printf("IMPORT TABLE: %s_TEST_IMP\n",(const char *)test_svc->name());
  /*
  sql=QString().sprintf("drop table `%s_TEST_IMP`",
			(const char *)test_svc->name());
  q=new RDSqlQuery(sql);
  delete q;
  */
}


void TestImport::dateChangedData(const QDate &date)
{
  test_filename_edit->
    setText(RDDateDecode(test_svc->importPath(test_src,RDSvc::Linux),date));
}


void TestImport::closeData()
{
  done(0);
}


void TestImport::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);
  p->setPen(QColor(black));
  p->moveTo(10,105);
  p->lineTo(size().width()-20,105);
  p->end();
  delete p;
}


void TestImport::resizeEvent(QResizeEvent *e)
{
  test_date_label->setGeometry(5,10,85,20);
  test_date_edit->setGeometry(95,10,95,20);

  test_select_button->setGeometry(200,5,60,30);

  test_import_button->setGeometry(30,45,size().width()-60,50);

  test_filename_label->setGeometry(15,115,size().width()-30,18);
  test_filename_edit->setGeometry(10,133,size().width()-20,18);

  test_events_label->setGeometry(15,160,size().width()-30,18);
  test_events_list->
    setGeometry(10,178,size().width()-20,size().height()-248);

  test_close_button->setGeometry(size().width()-90,size().height()-60,
			     80,50);
}
