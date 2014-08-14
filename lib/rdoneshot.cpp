// rdoneshot.cpp
//
// A class for providing one-shot single use timers.
//
//   (C) Copyright 2008 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdoneshot.h>

RDOneShot::RDOneShot(QObject *parent,const char *name)
  : QObject(parent,name)
{
  //
  // Timeout Mapper
  //
  shot_count=0;
  shot_mapper=new QSignalMapper(this);
  connect(shot_mapper,SIGNAL(mapped(int)),this,SLOT(timeoutData(int)));

  //
  // Zombie Timer
  //
  shot_zombie_timer=new QTimer(this);
  connect(shot_zombie_timer,SIGNAL(timeout()),this,SLOT(zombieData()));
}


void RDOneShot::start(int value,int msecs)
{
  shot_values[shot_count]=value;
  shot_timers[shot_count]=new QTimer(this);
  shot_mapper->setMapping(shot_timers[shot_count],shot_count);
  connect(shot_timers[shot_count],SIGNAL(timeout()),
	  shot_mapper,SLOT(map()));
  shot_timers[shot_count]->start(msecs,true);
  shot_count++;
}


void RDOneShot::timeoutData(int id)
{
  emit timeout(shot_values[id]);
  shot_zombie_timer->start(10,true);
}


void RDOneShot::zombieData()
{
  for(std::map<int,QTimer *>::iterator it=shot_timers.begin();
      it!=shot_timers.end();it++) {
    if(!it->second->isActive()) {
      shot_values.erase(it->first);
      delete it->second;
      shot_timers.erase(it);
    }
  }
}
