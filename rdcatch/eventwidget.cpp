// eventwidget.cpp
//
// Widget for setting basic event parameters in rdcatch(1)
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

#include <QStringList>

#include <rdconf.h>
#include <rddb.h>
#include <rdescape_string.h>

#include "eventwidget.h"

EventWidget::EventWidget(EventWidget::EventType type,QWidget *parent)
  : RDWidget(parent)
{
  QString sql;
  RDSqlQuery *q=NULL;
  
  d_event_type=type;

  //
  // Event Active
  //
  d_state_check=new QCheckBox(this);
  d_state_label=new QLabel(tr("Event Active"),this);
  d_state_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
  d_state_label->setFont(labelFont());

  //
  // Location
  //
  d_location_label=new QLabel(tr("Location")+":",this);
  d_location_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  d_location_label->setFont(labelFont());
  d_location_box=new QComboBox(this);
  connect(d_location_box,SIGNAL(activated(const QString &)),
	  this,SLOT(locationActivatedData(const QString &)));

  //
  // Start Time
  //
  d_time_label=new QLabel(tr("Start Time")+":",this);
  d_time_label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
  d_time_label->setFont(labelFont());
  d_time_edit=new QTimeEdit(this);
  d_time_edit->setDisplayFormat("hh:mm:ss");

  switch(d_event_type) {
  case EventWidget::RecordEvent:
    break;

  case EventWidget::PlayEvent:
    break;

  case EventWidget::OtherEvent:
    sql=QString("select ")+
      "`NAME` "+  // 00
      "from `STATIONS` "+
      "order by `NAME`";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      d_location_box->
	insertItem(d_location_box->count(),q->value(0).toString());
    }
    delete q;
    break;
  }
}


QSize EventWidget::sizeHint() const
{
  return QSize(510,23);
}


QSizePolicy EventWidget::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


bool EventWidget::isActive() const
{
  return d_state_check->isChecked();
}


QString EventWidget::stationName() const
{
  return d_current_station_name;
}


int EventWidget::deckNumber() const
{
  return d_current_deck_number;
}


QTime EventWidget::startTime()
{
  return d_time_edit->time();
}


void EventWidget::toRecording(unsigned record_id) const
{
  QString sql=QString("update `RECORDINGS` set ")+
    "`IS_ACTIVE`='"+RDYesNo(d_state_check->isChecked())+"',"+
    "`STATION_NAME`='"+RDEscapeString(d_current_station_name)+"',"+
    "`START_TIME`='"+
    RDEscapeString(d_time_edit->time().toString("hh:mm:ss"))+"' "+
    "where "+
    QString().sprintf("`ID`=%u",record_id);
  RDSqlQuery::apply(sql);
}


void EventWidget::fromRecording(unsigned record_id)
{
  QString sql=QString("select ")+
    "`IS_ACTIVE`,"+     // 00
    "`STATION_NAME`,"+  // 01
    "`CHANNEL`,"+       // 02
    "`START_TIME` "+    // 03
    "from `RECORDINGS` where "+
    QString().sprintf("`ID`=%u",record_id);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    d_state_check->setChecked(q->value(0).toString()=="Y");
    d_time_edit->setTime(q->value(3).toTime());
    switch(d_event_type) {
    case EventWidget::RecordEvent:
      break;

    case EventWidget::PlayEvent:
      break;

    case EventWidget::OtherEvent:
      d_location_box->setCurrentText(q->value(1).toString());
      d_current_station_name=q->value(1).toString();
      d_current_deck_number=-1;
      break;
    }
  }
  delete q;
}


void EventWidget::locationActivatedData(const QString &str)
{
  QStringList f0=str.split(":",QString::KeepEmptyParts);

  switch(f0.size()) {
  case 1:
    d_current_station_name=f0.at(0).trimmed();
    emit locationChanged(d_current_station_name);
    break;

  case 2:
    d_current_station_name=f0.at(0).trimmed();
    d_current_deck_number=f0.at(1).toInt()-1;
    emit locationChanged(d_current_station_name,d_current_deck_number);
    break;
  }
}


void EventWidget::resizeEvent(QResizeEvent *e)
{
  d_state_check->setGeometry(0,1,20,20);
  d_state_label->setGeometry(20,1,125,20);
  d_location_box->setGeometry(190,0,140,23);
  d_location_label->setGeometry(115,0,70,23);
  d_time_edit->setGeometry(size().width()-90,2,80,20);
  d_time_label->setGeometry(size().width()-175,2,80,20);
}
