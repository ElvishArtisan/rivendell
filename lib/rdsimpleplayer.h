// rdsimpleplayer.h
//
// A naively simple player for Rivendell Carts.
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdsimpleplayer.h,v 1.7 2010/07/29 19:32:34 cvs Exp $
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

#include <queue>

#include <qwidget.h>

#include <rdtransportbutton.h>
#include <rdcae.h>
#include <rdripc.h>
#include <rdevent_player.h>

#ifndef RDSIMPLEPLAYER_H
#define RDSIMPLEPLAYER_H


class RDSimplePlayer : public QWidget
{
  Q_OBJECT
 public:
  RDSimplePlayer(RDCae *cae,RDRipc *ripc,int card,int port,
		 unsigned start_cart,unsigned end_cart,
		 QWidget *parent=0,const char *name=0);
  ~RDSimplePlayer();
  void setCart(unsigned cart);
  RDTransportButton *playButton() const;
  RDTransportButton *stopButton() const;

 public slots:
  void play();
  void play(int start_pos);
  void stop();

 signals:
  void played();
  void stopped();

 private slots:
  void playingData(int handle);
  void playStoppedData(int handle);

 private:
  RDCae *play_cae;
  RDRipc *play_ripc;
  int play_card;
  int play_stream;
  int play_port;
  std::queue<int> play_handles;
  unsigned play_cart;
  unsigned play_start_cart;
  unsigned play_end_cart;
  bool play_is_playing;
  RDEventPlayer *play_event_player;
  RDTransportButton *play_start_button;
  RDTransportButton *play_stop_button;
};


#endif  // RDSIMPLEPLAYER_H
