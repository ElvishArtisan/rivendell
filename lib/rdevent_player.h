// rdevent_player.h
//
// Execute a list of RML commands asynchronously.
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdevent_player.h,v 1.6 2010/07/29 19:32:33 cvs Exp $
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

#include <qobject.h>
#include <qsignalmapper.h>
#include <qtimer.h>

#include <rdmacro_event.h>
#include <rdmacro.h>
#include <rdripc.h>


#ifndef RDEVENT_PLAYER_H
#define RDEVENT_PLAYER_H

#define RDEVENT_PLAYER_MAX_EVENTS 10

class RDEventPlayer : public QObject
{
  Q_OBJECT
 public:
  RDEventPlayer(RDRipc *ripc,QObject *parent=0,const char *name=0);
  ~RDEventPlayer();
  bool exec(const QString &rml);
  bool exec(unsigned cartnum);

 private slots:
  void macroFinishedData(int id);
  void macroTimerData();

 private:
  RDMacroEvent *player_events[RDEVENT_PLAYER_MAX_EVENTS];
  bool player_state[RDEVENT_PLAYER_MAX_EVENTS];
  RDRipc *player_ripc;
  QSignalMapper *player_mapper;
  QTimer *player_timer;
};


#endif  // RDEVENT_PLAYER_H
