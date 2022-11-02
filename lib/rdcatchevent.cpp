// rdcatchevent.cpp
//
// A container class for a Rivendell Catch Event message.
//
//   (C) Copyright 2022 Fred Gleason <fredg@paravelsystems.com>
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

#include <QStringList>

#include "rdcatchevent.h"
#include "rdapplication.h"

RDCatchMeterLevel::RDCatchMeterLevel(int deck,int16_t *lvls)
{
  d_deck_channel=deck;
  for(int i=0;i<RDCatchMeterLevel::LastChannel;i++) {
    d_levels[i]=-lvls[i];
  }
}


unsigned RDCatchMeterLevel::deckChannel() const
{
  return d_deck_channel;
}


int16_t RDCatchMeterLevel::level(Channel chan) const
{
  return -d_levels[chan];
}


QString RDCatchMeterLevel::dump() const
{
  QString lvls;
  QString ret;

  for(int j=0;j<RDCatchMeterLevel::LastChannel;j++) {
    lvls+=QString::asprintf(":%04X",d_levels[j]);
  }
  ret+=QString::asprintf("%u",d_deck_channel)+lvls;

  return ret;
}




RDCatchEvent::RDCatchEvent(RDDeck::Status status)
{
  clear();
  d_operation=RDCatchEvent::DeckEventProcessedOp;
  d_deck_status=status;
}


RDCatchEvent::RDCatchEvent()
{
  clear();
}


RDCatchEvent::Operation RDCatchEvent::operation() const
{
  return d_operation;
}


void RDCatchEvent::setOperation(RDCatchEvent::Operation op)
{
  d_operation=op;
}


int RDCatchEvent::eventNumber() const
{
  return d_event_number;
}


void RDCatchEvent::setEventNumber(int num)
{
  d_event_number=num;
}


QString RDCatchEvent::hostName() const
{
  return d_host_name;
}


void RDCatchEvent::setHostName(const QString &str)
{
  d_host_name=str;
}


QString RDCatchEvent::targetHostName() const
{
  return d_target_host_name;
}


void RDCatchEvent::setTargetHostName(const QString &str)
{
  d_target_host_name=str;
}


unsigned RDCatchEvent::eventId() const
{
  return d_event_id;
}


void RDCatchEvent::setEventId(unsigned id)
{
  d_event_id=id;
}


unsigned RDCatchEvent::cartNumber() const
{
  return d_cart_number;
}


void RDCatchEvent::setCartNumber(unsigned cartnum)
{
  d_cart_number=cartnum;
}


int RDCatchEvent::cutNumber() const
{
  return d_cut_number;
}


void RDCatchEvent::setCutNumber(int cutnum)
{
  d_cut_number=cutnum;
}


unsigned RDCatchEvent::deckChannel() const
{
  return d_deck_channel;
}


void RDCatchEvent::setDeckChannel(unsigned chan)
{
  d_deck_channel=chan;
}


RDDeck::Status RDCatchEvent::deckStatus() const
{
  return d_deck_status;
}


void RDCatchEvent::setDeckStatus(RDDeck::Status status)
{
  d_deck_status=status;
}


bool RDCatchEvent::inputMonitorActive() const
{
  return d_input_monitor_active;
}


void RDCatchEvent::setInputMonitorActive(bool state)
{
  d_input_monitor_active=state;
}


QList<RDCatchMeterLevel> RDCatchEvent::meterLevels() const
{
  return d_meter_levels;
}


void RDCatchEvent::setMeterLevels(const QList<RDCatchMeterLevel> &lvls)
{
  d_meter_levels=lvls;
}


bool RDCatchEvent::read(const QString &str)
{
  //  printf("RDCatchEvent::read(\"%s\")\n",str.toUtf8().constData());

  RDCatchEvent::Operation op=RDCatchEvent::NullOp;
  QStringList f0=str.split(" ");

  unsigned chan=0;
  unsigned num=0;
  RDDeck::Status status=RDDeck::Offline;
  int state=0;
  bool ok=false;

  clear();

  //
  // Common Fields
  //
  if((f0.size()<3)||(f0.at(0)!="CATCH")) {
    return false;
  }
  op=(RDCatchEvent::Operation)f0.at(2).toUInt(&ok);
  if(!ok) {
    return false;
  }

  //
  // Operation-specific Fields
  //
  switch(op) {
  case RDCatchEvent::DeckEventProcessedOp:
    if(f0.size()!=5) {
      return false;
    }
    chan=f0.at(3).toUInt(&ok);
    if(!ok) {
      return false;
    }
    num=f0.at(4).toUInt(&ok);
    if(ok) {
      d_operation=op;
      d_host_name=f0.at(1);
      d_deck_channel=chan;
      d_event_number=num;
      return true;
    }
    break;

  case RDCatchEvent::SendMeterLevelsOp:
    for(int i=2;i<f0.size();i++) {
      QStringList f1=f0.at(i).split(":",QString::KeepEmptyParts);
      if(f1.size()==(1+RDCatchMeterLevel::LastChannel)) {
	chan=f1.at(0).toUInt(&ok);
	if(chan>=255) {
	  d_meter_levels.clear();
	  return false;
	}
	int16_t lvls[RDCatchMeterLevel::LastChannel];
	for(int j=0;j<RDCatchMeterLevel::LastChannel;j++) {
	  int lvl=f1.at(1+j).toInt(&ok,16);
	  if((!ok)||(lvl>0xFFFF)) {
	    d_meter_levels.clear();
	    return false;
	  }
	  lvls[j]=-(int16_t)lvl;
	}
	d_meter_levels.push_back(RDCatchMeterLevel(chan,lvls));
      }
    }
    d_operation=op;
    d_host_name=f0.at(1);
    return true;
    break;

  case RDCatchEvent::DeckStatusQueryOp:
    if(f0.size()!=3) {
      return false;
    }
    d_operation=op;
    d_host_name=f0.at(1);
    return true;
    break;

  case RDCatchEvent::DeckStatusResponseOp:
    if(f0.size()!=8) {
      return false;
    }
    chan=f0.at(3).toUInt(&ok);
    if(ok&&(chan<255)) {
      status=(RDDeck::Status)f0.at(4).toUInt(&ok);
      if(ok&&(status<RDDeck::LastStatus)) {
	unsigned id=f0.at(5).toUInt(&ok);
	if(ok) {
	  unsigned cartnum=f0.at(6).toUInt(&ok);
	  if(ok&&(cartnum<=RD_MAX_CART_NUMBER)) {
	    int cutnum=f0.at(7).toInt(&ok);
	    if(ok&&(cutnum>=0)&&(cutnum<=RD_MAX_CUT_NUMBER)) {
	      d_operation=op;
	      d_host_name=f0.at(1);
	      d_deck_channel=chan;
	      d_deck_status=status;
	      d_event_id=id;
	      d_cart_number=cartnum;
	      d_cut_number=cutnum;
	      return true;
	    }
	  }
	}
      }
    }
    break;

  case RDCatchEvent::StopDeckOp:
    if(f0.size()!=5) {
      return false;
    }    
    chan=f0.at(4).toInt(&ok);
    if(ok&&(chan<255)) {
      d_operation=op;
      d_host_name=f0.at(1);
      d_target_host_name=f0.at(3);
      d_deck_channel=chan;
      return true;
    }
    break;

  case RDCatchEvent::SetInputMonitorOp:
    if(f0.size()!=6) {
      return false;
    }    
    chan=f0.at(4).toInt(&ok);
    if(ok&&(chan<255)) {
      state=f0.at(5).toUInt(&ok);
      if((state==0)||(state==1)) {
	d_operation=op;
	d_host_name=f0.at(1);
	d_target_host_name=f0.at(3);
	d_deck_channel=chan;
	d_input_monitor_active=(state==1);
	return true;
      }
    }
    break;

  case RDCatchEvent::SetInputMonitorResponseOp:
    if(f0.size()!=5) {
      return false;
    }    
    chan=f0.at(3).toInt(&ok);
    if(ok&&(chan<255)) {
      state=f0.at(4).toUInt(&ok);
      if((state==0)||(state==1)) {
	d_operation=op;
	d_host_name=f0.at(1);
	d_deck_channel=chan;
	d_input_monitor_active=(state==1);
	return true;
      }
    }
    break;

  case RDCatchEvent::ReloadDecksOp:
    if(f0.size()!=4) {
      return false;
    }    
    chan=f0.at(4).toInt(&ok);
    if(ok&&(chan<255)) {
      d_operation=op;
      d_host_name=f0.at(1);
      d_target_host_name=f0.at(3);
      return true;
    }
    break;

  case RDCatchEvent::NullOp:
  case RDCatchEvent::LastOp:
    break;
  }

  return false;
}


QString RDCatchEvent::write() const
{
  QString ret;

  //
  // Common Fields
  //
  ret+="CATCH ";
  ret+=d_host_name+" ";
  ret+=QString::asprintf("%u",d_operation);

  //
  // Operation-specific Fields
  //
  switch(d_operation) {
  case RDCatchEvent::DeckEventProcessedOp:
    ret+=QString::asprintf(" %u",d_deck_channel);
    ret+=QString::asprintf(" %u",d_event_number);
    break;

  case RDCatchEvent::DeckStatusResponseOp:
    ret+=QString::asprintf(" %u",d_deck_channel);
    ret+=QString::asprintf(" %u",d_deck_status);
    ret+=QString::asprintf(" %u",d_event_id);
    ret+=QString::asprintf(" %u",d_cart_number);
    ret+=QString::asprintf(" %d",d_cut_number);
    break;

  case RDCatchEvent::DeckStatusQueryOp:
  case RDCatchEvent::NullOp:
  case RDCatchEvent::LastOp:
    break;

  case RDCatchEvent::SendMeterLevelsOp:
    for(int i=0;i<d_meter_levels.size();i++) {
      ret+=" "+d_meter_levels.at(i).dump();
    }
    break;

  case RDCatchEvent::SetInputMonitorOp:
    ret+=" "+d_target_host_name;
    ret+=QString::asprintf(" %u",d_deck_channel);
    ret+=QString::asprintf(" %u",d_input_monitor_active);
    break;

  case RDCatchEvent::SetInputMonitorResponseOp:
    ret+=QString::asprintf(" %u",d_deck_channel);
    ret+=QString::asprintf(" %u",d_input_monitor_active);
    break;

  case RDCatchEvent::StopDeckOp:
    ret+=" "+d_target_host_name;
    ret+=QString::asprintf(" %u",d_deck_channel);
    break;

  case RDCatchEvent::ReloadDecksOp:
    ret+=" "+d_target_host_name;
    break;
  }

  return ret;
}


QString RDCatchEvent::dump() const
{
  QString ret;

  //
  // Common Fields
  //
  ret+="hostName: "+d_host_name+"\n";

  //
  // Operation-specific Fields
  //
  switch(d_operation) {
  case RDCatchEvent::DeckEventProcessedOp:
    ret+="operation: RDCatchEvent::DeckEventProcessedOp\n";
    ret+=QString::asprintf("deck channel: %u\n",d_deck_channel);
    ret+=QString::asprintf("event number: %u\n",d_event_number);
    break;

  case RDCatchEvent::SendMeterLevelsOp:
    for(int i=0;i<d_meter_levels.size();i++) {
      ret+="meter level: "+d_meter_levels.at(i).dump()+"\n";
    }
    break;

  case RDCatchEvent::DeckStatusQueryOp:
    ret+="operation: RDCatchEvent::DeckStatusQueryOp\n";
    break;

  case RDCatchEvent::DeckStatusResponseOp:
    ret+="operation: RDCatchEvent::DeckStatusResponseOp\n";
    ret+=QString::asprintf("deck channel: %u\n",d_deck_channel);
    ret+=QString::asprintf("deck status: %u\n",d_deck_status);
    ret+=QString::asprintf("event id: %u\n",d_event_id);
    ret+=QString::asprintf("cart number: %u\n",d_cart_number);
    ret+=QString::asprintf("cut number: %d\n",d_cut_number);
    break;

  case RDCatchEvent::ReloadDecksOp:
    ret+="operation: RDCatchEvent::ReloadDecksOp\n";
    ret+="target hostname: "+d_target_host_name+"\n";
    break;

  case RDCatchEvent::StopDeckOp:
    ret+="operation: RDCatchEvent::StopDeckOp\n";
    ret+="target hostname: "+d_target_host_name+"\n";
    ret+=QString::asprintf("deck channel: %u\n",d_deck_channel);
    break;

  case RDCatchEvent::SetInputMonitorOp:
    ret+="operation: RDCatchEvent::SetInputMonitorOp\n";
    ret+="target hostname: "+d_target_host_name+"\n";
    ret+=QString::asprintf("deck channel: %u\n",d_deck_channel);
    ret+=QString::asprintf("input monitor active: %u\n",d_input_monitor_active);
    break;

  case RDCatchEvent::SetInputMonitorResponseOp:
    ret+="operation: RDCatchEvent::SetInputMonitorResponseOp\n";
    ret+=QString::asprintf("deck channel: %u\n",d_deck_channel);
    ret+=QString::asprintf("input monitor active: %u\n",d_input_monitor_active);
    break;

  case RDCatchEvent::NullOp:
  case RDCatchEvent::LastOp:
    break;
  }

  return ret;
}


void RDCatchEvent::clear()
{
  d_operation=RDCatchEvent::NullOp;
  d_host_name=rda->station()->name();
  d_target_host_name="";
  d_event_id=0;
  d_cart_number=0;
  d_cut_number=0;
  d_deck_channel=0;
  d_event_number=0;
  d_input_monitor_active=false;
  d_deck_status=RDDeck::Offline;
  d_meter_levels.clear();
}
