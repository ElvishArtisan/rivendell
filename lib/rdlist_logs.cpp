// rdlist_logs.cpp
//
// Select a Rivendell Log
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

#include <QDateTime>
#include <QPushButton>

#include "rdapplication.h"
#include "rdescape_string.h"
#include "rdlist_logs.h"

RDListLogs::RDListLogs(QString *logname,const QString &stationname,
		       QWidget *parent)
  : QDialog(parent)
{
  list_stationname=stationname;
  list_logname=logname;

  //
  // Fix the Window Size
  //
  setMinimumSize(sizeHint());

  //
  // Generate Fonts
  //
  QFont button_font("Helvetica",12,QFont::Bold);
  button_font.setPixelSize(12);

  setWindowTitle(tr("Select Log"));

  //
  // Log List
  //
  QDate current_date=QDate::currentDate();
  list_model=new RDSqlTableModel(this);
  QStringList services_list;
  QString sql=QString("select ")+
    "SERVICE_NAME "+
    "from SERVICE_PERMS where "+
    "STATION_NAME=\""+RDEscapeString(list_stationname)+"\"";
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    services_list.append(q->value(0).toString());
  }
  delete q;
  sql=QString("select ")+
    "NAME,"+
    "DESCRIPTION,"+
    "SERVICE "+
    "from LOGS where (TYPE=0)&&(LOG_EXISTS=\"Y\")&&"+
    "((START_DATE<=\""+current_date.toString("yyyy-MM-dd")+"\")||"+
    "(START_DATE=\"0000-00-00\")||"+
    "(START_DATE is null))&&"+
    "((END_DATE>=\""+current_date.toString("yyyy-MM-dd")+"\")||"+
    "(END_DATE=\"0000-00-00\")||"+
    "(END_DATE is null))&&(";
  for(QStringList::Iterator it = services_list.begin();
      it!=services_list.end();++it ) {
    sql+=QString().sprintf("SERVICE=\"%s\"||",
               (const char *)*it);
  }
  sql=sql.left(sql.length()-2);
  sql+=")";
  list_model->setQuery(sql);
  list_model->setHeaderData(0,Qt::Horizontal,tr("Name"),Qt::DisplayRole);
  list_model->setHeaderData(1,Qt::Horizontal,tr("Description"),Qt::DisplayRole);
  list_model->setHeaderData(2,Qt::Horizontal,tr("Service"),Qt::DisplayRole);
  list_view=new RDTableView(this);
  list_view->setModel(list_model);
  list_view->resizeColumnsToContents();
  connect(list_view,SIGNAL(doubleClicked(const QModelIndex &)),
	  this,SLOT(doubleClickedData(const QModelIndex &)));

  //
  // OK Button
  //
  list_ok_button=new QPushButton(this);
  list_ok_button->setFont(button_font);
  list_ok_button->setText(tr("OK"));
  connect(list_ok_button,SIGNAL(clicked()),this,SLOT(okButtonData()));

  //
  // Cancel Button
  //
  list_cancel_button=new QPushButton(this);
  list_cancel_button->setFont(button_font);
  list_cancel_button->setText(tr("Cancel"));
  list_cancel_button->setDefault(true);
  connect(list_cancel_button,SIGNAL(clicked()),this,SLOT(cancelButtonData()));
}


QSize RDListLogs::sizeHint() const
{
  return QSize(500,300);
}


QSizePolicy RDListLogs::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void RDListLogs::doubleClickedData(const QModelIndex &index)
{
  *list_logname=list_model->data(index.row(),0).toString();
  done(0);
}


void RDListLogs::okButtonData()
{
  QItemSelectionModel *s=list_view->selectionModel();
  if(s->hasSelection()) {
    doubleClickedData(s->selectedRows()[0]);
  }
}


void RDListLogs::cancelButtonData()
{
  done(1);
}


void RDListLogs::resizeEvent(QResizeEvent *e)
{
  list_view->setGeometry(10,10,size().width()-20,size().height()-80);
  list_ok_button->setGeometry(size().width()-190,size().height()-60,80,50);
  list_cancel_button->setGeometry(size().width()-90,size().height()-60,80,50);
}


void RDListLogs::closeEvent(QCloseEvent *e)
{
  done(1);
}
