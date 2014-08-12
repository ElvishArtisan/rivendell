// livewire_lwrpgpio.h
//
// A Rivendell LWRP GPIO driver for LiveWire networks.
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: livewire_lwrpgpio.h,v 1.1.2.1 2013/11/17 02:03:21 cvs Exp $
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

#ifndef LIVEWIRE_LWRPGPIO_H
#define LIVEWIRE_LWRPGPIO_H

#include <stdint.h>
#include <sys/socket.h>

#include <vector>

#include <qsocket.h>
#include <qhostaddress.h>
#include <qsignalmapper.h>
#include <qtimer.h>
#include <qsocketnotifier.h>

#include <rd.h>
#include <rdmatrix.h>
#include <rdmacro.h>
#include <rdlivewire.h>
#include <rdoneshot.h>

#include <switcher.h>

class LiveWireLwrpGpio : public Switcher
{
 Q_OBJECT
 public:
  LiveWireLwrpGpio(RDMatrix *matrix,QObject *parent=0,const char *name=0);
  ~LiveWireLwrpGpio();
  RDMatrix::Type type();
  unsigned gpiQuantity();
  unsigned gpoQuantity();
  bool primaryTtyActive();
  bool secondaryTtyActive();
  void processCommand(RDMacro *cmd);

 private slots:
  void gpiChangedData(unsigned id,unsigned slot,unsigned line,bool state);
  void gpoChangedData(unsigned id,unsigned slot,unsigned line,bool state);
  void connectedData(unsigned id);
  void watchdogStateChangedData(unsigned id,const QString &msg);

 private:
  RDLiveWire *gpio_livewire;
  bool gpio_is_virtual;
  unsigned gpio_gpi_limit;
  unsigned gpio_gpo_limit;
};


#endif  // LIVEWIRE_LWRPGPIO_H
