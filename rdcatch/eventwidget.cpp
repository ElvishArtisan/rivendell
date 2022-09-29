// eventwidget.cpp
//
// Widget for setting basic event parameters in rdcatch(1)
//
//   (C) Copyright 2022 Fred Gleason <fredg@paravelsystems.com>
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
#include <rdiconengine.h>

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
  d_time_edit=new RDTimeEdit(this);
  if(d_event_type==EventWidget::RecordEvent) {
    d_time_label->hide();
    d_time_edit->hide();
  }
  
  switch(d_event_type) {
  case EventWidget::RecordEvent:
    d_location_box->clear();
    sql=QString("select ")+
      "`STATION_NAME`,"+  // 00
      "`CHANNEL` "+       // 01
      "from `DECKS` where "+
      "(`CARD_NUMBER`!=-1)&&"+
      "(`PORT_NUMBER`!=-1)&&"+
      "(`CHANNEL`!=0)&&"+
      "(`CHANNEL`<9) "+
      "order by `STATION_NAME`,`CHANNEL`";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      d_location_box->
	insertItem(d_location_box->count(),
		   q->value(0).toString()+
		   QString::asprintf(" : %dR",q->value(1).toInt()),
		   q->value(0).toString()+"\t"+
		   QString::asprintf("%d",q->value(1).toInt()));
      d_location_box->
	setItemData(d_location_box->count()-1,
		    rda->iconEngine()->
		    catchIcon(RDRecording::Recording),Qt::DecorationRole);
    }
    delete q;
    break;

  case EventWidget::PlayEvent:
    d_location_box->clear();
    sql=QString("select ")+
      "`STATION_NAME`,"+  // 00
      "`CHANNEL` "+       // 01
      "from `DECKS` where "+
      "(`CARD_NUMBER`!=-1)&&"+
      "(`PORT_NUMBER`!=-1)&&"+
      "(`CHANNEL`>128) order by `STATION_NAME`,`CHANNEL`";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      d_location_box->
	insertItem(d_location_box->count(),
		   q->value(0).toString()+
		   QString::asprintf(" : %dP",q->value(1).toInt()-128),
		   q->value(0).toString()+"\t"+
		   QString::asprintf("%d",q->value(1).toInt()-128));
      d_location_box->
	setItemData(d_location_box->count()-1,
		    rda->iconEngine()->
		    catchIcon(RDRecording::Playout),Qt::DecorationRole);
    }
    delete q;
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
      d_location_box->
	setItemData(d_location_box->count()-1,
		    rda->iconEngine()->stationIcon(),Qt::DecorationRole);
    }
    delete q;
    break;
  }
}


QSize EventWidget::sizeHint() const
{
  return QSize(550,23);
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
    "`STATION_NAME`='"+RDEscapeString(d_current_station_name)+"' ";
  switch(d_event_type) {
    case EventWidget::RecordEvent:
      sql+=QString::asprintf(",`CHANNEL`=%u ",d_current_deck_number);
      break;

    case EventWidget::PlayEvent:
      sql+=",`START_TIME`='"+RDEscapeString(d_time_edit->time().
					 toString("hh:mm:ss"))+"' ";
      sql+=QString::asprintf(",`CHANNEL`=%u ",d_current_deck_number+128);
      break;

  case EventWidget::OtherEvent:
    sql+=",`START_TIME`='"+RDEscapeString(d_time_edit->time().
					 toString("hh:mm:ss"))+"' ";
    break;
  }
  sql+="where "+
    QString::asprintf("`ID`=%u",record_id);
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
    QString::asprintf("`ID`=%u",record_id);
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    d_state_check->setChecked(q->value(0).toString()=="Y");
    switch(d_event_type) {
    case EventWidget::RecordEvent:
      d_location_box->
	setCurrentText(q->value(1).toString()+
		       QString::asprintf(" : %uR",q->value(2).toUInt()));
      d_current_station_name=q->value(1).toString();
      d_current_deck_number=q->value(2).toUInt();
      break;

    case EventWidget::PlayEvent:
      d_current_deck_number=q->value(2).toUInt()-128;
      if(d_current_deck_number<=0) {
	d_current_deck_number=1;
      }
      d_time_edit->setTime(q->value(3).toTime());
      d_location_box->
	setCurrentText(q->value(1).toString()+
		       QString::asprintf(" : %uP",q->value(2).toUInt()-128));
      d_current_station_name=q->value(1).toString();
      break;

    case EventWidget::OtherEvent:
      if(q->value(3).toTime().isValid()) {
	d_time_edit->setTime(q->value(3).toTime());
      }
      else {
	d_time_edit->setTime(QTime(0,0,0));
      }
      d_location_box->setCurrentText(q->value(1).toString());
      d_current_station_name=q->value(1).toString();
      d_current_deck_number=-1;
      break;
    }
  }
  delete q;
  locationActivatedData(d_location_box->currentText());
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
    d_current_deck_number=f0.at(1).left(f0.at(1).length()-1).toInt();
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
  d_time_edit->setGeometry(size().width()-130,2,100,20);
  d_time_label->setGeometry(size().width()-215,2,80,20);
}
