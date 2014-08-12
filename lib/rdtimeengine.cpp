//   rdtimeengine.cpp
//
//   An event timer engine.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdtimeengine.cpp,v 1.4 2010/07/29 19:32:34 cvs Exp $
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
//


#include <stdlib.h>

#include <rdtimeengine.h>

RDTimeEngine::RDTimeEngine(QObject *parent,const char *name)
  : QObject(parent,name)
{
  engine_pending_id=-1;
  engine_timer=new QTimer(this,"engine_timer");
  engine_time_offset=0;
  connect(engine_timer,SIGNAL(timeout()),this,SLOT(timerData()));
}


RDTimeEngine::~RDTimeEngine()
{
  clear();
  delete engine_timer;
}


void RDTimeEngine::clear()
{
  engine_time_offset=0;
  engine_events.clear();
  SetTimer();
}


QTime RDTimeEngine::event(int id) const
{
  for(unsigned i=0;i<engine_events.size();i++) {
    for(int j=0;j<engine_events[i].size();j++) {
      if(id==engine_events[i].id(j)) {
	return engine_events[i].time();
      }
    }
  }
  return QTime();
}


int RDTimeEngine::timeOffset() const
{
  return engine_time_offset;
}


void RDTimeEngine::setTimeOffset(int msecs)
{
  engine_time_offset=msecs;
  SetTimer();
}


void RDTimeEngine::addEvent(int id,QTime time)
{
  for(unsigned i=0;i<engine_events.size();i++) {
    if(time==engine_events[i].time()) {
      engine_events[i].addId(id);
      SetTimer();
      return;
    }
  }
  engine_events.push_back(RDTimeEvent());
  engine_events.back().setTime(time);
  engine_events.back().addId(id);
  SetTimer();
}


void RDTimeEngine::removeEvent(int id)
{
  for(unsigned i=0;i<engine_events.size();i++) {
    for(int j=0;j<engine_events[i].size();j++) {
      if(id==engine_events[i].id(j)) {
	if(engine_events[i].size()==1) {
	  std::vector<RDTimeEvent>::iterator it=engine_events.begin()+i;
	  engine_events.erase(it,it+1);
	}
	else {
	  engine_events[i].removeId(j);
	}
	SetTimer();
	return;
      }
    }
  }
}


int RDTimeEngine::next() const
{
  return engine_pending_id;
}


void RDTimeEngine::timerData()
{
  for(unsigned i=0;i<engine_events.size();i++) {
    for(int j=0;j<engine_events[i].size();j++) {
      if(engine_pending_id==engine_events[i].id(j)) {
	EmitEvents(i);
	SetTimer();
	return;
      }
    }
  }
}


void RDTimeEngine::EmitEvents(int offset)
{
//  for(int i=0;i<engine_events[offset].size();i++) {
  for(int i=engine_events[offset].size()-1;i>=0;i--) {
    emit timeout(engine_events[offset].id(i));
  }
}


void RDTimeEngine::SetTimer()
{
  engine_timer->stop();
  if(engine_events.size()==0) {
    return;
  }
  QTime current_time=QTime::currentTime().addMSecs(engine_time_offset);
  int diff=GetNextDiff(current_time,&engine_pending_id);
  if(diff!=86400001) {
    engine_timer->start(diff,true);
    return;
  }
  diff=GetNextDiff(QTime(),&engine_pending_id);
  if(diff!=86400001) {
    diff+=(current_time.msecsTo(QTime(23,59,59))+1000);
    engine_timer->start(diff,true);
    return;
  }
}


int RDTimeEngine::GetNextDiff(QTime time,int *pending_id)
{
  int diff=86400001;
  *pending_id=-1;

  for(unsigned i=0;i<engine_events.size();i++) {
    if(((time.msecsTo(engine_events[i].time()))>=0)&&
       (time.msecsTo(engine_events[i].time())<diff)) {
      diff=time.msecsTo(engine_events[i].time());
      *pending_id=engine_events[i].id(0);
    }
  }
  return diff;
}
