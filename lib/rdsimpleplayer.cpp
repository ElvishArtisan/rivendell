// rdsimpleplayer.cpp
//
// A naively simple player for Rivendell Carts.
//
//   (C) Copyright 2002-2023 Fred Gleason <fredg@paravelsystems.com>
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

#include "rdcart.h"
#include "rddb.h"
#include "rdescape_string.h"
#include "rdsimpleplayer.h"

RDSimplePlayer::RDSimplePlayer(RDCae *cae,RDRipc *ripc,int card,int port,
			       unsigned start_cart,unsigned end_cart,
			       QWidget *parent)
  : QWidget(parent)
{
  play_cae=cae;
  play_ripc=ripc;
  play_card=card;
  play_port=port;
  play_start_cart=start_cart;
  play_end_cart=end_cart;
  play_serial=-1;
  play_cart=0;
  play_cut="";
  play_is_playing=false;

  //
  // RDCae Connections
  //
  connect(play_cae,SIGNAL(playbackStopped(int)),
	  this,SLOT(playStoppedData(int)));

  //
  // Event Player
  //
  play_event_player=new RDEventPlayer(play_ripc,this);

  //
  //  Start Button
  //
  play_start_button=new RDTransportButton(RDTransportButton::Play,parent);
  play_start_button->setEnabled((play_card>=0)&&(play_port>=0));
  connect(play_start_button,SIGNAL(clicked()),this,SLOT(play()));

  //
  //  Stop Button
  //
  play_stop_button=new RDTransportButton(RDTransportButton::Stop,parent);
  play_stop_button->on();
  play_stop_button->setEnabled((play_card>=0)&&(play_port>=0));
  connect(play_stop_button,SIGNAL(clicked()),this,SLOT(stop()));

  hide();
}


RDSimplePlayer::~RDSimplePlayer()
{
  stop();
}


bool RDSimplePlayer::isPlaying()
{
  return play_is_playing;
}


void RDSimplePlayer::setCart(unsigned cart)
{
  play_cart=cart;
  play_cut="";
}


void RDSimplePlayer::setCart(QString cart)
{
  QStringList cartcut=cart.split("_");
  play_cart=cartcut[0].toUInt();
  if(cartcut.size()>1) {
    setCut(cart);
  }
  else {
    play_cut="";
  }
}


void RDSimplePlayer::setCut(QString cut)
{
  play_cut=cut;
}


RDTransportButton *RDSimplePlayer::playButton() const
{
  return play_start_button;
}


RDTransportButton *RDSimplePlayer::stopButton() const
{
  return play_stop_button;
}


void RDSimplePlayer::play()
{
  play(0);
}


void RDSimplePlayer::play(int start_pos)
{
  QString sql;
  RDSqlQuery *q;

  if(play_cart==0) {
    return;
  }
  if(play_is_playing) {
    stop();
  }

  if(play_cut.isEmpty()) {
    RDCart *cart=new RDCart(play_cart);
    cart->selectCut(&play_cut);
    delete cart;
  }
  if(!play_cut.isEmpty()) {
    sql=QString("select ")+
      "`START_POINT`,"+  // 00
      "`END_POINT`,"+    // 01
      "`PLAY_GAIN` "+    // 02
      "from `CUTS` where "+
      "`CUT_NAME`='"+RDEscapeString(play_cut)+"'";
    q=new RDSqlQuery(sql);
    if(q->first()) {
      /*
      play_handles.push(handle);
      for(int i=0;i<RD_MAX_PORTS;i++) {
        play_cae->setOutputVolume(play_card,play_stream,i,RD_MUTE_DEPTH);
      }
      play_cae->setOutputVolume(play_card,play_stream,play_port,0+play_cut_gain);
      play_cae->positionPlay(play_handles.back(),q->value(0).toUInt()+start_pos);
      play_cae->play(play_handles.back(),
                     q->value(1).toUInt()-(q->value(0).toUInt()+start_pos),
                     RD_TIMESCALE_DIVISOR,false);
      */
      play_serial=
	play_cae->startPlayback(play_cut,play_card,play_port,
				q->value(0).toInt()+start_pos,
				q->value(1).toInt(),
				RD_TIMESCALE_DIVISOR,q->value(2).toInt());
      //      play_cae->setPlayPortActive(play_card,play_port,play_stream);
      if(play_serial>0) {
	play_event_player->exec(play_start_cart);
	play_start_button->on();
	play_stop_button->off();
	play_is_playing=true;
	emit played();
      }
    }
    delete q;
  }
}


void RDSimplePlayer::stop()
{
  if(!play_is_playing) {
    return;
  }
  play_cae->stopPlayback(play_serial);
  play_cut="";
  play_is_playing=false;
}


void RDSimplePlayer::playStoppedData(int handle)
{
  play_event_player->exec(play_end_cart);
  play_start_button->off();
  play_stop_button->on();
  play_is_playing=false;

  emit stopped();
}
