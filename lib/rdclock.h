// rdclock.h
//
// Abstract a Rivendell Log Manager Clock
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDCLOCK_H
#define RDCLOCK_H

#include <vector>

#include <rdevent_line.h>
#include <rdstation.h>

class RDClock
{
  public:
   RDClock(RDStation *station);
   QString clockName() const;
   void setClockName(const QString &name);
   QString shortName() const;
   void setShortName(const QString &name);
   QColor color() const;
   void setColor(const QColor &color);
   QString remarks() const;
   void setRemarks(const QString &str);
   unsigned getArtistSep();
   void setArtistSep(unsigned artist_sep);
   bool getRulesModified();
   void setRulesModified(bool modified);

   RDEventLine *eventLine(int line) const;
   void clear();
   int size() const;
   bool load();
   bool save();
   virtual int insert(const QString &event_name,const QTime &start,int len);
   void remove(int line);
   bool validate(const QTime &start_time,int length,int except_line=-1);
   bool generateLog(int hour,const QString &logname,const QString &svc_name,
		    QString *errors);

 protected:
  virtual int preInsert(const QString &event_name,const QTime &time) const;
  virtual void execInsert(int line,const QString &event_name,const QTime &time,
			  int len);

 private:
   QString clock_name;
   QString clock_short_name;
   QColor clock_color;
   QString clock_remarks;
   QList<RDEventLine *> clock_events;
   unsigned artistsep;
   bool rules_modified;
   RDStation *clock_station;
};

#endif  // RDCLOCK_H
