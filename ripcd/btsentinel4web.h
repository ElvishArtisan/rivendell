// btsentinel4web.h
//
// Rivendell switcher driver for the BroadcastTools Sentinel4Web AES switcher
//
//   (C) Copyright 2002-2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: btsentinel4web.h,v 1.1.2.1 2014/02/17 02:19:03 cvs Exp $
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

#ifndef BTSENTINEL4WEB_H
#define BTSENTINEL4WEB_H

#include <stdint.h>

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

#define BTSENTINEL4WEB_INPUTS 4
#define BTSENTINEL4WEB_OUTPUTS 1
#define BTSENTINEL4WEB_WATCHDOG_INTERVAL 10000

class BtSentinel4Web : public Switcher
{
 Q_OBJECT
 public:
  BtSentinel4Web(RDMatrix *matrix,QObject *parent=0,const char *name=0);
  ~BtSentinel4Web();
  RDMatrix::Type type();
  unsigned gpiQuantity();
  unsigned gpoQuantity();
  bool primaryTtyActive();
  bool secondaryTtyActive();
  void processCommand(RDMacro *cmd);

 private slots:
  void connectedData();
  void errorData(int err);
  void readyReadData();
  void watchdogData();
  void watchdogResetData();

 private:
  QSocket *bt_socket;
  QHostAddress bt_address;
  uint16_t bt_port;
  QTimer *bt_watchdog_timer;
  QTimer *bt_watchdog_reset_timer;
};


#endif  // BTSENTINEL4WEB_H
