// rdevent.cpp
//
// Abstract a Rivendell Log Manager Event.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdevent.cpp,v 1.14.8.1 2014/01/13 18:36:57 cvs Exp $
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
#include <rddb.h>
#include <rdconf.h>
#include <rdevent.h>
#include <rdescape_string.h>

//
// Global Classes
//
RDEvent::RDEvent(const QString &name,bool create)
{
  RDSqlQuery *q;
  QString sql;

  event_name=name;

  if(create) {
    sql=QString().sprintf("select NAME from EVENTS where \
(NAME=\"%s\")",(const char *)event_name);
    q=new RDSqlQuery(sql);
    if(q->size()!=1) {
      delete q;
      sql=QString().
        sprintf("INSERT INTO EVENTS SET NAME=\"%s\",TITLE_SEP=100",
		(const char *)event_name);
      q=new RDSqlQuery(sql);
      delete q;
    }
    else {
      delete q;
    }
  }
}


QString RDEvent::name() const
{
  return event_name;
}


QString RDEvent::properties() const
{
  bool ok;
  return GetStringValue("PROPERTIES",&ok);
}


void RDEvent::setProperties(const QString &str) const
{
  SetRow("PROPERTIES",str,true);
}


QString RDEvent::displayText(bool *ok) const
{
  return GetStringValue("DISPLAY_TEXT",ok);
}


void RDEvent::setDisplayText(bool ok,const QString &text) const
{
  SetRow("DISPLAY_TEXT",text,ok);
}


QString RDEvent::noteText(bool *ok) const
{
  return GetStringValue("NOTE_TEXT",ok);
}


void RDEvent::setNoteText(bool ok,const QString &text) const
{
  SetRow("NOTE_TEXT",text,ok);
}


int RDEvent::preposition() const
{
  return GetIntValue("PREPOSITION");
}


void RDEvent::setPreposition(int offset) const
{
  SetRow("PREPOSITION",offset);
}


RDLogLine::TimeType RDEvent::timeType() const
{
  return (RDLogLine::TimeType)GetIntValue("TIME_TYPE");
}


void RDEvent::setTimeType(RDLogLine::TimeType type) const
{
  SetRow("TIME_TYPE",(int)type);
}


int RDEvent::graceTime() const
{
  return GetIntValue("GRACE_TIME");
}


void RDEvent::setGraceTime(int offset) const
{
  SetRow("GRACE_TIME",offset);
}


bool RDEvent::postPoint() const
{
  bool ok;
  return RDBool(GetStringValue("POST_POINT",&ok));
}


void RDEvent::setPostPoint(bool state) const
{
  SetRow("POST_POINT",RDYesNo(state),true);
}


bool RDEvent::useAutofill() const
{
  bool ok;
  return RDBool(GetStringValue("USE_AUTOFILL",&ok));
}


void RDEvent::setUseAutofill(bool state) const
{
  SetRow("USE_AUTOFILL",RDYesNo(state),true);
}


int RDEvent::autofillSlop() const
{
  return GetIntValue("AUTOFILL_SLOP");
}


void RDEvent::setAutofillSlop(int slop) const
{
  SetRow("AUTOFILL_SLOP",slop);
}


bool RDEvent::useTimescale() const
{
  bool ok;
  return RDBool(GetStringValue("USE_TIMESCALE",&ok));
}


void RDEvent::setUseTimescale(bool state) const
{
  SetRow("USE_TIMESCALE",RDYesNo(state),true);
}


RDEventLine::ImportSource RDEvent::importSource() const
{
  return (RDEventLine::ImportSource)GetIntValue("IMPORT_SOURCE");
}


void RDEvent::setImportSource(RDEventLine::ImportSource src) const
{
  SetRow("IMPORT_SOURCE",(int)src);
}


int RDEvent::startSlop() const
{
  return GetIntValue("START_SLOP");
}


void RDEvent::setStartSlop(int slop) const
{
  SetRow("START_SLOP",slop);
}


int RDEvent::endSlop() const
{
  return GetIntValue("END_SLOP");
}


void RDEvent::setEndSlop(int slop) const
{
  SetRow("END_SLOP",slop);
}


RDLogLine::TransType RDEvent::firstTransType() const
{
  return (RDLogLine::TransType)GetIntValue("FIRST_TRANS_TYPE");
}


void RDEvent::setFirstTransType(RDLogLine::TransType trans) const
{
  SetRow("FIRST_TRANS_TYPE",(int)trans);
}


RDLogLine::TransType RDEvent::defaultTransType() const
{
  return (RDLogLine::TransType)GetIntValue("DEFAULT_TRANS_TYPE");
}


void RDEvent::setDefaultTransType(RDLogLine::TransType trans) const
{
  SetRow("DEFAULT_TRANS_TYPE",(int)trans);
}


QColor RDEvent::color() const
{
  bool ok;
  QColor color;
  color=QColor(GetStringValue("COLOR",&ok));
  if(!ok) {
    color=QColor();
  }
  return color;
}


void RDEvent::setColor(const QColor &color) const
{
  SetRow("COLOR",color.name(),color.isValid());
}


QString RDEvent::nestedEvent() const
{
  bool ok;
  return GetStringValue("NESTED_EVENT",&ok);
}


void RDEvent::setNestedEvent(const QString &eventname)
{
  SetRow("NESTED_EVENT",eventname,true);
}


QString RDEvent::remarks() const
{
  bool ok;
  return GetStringValue("REMARKS",&ok);
}


void RDEvent::setRemarks(const QString &str) const
{
  SetRow("REMARKS",str,true);
}


QString RDEvent::preimportTableName(const QString event_name)
{
  QString ret=event_name;
  ret.replace(" ","_");
  return ret+"_PRE";
}


QString RDEvent::postimportTableName(const QString event_name)
{
  QString ret=event_name;
  ret.replace(" ","_");
  return ret+"_POST";
}


int RDEvent::GetIntValue(const QString &field) const
{
  QString sql;
  RDSqlQuery *q;
  int accum;
  
  sql=QString().sprintf("select %s from EVENTS where NAME=\"%s\"",
			(const char *)field,
			(const char *)event_name);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    accum=q->value(0).toInt();
    delete q;
    return accum;
  }
  delete q;
  return 0;    
}


QString RDEvent::GetStringValue(const QString &field,bool *ok) const
{
  QString sql;
  RDSqlQuery *q;
  QString accum;
  
  sql=QString().sprintf("select %s from EVENTS where NAME=\"%s\"",
			(const char *)field,
			(const char *)event_name);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    if(q->value(0).isNull()) {
      *ok=false;
    }
    else {
      accum=q->value(0).toString();
      *ok=true;
    }
    delete q;
    return accum;
  }
  delete q;
  return 0;    
}


void RDEvent::SetRow(const QString &param,int value) const
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("UPDATE EVENTS SET %s=%d WHERE NAME=\"%s\"",
			(const char *)param,
			value,
			(const char *)event_name);
  q=new RDSqlQuery(sql);
  delete q;
}


void RDEvent::SetRow(const QString &param,const QString &value,bool ok) const
{
  RDSqlQuery *q;
  QString sql;

  if(ok) {
    sql=QString().sprintf("UPDATE EVENTS SET %s=\"%s\" WHERE NAME=\"%s\"",
			  (const char *)param,
			  (const char *)RDEscapeString(value),
			  (const char *)event_name);
  }
  else {
    sql=QString().sprintf("UPDATE EVENTS SET %s=NULL WHERE NAME=\"%s\"",
			  (const char *)param,
			  (const char *)event_name);
  }
  q=new RDSqlQuery(sql);
  delete q;
}


QString RDEvent::SchedGroup()
{
  bool ok;
  return GetStringValue("SCHED_GROUP",&ok);
}


void RDEvent::setSchedGroup(QString str)
{
  SetRow("SCHED_GROUP",str,true);
}

QString RDEvent::HaveCode()
{
  bool ok;
  return GetStringValue("HAVE_CODE",&ok);
}


void RDEvent::setHaveCode(QString str)
{
  SetRow("HAVE_CODE",str,true);
}


unsigned RDEvent::titleSep()
{
  return GetIntValue("TITLE_SEP");
}


void RDEvent::setTitleSep(unsigned titlesep)
{
  SetRow("TITLE_SEP",titlesep);
}
