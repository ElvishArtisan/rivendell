// rdclock.h
//
// Abstract a Rivendell Log Manager Clock
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdclock.h,v 1.17.10.1 2014/06/24 18:27:03 cvs Exp $
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

#include <qsqldatabase.h>

#include <rdevent_line.h>

using namespace std;


class RDClock
{
  public:
   RDClock();
   QString name() const;
   void setName(const QString &name);
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

   RDEventLine *eventLine(int line);
   void clear();
   int size() const;
   bool load();
   bool save();
   bool insert(const QString &event_name,int line);
   void remove(int line);
   void move(int from_line,int to_line);
   bool validate(const QTime &start_time,int length,int except_line=-1);
   bool generateLog(int hour,const QString &logname,const QString &svc_name,
		    QString *errors);
   static QString tableName(const QString &name);

  private:
   QString clock_name;
   QString clock_name_esc;
   QString clock_short_name;
   QColor clock_color;
   QString clock_remarks;
   vector<RDEventLine> clock_events;
   unsigned artistsep;
   bool rules_modified;
};

#endif 
