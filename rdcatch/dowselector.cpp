// dowselector.cpp
//
// Day of the week selector
//
//   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdconf.h>
#include <rddb.h>

#include "dowselector.h"

DowSelector::DowSelector(QWidget *parent)
  : RDWidget(parent)
{
  d_group_box=new QGroupBox(tr("Active Days"),this);
  d_group_box->setFont(labelFont());

  QDate monday=QDate::currentDate();
  while(monday.dayOfWeek()!=1) {
    monday=monday.addDays(1);
  }

  for(int i=0;i<7;i++) {
    d_labels[i]=new QLabel(monday.addDays(i).toString("dddd"),this);
    d_labels[i]->setFont(defaultFont());
    d_checks[i]=new QCheckBox(this);
    d_checks[i]->setFont(defaultFont());
  }
}


QSize DowSelector::sizeHint() const
{
  return QSize(500,62);
}


QSizePolicy DowSelector::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


bool DowSelector::dayOfWeekEnabled(int dow)
{
  return d_checks[dow]->isChecked();
}


void DowSelector::toRecording(unsigned record_id) const
{
  QString sql=QString("update `RECORDINGS` set ")+
    "`MON`='"+RDYesNo(d_checks[0]->isChecked())+"',"+
    "`TUE`='"+RDYesNo(d_checks[1]->isChecked())+"',"+
    "`WED`='"+RDYesNo(d_checks[2]->isChecked())+"',"+
    "`THU`='"+RDYesNo(d_checks[3]->isChecked())+"',"+
    "`FRI`='"+RDYesNo(d_checks[4]->isChecked())+"',"+
    "`SAT`='"+RDYesNo(d_checks[5]->isChecked())+"',"+
    "`SUN`='"+RDYesNo(d_checks[6]->isChecked())+"' "+
    "where "+
    QString().sprintf("`ID`=%u",record_id);
  RDSqlQuery::apply(sql);
}


void DowSelector::fromRecording(unsigned record_id)
{
  QString sql=QString("select ")+
    "`MON`,"+  // 00
    "`TUE`,"+  // 01
    "`WED`,"+  // 02
    "`THU`,"+  // 03
    "`FRI`,"+  // 04
    "`SAT`,"+  // 05
    "`SUN` "+  // 06
    "from `RECORDINGS` where "+
    QString().sprintf("`ID`=%u",record_id);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    for(int i=0;i<7;i++) {
      d_checks[i]->setChecked(q->value(i).toString()=="Y");
    }
  }
  delete q;
}


void DowSelector::enableDayOfWeek(int dow,bool state)
{
  d_checks[dow]->setChecked(state);
}


void DowSelector::resizeEvent(QResizeEvent *e)
{
  d_group_box->setGeometry(0,0,width(),height());

  d_checks[0]->setGeometry(10,16,20,20);
  d_labels[0]->setGeometry(30,16,115,20);
  
  d_checks[1]->setGeometry(105,16,20,20);
  d_labels[1]->setGeometry(125,16,115,20);
  
  d_checks[2]->setGeometry(205,16,20,20);
  d_labels[2]->setGeometry(225,16,115,20);
  
  d_checks[3]->setGeometry(325,16,20,20);
  d_labels[3]->setGeometry(345,16,115,20);
  
  d_checks[4]->setGeometry(430,16,20,20);
  d_labels[4]->setGeometry(450,16,40,20);
  
  d_checks[5]->setGeometry(120,38,20,20);
  d_labels[5]->setGeometry(140,38,60,20);
  
  d_checks[6]->setGeometry(290,38,20,20);
  d_labels[6]->setGeometry(310,38,60,20);
}
