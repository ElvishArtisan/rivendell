// caedriver.cpp
//
// Abstract base class for caed(8) audio driver implementations.
//
//   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
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

#include "caedriver.h"

CaeDriver::CaeDriver(RDStation::AudioDriver type,QObject *parent)
  : QObject(parent)
{
  d_driver_type=type;
}


RDStation::AudioDriver CaeDriver::driverType() const
{
  return d_driver_type;
}


bool CaeDriver::hasCard(int cardnum) const
{
  return d_cards.contains(cardnum);
}


void CaeDriver::statePlayUpdate(int card,int stream,int state)
{
  emit playStateChanged(card,stream,state);
}


void CaeDriver::stateRecordUpdate(int card,int stream,int state)
{
  emit recordStateChanged(card,stream,state);
}


void CaeDriver::addCard(unsigned cardnum)
{
  if(d_cards.contains(cardnum)) {
    rda->syslog(LOG_WARNING,
		"%s driver attempted to register card %u multiple times",
		RDStation::audioDriverText(d_driver_type).toUtf8().constData(),
		cardnum);
  }
  else {
    d_cards.push_back(cardnum);
  }
}
