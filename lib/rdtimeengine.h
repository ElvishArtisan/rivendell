//   rdtimeengine.h
//
//   An event timer engine.
//
//   (C) Copyright 2002 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdtimeengine.h,v 1.4 2010/07/29 19:32:34 cvs Exp $
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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
//

#ifndef RDTIMEENGINE_H
#define RDTIMEENGINE_H


#include <vector>

#include <qwidget.h>
#include <qdatetime.h>
#include <qtimer.h>

#include <rdtimeevent.h>


class RDTimeEngine : public QObject
{
  Q_OBJECT
 public:
  RDTimeEngine(QObject *parent=0,const char *name=0);
  ~RDTimeEngine();
  void clear();
  QTime event(int id) const;
  int timeOffset() const;
  void setTimeOffset(int msecs);
  void addEvent(int id,QTime time);
  void removeEvent(int id);
  int next() const;
  
 signals:
  void timeout(int id);
  
 private slots:
  void timerData();

 private:
  void EmitEvents(int offset);
  void SetTimer();
  int GetNextDiff(QTime time,int *pending_id);
  QTimer *engine_timer;
  std::vector<RDTimeEvent> engine_events;
  int engine_pending_id;
  int engine_time_offset;
};


#endif  // RDTIMEENGINE_H
