// rdlivewiredestination.h
//
// Accessor Methods for LiveWire Destination Parameters
//
//   (C) Copyright 2007-2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDLIVEWIREDESTINATION_H
#define RDLIVEWIREDESTINATION_H

#include <stdint.h>

#include <qhostaddress.h>

class RDLiveWireDestination
{
 public:
  enum Load {LoadHighZ=0,Load600Ohm=600,LoadMinus10=-125};
  RDLiveWireDestination();
  int slotNumber() const;
  void setSlotNumber(int chan);
  QString primaryName() const;
  void setPrimaryName(const QString &name);
  QHostAddress streamAddress() const;
  void setStreamAddress(const QHostAddress &addr);
  uint16_t streamUdpPort() const;
  void setStreamUdpPort(uint16_t port);
  int channels() const;
  void setChannels(int chans);
  RDLiveWireDestination::Load load() const;
  void setLoad(RDLiveWireDestination::Load load);
  int outputGain() const;
  void setOutputGain(int lvl);
  void clear();
  static QString loadString(RDLiveWireDestination::Load load);

 private:
  int live_slot_number;
  QString live_primary_name;
  QHostAddress live_stream_address;
  uint16_t live_stream_udp_port;
  int live_channels;
  RDLiveWireDestination::Load live_load;
  int live_output_gain;
};


#endif  // RDLIVEWIREDESTINATION_H
