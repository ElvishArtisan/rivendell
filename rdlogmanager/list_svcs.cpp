// list_svcs.cpp
//
// List Rivendell Services and Report Ages
//
//   (C) Copyright 2002-2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <qmessagebox.h>
#include <qdatetime.h>

#include <rddb.h>
#include <rdescape_string.h>
#include <rdlog.h>

#include "globals.h"
#include "list_svcs.h"
#include "svc_rec_dialog.h"
#include "pick_report_dates.h"

ListSvcs::ListSvcs(QWidget *parent)
  : QDialog(parent)
{
  setModal(true);

  setWindowTitle("RDLogManager - "+tr("Rivendell Services"));

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());

  //
  // Create Fonts
  //
  QFont bold_font=QFont("Helvetica",12,QFont::Bold);
  bold_font.setPixelSize(12);
  QFont font=QFont("Helvetica",12,QFont::Normal);
  font.setPixelSize(12);

  //
  // Log List
  //
  list_log_list=new Q3ListView(this);
  list_log_list->setAllColumnsShowFocus(true);
  list_log_list->setItemMargin(5);
  list_log_list->addColumn(tr("Service"));
  list_log_list->setColumnAlignment(0,Qt::AlignLeft);
  list_log_list->addColumn(tr("Oldest Report"));
  list_log_list->setColumnAlignment(1,Qt::AlignCenter);
  connect(list_log_list,
	  SIGNAL(doubleClicked(Q3ListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(listDoubleClickedData(Q3ListViewItem *,const QPoint &,int)));

  //
  //  Generate Report Button
  //
  list_generate_button=new QPushButton(this);
  list_generate_button->setFont(bold_font);
  list_generate_button->setText(tr("&Generate\nReports"));
  connect(list_generate_button,SIGNAL(clicked()),this,SLOT(generateData()));

  //
  //  Purge Button
  //
  list_purge_button=new QPushButton(this);
  list_purge_button->setFont(bold_font);
  list_purge_button->setText(tr("&Purge\nData"));
  connect(list_purge_button,SIGNAL(clicked()),this,SLOT(purgeData()));

  //
  //  Close Button
  //
  list_close_button=new QPushButton(this);
  list_close_button->setDefault(true);
  list_close_button->setFont(bold_font);
  list_close_button->setText(tr("C&lose"));
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));

  RefreshList();
}


QSize ListSvcs::sizeHint() const
{
  return QSize(400,300);
} 


QSizePolicy ListSvcs::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListSvcs::generateData()
{
  Q3ListViewItem *item=list_log_list->selectedItem();
  if(item==NULL) {
    return;
  }
  PickReportDates *dialog=new PickReportDates(item->text(0),this);
  dialog->exec();
  delete dialog;
}


void ListSvcs::purgeData()
{
  Q3ListViewItem *item=list_log_list->selectedItem();
  if(item==NULL) {
    return;
  }
  SvcRecDialog *dialog=new SvcRecDialog(item->text(0),this);
  dialog->exec();
  delete dialog;
  RefreshLine(item);
}


void ListSvcs::listDoubleClickedData(Q3ListViewItem *item,const QPoint &pt,
				     int c)
{
  generateData();
}


void ListSvcs::closeData()
{
  done(0);
}


void ListSvcs::resizeEvent(QResizeEvent *e)
{
  list_log_list->setGeometry(10,10,size().width()-20,size().height()-80);
  list_generate_button->setGeometry(10,size().height()-60,80,50);
  list_purge_button->setGeometry(100,size().height()-60,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void ListSvcs::RefreshList()
{
  RDSqlQuery *q1;
  Q3ListViewItem *item;
  list_log_list->clear();
  QString sql="select NAME from SERVICES order by NAME";

  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    item=new Q3ListViewItem(list_log_list);
    item->setText(0,q->value(0).toString());
    sql=QString("select EVENT_DATETIME from ELR_LINES where ")+
      "SERVICE_NAME=\""+RDEscapeString(q->value(0).toString())+"\" "+
      "order by EVENT_DATETIME";
    q1=new RDSqlQuery(sql);
    if(q1->first()) {
      item->setText(1,q1->value(0).toDate().toString("MM/dd/yyyy"));
    }
    else {
      item->setText(1,tr("[none]"));
    }
    delete q1;
  }
  delete q;
}


void ListSvcs::RefreshLine(Q3ListViewItem *item)
{
  QString sql;
  RDSqlQuery *q;
    sql=QString("select EVENT_DATETIME from ELR_LINES where ")+
      "SERVICE_NAME=\""+RDEscapeString(item->text(0))+"\" "+
      "order by EVENT_DATETIME";
    q=new RDSqlQuery(sql);
    if(q->first()) {
      item->setText(1,q->value(0).toDate().toString("MM/dd/yyyy"));
    }
    else {
      item->setText(1,tr("[none]"));
    }
    delete q;
}
