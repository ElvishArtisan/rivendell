// rdlogfilter.cpp
//
// Filter widget for picking Rivendell logs.
//
//   (C) Copyright 2017 Fred Gleason <fredg@paravelsystems.com>
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

#include "rddb.h"
#include "rd.h"
#include "rdescape_string.h"
#include "rdlogfilter.h"

RDLogFilter::RDLogFilter(QWidget *parent)
   : QWidget(parent)
 {
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
   filter_service_box->insertItem(tr("ALL"));
   QString sql=QString("select NAME from SERVICES order by NAME");
   RDSqlQuery *q=new RDSqlQuery(sql);
   while(q->next()) {
     filter_service_box->insertItem(q->value(0).toString());
   }
   delete q;
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
   connect(filter_clear_button,SIGNAL(clicked()),this,SLOT(filterChangedData()));

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


 QString RDLogFilter::whereSql() const
 {
   QString sql="";

   if(filter_service_box->currentItem()!=0) {
     sql+="&&(LOGS.SERVICE=\""+
       RDEscapeString(filter_service_box->currentText())+"\")";
   }
   QString filter=filter_filter_edit->text();
   if(!filter.isEmpty()) {
     sql+="&&((LOGS.NAME like \"%%"+RDEscapeString(filter)+"%%\")||";
     sql+="(LOGS.DESCRIPTION like \"%%"+RDEscapeString(filter)+"%%\")||";
     sql+="(LOGS.SERVICE like \"%%"+RDEscapeString(filter)+"%%\"))";
   }
   if(filter_recent_check->isChecked()) {
     sql+=QString().sprintf("order by LOGS.ORIGIN_DATETIME desc limit %d",
 			   RD_LOGFILTER_LIMIT_QUAN);
   }

   return sql;
 }


void RDLogFilter::setServices(const QStringList &svc_names)
{
   filter_service_box->clear();
   for(unsigned i=0;i<svc_names.size();i++) {
     filter_service_box->insertItem(svc_names[i]);
   }
}


 void RDLogFilter::filterChangedData(const QString &str)
 {
   emit filterChanged(whereSql());
 }


 void RDLogFilter::filterChangedData()
 {
   emit filterChanged(whereSql());
 }


 void RDLogFilter::serviceChangedData(int n)
 {
   emit filterChanged(whereSql());
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
