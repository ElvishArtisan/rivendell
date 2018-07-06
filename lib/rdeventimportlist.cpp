// rdeventimportlist.cpp
//
// Abstract an rdlogmanager(1) Import List
//
//   (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
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
#include "rdescape_string.h"
#include "rdeventimportlist.h"

RDEventImportItem::RDEventImportItem()
{
  event_type=RDLogLine::UnknownType;
  event_cart_number=0;
  event_trans_type=RDLogLine::Play;
  event_marker_comment="";
}


RDLogLine::Type RDEventImportItem::eventType() const
{
  return event_type;
}


void RDEventImportItem::setEventType(RDLogLine::Type type)
{
  event_type=type;
}


unsigned RDEventImportItem::cartNumber() const
{
  return event_cart_number;
}


void RDEventImportItem::setCartNumber(unsigned cartnum)
{
  event_cart_number=cartnum;
}


RDLogLine::TransType RDEventImportItem::transType() const
{
  return event_trans_type;
}


void RDEventImportItem::setTransType(RDLogLine::TransType type)
{
  event_trans_type=type;
}


QString RDEventImportItem::markerComment() const
{
  return event_marker_comment;
}


void RDEventImportItem::setMarkerComment(const QString &str)
{
  event_marker_comment=str;
}




RDEventImportList::RDEventImportList()
{
  list_event_name="";
  list_type=RDEventImportList::PreImport;
}


QString RDEventImportList::eventName() const
{
  return list_event_name;
}


void RDEventImportList::setEventName(const QString &str)
{
  list_event_name=str;
}


RDEventImportList::ImportType RDEventImportList::type() const
{
  return list_type;
}


void RDEventImportList::setType(RDEventImportList::ImportType type)
{
  list_type=type;
}


int RDEventImportList::size()
{
  return list_items.size();
}


RDEventImportItem *RDEventImportList::item(int n) const
{
  return list_items.at(n);
}


void RDEventImportList::takeItem(int before_line,RDEventImportItem *item)
{
  list_items.insert(list_items.begin()+before_line,item);
}


void RDEventImportList::removeItem(int n)
{
  delete list_items.at(n);
  list_items.erase(list_items.begin()+n);
}


void RDEventImportList::moveItem(int from_line,int to_line)
{
  RDEventImportItem *item=list_items.at(from_line);
  list_items.erase(list_items.begin()+from_line);
  if(to_line>=(int)list_items.size()) {
    list_items.push_back(item);
  }
  else {
    list_items.insert(list_items.begin()+to_line,item);
  }
}


void RDEventImportList::load()
{
  list_items.clear();
  QString sql=QString("select ")+
    "EVENT_TYPE,"+      // 00
    "CART_NUMBER,"+     // 01
    "TRANS_TYPE,"+      // 02
    "MARKER_COMMENT "+  // 03
    "from EVENT_LINES where "+
    "EVENT_NAME=\""+RDEscapeString(list_event_name)+"\" && "+
    QString().sprintf("TYPE=%d ",list_type)+
    "order by COUNT";
  RDSqlQuery *q=new RDSqlQuery(sql);
  while(q->next()) {
    list_items.push_back(new RDEventImportItem());
    list_items.back()->setEventType((RDLogLine::Type)q->value(0).toInt());
    list_items.back()->setCartNumber(q->value(1).toUInt());
    list_items.back()->setTransType((RDLogLine::TransType)q->value(2).toInt());
    list_items.back()->setMarkerComment(q->value(3).toString());
  }
  delete q;
}


void RDEventImportList::save() const
{
  QString sql=QString("delete from EVENT_LINES where ")+
    "EVENT_NAME=\""+RDEscapeString(list_event_name)+"\" && "+
    QString().sprintf("TYPE=%d",list_type);
  RDSqlQuery::apply(sql);
  for(unsigned i=0;i<list_items.size();i++) {
    RDEventImportItem *item=list_items.at(i);
    sql=QString("insert into EVENT_LINES set ")+
      "EVENT_NAME=\""+RDEscapeString(list_event_name)+"\","+
      QString().sprintf("TYPE=%d,",list_type)+
      QString().sprintf("COUNT=%u,",i)+
      QString().sprintf("EVENT_TYPE=%d,",item->eventType())+
      QString().sprintf("CART_NUMBER=%u,",item->cartNumber())+
      QString().sprintf("TRANS_TYPE=%d,",item->transType())+
      "MARKER_COMMENT=\""+RDEscapeString(item->markerComment())+"\"";
    RDSqlQuery::apply(sql);
  }
}


void RDEventImportList::clear()
{
  list_event_name="";
  list_type=RDEventImportList::PreImport;
  for(unsigned i=0;i<list_items.size();i++) {
    delete list_items.at(i);
  }
  list_items.clear();
}
