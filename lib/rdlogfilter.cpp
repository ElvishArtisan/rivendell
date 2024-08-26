// rdlogfilter.cpp
//
// Filter widget for picking Rivendell logs.
//
//   (C) Copyright 2017-2024 Fred Gleason <fredg@paravelsystems.com>
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
#include "rd.h"
#include "rdescape_string.h"
#include "rdlogfilter.h"

RDLogFilter::RDLogFilter(RDLogFilter::FilterMode mode,QWidget *parent)
   : QWidget(parent)
{
  QString sql;
  RDSqlQuery *q;

  filter_filter_mode=mode;

  QFont label_font(font().family(),font().pointSize(),QFont::Bold);
  QFont check_font(font().family(),font().pointSize()-2,QFont::DemiBold);

  //
  // Service
  //
  filter_service_label=new QLabel(tr("Service")+":",this);
  filter_service_label->setFont(label_font);
  filter_service_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  filter_service_box=new QComboBox(this);
  filter_service_label->setBuddy(filter_service_box);
  switch(mode) {
  case RDLogFilter::NoFilter:
    filter_service_box->insertItem(filter_service_box->count(),tr("ALL"));
    sql=QString("select `NAME` from `SERVICES` order by `NAME`");
    q=new RDSqlQuery(sql);
    while(q->next()) {
      filter_service_box->
	insertItem(filter_service_box->count(),rda->iconEngine()->serviceIcon(),
		   q->value(0).toString());
    }
    delete q;
    break;

  case RDLogFilter::UserFilter:
    changeUser();
    break;

  case RDLogFilter::StationFilter:
    filter_service_box->insertItem(filter_service_box->count(),tr("ALL"));
    sql=QString("select SERVICE_NAME from SERVICE_PERMS where ")+
      "STATION_NAME='"+RDEscapeString(rda->config()->stationName())+"' "+
      "order by `SERVICE_NAME`";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      filter_service_box->
	insertItem(filter_service_box->count(),rda->iconEngine()->serviceIcon(),
		   q->value(0).toString());
    }
    delete q;
    break;
  }
  connect(filter_service_box,SIGNAL(activated(int)),
 	  this,SLOT(serviceChangedData(int)));

  //
  // Filter
  //
  filter_filter_label=new QLabel(tr("Filter")+":",this);
  filter_filter_label->setFont(label_font);
  filter_filter_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  filter_filter_edit=new QLineEdit(this);
  filter_clear_button=new QPushButton(tr("Clear"),this);
  filter_filter_label->setBuddy(filter_filter_edit);
  connect(filter_filter_edit,SIGNAL(textChanged(const QString &)),
 	  this,SLOT(filterChangedData(const QString &)));
  connect(filter_clear_button,SIGNAL(clicked()),this,SLOT(filterClearedData()));
  
  //
  // Show Recent
  //
  filter_recent_check=new QCheckBox(this);
  filter_recent_label=new QLabel(tr("Show Only Recent Logs"),this);
  filter_recent_label->setFont(check_font);
  filter_recent_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  connect(filter_recent_check,SIGNAL(clicked()),
	  this,SLOT(filterChangedData()));
}


RDLogFilter::~RDLogFilter()
{
  delete filter_clear_button;
  delete filter_filter_edit;
  delete filter_filter_label;
  delete filter_recent_label;
}


QSize RDLogFilter::sizeHint() const
{
  return QSize(400,60);
}


QSizePolicy RDLogFilter::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
}


QString RDLogFilter::filterServiceName() const
{
  return filter_service_box->currentText();
}


QString RDLogFilter::filterText() const
{
  return filter_filter_edit->text();
}


QString RDLogFilter::whereSql() const
{
  QString sql="";
  
  if(filter_service_box->currentIndex()!=0) {
    sql+="&&(`LOGS`.`SERVICE`='"+
      RDEscapeString(filter_service_box->currentText())+"')";
  }
  else {
    sql+="&&(";
    for(int i=1;i<filter_service_box->count();i++) {
      sql+="(`SERVICE`='"+RDEscapeString(filter_service_box->itemText(i))+"')||";
    }
    sql=sql.left(sql.length()-2);
    sql+=")";
  }
  QString filter=filter_filter_edit->text();
  if(!filter.isEmpty()) {
    if(filter_service_box->currentIndex()==0) {
      sql+="&&((`LOGS`.`NAME` like '%%"+RDEscapeString(filter)+"%%')||";
      sql+="(`LOGS`.`DESCRIPTION` like '%%"+RDEscapeString(filter)+"%%')||";
      sql+="(`LOGS`.`SERVICE` like '%%"+RDEscapeString(filter)+"%%'))";
    }
    else {
      sql+="&&((`LOGS`.`NAME` like '%%"+RDEscapeString(filter)+"%%')||";
      sql+="(`LOGS`.`DESCRIPTION` like '%%"+RDEscapeString(filter)+"%%'))";
    }
  }

  return sql;
}


QString RDLogFilter::limitSql() const
{
  QString sql=" ";

  if(filter_recent_check->isChecked()) {
    sql+=", `LOGS`.`ORIGIN_DATETIME` desc ";
    sql+=QString::asprintf("limit %d ",RD_LOGFILTER_LIMIT_QUAN);
  }

  return sql;
}


void RDLogFilter::changeUser()
{
  if(filter_filter_mode==RDLogFilter::UserFilter) {
    if(filter_current_username!=rda->user()->name()) {
      filter_service_box->clear();
      filter_service_box->insertItem(filter_service_box->count(),tr("ALL"));
      QString sql=
	QString("select `SERVICE_NAME` from `USER_SERVICE_PERMS` where ")+
	"`USER_NAME`='"+RDEscapeString(rda->user()->name())+"' "+
	"order by `SERVICE_NAME`";
      RDSqlQuery *q=new RDSqlQuery(sql);
      while(q->next()) {
	filter_service_box->insertItem(filter_service_box->count(),
				       rda->iconEngine()->serviceIcon(),
				       q->value(0).toString());
      }
      delete q;
      filter_current_username=rda->user()->name();
    }
  }
}


void RDLogFilter::filterChangedData(const QString &str)
{
  LogSearchString(filter_filter_edit->text());
  emit filterChanged(whereSql(),limitSql());
}


void RDLogFilter::filterChangedData()
{
  LogSearchString(filter_filter_edit->text());
  emit filterChanged(whereSql(),limitSql());
}


void RDLogFilter::filterClearedData()
{
  filter_filter_edit->clear();
  filterChangedData();
}


void RDLogFilter::serviceChangedData(int n)
{
  LogSearchString(filter_filter_edit->text());
  emit filterChanged(whereSql(),limitSql());
}


void RDLogFilter::resizeEvent(QResizeEvent *e)
{
  filter_service_label->setGeometry(0,2,70,20);
  filter_service_box->setGeometry(75,2,140,20);
  
  filter_filter_label->setGeometry(220,2,50,20);
  filter_filter_edit->setGeometry(275,2,size().width()-360,20);
  
  filter_clear_button->setGeometry(size().width()-70,0,50,25);
  
  filter_recent_check->setGeometry(275,27,15,15);
  filter_recent_label->setGeometry(295,25,200,20);
}


void RDLogFilter::LogSearchString(const QString &str)
{
  if(rda->config()->logSearchStrings()) {
    rda->syslog(rda->config()->logSearchStringsLevel(),
		"searching log list by string: \"%s\" [%s]",
		str.toUtf8().constData(),
		RDConfig::hexify(str).toUtf8().constData());
  }
}
