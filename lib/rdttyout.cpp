// rdttyout.cpp
//
// Output a string on a Rivendell TTY
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdttyout.cpp,v 1.10 2010/07/29 19:32:34 cvs Exp $
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

#include <rdtty.h>
#include <rdttyout.h>


bool RDTtyOut(const QString &station,unsigned port_id,const QString &str)
{
  RDTty *tty_entry=new RDTty(station,port_id);
  if(!tty_entry->active()) {
    delete tty_entry;
    return false;
  }
  RDTTYDevice *tty_device=new RDTTYDevice();
  tty_device->setName(tty_entry->port());
  tty_device->setSpeed(tty_entry->baudRate());
  tty_device->setWordLength(tty_entry->dataBits());
  tty_device->setParity(tty_entry->parity());
  if(!tty_device->open(IO_Raw|IO_WriteOnly)) {
    delete tty_device;
    delete tty_entry;
    return false;
  }
  tty_device->writeBlock((const char *)str,strlen((const char *)str));
  switch(tty_entry->termination()) {
      case RDTty::CrTerm:
	tty_device->putch(13);
	break;

      case RDTty::LfTerm:
	tty_device->putch(10);
	break;

      case RDTty::CrLfTerm:
	tty_device->putch(13);
	tty_device->putch(10);
	break;

      default:
	break;
  }
  tty_device->close();
  delete tty_device;
  delete tty_entry;
  return true;
}
