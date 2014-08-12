// rdsimpleplayer.cpp
//
// A naively simple player for Rivendell Carts.
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdsimpleplayer.cpp,v 1.14 2010/07/29 19:32:34 cvs Exp $
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

#include <rddb.h>
#include <rdsimpleplayer.h>


RDSimplePlayer::RDSimplePlayer(RDCae *cae,RDRipc *ripc,int card,int port,
			       unsigned start_cart,unsigned end_cart,
			       QWidget *parent,const char *name)
  : QWidget(parent,name)
{
  play_cae=cae;
  play_ripc=ripc;
  play_card=card;
  play_port=port;
  play_start_cart=start_cart;
  play_end_cart=end_cart;
  play_stream=-1;
  play_cart=0;
  play_is_playing=false;

  //
  // RDCae Connections
  //
  connect(play_cae,SIGNAL(playing(int)),this,SLOT(playingData(int)));
  connect(play_cae,SIGNAL(playStopped(int)),this,SLOT(playStoppedData(int)));

  //
  // Event Player
  //
  play_event_player=new RDEventPlayer(play_ripc,this,"play_event_player");

  //
  //  Start Button
  //
  play_start_button=
    new RDTransportButton(RDTransportButton::Play,parent,"play_start_button");
  play_start_button->setEnabled((play_card>=0)&&(play_port>=0));
  connect(play_start_button,SIGNAL(clicked()),this,SLOT(play()));

  //
  //  Stop Button
  //
  play_stop_button=
    new RDTransportButton(RDTransportButton::Stop,parent,"play_stop_button");
  play_stop_button->on();
  play_stop_button->setEnabled((play_card>=0)&&(play_port>=0));
  connect(play_stop_button,SIGNAL(clicked()),this,SLOT(stop()));

  hide();
}


RDSimplePlayer::~RDSimplePlayer()
{
  stop();
}


void RDSimplePlayer::setCart(unsigned cart)
{
  play_cart=cart;
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
  int handle=0;
  int play_cut_gain=0;
  QString sql;
  RDSqlQuery *q;

  if(play_cart==0) {
    return;
  }
  if(play_is_playing) {
    stop();
  }
  sql=QString().sprintf("select CUT_NAME,START_POINT,END_POINT,PLAY_GAIN\
                         from CUTS where (CART_NUMBER=%u)&&(LENGTH>0)",
			play_cart);
  q=new RDSqlQuery(sql);
  if(q->first()) {
    play_cae->
      loadPlay(play_card,q->value(0).toString(),&play_stream,&handle);
    play_cut_gain=q->value(3).toInt(); 
    if(play_stream<0) {
      delete q;
      return;
    }
    play_handles.push(handle);
    for(int i=0;i<RD_MAX_PORTS;i++) {
      play_cae->setOutputVolume(play_card,play_stream,i,RD_MUTE_DEPTH);
    }
    play_cae->setOutputVolume(play_card,play_stream,play_port,0+play_cut_gain);
    play_cae->positionPlay(play_handles.back(),q->value(1).toUInt()+start_pos);
    play_cae->play(play_handles.back(),
		   q->value(2).toUInt()-(q->value(1).toUInt()+start_pos),
		   RD_TIMESCALE_DIVISOR,false);
    play_cae->setPlayPortActive(play_card,play_port,play_stream);
  }
  delete q;
}


void RDSimplePlayer::stop()
{
  if(!play_is_playing) {
    return;
  }
  play_cae->stopPlay(play_handles.back());
}


void RDSimplePlayer::playingData(int handle)
{
  if(play_handles.empty()) {
    return;
  }
  if(handle!=play_handles.back()) {
    return;
  }
  play_event_player->exec(play_start_cart);
  play_start_button->on();
  play_stop_button->off();
  play_is_playing=true;
  emit played();
}


void RDSimplePlayer::playStoppedData(int handle)
{
  if(play_handles.empty()) {
    return;
  }
  if(handle!=play_handles.front()) {
    return;
  }
  play_cae->unloadPlay(play_handles.front());
  play_event_player->exec(play_end_cart);
  play_start_button->off();
  play_stop_button->on();
  play_handles.pop();
  play_is_playing=false;
  emit stopped();
}
