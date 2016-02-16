// rdevent_list.cpp
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

#include "rddb.h"
#include "rdevent_list.h"
#include "rdescape_string.h"

RDEventList::RDEventList(const QString &eventname,RDEvent::EventPlace place)
{
  event_name=eventname;
  event_place=place;
}


RDEventList::RDEventList()
{
  event_place=RDEvent::PreImport;
}


QString RDEventList::eventName() const
{
  return event_name;
}


void RDEventList::setEventName(const QString &str)
{
  event_name=str;
}


RDEvent::EventPlace RDEventList::eventPlace() const
{
  return event_place;
}


void RDEventList::setEventPlace(RDEvent::EventPlace place)
{
  event_place=place;
}


unsigned RDEventList::size() const
{
  return event_types.size();
}


RDLogLine::Type RDEventList::type(unsigned line) const
{
  return event_types[line];
}


void RDEventList::setType(unsigned line,RDLogLine::Type type)
{
  event_types[line]=type;
}


RDLogLine::TransType RDEventList::transType(unsigned line) const
{
  return event_trans_types[line];
}


void RDEventList::setTransType(unsigned line,RDLogLine::TransType trans)
{
  event_trans_types[line]=trans;
}


unsigned RDEventList::cartNumber(unsigned line) const
{
  return event_cart_numbers[line];
}


void RDEventList::setCartNumber(unsigned line,unsigned cartnum)
{
  event_cart_numbers[line]=cartnum;
}


QString RDEventList::text(unsigned line) const
{
  return event_texts[line];
}


void RDEventList::setText(unsigned line,const QString &str)
{
  event_texts[line]=str;
}


void RDEventList::insert(unsigned line,RDLogLine::Type type)
{
  event_types.insert(event_types.begin()+line,1,type);
  event_trans_types.insert(event_trans_types.begin()+line,1,RDLogLine::Play);
  event_cart_numbers.insert(event_cart_numbers.begin()+line,1,0);
  event_texts.insert(event_texts.begin()+line,1,QString());
}


void RDEventList::remove(unsigned line)
{
  event_types.erase(event_types.begin()+line);
  event_trans_types.erase(event_trans_types.begin()+line);
  event_cart_numbers.erase(event_cart_numbers.begin()+line);
  event_texts.erase(event_texts.begin()+line);
}


void RDEventList::move(unsigned from_line,unsigned to_line)
{
  int src_offset=0;
  int dest_offset=1;

  if(to_line<from_line) {
    src_offset=1;
    dest_offset=0;
  }
  insert(to_line+dest_offset,event_types[from_line]);
  event_trans_types[to_line+dest_offset]=event_trans_types[from_line];
  event_cart_numbers[to_line+dest_offset]=event_cart_numbers[from_line];
  event_texts[to_line+dest_offset]=event_texts[from_line];
  if((to_line+1)>=size()) {
    to_line=size()-1;
    dest_offset=0;
  }
  remove(from_line+src_offset);
}


void RDEventList::load()
{
  QString sql;
  RDSqlQuery *q;

  clear();
  sql=QString("select TYPE,TRANS_TYPE,CART_NUMBER,TEXT ")+
    "from EVENT_METADATA where "+
    "(EVENT_NAME=\""+RDEscapeString(event_name)+"\")&&"+
    QString().sprintf("(PLACE=%d) ",event_place)+
    "order by COUNT";
  q=new RDSqlQuery(sql);
  while(q->next()) {
    event_types.push_back((RDLogLine::Type)q->value(0).toInt());
    event_trans_types.push_back((RDLogLine::TransType)q->value(1).toInt());
    event_cart_numbers.push_back(q->value(2).toUInt());
    event_texts.push_back(q->value(3).toString());
  }
  delete q;
}


void RDEventList::save()
{
  QString sql;
  RDSqlQuery *q;

  sql=QString("delete from EVENT_METADATA where ")+
    "(EVENT_NAME=\""+RDEscapeString(event_name)+"\")&&"+
    QString().sprintf("(PLACE=%d)",event_place);
  q=new RDSqlQuery(sql);
  delete q;

  for(unsigned i=0;i<size();i++) {
    sql=QString("insert into EVENT_METADATA set ")+
      "EVENT_NAME=\""+RDEscapeString(event_name)+"\","+
      QString().sprintf("PLACE=%d,",event_place)+
      QString().sprintf("COUNT=%u,",i)+
      QString().sprintf("TYPE=%d,",event_types[i])+
      QString().sprintf("TRANS_TYPE=%d,",event_trans_types[i]);
    if(event_types[i]==RDLogLine::Cart) {
      sql+=QString().sprintf("CART_NUMBER=%u",event_cart_numbers[i]);
    }
    else {
      sql+="TEXT=\""+RDEscapeString(event_texts[i])+"\"";
    }
    q=new RDSqlQuery(sql);
    delete q;
  }
}


void RDEventList::clear()
{
  event_types.clear();
  event_trans_types.clear();
  event_cart_numbers.clear();
  event_texts.clear();
}
