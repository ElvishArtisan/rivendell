// test_import.cpp
//
// Test a Rivendell Log Import
//
//   (C) Copyright 2002-2004,2016 Fred Gleason <fredg@paravelsystems.com>
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
#include <QPainter>

#include <rdconf.h>
#include <rddb.h>
#include <rddatedecode.h>
#include <rddatedialog.h>
#include <rdevent_line.h>

#include "test_import.h"

TestImport::TestImport(RDSvc *svc,RDSvc::ImportSource src,QWidget *parent)
  : QDialog(parent,"",true)
{
  QString sql;
  QDateTime current_datetime=QDateTime::currentDateTime();

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
  test_date_edit=new QDateTimeEdit(this);
  test_date_edit->setDisplayFormat("MM/dd/yyyy");
  test_date_label=new QLabel(test_date_edit,tr("Test Date:"),this);
  test_date_label->setFont(font);
  test_date_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  test_date_edit->setDate(current_datetime.date());
  connect(test_date_edit,SIGNAL(dateTimeChanged(const QDateTime &)),
	  this,SLOT(dateChangedData(const QDateTime &)));

  //
  // Select Date Button
  //
  QPushButton *button=new QPushButton(this);
  button->setGeometry(200,5,60,30);
  button->setFont(font);
  button->setText(tr("&Select"));
  connect(button,SIGNAL(clicked()),this,SLOT(selectData()));

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
  test_filename_label->setGeometry(15,115,sizeHint().width()-30,18);
  test_filename_label->setFont(font);

  //
  // Events List
  //
  test_events_label=new QLabel(tr("Imported Events"),this);
  test_events_label->setGeometry(15,160,sizeHint().width()-30,18);
  test_events_label->setFont(font);
  test_events_widget=new RDTableWidget(this);
  test_events_widget->setColumnCount(8);
  test_events_widget->
    setHorizontalHeaderItem(0,new QTableWidgetItem(tr("Line")));
  test_events_widget->
    setHorizontalHeaderItem(1,new QTableWidgetItem(tr("Start Time")));
  test_events_widget->
    setHorizontalHeaderItem(2,new QTableWidgetItem(tr("Cart")));
  test_events_widget->
    setHorizontalHeaderItem(3,new QTableWidgetItem(tr("Len")));
  test_events_widget->
    setHorizontalHeaderItem(4,new QTableWidgetItem(tr("Cart Title")));
  test_events_widget->
    setHorizontalHeaderItem(5,new QTableWidgetItem(tr("Contract #")));
  test_events_widget->
    setHorizontalHeaderItem(6,new QTableWidgetItem(tr("Event ID")));
  test_events_widget->
    setHorizontalHeaderItem(7,new QTableWidgetItem(tr("Announcement Type")));

  //
  //  Close Button
  //
  test_close_button=new QPushButton(this);
  test_close_button->setFont(font);
  test_close_button->setText(tr("&Close"));
  connect(test_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  dateChangedData(current_datetime);
}


TestImport::~TestImport()
{
}


QSize TestImport::sizeHint() const
{
  return QSize(700,400);
} 


QSizePolicy TestImport::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void TestImport::selectData()
{
  RDDateDialog *datedialog=
    new RDDateDialog(QDate::currentDate().year(),QDate::currentDate().year()+1,
		     this);
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
  QTableWidgetItem *item;
  int next_line=0;

  test_events_widget->clearContents();
  if(!test_svc->import(test_src,test_date_edit->date(),test_svc->breakString(),
		       test_svc->trackString(test_src),QString().
		       sprintf("%s_TEST_IMP",
			       (const char *)test_svc->name()))) {
    QMessageBox::information(this,tr("Import Error"),
			     tr("There was an error during import\nplease check your settings and try again."));
    return;
  }
  QString sql=QString("select ")+
    "START_HOUR,"+     // 00
    "START_SECS,"+     // 01
    "EXT_CART_NAME,"+  // 02
    "LENGTH,"+         // 03
    "EXT_DATA,"+       // 04
    "EXT_EVENT_ID,"+   // 05
    "EXT_ANNC_TYPE,"+  // 06
    "INSERT_BREAK,"+   // 07
    "INSERT_TRACK,"+   // 08
    "INSERT_FIRST,"+   // 09
    "TITLE,"+          // 10
    "TRACK_STRING "+   // 11
    "from `"+test_svc->name()+"_TEST_IMP`";
  RDSqlQuery *q=new RDSqlQuery(sql);
  test_events_widget->setRowCount(q->size());
  while(q->next()) {
    AddLine(next_line);
    if(q->value(9).toUInt()==RDEventLine::InsertBreak) {
      if(q->value(7).toString()=="Y") {
	item=new QTableWidgetItem(RDSvc::timeString(q->value(0).toInt(),
						    q->value(1).toInt()));
	item->setData(Qt::TextAlignmentRole,
		      QVariant(Qt::AlignCenter|Qt::AlignVCenter));
	test_events_widget->setItem(next_line,1,item);
	item=new QTableWidgetItem(tr("[spot break]"));
	item->setData(Qt::TextAlignmentRole,
		      QVariant(Qt::AlignCenter|Qt::AlignVCenter));
	test_events_widget->setItem(next_line,2,item);
	AddLine(++next_line);
      }
      if(q->value(8).toString()=="Y") {
	item=new QTableWidgetItem(RDSvc::timeString(q->value(0).toInt(),
						    q->value(1).toInt()));
	item->setData(Qt::TextAlignmentRole,
		      QVariant(Qt::AlignCenter|Qt::AlignVCenter));
	test_events_widget->setItem(next_line,1,item);
	item=new QTableWidgetItem("["+q->value(11).toString()+"]");
	item->setData(Qt::TextAlignmentRole,
		      QVariant(Qt::AlignCenter|Qt::AlignVCenter));
	test_events_widget->setItem(next_line,2,item);
	AddLine(++next_line);
      }
    }
    else {
      if(q->value(8).toString()=="Y") {
	item=new QTableWidgetItem(RDSvc::timeString(q->value(0).toInt(),
						    q->value(1).toInt()));
	item->setData(Qt::TextAlignmentRole,
		      QVariant(Qt::AlignCenter|Qt::AlignVCenter));
	test_events_widget->setItem(next_line,1,item);
	item=new QTableWidgetItem("["+q->value(11).toString()+"]");
	item->setData(Qt::TextAlignmentRole,
		      QVariant(Qt::AlignCenter|Qt::AlignVCenter));
	test_events_widget->setItem(next_line,2,item);
	AddLine(++next_line);
      }
      if(q->value(7).toString()=="Y") {
	item=new QTableWidgetItem(RDSvc::timeString(q->value(0).toInt(),
						    q->value(1).toInt()));
	item->setData(Qt::TextAlignmentRole,
		      QVariant(Qt::AlignCenter|Qt::AlignVCenter));
	test_events_widget->setItem(next_line,1,item);
	item=new QTableWidgetItem(tr("[spot break]"));
	item->setData(Qt::TextAlignmentRole,
		      QVariant(Qt::AlignCenter|Qt::AlignVCenter));
	test_events_widget->setItem(next_line,2,item);
	AddLine(++next_line);
      }
    }
    item=new QTableWidgetItem(RDSvc::timeString(q->value(0).toInt(),
						q->value(1).toInt()));
    item->
      setData(Qt::TextAlignmentRole,QVariant(Qt::AlignCenter|Qt::AlignVCenter));
    test_events_widget->setItem(next_line,1,item);
    item=new QTableWidgetItem(QString().sprintf("%06u",q->value(2).toUInt()));
    item->
      setData(Qt::TextAlignmentRole,QVariant(Qt::AlignCenter|Qt::AlignVCenter));
    test_events_widget->setItem(next_line,2,item);
    if(q->value(3).toInt()>=0) {
      item=
	new QTableWidgetItem(RDGetTimeLength(q->value(3).toInt(),false,false));
      item->setData(Qt::TextAlignmentRole,
		    QVariant(Qt::AlignRight|Qt::AlignVCenter));
      test_events_widget->setItem(next_line,3,item);
    }
    item=new QTableWidgetItem(q->value(10).toString());
    item->
      setData(Qt::TextAlignmentRole,QVariant(Qt::AlignLeft|Qt::AlignVCenter));
    test_events_widget->setItem(next_line,4,item);    

    item=new QTableWidgetItem(q->value(4).toString());
    item->
      setData(Qt::TextAlignmentRole,QVariant(Qt::AlignLeft|Qt::AlignVCenter));
    test_events_widget->setItem(next_line,5,item);    

    item=new QTableWidgetItem(q->value(5).toString());
    item->
      setData(Qt::TextAlignmentRole,QVariant(Qt::AlignLeft|Qt::AlignVCenter));
    test_events_widget->setItem(next_line,6,item);    

    item=new QTableWidgetItem(q->value(6).toString());
    item->
      setData(Qt::TextAlignmentRole,QVariant(Qt::AlignLeft|Qt::AlignVCenter));
    test_events_widget->setItem(next_line,7,item);    
    next_line++;
  }
  delete q;
  test_events_widget->resizeColumnsToContents();
  //printf("IMPORT TABLE: %s_TEST_IMP\n",(const char *)test_svc->name());
  sql=QString("drop table `")+test_svc->name()+"_TEST_IMP`",
    RDSqlQuery::run(sql);
}


void TestImport::dateChangedData(const QDateTime &dt)
{
  test_filename_edit->
    setText(RDDateDecode(test_svc->importPath(test_src,RDSvc::Linux),dt.date()));
}


void TestImport::closeData()
{
  done(0);
}


void TestImport::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);
  p->setPen(QColor(Qt::black));
  p->drawLine(10,105,size().width()-20,105);
  p->end();
  delete p;
}


void TestImport::resizeEvent(QResizeEvent *e)
{
  test_date_edit->setGeometry(95,10,95,20);
  test_date_label->setGeometry(5,10,85,20);
  test_filename_edit->setGeometry(10,133,size().width()-20,18);
  test_import_button->setGeometry(30,45,size().width()-60,50);
  test_events_widget->
    setGeometry(10,178,size().width()-20,size().height()-248);
  test_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void TestImport::AddLine(int line)
{
  QTableWidgetItem *item=
    new QTableWidgetItem(QString().sprintf("%d",line+1));
  item->setData(Qt::TextAlignmentRole,QVariant(Qt::Right|Qt::AlignVCenter));
  test_events_widget->setItem(line,0,item);
}
