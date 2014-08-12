// rdevent_player.cpp
//
// Execute a list of RML commands asynchronously.
//
//   (C) Copyright 2002-2006 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdevent_player.cpp,v 1.6.8.1 2012/12/13 22:33:44 cvs Exp $
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

#include <rdevent_player.h>


RDEventPlayer::RDEventPlayer(RDRipc *ripc,QObject *parent,const char *name)
  : QObject(parent,name)
{
  player_ripc=ripc;

  for(unsigned i=0;i<RDEVENT_PLAYER_MAX_EVENTS;i++) {
    player_events[i]=NULL;
    player_state[i]=false;
  }
  player_mapper=new QSignalMapper(this,"player_mapper");
  connect(player_mapper,SIGNAL(mapped(int)),
	  this,SLOT(macroFinishedData(int)));
  player_timer=new QTimer(this,"player_timer");
  connect(player_timer,SIGNAL(timeout()),this,SLOT(macroTimerData()));
}


RDEventPlayer::~RDEventPlayer()
{
}


bool RDEventPlayer::exec(const QString &rml)
{
  bool ret=false;

  for(int i=0;i<RDEVENT_PLAYER_MAX_EVENTS;i++) {
    if(player_events[i]==NULL) {
      player_events[i]=new RDMacroEvent(player_ripc,this);
      player_state[i]=true;
      player_mapper->setMapping(player_events[i],i);
      connect(player_events[i],SIGNAL(finished()),
	      player_mapper,SLOT(map()));
      if(player_events[i]->load(rml)) {
	player_events[i]->exec();
	ret=true;
      }
      i=RDEVENT_PLAYER_MAX_EVENTS;
    }
  }
  return ret;
}


bool RDEventPlayer::exec(unsigned cartnum)
{
  if(cartnum==0) {
    return false;
  }
  return exec(QString().sprintf("EX %u!",cartnum));
}


void RDEventPlayer::macroFinishedData(int id)
{
  player_state[id]=false;
  player_timer->start(1,true);
}


void RDEventPlayer::macroTimerData()
{
  for(int i=0;i<RDEVENT_PLAYER_MAX_EVENTS;i++) {
    if((!player_state[i])&&(player_events[i]!=NULL)) {
      delete player_events[i];
      player_events[i]=NULL;
    }
  }
}
