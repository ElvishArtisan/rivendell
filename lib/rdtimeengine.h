//   rdtimeengine.h
//
//   Event timer engine.
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDTIMEENGINE_H
#define RDTIMEENGINE_H

#include <QDateTime>
#include <QMap>
#include <QObject>
#include <QSignalMapper>
#include <QTimer>
#include <QTimeZone>

class RDTimeEngine : public QObject
{
  Q_OBJECT
 public:
  RDTimeEngine(QObject *parent=0);
  ~RDTimeEngine();
  void clear();
  QTime event(int id) const;
  void addEvent(int id,const QTime &time);
  void removeEvent(int id);
  
 signals:
  void timeout(int id);
  
 private slots:
  void timerData(int id);

 private:
  void StartEvent(int id);
  void DumpTimeZone(const QTimeZone &tz) const;
  void DumpTransition(QTimeZone::OffsetData offset) const;
  QMap<int,QTime> d_times;
  QMap<int,QTimer *> d_timers;
  QSignalMapper *d_mapper;
};


#endif  // RDTIMEENGINE_H
