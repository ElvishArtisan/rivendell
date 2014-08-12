// rdplay_deck.h
//
// Abstract a Rivendell Playback Deck
//
//   (C) Copyright 2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdplay_deck.h,v 1.36.8.1 2013/05/21 19:04:44 cvs Exp $
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

#include <qdatetime.h>
#include <qsqldatabase.h>
#include <qobject.h>
#include <qtimer.h>

#include <rdcae.h>
#include <rdcart.h>
#include <rdcut.h>
#include <rdlog_line.h>

#ifndef RDPLAY_DECK_H
#define RDPLAY_DECK_H

#define POSITION_INTERVAL 100
#define RDPLAYDECK_AUDITION_ID 2147483647
#define RDPLAYDECK_DUCKDOWN_LENGTH 750
#define RDPLAYDECK_DUCKUP_LENGTH 1500

class RDPlayDeck : public QObject
{
 Q_OBJECT

 public:
  enum State {Stopped=0,Stopping=1,Playing=2,Paused=3,Finished=4};
  RDPlayDeck(RDCae *cae,int id,QObject *parent=0,const char *name=0);
  ~RDPlayDeck();
  int id() const;
  void setId(int id);
  int owner() const;
  void setOwner(int owner);
  RDCart *cart() const;
  bool setCart(RDLogLine *logline,bool rotate);
  RDCut *cut() const;
  bool playable() const;
  int card() const;
  void setCard(int card_num);
  int stream() const;
  int port() const;
  void setPort(int port_num);
  int channel() const;
  void setChannel(int chan);
  RDPlayDeck::State state() const;
  QTime startTime() const;
  int currentPosition() const;
  int lastStartPosition() const;
  void clear();
  void reset();

 public slots:
  void play(unsigned pos,int segue_start=-1,int segue_end=-1,int duck_up_end=0);
  void playHook();
  void pause();
  void stop();
 // void stop(int interval);
  void stop(int interval,int gain=-10000);
  void duckDown(int interval);
  void duckVolume(int level,int fade);

 signals:
  void stateChanged(int id,RDPlayDeck::State);
  void position(int id,int msecs);
  void segueStart(int id);
  void segueEnd(int id);
  void hookStart(int id);
  void hookEnd(int id);
  void talkStart(int id);
  void talkEnd(int id);

 private slots:
  void playingData(int handle);
  void playStoppedData(int handle); 
  void pointTimerData(int);
  void positionTimerData();
  void fadeTimerData();
  void duckTimerData();

 private:
  enum Point {Segue=0,Hook=1,Talk=2,SizeOf=3};
  void StartTimers(int offset);
  void StopTimers();
  QTimer *play_position_timer;
  RDCart *play_cart;
  RDCut *play_cut;
  RDCae *play_cae;
  QTimer *play_point_timer[3];
  QTimer *play_stop_timer;
  QTimer *play_fade_timer;
  QTimer *play_duck_timer;
  bool play_duck_down_state;
  bool play_fade_down_state;
  int play_segue_interval;
  bool play_point_state[3];
  int play_point_value[3][2];
  int play_point_gain;
  int play_audio_point[2];
  int play_audio_length;
  int play_fade_point[2];
  int play_fade_gain[2];
  int play_fade_down;
  int play_cut_gain;
  int play_duck_level;
  int play_duck_gain[2];
  int play_duck_up;
  int play_duck_down;
  int play_ducked;
  int play_duck_up_point;
  int play_card;
  int play_stream;
  int play_port;
  int play_channel;
  int play_handle;
  unsigned play_forced_length;
  bool play_hook_mode;
  QTime play_start_time;
  RDPlayDeck::State play_state;
  bool stop_called;
  bool pause_called;
  int play_id;
  int play_owner;
  unsigned play_start_position;
  int play_last_start_position;
  int play_current_position;
  bool play_timescale_active;
  int play_timescale_speed;
};


#endif 
