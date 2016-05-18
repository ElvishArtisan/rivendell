// event_player.h
//
// A player class for Deck Events
//
//   (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef EVENT_PLAYER_H
#define EVENT_PLAYER_H

#include <vector>

#include <qdatetime.h>
#include <qobject.h>
#include <qtimer.h>

#include <rdstation.h>

class EventPlayer : public QObject
{
  Q_OBJECT;
 public:
  EventPlayer(RDStation *station,int chan,QObject *parent=0);
  void load(const QString &cutname);
  void start(int start_ptr);
  void stop();

 signals:
  void runCart(unsigned cartnum);

 private slots:
  void timeoutData();

 private:
  std::vector<unsigned> event_deck_events;
  std::vector<int> event_numbers;
  std::vector<int> event_points;
  int event_current_event;
  int event_start_point;
  QTime event_start_time;
  QTimer *event_timer;
  RDStation *event_station;
  int event_channel;
};


#endif  // EVENT_PLAYER_H
