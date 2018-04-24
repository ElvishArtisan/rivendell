//   rdtimeevent.h
//
//   A Container Class for RTimeEngine events.
//
//   (C) Copyright 2003-2004,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDTIMEEVENT_H
#define RDTIMEEVENT_H

#include <vector>

#include <qdatetime.h>

class RDTimeEvent
{
  public:
  RDTimeEvent();
  QTime time() const;
  void setTime(QTime time);
  int id(int num) const;
  void setId(int num,int id);
  void addId(int id);
  void removeId(int num);
  int size() const;
  void clear();

 private:
  QTime event_time;
  std::vector<int> event_id;
};


#endif  // RDTIMEEVENT_H
