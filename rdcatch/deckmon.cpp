// deckmon.cpp
//
// Monitor a Rivendell RDCatch Deck
//
//   (C) Copyright 2002-2022 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdescape_string.h>

#include "colors.h"
#include "deckmon.h"
#include "globals.h"

DeckMon::DeckMon(QString station,unsigned channel,QWidget *parent)
  : RDFrame(parent)
{
  mon_station=station;
  mon_channel=channel;
  mon_monitor_state=false;

  setFrameStyle(Box|Raised);
  setLineWidth(1);
  setMidLineWidth(2);

  //
  // Generate Palettes
  //
  mon_red_palette=palette();
  mon_red_palette.setColor(QPalette::Background,Qt::darkRed);
  mon_red_palette.setColor(QPalette::Foreground,Qt::white);
  mon_red_stylesheet="color: white;background-color: darkRed;";
  mon_dark_palette=palette();

  //
  // Station/Channel
  //
  mon_station_label=new QLabel(this);
  mon_station_label->setFont(defaultFont());
  if((mon_channel>0)&&(mon_channel<(MAX_DECKS+1))) {
    mon_station_label->
      setText(mon_station+QString::asprintf(" : %uR",mon_channel));
  }
  if((mon_channel>128)&&(mon_channel<(MAX_DECKS+129))) {
    mon_station_label->
      setText(mon_station+QString::asprintf(" : %uP",mon_channel-128));
  }

  //
  // Monitor Button
  //
  mon_monitor_button=new QPushButton(this);
  mon_monitor_button->setFont(subButtonFont());
  mon_monitor_button->setText(tr("MON"));
  mon_monitor_palette=new QPalette(QColor(BUTTON_MONITOR_FLASHING_COLOR),
				   palette().color(QPalette::Background));
  if((mon_channel>128)&&(mon_channel<(MAX_DECKS+129))) {
    mon_monitor_button->hide();
  }
  connect(mon_monitor_button,SIGNAL(clicked()),
	  this,SLOT(monitorButtonData()));

  //
  // Abort Button
  //
  mon_abort_button=new QPushButton(this);
  mon_abort_button->setFont(subButtonFont());
  mon_abort_button->setText(tr("ABORT"));
  mon_abort_button->setDisabled(true);
  connect(mon_abort_button,SIGNAL(clicked()),this,SLOT(abortButtonData()));

  //
  // Cut
  //
  mon_cut_label=new QLabel(this);
  mon_cut_label->setFont(defaultFont());

  //
  // Event Indicator
  //
  mon_event_light=new EventLight(this);
  mon_event_light->setFont(labelFont());

  //
  // Status
  //
  mon_status_label=new QLabel(tr("OFFLINE"),this);
  mon_status_label->setFont(defaultFont());

  //
  // Audio Meter
  //
  mon_left_meter=new RDPlayMeter(RDSegMeter::Right,this);
  mon_left_meter->setMode(RDSegMeter::Peak);
  mon_left_meter->setRange(-4600,-800);
  mon_left_meter->setHighThreshold(-1600);
  mon_left_meter->setClipThreshold(-1100);
  mon_left_meter->setLabel(tr("L"));
  mon_right_meter=new RDPlayMeter(RDSegMeter::Right,this);
  mon_right_meter->setMode(RDSegMeter::Peak);
  mon_right_meter->setRange(-4600,-800);
  mon_right_meter->setHighThreshold(-1600);
  mon_right_meter->setClipThreshold(-1100);
  mon_right_meter->setLabel(tr("R"));
}


DeckMon::~DeckMon()
{
}


QSize DeckMon::sizeHint() const
{
  return QSize(780,30);
}


QSizePolicy DeckMon::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void DeckMon::enableMonitorButton(bool state)
{
  mon_monitor_button->setEnabled(state);
}


void DeckMon::processCatchEvent(RDCatchEvent *evt)
{
  //  printf("processCatchEvent(): %s\n",evt->dump().toUtf8().constData());

  QList<RDCatchMeterLevel> meter_levels;

  if(evt->hostName()==mon_station) {
    switch(evt->operation()) {
    case RDCatchEvent::DeckEventProcessedOp:
      if(evt->deckChannel()==mon_channel) {
	mon_event_light->trigger(evt->eventNumber());
      }
      break;

    case RDCatchEvent::DeckStatusResponseOp:
      if(evt->deckChannel()==mon_channel) {
	SetStatus(evt->deckStatus(),evt->eventId(),
		  RDCut::cutName(evt->cartNumber(),evt->cutNumber()));
      }
      break;

    case RDCatchEvent::SendMeterLevelsOp:
      meter_levels=evt->meterLevels();
      for(int i=0;i<meter_levels.size();i++) {
	if(meter_levels.at(i).deckChannel()==mon_channel) {
	  mon_left_meter->
	    setPeakBar(meter_levels.at(i).level(RDCatchMeterLevel::Left));
	  mon_right_meter->
	    setPeakBar(meter_levels.at(i).level(RDCatchMeterLevel::Right));
	}
      }
      break;

    case RDCatchEvent::SetInputMonitorResponseOp:
      if((evt->hostName()==mon_station)&&
	 (evt->deckChannel()==mon_channel)&&
	 (evt->inputMonitorActive()!=mon_monitor_state)) {
	mon_monitor_state=evt->inputMonitorActive();
	if(mon_monitor_state) {
	  mon_monitor_button->setPalette(mon_red_palette);
	  mon_monitor_button->setStyleSheet(mon_red_stylesheet);
	}
	else {
	  mon_monitor_button->setPalette(palette());
	  mon_monitor_button->setStyleSheet("");
	}
      }
      break;

    case RDCatchEvent::DeckStatusQueryOp:
    case RDCatchEvent::ReloadDecksOp:
    case RDCatchEvent::StopDeckOp:
    case RDCatchEvent::SetInputMonitorOp:
    case RDCatchEvent::NullOp:
    case RDCatchEvent::LastOp:
      break;
    }
  }
}


void DeckMon::monitorButtonData()
{
  RDCatchEvent *evt=new RDCatchEvent();

  evt->setOperation(RDCatchEvent::SetInputMonitorOp);
  evt->setTargetHostName(mon_station);
  evt->setDeckChannel(mon_channel);
  evt->setInputMonitorActive(!mon_monitor_state);
  rda->ripc()->sendCatchEvent(evt);

  delete evt;
}


void DeckMon::abortButtonData()
{
  RDCatchEvent *evt=new RDCatchEvent();

  evt->setOperation(RDCatchEvent::StopDeckOp);
  evt->setTargetHostName(mon_station);
  evt->setDeckChannel(mon_channel);
  rda->ripc()->sendCatchEvent(evt);

  delete evt;
}


void DeckMon::SetStatus(RDDeck::Status status,int id,const QString &cutname)
{
  if(id==0) {
    mon_status_label->setText(tr("IDLE"));
    SetCutInfo(0,"");
    mon_left_meter->setPeakBar(-10000);
    mon_right_meter->setPeakBar(-10000);
    mon_abort_button->setDisabled(true);
    mon_event_light->setDisabled(true);
    return;
  }
  switch(status) {
  case RDDeck::Offline:
    mon_status_label->setText(tr("OFFLINE"));
    SetCutInfo(0,"");
    mon_left_meter->setPeakBar(-10000);
    mon_right_meter->setPeakBar(-10000);
    mon_abort_button->setDisabled(true);
    mon_event_light->setDisabled(true);
    break;

  case RDDeck::Idle:
    mon_status_label->setText(tr("IDLE"));
    SetCutInfo(0,"");
    mon_left_meter->setPeakBar(-10000);
    mon_right_meter->setPeakBar(-10000);
    mon_abort_button->setDisabled(true);
    mon_event_light->setDisabled(true);
    break;

  case RDDeck::Ready:
    mon_status_label->setText(tr("READY"));
    SetCutInfo(id,"");
    mon_left_meter->setPeakBar(-10000);
    mon_right_meter->setPeakBar(-10000);
    mon_abort_button->setEnabled(true);
    mon_event_light->setDisabled(true);
    break;

  case RDDeck::Waiting:
    mon_status_label->setText(tr("WAITING"));
    SetCutInfo(id,"");
    mon_left_meter->setPeakBar(-10000);
    mon_right_meter->setPeakBar(-10000);
    mon_abort_button->setEnabled(true);
    mon_event_light->setDisabled(true);
    break;

  case RDDeck::Recording:
    if((mon_channel>0)&&(mon_channel<(MAX_DECKS+1))) {
      mon_status_label->setText(tr("RECORDING"));
    }
    if((mon_channel>128)&&(mon_channel<(MAX_DECKS+129))) {
      mon_status_label->setText(tr("PLAYING"));
    }
    SetCutInfo(id,cutname);
    mon_abort_button->setEnabled(true);
    mon_event_light->setEnabled(true);
    break;

  case RDDeck::LastStatus:
    break;
  }
}


void DeckMon::resizeEvent(QResizeEvent *e)
{
  mon_station_label->setGeometry(10,6,140,18);
  mon_monitor_button->setGeometry(155,5,40,20);
  mon_abort_button->setGeometry(200,5,40,20);
  mon_cut_label->setGeometry(245,6,e->size().width()-595,18);
  mon_event_light->setGeometry(e->size().width()-345,6,20,18);
  mon_status_label->setGeometry(e->size().width()-320,6,80,18);
  mon_left_meter->setGeometry(e->size().width()-235,6,225,10);
  mon_right_meter->setGeometry(e->size().width()-235,16,225,10);
}


void DeckMon::SetCutInfo(int id,const QString &cutname)
{
  QString sql;
  RDSqlQuery *q;
  RDSqlQuery *q1;

  if(id<0) {
    mon_cut_label->setText(tr("[multiple events]"));
    return;
  }
  if(id==0) {
    mon_cut_label->setText("");
    return;
  }
  sql=QString("select `DESCRIPTION` from `RECORDINGS` where ")+
    QString::asprintf("`ID`=%d",id);
  q=new RDSqlQuery(sql);
  if(!q->first()) {
    if(cutname.isEmpty()) {
      mon_cut_label->setText(tr("[unknown cut]"));
    }
    else {
      sql=QString("select ")+
	"`CART`.`TITLE`,"+        // 00
	"`CUTS`.`DESCRIPTION` "+  // 01
	"from `CART` left join `CUTS` "+
	"on `CART`.`NUMBER`=`CUTS`.`CART_NUMBER` where "+
	"`CUTS`.`CUT_NAME`='"+RDEscapeString(cutname)+"'";
      q1=new RDSqlQuery(sql);
      if(q1->first()) {
	mon_cut_label->
	  setText(q1->value(0).toString()+"->"+q1->value(1).toString());
      }
      else {
	mon_cut_label->setText(tr("[unknown cut]"));
      }
      delete q1;
    }
    delete q;
    return;
  }
  if(q->value(0).toString().isEmpty()) {
    mon_cut_label->setText(tr("[no description]"));
    delete q;
    return;
  }
  mon_cut_label->setText(q->value(0).toString());
  delete q;
}
