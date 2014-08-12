// rdmixer.cpp
//
// Mixer Functions for Rivendell.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdmixer.cpp,v 1.13.8.2 2012/08/03 16:52:39 cvs Exp $
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

#include <rdaudio_port.h>
#include <rd.h>

#include <rdmixer.h>


void RDSetMixerPorts(QString station,RDCae *cae)
{
  for(int i=0;i<RD_MAX_CARDS;i++) {
    RDAudioPort *port=new RDAudioPort(station,i);
    cae->setClockSource(i,port->clockSource());
    for(int j=0;j<RD_MAX_PORTS;j++) {
      if(port->inputPortType(j)==RDAudioPort::Analog) {
	cae->setInputType(i,j,RDCae::Analog);
      }
      else {
	cae->setInputType(i,j,RDCae::AesEbu);
      }
      cae->setInputLevel(i,j,RD_BASE_ANALOG+port->inputPortLevel(j));
      cae->setOutputLevel(i,j,RD_BASE_ANALOG+port->outputPortLevel(j));
      cae->setInputMode(i,j,port->inputPortMode(j));
    }
    delete port;
  }
}


void RDSetMixerOutputPort(RDCae *cae,int card,int stream,int port)
{
  for(int i=0;i<RD_MAX_PORTS;i++) {
    if(i!=port) {
      cae->setOutputVolume(card,stream,i,-10000);
    }
  }
  cae->setOutputVolume(card,stream,port,0);
}
