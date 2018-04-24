// list_logs.cpp
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

#include <qpushbutton.h>
//Added by qt3to4:
#include <QCloseEvent>

#include <rdadd_log.h>
#include <rdapplication.h>
#include <rdescape_string.h>

#include "globals.h"
#include "list_logs.h"

ListLogs::ListLogs(LogPlay *log,QWidget *parent)
  : QDialog(parent,"",true)
{
  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  //
  // Generate Fonts
  //
  QFont button_font("Helvetica",12,QFont::Bold);
  button_font.setPixelSize(12);

  list_log=log;
  setCaption(tr("Select a Log"));

  //
  // Log List
  //
  list_log_list=new Q3ListView(this,"list_log_list");
  list_log_list->setGeometry(10,10,
                            sizeHint().width()-20,sizeHint().height()-80);
  list_log_list->setAllColumnsShowFocus(true);
  list_log_list->setItemMargin(5);
  connect(list_log_list,
          SIGNAL(doubleClicked(Q3ListViewItem *,const QPoint &,int)),
          this,
          SLOT(doubleClickedData(Q3ListViewItem *,const QPoint &,int)));
  list_log_list->addColumn(tr("NAME"));
  list_log_list->setColumnAlignment(0,Qt::AlignLeft);
  list_log_list->addColumn(tr("DESCRIPTION"));
  list_log_list->setColumnAlignment(1,Qt::AlignLeft);
  list_log_list->addColumn(tr("SERVICE"));
  list_log_list->setColumnAlignment(2,Qt::AlignLeft);

  //
  // Load Button
  //
  QPushButton *button=new QPushButton(this,"load_button");
  button->setGeometry(10,sizeHint().height()-60,80,50);
  button->setFont(button_font);
  button->setText(tr("Load"));
  connect(button,SIGNAL(clicked()),this,SLOT(loadButtonData()));

  //
  // Unload Button
  //
  list_unload_button=new QPushButton(this,"list_unload_button");
  list_unload_button->setGeometry(100,sizeHint().height()-60,80,50);
  list_unload_button->setFont(button_font);
  list_unload_button->setText(tr("Unload"));
  connect(list_unload_button,SIGNAL(clicked()),this,SLOT(unloadButtonData()));

  //
  // Save Button
  //
  button=new QPushButton(this,"save_button");
  button->setGeometry(210,sizeHint().height()-60,80,50);
  button->setFont(button_font);
  button->setText(tr("Save"));
  connect(button,SIGNAL(clicked()),this,SLOT(saveButtonData()));

  //
  // Save As Button
  //
  list_saveas_button=new QPushButton(this,"list_saveas_button");
  list_saveas_button->setGeometry(300,sizeHint().height()-60,80,50);
  list_saveas_button->setFont(button_font);
  list_saveas_button->setText(tr("Save &As"));
  connect(list_saveas_button,SIGNAL(clicked()),this,SLOT(saveAsButtonData()));

  //
  // Cancel Button
  //
  button=new QPushButton(this,"cancel_button");
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setFont(button_font);
  button->setText(tr("Cancel"));
  button->setDefault(true);
  connect(button,SIGNAL(clicked()),this,SLOT(cancelButtonData()));

  RefreshList();
}


QSize ListLogs::sizeHint() const
{
  return QSize(500,300);
}


QSizePolicy ListLogs::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


int ListLogs::exec(QString *logname,QString *svcname)
{
  list_logname=logname;
  list_svcname=svcname;
  list_saveas_button->setEnabled(rda->user()->createLog());
  RefreshList();
  return QDialog::exec();
}


void ListLogs::closeEvent(QCloseEvent *e)
{
  done(1);
}


void ListLogs::doubleClickedData(Q3ListViewItem *,const QPoint &,int)
{
  loadButtonData();
}


void ListLogs::loadButtonData()
{
  Q3ListViewItem *item=list_log_list->selectedItem();
  if(item==NULL) {
    return;
  }
  *list_logname=item->text(0);
  done(0);
}


void ListLogs::saveButtonData()
{
  if(list_logname->isEmpty()) {
    saveAsButtonData();
  }
  else {
    done(2);
  }
}


void ListLogs::saveAsButtonData()
{
  QString logname;
  QString svcname=*list_svcname;
  RDAddLog *log;

  log=new RDAddLog(&logname,&svcname,rda->station(),tr("Rename Log"),this);
  if(log->exec()<0) {
    delete log;
    return;
  }
  delete log;
  *list_logname=logname;
  *list_svcname=svcname;
  done(3);
}


void ListLogs::unloadButtonData()
{
  done(-1);
}


void ListLogs::cancelButtonData()
{
  done(1);
}


void ListLogs::RefreshList()
{
  RDSqlQuery *q;
  QString sql;
  Q3ListViewItem *l;
  QDate current_date=QDate::currentDate();
  QStringList services_list;

  list_log_list->clear();

  sql=QString("select SERVICE_NAME from SERVICE_PERMS where ")+
    "STATION_NAME=\""+RDEscapeString(rda->station()->name())+"\"";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    services_list.append( q->value(0).toString() );
  }
  delete q;

  if(services_list.size()==0) {
    return;
  }
  sql=QString("select NAME,DESCRIPTION,SERVICE from LOGS ")+
    "where (TYPE=0)&&(LOG_EXISTS=\"Y\")&&"+
    "((START_DATE<=\""+current_date.toString("yyyy-MM-dd")+"\")||"+
    "(START_DATE=\"0000-00-00\")||"+
    "(START_DATE is null))&&"+
    "((END_DATE>=\""+current_date.toString("yyyy-MM-dd")+"\")||"+
    "(END_DATE=\"0000-00-00\")||"+
    "(END_DATE is null))&&(";
  for ( QStringList::Iterator it = services_list.begin(); 
        it != services_list.end(); ++it ) {
    sql+=QString().sprintf("SERVICE=\"%s\"||",
                           (const char *)*it);
  }
  sql=sql.left(sql.length()-2);
  sql+=")";

  q=new RDSqlQuery(sql);
  while(q->next()) {
    l=new Q3ListViewItem(list_log_list);
    l->setText(0,q->value(0).toString());
    l->setText(1,q->value(1).toString());
    l->setText(2,q->value(2).toString());
  }
  delete q;
}
