// catch_monitor.cpp
//
// A container class for RDCatch deck monitors and assorted metadata.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: catch_monitor.cpp,v 1.5 2010/07/29 19:32:36 cvs Exp $
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

#include <catch_monitor.h>


CatchMonitor::CatchMonitor()
{
  catch_deck_mon=NULL;
  catch_serial_number=-1;
  catch_channel_number=-1;
}


CatchMonitor::CatchMonitor(DeckMon *mon,int serial,int channel)
{
  catch_deck_mon=mon;
  catch_serial_number=serial;
  catch_channel_number=channel;
}


DeckMon *CatchMonitor::deckMon() const
{
  return catch_deck_mon;
}


void CatchMonitor::setDeckMon(DeckMon *mon)
{
  catch_deck_mon=mon;
}


int CatchMonitor::serialNumber() const
{
  return catch_serial_number;
}


void CatchMonitor::setSerialNumber(int num)
{
  catch_serial_number=num;
}


int CatchMonitor::channelNumber() const
{
  return catch_channel_number;
}


void CatchMonitor::setChannelNumber(int num)
{
  catch_channel_number=num;
}
