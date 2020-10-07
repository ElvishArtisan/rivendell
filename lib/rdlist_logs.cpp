// rdlist_logs.cpp
//
// Select a Rivendell Log
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

#include "rdapplication.h"
#include "rddb.h"
#include "rdescape_string.h"
#include "rdlist_logs.h"

RDListLogs::RDListLogs(QString *logname,RDLogFilter::FilterMode mode,
		       QWidget *parent)
  : RDDialog(parent)
{
  list_logname=logname;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  setWindowTitle(tr("Select Log"));

  //
  // Log Filter
  //
  list_filter_widget=new RDLogFilter(mode,this);
  connect(list_filter_widget,SIGNAL(filterChanged(const QString &)),
	  this,SLOT(filterChangedData(const QString &)));

  //
  // Log List
  //
  list_log_list=new Q3ListView(this);
  list_log_list->setAllColumnsShowFocus(true);
  list_log_list->setItemMargin(5);
  list_log_list->setSelectionMode(Q3ListView::Single);
  connect(list_log_list,
	  SIGNAL(doubleClicked(Q3ListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(doubleClickedData(Q3ListViewItem *,const QPoint &,int)));
  list_log_list->addColumn(tr("Name"));
  list_log_list->setColumnAlignment(0,Qt::AlignLeft);
  list_log_list->addColumn(tr("Description"));
  list_log_list->setColumnAlignment(1,Qt::AlignLeft);
  list_log_list->addColumn(tr("Service"));
  list_log_list->setColumnAlignment(2,Qt::AlignLeft);

  //
  // OK Button
  //
  list_ok_button=new QPushButton(this);
  list_ok_button->setFont(buttonFont());
  list_ok_button->setText(tr("OK"));
  connect(list_ok_button,SIGNAL(clicked()),this,SLOT(okButtonData()));

  //
  // Cancel Button
  //
  list_cancel_button=new QPushButton(this);
  list_cancel_button->setFont(buttonFont());
  list_cancel_button->setText(tr("Cancel"));
  list_cancel_button->setDefault(true);
  connect(list_cancel_button,SIGNAL(clicked()),this,SLOT(cancelButtonData()));

  RefreshList();
}


QSize RDListLogs::sizeHint() const
{
  return QSize(500,300);
}


QSizePolicy RDListLogs::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void RDListLogs::closeEvent(QCloseEvent *e)
{
  done(1);
}


void RDListLogs::filterChangedData(const QString &where_sql)
{
  RefreshList();
}


void RDListLogs::doubleClickedData(Q3ListViewItem *,const QPoint &,int)
{
  okButtonData();
}


void RDListLogs::okButtonData()
{
  Q3ListViewItem *item=list_log_list->selectedItem();
  if(item==NULL) {
    return;
  }
  *list_logname=item->text(0);
  done(true);
}


void RDListLogs::cancelButtonData()
{
  done(false);
}


void RDListLogs::resizeEvent(QResizeEvent *e)
{
  list_filter_widget->
    setGeometry(10,10,size().width()-10,
		list_filter_widget->sizeHint().height());
  list_log_list->
    setGeometry(10,list_filter_widget->sizeHint().height(),
		size().width()-20,
		size().height()-list_filter_widget->sizeHint().height()-70);
  list_ok_button->
    setGeometry(size().width()-190,size().height()-60,80,50);
  list_cancel_button->
    setGeometry(size().width()-90,size().height()-60,80,50);
}


void RDListLogs::RefreshList()
{
  RDSqlQuery *q;
  QString sql;
  Q3ListViewItem *l;
  Q3ListViewItem *view_item=NULL;
  QDate current_date=QDate::currentDate();

  list_log_list->clear();
  sql=QString("select NAME,DESCRIPTION,SERVICE from LOGS ")+
    "where (TYPE=0)&&(LOG_EXISTS=\"Y\")&&"+
    "((START_DATE<=\""+current_date.toString("yyyy-MM-dd")+"\")||"+
    "(START_DATE=\"0000-00-00\")||"+
    "(START_DATE is null))&&"+
    "((END_DATE>=\""+current_date.toString("yyyy-MM-dd")+"\")||"+
    "(END_DATE=\"0000-00-00\")||"+
    "(END_DATE is null))"+
    list_filter_widget->whereSql();

  q=new RDSqlQuery(sql);
  while(q->next()) {
    l=new Q3ListViewItem(list_log_list);
    l->setText(0,q->value(0).toString());
    l->setText(1,q->value(1).toString());
    l->setText(2,q->value(2).toString());
    if(l->text(0)==*list_logname) {
      view_item=l;
    }
  }
  delete q;
  if(view_item!=NULL) {
    list_log_list->setCurrentItem(view_item);
    list_log_list->ensureItemVisible(view_item);
  }
}
