// rdaudio_port.h
//
// Abstract a Rivendell Audio Port
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdaudio_port.h,v 1.9.8.1 2012/08/03 16:52:39 cvs Exp $
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
   RDAudioPort(QString station,int card,bool create=false);
   QString station() const;
   int card() const;
   RDCae::ClockSource clockSource();
   void setClockSource(RDCae::ClockSource src);
   RDAudioPort::PortType inputPortType(int port);
   void setInputPortType(int port,RDAudioPort::PortType type);
   RDCae::ChannelMode inputPortMode(int port);
   void setInputPortMode(int port,RDCae::ChannelMode mode);
   int inputPortLevel(int port);
   void setInputPortLevel(int port,int level);
   int outputPortLevel(int port);
   void setOutputPortLevel(int port,int level);

  private:
   int GetIntValue(QString field);
   void SetRow(QString param,int value);
   QString port_station;
   int port_card;
};


#endif 
