// list_svcs.cpp
//
// List Rivendell Services and Report Ages
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_svcs.cpp,v 1.10 2010/07/29 19:32:37 cvs Exp $
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

#include <rdlog.h>
#include <rddb.h>
#include <list_svcs.h>
#include <svc_rec_dialog.h>
#include <pick_report_dates.h>
#include <globals.h>


ListSvcs::ListSvcs(QWidget *parent,const char *name)
  : QDialog(parent,name,true)
{
  setCaption(tr("Rivendell Services"));

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
  list_log_list=new QListView(this,"list_log_list");
  list_log_list->setAllColumnsShowFocus(true);
  list_log_list->setItemMargin(5);
  list_log_list->addColumn(tr("SERVICE"));
  list_log_list->setColumnAlignment(0,Qt::AlignLeft);
  list_log_list->addColumn(tr("OLDEST REPORT"));
  list_log_list->setColumnAlignment(1,Qt::AlignCenter);
  connect(list_log_list,
	  SIGNAL(doubleClicked(QListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(listDoubleClickedData(QListViewItem *,const QPoint &,int)));

  //
  //  Generate Report Button
  //
  list_generate_button=new QPushButton(this,"list_generate_button");
  list_generate_button->setFont(bold_font);
  list_generate_button->setText(tr("&Generate\nReports"));
  connect(list_generate_button,SIGNAL(clicked()),this,SLOT(generateData()));

  //
  //  Purge Button
  //
  list_purge_button=new QPushButton(this,"list_purge_button");
  list_purge_button->setFont(bold_font);
  list_purge_button->setText(tr("&Purge\nData"));
  connect(list_purge_button,SIGNAL(clicked()),this,SLOT(purgeData()));

  //
  //  Close Button
  //
  list_close_button=new QPushButton(this,"close_button");
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
  QListViewItem *item=list_log_list->selectedItem();
  if(item==NULL) {
    return;
  }
  PickReportDates *dialog=new PickReportDates(item->text(0),this);
  dialog->exec();
  delete dialog;
}


void ListSvcs::purgeData()
{
  QListViewItem *item=list_log_list->selectedItem();
  if(item==NULL) {
    return;
  }
  SvcRecDialog *dialog=new SvcRecDialog(item->text(0),this,"dialog");
  dialog->exec();
  delete dialog;
  RefreshLine(item);
}


void ListSvcs::listDoubleClickedData(QListViewItem *item,const QPoint &pt,
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
  QString tablename;
  QListViewItem *item;
  list_log_list->clear();
  QString sql="select NAME from SERVICES order by NAME";

  if (rdstation_conf->broadcastSecurity() == RDStation::UserSec
      && rduser != NULL) {
    QStringList services_list;
    QString sql_where;

    services_list = rduser->services();
    if(services_list.size()==0) {
      return;
    }

    sql_where=" and (";
    for ( QStringList::Iterator it = services_list.begin(); 
          it != services_list.end(); ++it ) {
      sql_where+=QString().sprintf("SERVICE=\"%s\"||",
                             (const char *)*it);
    }
    sql_where=sql_where.left(sql_where.length()-2);
    sql_where+=")";

    sql=sql+sql_where;
  } // else no filter for RDStation::HostSec

  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    item=new QListViewItem(list_log_list);
    item->setText(0,q->value(0).toString());
    tablename=q->value(0).toString();
    tablename.replace(" ","_");
    sql=QString().sprintf("select EVENT_DATETIME from `%s_SRT` \
                           order by EVENT_DATETIME",(const char *)tablename);
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


void ListSvcs::RefreshLine(QListViewItem *item)
{
  QString sql;
  RDSqlQuery *q;
  QString tablename=item->text(0);
    tablename.replace(" ","_");
    sql=QString().sprintf("select EVENT_DATETIME from `%s_SRT` \
                           order by EVENT_DATETIME",(const char *)tablename);
    q=new RDSqlQuery(sql);
    if(q->first()) {
      item->setText(1,q->value(0).toDate().toString("MM/dd/yyyy"));
    }
    else {
      item->setText(1,tr("[none]"));
    }
    delete q;
}
