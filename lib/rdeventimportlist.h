// rdeventimportlist.h
//
// Abstract an rdlogmanager(1) Import List
//
//   (C) Copyright 2018-2020 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDEVENTIMPORTLIST_H
#define RDEVENTIMPORTLIST_H

#include <qlist.h>

#include <rdlog_line.h>

class RDEventImportItem
{
 public:
  RDEventImportItem(bool end_marker=false);
  bool isEndMarker() const;
  RDLogLine::Type eventType() const;
  void setEventType(RDLogLine::Type type);
  unsigned cartNumber() const;
  void setCartNumber(unsigned cartnum);
  RDLogLine::TransType transType() const;
  void setTransType(RDLogLine::TransType type);
  QString markerComment() const;
  void setMarkerComment(const QString &str);

 private:
  bool event_end_marker;
  RDLogLine::Type event_type;
  unsigned event_cart_number;
  RDLogLine::TransType event_trans_type;
  QString event_marker_comment;
};


class RDEventImportList
{
 public:
  enum ImportType {PreImport=0,PostImport=1};
  RDEventImportList();
  QString eventName() const;
  void setEventName(const QString &str);
  ImportType type() const;
  void setType(ImportType type);
  int size();
  RDEventImportItem *item(int n) const;
  RDEventImportItem *endMarkerItem() const;
  void takeItem(int before_line,RDEventImportItem *item);
  void removeItem(int n);
  void moveItem(int from_line,int to_line);
  void load();
  void save(RDLogLine::TransType first_trans=RDLogLine::NoTrans) const;
  void clear();

 private:
  QString list_event_name;
  ImportType list_type;
  QList<RDEventImportItem *> list_items;
};


#endif   // RDEVENTIMPORTLIST_H

