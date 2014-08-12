// rdevent_line.h
//
// Abstract a Rivendell Log Manager Event
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdevent_line.h,v 1.18 2010/07/29 19:32:33 cvs Exp $
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

#include <qsqldatabase.h>
#include <qdatetime.h>

#include <rdlog_event.h>
#include <rdlog_line.h>


#ifndef RDEVENT_LINE_H
#define RDEVENT_LINE_H

class RDEventLine
{
 public:
  enum ImportSource {None=0,Traffic=1,Music=2,Scheduler=3};
  enum InsertFirst {InsertNone=0,InsertBreak=1,InsertTrack=2};
  RDEventLine();
  QString name() const;
  void setName(const QString &name);
  QString properties() const;
  void setProperties(const QString &str);
  int preposition() const;
  void setPreposition(int offset);
  RDLogLine::TimeType timeType() const;
  void setTimeType(RDLogLine::TimeType type);
  int graceTime() const;
  void setGraceTime(int offset);
  bool postPoint() const;
  void setPostPoint(bool state);
  bool useAutofill() const;
  void setUseAutofill(bool state);
  int autofillSlop() const;
  void setAutofillSlop(int slop);
  bool useTimescale() const;
  void setUseTimescale(bool state);
  RDEventLine::ImportSource importSource() const;
  void setImportSource(RDEventLine::ImportSource src);
  int startSlop() const;
  void setStartSlop(int slop);
  int endSlop() const;
  void setEndSlop(int slop);
  RDLogLine::TransType firstTransType() const;
  void setFirstTransType(RDLogLine::TransType trans);
  RDLogLine::TransType defaultTransType() const;
  void setDefaultTransType(RDLogLine::TransType trans);
  QColor color() const;
  void setColor(const QColor &color);
  QString SchedGroup() const;
  void setSchedGroup(QString str);
  QString HaveCode() const;
  void setHaveCode(QString str);
  unsigned titleSep() const;
  void setTitleSep(unsigned titlesep);
  RDLogEvent *preimportCarts();
  RDLogEvent *postimportCarts();
  QTime startTime() const;
  void setStartTime(const QTime &time);
  int length() const;
  void setLength(int msecs);
  void clear();
  bool load();
  bool save();
  bool generateLog(QString logname,const QString &svcname,
		   QString *errors, unsigned artistsep,QString clockname);
  bool linkLog(RDLogEvent *e,int next_id,const QString &svcname,
	       RDLogLine *link_logline,const QString &track_str,
	       const QString &label_cart,const QString &track_cart,
	       const QString &import_table,QString *errors);
  
 private:
  int GetLength(unsigned cartnum,int def_length=0);
  QString event_name;
  QString event_properties;
  int event_preposition;
  RDLogLine::TimeType event_time_type;
  int event_grace_time;
  bool event_post_point;
  bool event_use_autofill;
  bool event_use_timescale;
  RDEventLine::ImportSource event_import_source;
  int event_start_slop;
  int event_end_slop;
  RDLogLine::TransType event_first_transtype;
  RDLogLine::TransType event_default_transtype;
  QColor event_color;
  RDLogEvent *event_preimport_log;
  RDLogEvent *event_postimport_log;
  QTime event_start_time;
  int event_length;
  int event_autofill_slop;
  QString event_nested_event;
  QString event_sched_group;
  QString event_have_code;
  unsigned event_title_sep;
};

#endif 
