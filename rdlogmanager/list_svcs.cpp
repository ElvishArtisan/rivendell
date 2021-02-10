// list_svcs.cpp
//
// List Rivendell Services and Report Ages
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

#include <rdescape_string.h>

#include "globals.h"
#include "list_svcs.h"
#include "svc_rec_dialog.h"
#include "pick_report_dates.h"

ListSvcs::ListSvcs(QWidget *parent)
  : RDDialog(parent)
{
  setWindowTitle("RDLogManager - "+tr("Rivendell Services"));

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  //
  // Log List
  //
  list_log_view=new RDTableView(this);
  list_log_model=new RDServiceListModel(false,this);
  list_log_model->setFont(font());
  list_log_model->setPalette(palette());
  list_log_view->setModel(list_log_model);
  for(int i=2;i<list_log_model->columnCount();i++) {
    list_log_view->hideColumn(i);
  }
  connect(list_log_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(listDoubleClickedData(const QModelIndex &)));
  connect(list_log_model,SIGNAL(modelReset()),
	  list_log_view,SLOT(resizeColumnsToContents()));
  list_log_view->resizeColumnsToContents();

  //
  //  Generate Report Button
  //
  list_generate_button=new QPushButton(this);
  list_generate_button->setFont(buttonFont());
  list_generate_button->setText(tr("&Generate\nReports"));
  connect(list_generate_button,SIGNAL(clicked()),this,SLOT(generateData()));

  //
  //  Purge Button
  //
  list_purge_button=new QPushButton(this);
  list_purge_button->setFont(buttonFont());
  list_purge_button->setText(tr("&Purge\nData"));
  connect(list_purge_button,SIGNAL(clicked()),this,SLOT(purgeData()));

  //
  //  Close Button
  //
  list_close_button=new QPushButton(this);
  list_close_button->setDefault(true);
  list_close_button->setFont(buttonFont());
  list_close_button->setText(tr("C&lose"));
  connect(list_close_button,SIGNAL(clicked()),this,SLOT(closeData()));
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
  QModelIndexList rows=list_log_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  PickReportDates *d=
    new PickReportDates(list_log_model->serviceName(rows.first()),this);
  d->exec();
  delete d;
}


void ListSvcs::purgeData()
{
  QModelIndexList rows=list_log_view->selectionModel()->selectedRows();

  if(rows.size()!=1) {
    return;
  }
  SvcRecDialog *d=
    new SvcRecDialog(list_log_model->serviceName(rows.first()),this);
  d->exec();
  delete d;
}


void ListSvcs::listDoubleClickedData(const QModelIndex &index)
{
  generateData();
}


void ListSvcs::closeData()
{
  done(0);
}


void ListSvcs::resizeEvent(QResizeEvent *e)
{
  list_log_view->setGeometry(10,10,size().width()-20,size().height()-80);
  list_generate_button->setGeometry(10,size().height()-60,80,50);
  list_purge_button->setGeometry(100,size().height()-60,80,50);
  list_close_button->setGeometry(size().width()-90,size().height()-60,80,50);
}
