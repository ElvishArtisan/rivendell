// livewire_lwrpaudio.h
//
// A Rivendell LWRP audio switcher driver for LiveWire networks.
//
//   (C) Copyright 2002-2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: livewire_lwrpaudio.h,v 1.1.2.1 2013/11/17 04:27:06 cvs Exp $
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

#ifndef LIVEWIRE_LWRPAUDIO_H
#define LIVEWIRE_LWRPAUDIO_H

#include <vector>

#include <qsocket.h>
#include <qhostaddress.h>
#include <qtimer.h>

#include <rd.h>
#include <rdmatrix.h>
#include <rdmacro.h>
#include <rdlivewire.h>
#include <rdoneshot.h>

#include <switcher.h>

class LiveWireLwrpAudio : public Switcher
{
 Q_OBJECT
 public:
  LiveWireLwrpAudio(RDMatrix *matrix,QObject *parent=0,const char *name=0);
  ~LiveWireLwrpAudio();
  RDMatrix::Type type();
  unsigned gpiQuantity();
  unsigned gpoQuantity();
  bool primaryTtyActive();
  bool secondaryTtyActive();
  void processCommand(RDMacro *cmd);

 private slots:
  void nodeConnectedData(unsigned id);
  void sourceChangedData(unsigned id,RDLiveWireSource *src);
  void destinationChangedData(unsigned id,RDLiveWireDestination *dst);
  void watchdogStateChangedData(unsigned id,const QString &msg);

 private:
  QString livewire_stationname;
  int livewire_matrix;
  std::vector<RDLiveWire *> livewire_nodes;
};


#endif  // LIVEWIRE_LWRPAUDIO_H
