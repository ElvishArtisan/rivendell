// hourselector.cpp
//
// Hour Selector widget for RDAirPlay
//
//   (C) Copyright 2012-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include "hourselector.h"

HourSelector::HourSelector(QWidget *parent)
  : RDWidget(parent)
{
  //
  // Palettes
  //
  hour_active_palette=
    QPalette(QColor(BUTTON_STOPPED_BACKGROUND_COLOR),palette().color(QPalette::Background));

  //
  // Buttons
  //
  QSignalMapper *mapper=new QSignalMapper(this);
  connect(mapper,SIGNAL(mapped(int)),this,SLOT(hourClicked(int)));
  for(unsigned i=0;i<24;i++) {
    hour_button[i]=new QPushButton(this);
    hour_button[i]->setFont(bigButtonFont());
    hour_button[i]->setDisabled(true);
    mapper->setMapping(hour_button[i],i);
    connect(hour_button[i],SIGNAL(clicked()),mapper,SLOT(map()));
  }
  if(rda->showTwelveHourTime()) {
    hour_button[0]->setText(tr("12a"));
    hour_button[1]->setText(tr("1a"));
    hour_button[2]->setText(tr("2a"));
    hour_button[3]->setText(tr("3a"));
    hour_button[4]->setText(tr("4a"));
    hour_button[5]->setText(tr("5a"));
    hour_button[6]->setText(tr("6a"));
    hour_button[7]->setText(tr("7a"));
    hour_button[8]->setText(tr("8a"));
    hour_button[9]->setText(tr("9a"));
    hour_button[10]->setText(tr("10a"));
    hour_button[11]->setText(tr("11a"));
    hour_button[12]->setText(tr("12p"));
    hour_button[13]->setText(tr("1p"));
    hour_button[14]->setText(tr("2p"));
    hour_button[15]->setText(tr("3p"));
    hour_button[16]->setText(tr("4p"));
    hour_button[17]->setText(tr("5p"));
    hour_button[18]->setText(tr("6p"));
    hour_button[19]->setText(tr("7p"));
    hour_button[20]->setText(tr("8p"));
    hour_button[21]->setText(tr("9p"));
    hour_button[22]->setText(tr("10p"));
    hour_button[23]->setText(tr("11p"));
  }
  else {
    for(unsigned i=0;i<24;i++) {
      hour_button[i]->setText(QString::asprintf("%02u",i));
    }
  }

  //
  // Update Timer
  //
  hour_update_timer=new QTimer(this);
  hour_update_timer->setSingleShot(true);
  connect(hour_update_timer,SIGNAL(timeout()),this,SLOT(updateTimeData()));
  updateTimeData();
}


QSizePolicy HourSelector::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void HourSelector::updateHour(int hour,bool state)
{
  hour_button[hour]->setEnabled(state);
}


void HourSelector::resizeEvent(QResizeEvent *e)
{
  if(size().width()>=(24*HOURSELECTOR_BUTTON_EDGE)) {
    for(unsigned i=0;i<24;i++) {
      hour_button[i]->
	setGeometry(i*HOURSELECTOR_BUTTON_EDGE,0,
		    HOURSELECTOR_BUTTON_EDGE,HOURSELECTOR_BUTTON_EDGE);
    }
  }
  else {
    for(unsigned i=0;i<2;i++) {
      for(unsigned j=0;j<12;j++) {
	unsigned hour=12*i+j;
	hour_button[hour]->
	  setGeometry(j*HOURSELECTOR_BUTTON_EDGE,i*HOURSELECTOR_BUTTON_EDGE,
		      HOURSELECTOR_BUTTON_EDGE,HOURSELECTOR_BUTTON_EDGE);
      }
    }
  }
}


void HourSelector::hourClicked(int hour)
{
  emit hourSelected(hour);
}


void HourSelector::updateTimeData()
{
  QDateTime now=QDateTime::currentDateTime();
  QDateTime next;
  for(unsigned i=0;i<24;i++) {
    hour_button[i]->setPalette(palette());
  }
  hour_button[now.time().hour()]->setPalette(hour_active_palette);
  next=now.addSecs(60*60);			// Next hour
  next.setTime(QTime(next.time().hour(),0,1));	// Set top of hour HH:00:01
  hour_update_timer->start(now.msecsTo(next));	// Start timer
}
