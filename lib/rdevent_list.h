// rdevent_list.h
//
// Abstract Rivendell RDLogManager Event Import Lists
//
//   (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDEVENT_LIST_H
#define RDEVENT_LIST_H

#include <vector>

#include "rdevent.h"
#include "rdlog_line.h"

class RDEventList
{
 public:
  RDEventList(const QString &eventname,RDEvent::EventPlace place);
  RDEventList();
  QString eventName() const;
  void setEventName(const QString &str);
  RDEvent::EventPlace eventPlace() const;
  void setEventPlace(RDEvent::EventPlace place);
  unsigned size() const;
  RDLogLine::Type type(unsigned line) const;
  void setType(unsigned line,RDLogLine::Type type);
  RDLogLine::TransType transType(unsigned line) const;
  void setTransType(unsigned line,RDLogLine::TransType trans);
  unsigned cartNumber(unsigned line) const;
  void setCartNumber(unsigned line,unsigned cartnum);
  QString text(unsigned line) const;
  void setText(unsigned line,const QString &str);
  void insert(unsigned line,RDLogLine::Type type);
  void remove(unsigned line);
  void move(unsigned from_line,unsigned to_line);
  void load();
  void save();
  void clear();

 private:
  QString event_name;
  RDEvent::EventPlace event_place;
  std::vector<RDLogLine::Type> event_types;
  std::vector<RDLogLine::TransType> event_trans_types;
  std::vector<unsigned> event_cart_numbers;
  std::vector<QString> event_texts;
};


#endif  // RDEVENT_LIST_H
