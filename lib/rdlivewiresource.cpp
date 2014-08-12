// rdlivewiresource.cpp
//
// Accessor Methods for LiveWire Source Parameters
//
//   (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdlivewiresource.cpp,v 1.3 2010/07/29 19:32:33 cvs Exp $
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
#include <rdlivewiresource.h>


RDLiveWireSource::RDLiveWireSource()
{
  clear();
}


int RDLiveWireSource::slotNumber() const
{
  return live_slot_number;
}


void RDLiveWireSource::setSlotNumber(int slot)
{
  live_slot_number=slot;
}


int RDLiveWireSource::channelNumber() const
{
  return live_channel_number;
}


void RDLiveWireSource::setChannelNumber(int chan)
{
  live_channel_number=chan;
  live_stream_address.setAddress(0xEFC00000|chan);
}


QString RDLiveWireSource::primaryName() const
{
  return live_primary_name;
}


void RDLiveWireSource::setPrimaryName(const QString &name)
{
  live_primary_name=name;
}


QString RDLiveWireSource::labelName() const
{
  return live_label_name;
}


void RDLiveWireSource::setLabelName(const QString &name)
{
  live_label_name=name;
}


bool RDLiveWireSource::rtpEnabled() const
{
  return live_rtp_enabled;
}


void RDLiveWireSource::setRtpEnabled(bool state)
{
  live_rtp_enabled=state;
}


QHostAddress RDLiveWireSource::streamAddress() const
{
  return live_stream_address;
}


void RDLiveWireSource::setStreamAddress(const QHostAddress &addr)
{
  live_stream_address=addr;
  live_channel_number=addr.toIPv4Address()&0xFFFF;
}


Q_UINT16 RDLiveWireSource::streamUdpPort() const
{
  return live_stream_udp_port;
}


void RDLiveWireSource::setStreamUdpPort(Q_UINT16 port)
{
  live_stream_udp_port=port;
}


bool RDLiveWireSource::shareable() const
{
  return live_shareable;
}


void RDLiveWireSource::setShareable(bool state)
{
  live_shareable=state;
}


int RDLiveWireSource::inputGain() const
{
  return live_input_gain;
}


void RDLiveWireSource::setInputGain(int lvl)
{
  live_input_gain=lvl;
}


int RDLiveWireSource::channels() const
{
  return live_channels;
}


void RDLiveWireSource::setChannels(int chans)
{
  live_channels=chans;
}


void RDLiveWireSource::clear()
{
  live_slot_number=-1;
  live_channel_number=-1;
  live_primary_name="";
  live_label_name="";
  live_rtp_enabled=false;
  live_stream_address=QHostAddress();
  live_stream_udp_port=RD_LIVEWIRE_DEFAULT_STREAM_PORT;
  live_shareable=false;
  live_input_gain=0;
  live_channels=RD_LIVEWIRE_DEFAULT_CHANNELS;
}
