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
#include <rdlistviewitem.h>
#include <rdevent_line.h>


TestImport::TestImport(RDSvc *svc,RDSvc::ImportSource src,
		       QWidget *parent,const char *name)
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
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

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
  test_date_edit->setGeometry(95,10,95,20);
  QLabel *label=new QLabel(test_date_edit,tr("Test Date:"),this,"test_date_label");
  label->setGeometry(5,10,85,20);
  label->setFont(font);
  label->setAlignment(AlignVCenter|AlignRight);
  test_date_edit->setDate(current_date);
  connect(test_date_edit,SIGNAL(valueChanged(const QDate &)),
	  this,SLOT(dateChangedData(const QDate &)));

  //
  // Select Date Button
  //
  QPushButton *button=new QPushButton(this,"select_date_button");
  button->setGeometry(200,5,60,30);
  button->setFont(font);
  button->setText(tr("&Select"));
  connect(button,SIGNAL(clicked()),this,SLOT(selectData()));

  //
  // Import Button
  //
  button=new QPushButton(this,"import_button");
  button->setGeometry(30,45,sizeHint().width()-60,50);
  button->setFont(font);
  button->setText(tr("&Import"));
  connect(button,SIGNAL(clicked()),this,SLOT(importData()));

  //
  // Import Filename
  //
  test_filename_edit=new QLineEdit(this,"test_filename_edit");
  test_filename_edit->setReadOnly(true);
  test_filename_edit->setGeometry(10,133,sizeHint().width()-20,18);
  label=new QLabel(test_filename_edit,tr("Using source file:"),
		   this,"test_filename_label");
  label->setGeometry(15,115,sizeHint().width()-30,18);
  label->setFont(font);

  //
  // Events List
  //
  test_events_list=new RDListView(this,"test_events_list");
  test_events_list->
    setGeometry(10,178,sizeHint().width()-20,sizeHint().height()-248);
  test_events_list->setItemMargin(2);
  test_events_list->addColumn(tr("Start Time"));
  test_events_list->setColumnAlignment(0,AlignCenter);
  test_events_list->addColumn(tr("Cart"));
  test_events_list->setColumnAlignment(1,AlignCenter);
  test_events_list->addColumn(tr("Len"));
  test_events_list->setColumnAlignment(2,AlignRight);
  test_events_list->addColumn(tr("Title"));
  test_events_list->setColumnAlignment(3,AlignLeft);
  test_events_list->addColumn(tr("Contract #"));
  test_events_list->setColumnAlignment(4,AlignCenter);
  test_events_list->addColumn(tr("Event ID"));
  test_events_list->setColumnAlignment(5,AlignCenter);
  test_events_list->addColumn(tr("Announcement Type"));
  test_events_list->setColumnAlignment(6,AlignCenter);
  test_events_list->setColumnSortType(0,RDListView::LineSort);
  label=new QLabel(test_events_list,tr("Imported Events"),
		   this,"test_events_label");
  label->setGeometry(15,160,sizeHint().width()-30,18);
  label->setFont(font);

  //
  //  Close Button
  //
  button=new QPushButton(this,"close_button");
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  button->setFont(font);
  button->setText(tr("&Close"));
  connect(button,SIGNAL(clicked()),this,SLOT(closeData()));

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
  QString sql=QString().sprintf("select START_HOUR,START_SECS,EXT_CART_NAME,\
                                 LENGTH,EXT_DATA,EXT_EVENT_ID,EXT_ANNC_TYPE,\
                                 INSERT_BREAK,INSERT_TRACK,INSERT_FIRST,TITLE,\
                                 TRACK_STRING from `%s_TEST_IMP`",
				(const char *)test_svc->name());
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
	item->setText(1,QString().
		      sprintf("[%s]",(const char *)q->value(11).toString()));
      }
    }
    else {
      if(q->value(8).toString()=="Y") {
	item=new RDListViewItem(test_events_list);
	item->setLine(next_line++);
	item->setText(0,RDSvc::timeString(q->value(0).toInt(),
					  q->value(1).toInt()));
	item->setText(1,QString().
		      sprintf("[%s]",(const char *)q->value(11).toString()));
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
  // printf("IMPORT TABLE: %s_TEST_IMP\n",(const char *)test_svc->name());
  sql=QString().sprintf("drop table `%s_TEST_IMP`",
			(const char *)test_svc->name());
  q=new RDSqlQuery(sql);
  delete q;
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
  p->lineTo(sizeHint().width()-20,105);
  p->end();
  delete p;
}
