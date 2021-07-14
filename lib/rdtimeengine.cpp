//   rdtimeengine.cpp
//
//   An event timer engine.
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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
  QTime now=QTime::currentTime();
  int interval=now.msecsTo(d_times.value(id));

  if(interval<0) {  // Crosses midnight
    interval=
      now.msecsTo(QTime(23,59,59))+1000+QTime(0,0,0).msecsTo(d_times.value(id));
  }
  d_timers.value(id)->start(1+interval);
}
