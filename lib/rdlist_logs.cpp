// rdlist_logs.cpp
//
// Select a Rivendell Log
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

#include "rdapplication.h"
#include "rddb.h"
#include "rdescape_string.h"
#include "rdlist_logs.h"

RDListLogs::RDListLogs(QString *logname,RDLogFilter::FilterMode mode,
		       const QString &caption,QWidget *parent)
  : RDDialog(parent)
{
  list_logname=logname;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  setWindowTitle(caption+" - "+tr("Select Log"));

  //
  // Log Filter
  //
  list_filter_widget=new RDLogFilter(mode,this);

  //
  // Log List
  //
  list_log_view=new RDTableView(this);
  list_log_model=new RDLogListModel(this);
  list_log_model->setFont(defaultFont());
  list_log_model->setPalette(palette());
  list_log_view->setModel(list_log_model);
  list_log_view->resizeColumnsToContents();
  connect(list_filter_widget,SIGNAL(filterChanged(const QString &)),
	  list_log_model,SLOT(setFilterSql(const QString &)));
  connect(list_log_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));
  connect(list_log_model,SIGNAL(modelReset()),this,SLOT(modelResetData()));

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

  list_log_model->setFilterSql(list_filter_widget->whereSql());
}


QSize RDListLogs::sizeHint() const
{
  return QSize(600,400);
}


QSizePolicy RDListLogs::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void RDListLogs::closeEvent(QCloseEvent *e)
{
  cancelButtonData();
}


void RDListLogs::doubleClickedData(const QModelIndex &index)
{
  okButtonData();
}


void RDListLogs::modelResetData()
{
  list_log_view->resizeColumnsToContents();
}


void RDListLogs::okButtonData()
{
  if(list_log_view->selectionModel()->selectedRows().size()!=1) {
    return;
  }
  *list_logname=list_log_model->logName(list_log_view->selectionModel()->selectedRows().at(0));

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
  list_log_view->
    setGeometry(10,list_filter_widget->sizeHint().height(),
		size().width()-20,
		size().height()-list_filter_widget->sizeHint().height()-70);
  list_ok_button->
    setGeometry(size().width()-190,size().height()-60,80,50);
  list_cancel_button->
    setGeometry(size().width()-90,size().height()-60,80,50);
}
