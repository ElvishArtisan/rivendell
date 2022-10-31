// rdcatchevent.h
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

#ifndef RDCATCHEVENT_H
#define RDCATCHEVENT_H

#include <QString>
#include <QVariant>

#include <rddeck.h>

class RDCatchEvent
{
 public:
  enum Operation {NullOp=0,DeckEventProcessedOp=1,
		  DeckStatusQueryOp=2,DeckStatusResponseOp=3,
		  StopDeckOp=5,LastOp=6};
  RDCatchEvent(RDDeck::Status status);
  RDCatchEvent();
  Operation operation() const;
  void setOperation(Operation op);
  QString hostName() const;
  void setHostName(const QString &str);
  QString targetHostName() const;
  void setTargetHostName(const QString &str);
  unsigned eventId() const;
  void setEventId(unsigned id);
  unsigned cartNumber() const;
  void setCartNumber(unsigned cartnum);
  int cutNumber() const;
  void setCutNumber(int cutnum);
  unsigned deckChannel() const;
  void setDeckChannel(unsigned chan);
  int eventNumber() const;
  void setEventNumber(int num);
  RDDeck::Status deckStatus() const;
  void setDeckStatus(RDDeck::Status status);
  bool isValid() const;
  bool read(const QString &str);
  QString write() const;
  QString dump() const;
  void clear();

 private:
  Operation d_operation;
  QString d_host_name;
  QString d_target_host_name;
  unsigned d_event_id;
  unsigned d_cart_number;
  int d_cut_number;
  unsigned d_deck_channel;
  int d_event_number;
  RDDeck::Status d_deck_status;
};


#endif  // RDCATCHEVENT_H
