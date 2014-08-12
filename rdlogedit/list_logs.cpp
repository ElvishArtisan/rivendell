// list_logs.cpp
//
// Select a Rivendell Log
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: list_logs.cpp,v 1.9 2010/07/29 19:32:37 cvs Exp $
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
#include <rddb.h>
#include <list_logs.h>
#include <globals.h>


ListLogs::ListLogs(QString *logname,QWidget *parent,const char *name)
  : QDialog(parent,name,true)
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

  list_logname=logname;

  setCaption(tr("Select a Log"));

  //
  // Log List
  //
  list_log_list=new QListView(this,"list_log_list");
  list_log_list->setGeometry(10,10,
			    sizeHint().width()-20,sizeHint().height()-80);
  list_log_list->setAllColumnsShowFocus(true);
  list_log_list->setItemMargin(5);
  connect(list_log_list,
	  SIGNAL(doubleClicked(QListViewItem *,const QPoint &,int)),
	  this,
	  SLOT(doubleClickedData(QListViewItem *,const QPoint &,int)));
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
  button->setText(tr("&OK"));
  connect(button,SIGNAL(clicked()),this,SLOT(okButtonData()));

  //
  // Cancel Button
  //
  button=new QPushButton(this,"cancel_button");
  button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,80,50);
  button->setFont(button_font);
  button->setText(tr("&Cancel"));
  button->setDefault(true);
  connect(button,SIGNAL(clicked()),this,SLOT(cancelButtonData()));

  RefreshList();
}


QSize ListLogs::sizeHint() const
{
  return QSize(400,300);
}


QSizePolicy ListLogs::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void ListLogs::closeEvent(QCloseEvent *e)
{
  done(1);
}


void ListLogs::doubleClickedData(QListViewItem *,const QPoint &,int)
{
  okButtonData();
}


void ListLogs::okButtonData()
{
  QListViewItem *item=list_log_list->selectedItem();
  if(item==NULL) {
    return;
  }
  *list_logname=item->text(0);
  done(0);
}


void ListLogs::cancelButtonData()
{
  done(-1);
}


void ListLogs::RefreshList()
{
  RDSqlQuery *q;
  QString sql;
  QListViewItem *l;

  list_log_list->clear(); // Note: clear here, in case user has no perms.

  sql="select NAME,DESCRIPTION,SERVICE from LOGS where TYPE=0";

  if (rdstation_conf->broadcastSecurity() == RDStation::UserSec) {
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

  q=new RDSqlQuery(sql);
  while(q->next()) {
    l=new QListViewItem(list_log_list);
    l->setText(0,q->value(0).toString());
    l->setText(1,q->value(1).toString());
    l->setText(2,q->value(2).toString());
  }
  delete q;
}
