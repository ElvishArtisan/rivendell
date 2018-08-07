// test_import.cpp
//
// Test a Rivendell Log Import
//
//   (C) Copyright 2002-2004,2018 Fred Gleason <fredg@paravelsystems.com>
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
#include <qpainter.h>
#include <qmessagebox.h>
//Added by qt3to4:
#include <QPaintEvent>
#include <QResizeEvent>
#include <QLabel>

#include <rdapplication.h>
#include <rdconf.h>
#include <rddatedecode.h>
#include <rddatedialog.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdevent_line.h>
#include <rdlistviewitem.h>
#include <rdpasswd.h>

#include "globals.h"
#include "test_import.h"

TestImport::TestImport(RDSvc *svc,RDSvc::ImportSource src,QWidget *parent)
  : QDialog(parent,"",true)
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
  test_date_edit=new Q3DateEdit(this);
  test_date_label=new QLabel(test_date_edit,tr("Test Date:"),this);
  test_date_label->setFont(font);
  test_date_label->setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  test_date_edit->setDate(current_date);
  connect(test_date_edit,SIGNAL(valueChanged(const QDate &)),
	  this,SLOT(dateChangedData(const QDate &)));

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
  test_events_list=new RDListView(this);
  test_events_list->setItemMargin(2);
  test_events_list->addColumn(tr("Start Time"));
  test_events_list->setColumnAlignment(0,Qt::AlignCenter);
  test_events_list->addColumn(tr("Cart"));
  test_events_list->setColumnAlignment(1,Qt::AlignCenter);
  test_events_list->addColumn(tr("Len"));
  test_events_list->setColumnAlignment(2,Qt::AlignRight);
  test_events_list->addColumn(tr("Title"));
  test_events_list->setColumnAlignment(3,Qt::AlignLeft);
  test_events_list->addColumn(tr("GUID"));
  test_events_list->setColumnAlignment(4,Qt::AlignCenter);
  test_events_list->addColumn(tr("Event ID"));
  test_events_list->setColumnAlignment(5,Qt::AlignCenter);
  test_events_list->addColumn(tr("Announcement Type"));
  test_events_list->setColumnAlignment(6,Qt::AlignCenter);
  test_events_list->setColumnSortType(0,RDListView::LineSort);
  test_events_label=new QLabel(test_events_list,tr("Imported Events"),this);
  test_events_label->setGeometry(15,160,sizeHint().width()-30,18);
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
  RDListViewItem *item;
  int next_line=0;

  test_events_list->clear();
  if(!test_svc->import(test_src,test_date_edit->date(),test_svc->breakString(),
		       test_svc->trackString(test_src))) {
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
    "from IMPORTER_LINES where "+
    "STATION_NAME=\""+RDEscapeString(rda->station()->name())+"\" && "+
    QString().sprintf("PROCESS_ID=%u",getpid());
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    if(q->value(9).toUInt()==RDEventLine::InsertBreak) {
      if(q->value(7).toString()=="Y") {
	item=new RDListViewItem(test_events_list);
	item->setLine(next_line++);
	item->
	  setText(0,RDSvc::timeString(q->value(0).toInt(),
				      q->value(1).toInt()));
	item->setText(1,tr("[spot break]"));
      }
      if(q->value(8).toString()=="Y") {
	item=new RDListViewItem(test_events_list);
	item->setLine(next_line++);
	item->setText(0,RDSvc::timeString(q->value(0).toInt(),
					  q->value(1).toInt()));
	item->setText(1,"["+q->value(11).toString()+"]");
      }
    }
    else {
      if(q->value(8).toString()=="Y") {
	item=new RDListViewItem(test_events_list);
	item->setLine(next_line++);
	item->setText(0,RDSvc::timeString(q->value(0).toInt(),
					  q->value(1).toInt()));
	item->setText(1,"["+q->value(11).toString()+"]");
      }
      if(q->value(7).toString()=="Y") {
	item=new RDListViewItem(test_events_list);
	item->setLine(next_line++);
	item->
	  setText(0,RDSvc::timeString(q->value(0).toInt(),
				      q->value(1).toInt()));
	item->setText(1,tr("[spot break]"));
      }
    }
    item=new RDListViewItem(test_events_list);
    item->setLine(next_line++);
    item->setText(0,RDSvc::timeString(q->value(0).toInt(),
				      q->value(1).toInt()));
    item->setText(1,q->value(2).toString());
    if(q->value(3).toInt()>=0) {
      item->setText(2,RDGetTimeLength(q->value(3).toInt(),false,false));
    }
    item->setText(3,q->value(10).toString());
    item->setText(4,q->value(4).toString());
    item->setText(5,q->value(5).toString());
    item->setText(6,q->value(6).toString());
  }
  delete q;

  sql=QString("delete from IMPORTER_LINES where ")+
    "STATION_NAME=\""+RDEscapeString(rda->station()->name())+"\" && "+
    QString().sprintf("PROCESS_ID=%u",getpid());
  //  printf("IMPORTER_LINES cleanup SQL: %s\n",(const char *)sql);
  RDSqlQuery::apply(sql);
}


void TestImport::dateChangedData(const QDate &date)
{
  test_filename_edit->
    setText(RDDateDecode(test_svc->importPath(test_src,RDSvc::Linux),date,
			 rda->station(),rda->config(),test_svc->name()));
}


void TestImport::closeData()
{
  done(0);
}


void TestImport::paintEvent(QPaintEvent *e)
{
  QPainter *p=new QPainter(this);
  p->setPen(Qt::black);
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
  test_events_list->
    setGeometry(10,178,size().width()-20,size().height()-248);
  test_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
