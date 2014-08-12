// rdlivewiredestination.cpp
//
// Accessor Methods for LiveWire Destination Parameters
//
//   (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdlivewiredestination.cpp,v 1.3 2010/07/29 19:32:33 cvs Exp $
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

#include <rd.h>
#include <rdlivewiredestination.h>


RDLiveWireDestination::RDLiveWireDestination()
{
  clear();
}


int RDLiveWireDestination::slotNumber() const
{
  return live_slot_number;
}


void RDLiveWireDestination::setSlotNumber(int chan)
{
  live_slot_number=chan;
}


QString RDLiveWireDestination::primaryName() const
{
  return live_primary_name;
}


void RDLiveWireDestination::setPrimaryName(const QString &name)
{
  live_primary_name=name;
}


QHostAddress RDLiveWireDestination::streamAddress() const
{
  return live_stream_address;
}


void RDLiveWireDestination::setStreamAddress(const QHostAddress &addr)
{
  live_stream_address=addr;
}


Q_UINT16 RDLiveWireDestination::streamUdpPort() const
{
  return live_stream_udp_port;
}


void RDLiveWireDestination::setStreamUdpPort(Q_UINT16 port)
{
  live_stream_udp_port=port;
}


int RDLiveWireDestination::channels() const
{
  return live_channels;
}


void RDLiveWireDestination::setChannels(int chans)
{
  live_channels=chans;
}


RDLiveWireDestination::Load RDLiveWireDestination::load() const
{
  return live_load;
}


void RDLiveWireDestination::setLoad(RDLiveWireDestination::Load load)
{
  live_load=load;
}


int RDLiveWireDestination::outputGain() const
{
  return live_output_gain;
}


void RDLiveWireDestination::setOutputGain(int lvl)
{
  live_output_gain=lvl;
}


void RDLiveWireDestination::clear()
{
  live_slot_number=-1;
  live_primary_name="";
  live_stream_address=QHostAddress();
  live_stream_udp_port=RD_LIVEWIRE_DEFAULT_STREAM_PORT;
  live_channels=RD_LIVEWIRE_DEFAULT_CHANNELS;
  live_load=RD_LIVEWIRE_DEFAULT_LOAD;
  live_output_gain=0;
};


QString RDLiveWireDestination::loadString(RDLiveWireDestination::Load load)
{
  switch(load) {
    case RDLiveWireDestination::LoadHighZ:
      return QString("hi-Z");

    case RDLiveWireDestination::Load600Ohm:
      return QString("600 ohm");

    case RDLiveWireDestination::LoadMinus10:
      return QString("-10 dBv");
  }
  return QString("Unknown");
}
