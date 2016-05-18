// event_player.cpp
//
// A player class for Deck Events
//
//   (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <syslog.h>

#include <rddb.h>
#include <rdescape_string.h>

#include "event_player.h"

EventPlayer::EventPlayer(RDStation *station,int chan,QObject *parent)
{
  event_station=station;
  event_channel=chan;

  event_timer=new QTimer(this);
  connect(event_timer,SIGNAL(timeout()),this,SLOT(timeoutData()));
}


void EventPlayer::load(const QString &cutname)
{
  QString sql;
  RDSqlQuery *q;

  //
  // Load Deck Events
  //
  event_deck_events.clear();
  sql=QString("select CART_NUMBER from DECK_EVENTS where ")+
    "(STATION_NAME=\""+RDEscapeString(event_station->name())+"\")&&"+
    QString().sprintf("(CHANNEL=%d) ",event_channel)+
    "order by NUMBER";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    event_deck_events.push_back(q->value(0).toUInt());
  }
  delete q;
  printf("\n");

  //
  // Load Cut Events
  //
  event_numbers.clear();
  event_points.clear();
  event_current_event=-1;
  sql=QString("select NUMBER,POINT from CUT_EVENTS where ")+
    "CUT_NAME=\""+cutname+"\" "+
    "order by POINT";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    event_numbers.push_back(q->value(0).toInt());
    event_points.push_back(q->value(1).toInt());
  }
  delete q;
}


void EventPlayer::start(int start_ptr)
{
  event_start_point=start_ptr;
  event_start_time=QTime::currentTime();
  for(unsigned i=0;i<event_points.size();i++) {
    if(event_points[i]>=event_start_point) {
      event_current_event=i;
      event_timer->
	start(event_points[event_current_event]-event_start_point,true);
      return;
    }
  }
}


void EventPlayer::stop()
{
  event_timer->stop();
}


void EventPlayer::timeoutData()
{
  //
  // Dispatch current event
  //
  if(event_numbers[event_current_event]>=(int)event_deck_events.size()) {
    syslog(LOG_ERR,"playout cut event referenced non-existent deck event");
    return;
  }
  unsigned cartnum=event_deck_events[event_numbers[event_current_event]-1];
  if(cartnum>0) {
    emit runCart(cartnum);
  }

  //
  // Set up next event
  //
  if(++event_current_event<(int)event_numbers.size()) {
    int msec=event_points[event_current_event]-
      event_start_time.msecsTo(QTime::currentTime())-event_start_point;
    if(msec<0) {
      msec=0;
    }
    event_timer->start(msec,true);
  }  
}
