// rdaudio_port.h
//
// Abstract a Rivendell Audio Port
//
//   (C) Copyright 2002-2003,2016-2018 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDAUDIO_PORT_H
#define RDAUDIO_PORT_H

#include <qsqldatabase.h>
#include <rdcae.h>

class RDAudioPort
{
 public:
  enum PortType {Analog=0,AesEbu=1,SpDiff=2};
  RDAudioPort(QString station,int card);
  QString station() const;
  int card() const;
  RDCae::ClockSource clockSource();
  void setClockSource(RDCae::ClockSource src);
  QString inputPortLabel(int port) const;
  void setInputPortLabel(int port,const QString &str);
  RDAudioPort::PortType inputPortType(int port);
  void setInputPortType(int port,RDAudioPort::PortType type);
  RDCae::ChannelMode inputPortMode(int port);
  void setInputPortMode(int port,RDCae::ChannelMode mode);
  int inputPortLevel(int port);
  void setInputPortLevel(int port,int level);
  QString outputPortLabel(int port) const;
  void setOutputPortLabel(int port,const QString &str);
  int outputPortLevel(int port);
  void setOutputPortLevel(int port,int level);

 private:
  QString port_station;
  int port_card;
  QString audio_input_port_labels[RD_MAX_PORTS];
  int audio_input_port_level[RD_MAX_PORTS];
  RDAudioPort::PortType audio_input_port_type[RD_MAX_PORTS];
  RDCae::ChannelMode audio_input_port_mode[RD_MAX_PORTS];
  int audio_output_port_level[RD_MAX_PORTS];
  QString audio_output_port_labels[RD_MAX_PORTS];
};


#endif  // RDAUDIO_PORT_H
