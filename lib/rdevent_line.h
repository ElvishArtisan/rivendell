// rdevent_line.h
//
// Abstract a Rivendell Log Manager Event
//
//   (C) Copyright 2002-2022 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDEVENT_LINE_H
#define RDEVENT_LINE_H

#include <QDateTime>

#include <rdlogmodel.h>
#include <rdeventimportlist.h>
#include <rdlog.h>
#include <rdlog_line.h>
#include <rdstation.h>

class __RDEventLine_GeneratorState
{
 public:
  QTime start_time;
  int length;
  int count;
  RDLogLine::TransType trans_type;
  RDLogLine::TimeType time_type;
  RDLogLine::Type link_type;
  int grace_time;
  int link_id;
};


class RDEventLine
{
 public:
  enum ImportSource {None=0,Traffic=1,Music=2,Scheduler=3};
  enum InsertFirst {InsertNone=0,InsertBreak=1,InsertTrack=2};
  RDEventLine(RDStation *station);
  QString name() const;
  void setName(const QString &name);
  int preposition() const;
  void setPreposition(int offset);
  RDLogLine::TimeType timeType() const;
  void setTimeType(RDLogLine::TimeType type);
  int graceTime() const;
  void setGraceTime(int offset);
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
  QString schedGroup() const;
  void setSchedGroup(QString str);
  QString HaveCode() const;
  void setHaveCode(QString str);
  QString HaveCode2() const;
  void setHaveCode2(QString str);
  unsigned titleSep() const;
  void setTitleSep(unsigned titlesep);
  QTime startTime() const;
  void setStartTime(const QTime &time);
  int length() const;
  void setLength(int msecs);
  void clear();
  bool load();
  bool loadBypass();
  bool save(RDConfig *config);
  bool generateLog(const QString &logname,const QString &svcname,
		   const QString &clockname,QString *report);
  void linkLog(RDLogModel *e,RDLog *log,const QString &svcname,
	       RDLogLine *link_logline,const QString &track_str,
	       const QString &label_cart,const QString &track_cart,
	       QString *errors);
  QString propertiesText() const;
  static QString propertiesText(int prepos_msec,
				RDLogLine::TransType first_trans,
				RDLogLine::TimeType time_type,
				int grace_msec,
				bool autofill,
				RDEventLine::ImportSource import_src,
				bool inline_tfc);
  
 private:
  void GenerateMusicSchedEvent(__RDEventLine_GeneratorState *state,
			       const QString &logname,const QString &svcname,
			       const QString &clockname,QString *report);
  int GetLength(unsigned cartnum,int def_length=0);
  QString event_name;
  bool event_using_bypass;
  int event_preposition;
  RDLogLine::TimeType event_time_type;
  int event_grace_time;
  bool event_use_autofill;
  bool event_use_timescale;
  RDEventLine::ImportSource event_import_source;
  int event_start_slop;
  int event_end_slop;
  RDLogLine::TransType event_first_transtype;
  RDLogLine::TransType event_default_transtype;
  QColor event_color;
  RDEventImportList *event_preimport_list;
  RDEventImportList *event_postimport_list;
  QTime event_start_time;
  int event_length;
  int event_autofill_slop;
  QString event_nested_event;
  QString event_sched_group;
  QString event_have_code;
  QString event_have_code2;
  int event_artist_sep;
  int event_title_sep;
  RDStation *event_station;
};

#endif  // RDEVENT_LINE_H
