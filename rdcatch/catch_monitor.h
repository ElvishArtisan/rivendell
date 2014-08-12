// catch_monitor.h
//
// A container class for RDCatch deck monitors and assorted metadata.
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: catch_monitor.h,v 1.5 2010/07/29 19:32:36 cvs Exp $
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

#ifndef CATCH_MONITOR_H
#define CATCH_MONITOR_H

#include <deckmon.h>


class CatchMonitor
{
 public:
  CatchMonitor();
  CatchMonitor(DeckMon *mon,int serial,int channel);
  DeckMon *deckMon() const;
  void setDeckMon(DeckMon *mon);
  int serialNumber() const;
  void setSerialNumber(int num);
  int channelNumber() const;
  void setChannelNumber(int num);

 private:
  DeckMon *catch_deck_mon;
  int catch_serial_number;
  int catch_channel_number;
};


#endif  // CATCH_MONITOR_H

