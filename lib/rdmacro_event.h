// rdmacro_event.h
//
// A container class for a list of RML macros.
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdmacro_event.h,v 1.14 2011/01/04 22:50:35 cvs Exp $
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

#ifndef RDMACRO_EVENT_H
#define RDMACRO_EVENT_H

#include <vector>

#include <qobject.h>
#include <qtimer.h>
#include <qhostaddress.h>
#include <qdatetime.h>

#include <rdmacro.h>
#include <rdripc.h>

class RDMacroEvent : public QObject
{
  Q_OBJECT
 public:
  RDMacroEvent(RDRipc *ripc=0,QObject *parent=0,const char *name=0);
  RDMacroEvent(QHostAddress addr,RDRipc *ripc=0,
	       QObject *parent=0,const char *name=0);
  ~RDMacroEvent();
  int line() const;
  void setLine(int line);
  QTime startTime() const;
  void setStartTime(QTime time);
  RDMacro *command(int line);
  int size() const;
  unsigned length() const;
  bool load(QString str);
  bool load(unsigned cartnum);
  QString save();
  void insert(int line,const RDMacro *cmd);
  void remove(int line);
  void move(int from_line,int to_line);
  void copy(int from_line,int to_line);
  void clear();

 public slots:
  void exec();
  void exec(int line);
  void stop();

 signals:
  void started();
  void started(int line);
  void finished();
  void finished(int line);
  void stopped();

 private slots:
  void sleepTimerData();

 private:
  void ExecList(int line);
  std::vector<RDMacro *> event_cmds;
  RDRipc *event_ripc;
  QTimer *event_sleep_timer;
  bool event_whole_list;
  unsigned event_sleeping_line;
  QHostAddress event_address;
  int event_line;
  QTime event_start_time;
};


#endif  // RDMACRO_EVENT_H
