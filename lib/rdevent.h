// rdevent.h
//
// Abstract a Rivendell Log Manager Event
//
//   (C) Copyright 2002-2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDEVENT_H
#define RDEVENT_H

#include <qsqldatabase.h>

#include "rdlog_line.h"

class RDEvent
{
 public:
  enum EventPlace {PreImport=0,PostImport=1};
  enum ImportSource {None=0,Traffic=1,Music=2,Scheduler=3};
  RDEvent(const QString &name,bool create=false);
  QString name() const;
  QString properties() const;
  void setProperties(const QString &str) const;
  QString displayText(bool *ok) const;
  void setDisplayText(bool ok,const QString &text) const;
  QString noteText(bool *ok) const;
  void setNoteText(bool ok,const QString &text) const;
  int preposition() const;
  void setPreposition(int offset) const;
  RDLogLine::TimeType timeType() const;
  void setTimeType(RDLogLine::TimeType type) const;
  int graceTime() const;
  void setGraceTime(int offset) const;
  bool postPoint() const;
  void setPostPoint(bool state) const;
  bool useAutofill() const;
  void setUseAutofill(bool state) const;
  int autofillSlop() const;
  void setAutofillSlop(int slop) const;
  bool useTimescale() const;
  void setUseTimescale(bool state) const;
  RDEvent::ImportSource importSource() const;
  void setImportSource(RDEvent::ImportSource src) const;
  int startSlop() const;
  void setStartSlop(int slop) const;
  int endSlop() const;
  void setEndSlop(int slop) const;
  RDLogLine::TransType firstTransType() const;
  void setFirstTransType(RDLogLine::TransType trans) const;
  RDLogLine::TransType defaultTransType() const;
  void setDefaultTransType(RDLogLine::TransType trans) const;
  QColor color() const;
  void setColor(const QColor &color) const;
  QString nestedEvent() const;
  void setNestedEvent(const QString &eventname);
  QString remarks() const;
  void setRemarks(const QString &str) const;
  QString SchedGroup();
  void setSchedGroup(QString str);
  QString HaveCode();
  void setHaveCode(QString str);
  QString HaveCode2();
  void setHaveCode2(QString str);
  unsigned titleSep();
  void setTitleSep(unsigned titlesep);
  
 private:
  int GetIntValue(const QString &field) const;
  QString GetStringValue(const QString &field,bool *ok) const;
  void SetRow(const QString &param,int value) const;
  void SetRow(const QString &param,const QString &value,bool ok) const;
  QString event_name;
};


#endif  // RDEVENT_H
