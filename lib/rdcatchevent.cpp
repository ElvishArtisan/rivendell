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


bool RDCatchEvent::isValid() const
{
  return true;
}


bool RDCatchEvent::read(const QString &str)
{
  QStringList f0=str.split(" ");
  bool ok=false;

  clear();

  //
  // Common Fields
  //
  if((f0.size()<3)||(f0.at(0)!="CATCH")) {
    return false;
  }

  //
  // Operation-specific Fields
  //
  if(f0.at(2)==
     RDCatchEvent::operationString(RDCatchEvent::DeckEventProcessedOp)) {
    if(f0.size()!=5) {
      return false;
    }
    unsigned chan=f0.at(3).toUInt(&ok);
    if(!ok) {
      return false;
    }
    unsigned num=f0.at(4).toUInt(&ok);
    if(ok) {
      d_operation=RDCatchEvent::DeckEventProcessedOp;
      d_host_name=f0.at(1);
      d_deck_channel=chan;
      d_event_number=num;
      return true;
    }
  }
  /*
  if(f0.at(2)==
     RDCatchEvent::operationString(RDCatchEvent::DeckEventProcessedOp)) {
    if(f0.size()!=5) {
      return false;
    }
    unsigned chan=f0.at(3).toUInt(&ok);
    if(!ok) {
      return false;
    }
    unsigned val=f0.at(4).toUInt(&ok);
    if(ok&&(val<RDDeck::LastStatus)) {
      d_operation=RDCatchEvent::DeckEventProcessedOp;
      d_host_name=f0.at(1);
      d_deck_channel=chan;
      d_deck_status=(RDDeck::Status)val;
      return true;
    }
  }
  */
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

  //
  // Operation-specific Fields
  //
  ret+=RDCatchEvent::operationString(d_operation);
  switch(d_operation) {
  case RDCatchEvent::DeckEventProcessedOp:
    ret+=QString::asprintf(" %u",d_deck_channel);
    ret+=QString::asprintf(" %u",d_event_number);
    break;

  case RDCatchEvent::NullOp:
  case RDCatchEvent::LastOp:
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
  ret+="operation: "+RDCatchEvent::operationString(d_operation)+"\n";
  switch(d_operation) {
  case RDCatchEvent::DeckEventProcessedOp:
    ret+=QString::asprintf("deck channel: %u\n",d_deck_channel);
    ret+=QString::asprintf("event number: %u\n",d_event_number);
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
  d_deck_channel=0;
  d_event_number=0;
  d_deck_status=RDDeck::Offline;
}


QString RDCatchEvent::operationString(Operation op)
{
  QString ret="UNKNOWN";

  switch(op) {
  case RDCatchEvent::DeckEventProcessedOp:
    ret="DE";
    break;

  case RDCatchEvent::NullOp:
  case RDCatchEvent::LastOp:
    break;
  }

  return ret;
}
