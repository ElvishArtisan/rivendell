//   rdtimeengine.cpp
//
//   An event timer engine.
//
//   (C) Copyright 2002-2022 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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

#include "rdtimeengine.h"

RDTimeEngine::RDTimeEngine(QObject *parent)
  : QObject(parent)
{
  d_mapper=new QSignalMapper(this);
  connect(d_mapper,SIGNAL(mapped(int)),this,SLOT(timerData(int)));
}


RDTimeEngine::~RDTimeEngine()
{
  clear();
  delete d_mapper;
}


void RDTimeEngine::clear()
{
  for(QMap<int,QTimer *>::const_iterator it=d_timers.begin();
      it!=d_timers.end();it++) {
    d_mapper->removeMappings(it.value());
    delete it.value();
  }
  d_timers.clear();
  d_times.clear();
}


QTime RDTimeEngine::event(int id) const
{
  return d_times.value(id);
}


void RDTimeEngine::addEvent(int id,const QTime &time)
{
  d_times[id]=time;
  d_timers[id]=new QTimer(this);
  d_timers.value(id)->setTimerType(Qt::PreciseTimer);
  d_timers.value(id)->setSingleShot(true);
  d_mapper->setMapping(d_timers.value(id),id);
  connect(d_timers.value(id),SIGNAL(timeout()),d_mapper,SLOT(map()));
  StartEvent(id);
}


void RDTimeEngine::removeEvent(int id)
{
  d_timers.value(id)->stop();
  d_mapper->removeMappings(d_timers.value(id));
  delete d_timers.value(id);
  d_timers.remove(id);
  d_times.remove(id);
}


void RDTimeEngine::timerData(int id)
{
  emit timeout(id);
  StartEvent(id);
}


void RDTimeEngine::StartEvent(int id)
{
  //
  // Maintainer's Note
  //
  // In order to account for Daylight Saving Time (DST) transitions properly,
  // calculations for the timer interval *must* be done using QDateTime,
  // *not* QTime!
  //
  // See the Qt documentation for the QDateTime and QTimeZone classes for
  // discussion of DST and its implications.
  //
  QDateTime now=QDateTime::currentDateTime();
  QDateTime then=QDateTime(now.date(),d_times.value(id));

  if((now.time()>d_times.value(id))||(!then.isValid())) {
    //
    // Ensure that the advanced date/time is valid, in case we land on a
    // "non-existent" date/time --e.g. during a DST "spring forward"
    // transition.
    //
    do {
      then.setDate(then.date().addDays(1));
    }
    while(!then.isValid());
  }
  int interval=now.msecsTo(then);
  QTimer *timer=d_timers.value(id);
  if (timer!=NULL) {  // In case this is a one-shot
    d_timers.value(id)->start(1+interval);
  }
  /*
  printf("ID %d set interval %d mS [%s] for scheduled time %s\n",id,interval,
	 QTime(0,0,0).addMSecs(interval).toString("hh:mm:ss.zzz").toUtf8().constData(),
	 d_times.value(id).toString("hh:mm:ss.zzz").toUtf8().constData());
  */
}


void RDTimeEngine::DumpTimeZone(const QTimeZone &tz) const
{
  QTimeZone::OffsetData offset;

  printf("id: %s\n",tz.id().constData());
  printf("isValid: %u\n",tz.isValid());
  printf("hasTransitions: %u\n",tz.hasTransitions());
  if(tz.hasTransitions()) {
    offset=tz.nextTransition(QDateTime::currentDateTime());
    if(offset.atUtc.isValid()) {
      printf("  next ");
      DumpTransition(offset);
    }
    offset=tz.previousTransition(QDateTime::currentDateTime());
    if(offset.atUtc.isValid()) {
      printf("  previous ");
      DumpTransition(offset);
    }
  }
}


void RDTimeEngine::DumpTransition(QTimeZone::OffsetData offset) const
{
  printf("transition: %s UTC\n",offset.atUtc.toString("yyyy-MM-ddThh:mm:ss.zzz").toUtf8().constData());
  printf("    offsetFromUtc: %d secs\n",offset.offsetFromUtc);
  printf("    standardTimeOffset: %d secs\n",offset.standardTimeOffset);
  printf("    daylightTimeOffset: %d secs\n",offset.daylightTimeOffset);
  printf("    abbreviation: %s\n",offset.abbreviation.toUtf8().constData());
}
