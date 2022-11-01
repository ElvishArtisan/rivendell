// catch_connect.cpp
//
// Connect to the Rivendell Netcatcher Daemon.
//
//   (C) Copyright 2002-2022 Fred Gleason <fredg@paravelsystems.com>
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

#include <ctype.h>

#include <qobject.h>

#include <rdcatch_connect.h>


RDCatchConnect::RDCatchConnect(int serial,QObject *parent)
  : QObject(parent)
{
  cc_serial=serial;

  cc_connected=false;
  argnum=0;
  argptr=0;
  for(int i=0;i<MAX_DECKS;i++) {
    cc_monitor_state[i]=false;
  }

  //
  // TCP Connection
  //
  cc_socket=new QTcpSocket(this);
  connect(cc_socket,SIGNAL(connected()),this,SLOT(connectedData()));
  connect(cc_socket,SIGNAL(readyRead()),this,SLOT(readyData()));
}


RDCatchConnect::~RDCatchConnect()
{
//  printf("Destroying RDCatchConnect\n");
}


void RDCatchConnect::connectHost(QString hostname,uint16_t hostport,
			       QString password)
{
  cc_password=password;
  cc_socket->connectToHost(hostname,hostport);
}


RDDeck::Status RDCatchConnect::status(unsigned chan) const
{
  if(chan<=MAX_DECKS) {
    return cc_record_deck_status[chan-1];
  }
  return cc_play_deck_status[chan-128];
}


int RDCatchConnect::currentId(unsigned chan) const
{
  if(chan<=MAX_DECKS) {
    return cc_record_id[chan-1];
  }
  return cc_play_id[chan-129];
}


void RDCatchConnect::enableMetering(bool state)
{
  SendCommand(QString::asprintf("RM %d!",state));
}


void RDCatchConnect::reloadDropboxes()
{
  SendCommand("RX!");
}


void RDCatchConnect::connectedData()
{
  SendCommand(QString("PW ")+cc_password+"!");
}


void RDCatchConnect::readyData()
{
  char buf[1024];
  int c;

  while((c=cc_socket->read(buf,254))>0) {
    buf[c]=0;
    // printf("readyData: %s\n",buf);
    for(int i=0;i<c;i++) {
      if(buf[i]==' ') {
	if(argnum<CC_MAX_ARGS) {
	  args[argnum][argptr]=0;
	  argnum++;
	  argptr=0;
	}
	else {
	  if(debug) {
	    printf("Argument list truncated!\n");
	  }
	}
      }
      if(buf[i]=='!') {
	args[argnum++][argptr]=0;
	DispatchCommand();
	argnum=0;
	argptr=0;
	if(cc_socket==NULL) {
	  return;
	}
      }
      if((isgraph(buf[i]))&&(buf[i]!='!')) {
	if(argptr<CC_MAX_LENGTH) {
	  args[argnum][argptr]=buf[i];
	  argptr++;
	}
	else {
	  if(debug) {
	    printf("WARNING: argument truncated!\n");
	  }
	}
      }
    }
  }
}


void RDCatchConnect::SendCommand(QString cmd)
{
  // printf("SendCommand(%s)\n",(const char *)cmd);
  cc_socket->write(cmd.toUtf8());
}


void RDCatchConnect::DispatchCommand()
{
  int deck;
  int channel;
  int level;
  //  unsigned chan;
  //  int status;
  int id;
  //  int number;

  if(!strcmp(args[0],"PW")) {   // Password Response
    if(args[1][0]=='+') {
      emit connected(cc_serial,true);
      SendCommand("RE 0!");
    }
    else {
      emit connected(cc_serial,false);
    }
  }
  /*
  if(!strcmp(args[0],"DE")) {   // Deck Event
    if(sscanf(args[1],"%d",&deck)!=1) {
      return;
    }
    if(sscanf(args[2],"%d",&number)!=1) {
      return;
    }
    emit deckEventSent(cc_serial,deck,number);
  }    

  if(!strcmp(args[0],"RE")) {   // Channel Status
    if(sscanf(args[1],"%u",&chan)!=1){
      return;
    }
    if((chan<0)||((chan>(MAX_DECKS+1))&&(chan<(129)))||
       (chan>(MAX_DECKS+129))) {
      return;
    }

    if(sscanf(args[2],"%d",&status)!=1) {
      return;
    }
    if(sscanf(args[3],"%d",&id)!=1) {
      return;
    }
    if(chan==0) {
      emit statusChanged(cc_serial,chan,(RDDeck::Status)status,id,"");
      return;
    }
    chan--;
    if(chan<=MAX_DECKS) {
      if((status!=cc_record_deck_status[chan])||(id!=cc_record_id[chan])) {
	cc_record_deck_status[chan]=(RDDeck::Status)status;
	cc_record_id[chan]=id;
	emit statusChanged(cc_serial,chan+1,cc_record_deck_status[chan],
			   cc_record_id[chan],args[4]);
      }
    }
    if((chan>=128)&&(chan<MAX_DECKS+128)) {
      if((status!=cc_play_deck_status[chan-128])||(id!=cc_play_id[chan-128])) {
	cc_play_deck_status[chan-128]=(RDDeck::Status)status;
	cc_play_id[chan-128]=id;
	emit statusChanged(cc_serial,chan+1,cc_play_deck_status[chan-128],
			   cc_play_id[chan-128],args[4]);
      }
    }
    return;
  }
  */
  if(!strcmp(args[0],"RM")) {   // Meter Level
    if(sscanf(args[1],"%d",&deck)!=1) {
      return;
    }
    if(sscanf(args[2],"%d",&channel)!=1) {
      return;
    }
    if(sscanf(args[3],"%d",&level)!=1) {
      return;
    }
    emit meterLevel(cc_serial,deck,channel,level);
    return;
  }

  if(!strcmp(args[0],"RU")) {   // Update Event
    if(sscanf(args[1],"%d",&id)!=1) {
      return;
    }
    emit eventUpdated(id);
  }
  /*
  if(!strcmp(args[0],"PE")) {   // Purge Event
    if(sscanf(args[1],"%d",&id)!=1) {
      return;
    }
    emit eventPurged(id);
  }
  */
  /*
  if(!strcmp(args[0],"HB")) {   // Heartbeat
    cc_heartbeat_timer->stop();
    cc_heartbeat_timer->start(CC_HEARTBEAT_INTERVAL);
  }
  */
  /*
  if(!strcmp(args[0],"MN")) {  // Monitor State
    if(sscanf(args[1],"%d",&deck)!=1) {
      return;
    }
    if(args[2][0]=='1') {
      cc_monitor_state[deck-1]=true;
      emit monitorChanged(cc_serial,deck,true);
    }
    else {
      cc_monitor_state[deck-1]=false;
      emit monitorChanged(cc_serial,deck,false);
    }
  }
  */
}


