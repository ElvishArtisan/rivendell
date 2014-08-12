// rdlist_logs.cpp
//
// Select a Rivendell Log
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdlist_logs.cpp,v 1.12 2010/07/29 19:32:33 cvs Exp $
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
#include <qdatetime.h>
#include <rddb.h>
#include <rdlist_logs.h>

RDListLogs::RDListLogs(QString *logname,const QString &stationname,
		       QWidget *parent,const char *name,RDUser *rduser)
  : QDialog(parent,name,true)
{
  list_stationname=stationname;
  list_logname=logname;
  list_user=rduser;

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

  if (list_user != 0) { // RDStation::UserSec
    setCaption(QString().sprintf("%s%s",(const char *)tr("Select Log - User: "),
                                 (const char *)list_user->name() ));
  } else { // RDStation::HostSec
    setCaption(tr("Select Log"));
  }

  //
  // Log List
  //
  list_log_list=new QListView(this,"list_log_list");
  list_log_list->setGeometry(10,10,
			    sizeHint().width()-20,sizeHint().height()-80);
  list_log_list->setAllColumnsShowFocus(true);
  list_log_list->setItemMargin(5);
  list_log_list->setSelectionMode(QListView::Single);
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
  // OK Button
  //
  QPushButton *button=new QPushButton(this,"load_button");
  button->setGeometry(sizeHint().width()-190,sizeHint().height()-60,80,50);
  button->setFont(button_font);
  button->setText(tr("OK"));
  connect(button,SIGNAL(clicked()),this,SLOT(okButtonData()));

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


void RDListLogs::doubleClickedData(QListViewItem *,const QPoint &,int)
{
  okButtonData();
}


void RDListLogs::okButtonData()
{
  QListViewItem *item=list_log_list->selectedItem();
  if(item==NULL) {
    return;
  }
  *list_logname=item->text(0);
  done(0);
}


void RDListLogs::cancelButtonData()
{
  done(1);
}


void RDListLogs::RefreshList()
{
  RDSqlQuery *q;
  QString sql;
  QListViewItem *l;
  QListViewItem *view_item=NULL;
  QDate current_date=QDate::currentDate();
  QStringList services_list;

  list_log_list->clear();

  if (list_user != 0) { // RDStation::UserSec
    services_list = list_user->services();
  } else { // RDStation::HostSec
    sql=QString().sprintf("select SERVICE_NAME from SERVICE_PERMS \
                           where STATION_NAME=\"%s\"",
    		          (const char *)list_stationname);
    RDSqlQuery *q=new RDSqlQuery(sql);
    while(q->next()) {
      services_list.append( q->value(0).toString() );
    }
    delete q;
  }

  if(services_list.size()==0) {
    return;
  }
  sql=QString().sprintf("select NAME,DESCRIPTION,SERVICE from LOGS \
                         where (TYPE=0)&&(LOG_EXISTS=\"Y\")&&\
                         ((START_DATE<=\"%s\")||(START_DATE=\"0000-00-00\"))&&\
                         ((END_DATE>=\"%s\")||(END_DATE=\"0000-00-00\"))&&(",
			(const char *)current_date.toString("yyyy-MM-dd"),
			(const char *)current_date.toString("yyyy-MM-dd"));
  for ( QStringList::Iterator it = services_list.begin();
        it != services_list.end(); ++it ) {
    sql+=QString().sprintf("SERVICE=\"%s\"||",
               (const char *)*it);
  }
  sql=sql.left(sql.length()-2);
  sql+=")";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    l=new QListViewItem(list_log_list);
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
