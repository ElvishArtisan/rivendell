//   rdtimeevent.cpp
//
//   A Container Class for RTimeEngine events.
//
//   (C) Copyright 2003-2004 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdtimeevent.cpp,v 1.3 2010/07/29 19:32:34 cvs Exp $
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
///

#include <rdtimeevent.h>


RDTimeEvent::RDTimeEvent()
{
  clear();
}


QTime RDTimeEvent::time() const
{
  return event_time;
}


void RDTimeEvent::setTime(QTime time)
{
  event_time=time;
}


int RDTimeEvent::id(int num) const
{
  return event_id[num];
}


void RDTimeEvent::setId(int num,int id)
{
  event_id[num]=id;
}


void RDTimeEvent::addId(int id)
{
  event_id.push_back(id);
}


void RDTimeEvent::removeId(int num)
{
  std::vector<int>::iterator it=event_id.begin()+num;
  event_id.erase(it,it+1);
}


int RDTimeEvent::size() const
{
  return event_id.size();
}


void RDTimeEvent::clear()
{
  event_time=QTime();
  event_id.clear();
}
