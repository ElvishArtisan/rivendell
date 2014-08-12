// rdlivewiresource.h
//
// Accessor Methods for LiveWire Source Parameters
//
//   (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdlivewiresource.h,v 1.4 2010/07/29 19:32:33 cvs Exp $
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

#ifndef RDLIVEWIRESOURCE_H
#define RDLIVEWIRESOURCE_H

#include <qhostaddress.h>


class RDLiveWireSource
{
 public:
  RDLiveWireSource();
  int slotNumber() const;
  void setSlotNumber(int slot);
  int channelNumber() const;
  void setChannelNumber(int chan);
  QString primaryName() const;
  void setPrimaryName(const QString &name);
  QString labelName() const;
  void setLabelName(const QString &);
  bool rtpEnabled() const;
  void setRtpEnabled(bool state);
  QHostAddress streamAddress() const;
  void setStreamAddress(const QHostAddress &addr);
  Q_UINT16 streamUdpPort() const;
  void setStreamUdpPort(Q_UINT16 port);
  bool shareable() const;
  void setShareable(bool state);
  int inputGain() const;
  void setInputGain(int lvl);
  int channels() const;
  void setChannels(int chans);
  void clear();

 private:
  int live_slot_number;
  int live_channel_number;
  QString live_primary_name;
  QString live_label_name;
  bool live_rtp_enabled;
  QHostAddress live_stream_address;
  Q_UINT16 live_stream_udp_port;
  bool live_shareable;
  int live_input_gain;
  int live_channels;
};


#endif  // RDLIVEWIRESOURCE_H
