// harlond.h
//
// A Rivendell switcher driver for the Harlond Virtual Mixer
//
//   (C) Copyright 2002-2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: harlond.h,v 1.2.2.2 2012/08/06 00:12:07 cvs Exp $
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

#ifndef HARLOND_H
#define HARLOND_H

#include <vector>

#include <stdint.h>

#include <qsignalmapper.h>
#include <qtimer.h>
#include <qsocket.h>

#include <rd.h>
#include <rdmatrix.h>
#include <rdmacro.h>

#include "switcher.h"

#define HARLOND_RECONNECT_INTERVAL 10000

class Harlond : public Switcher
{
 Q_OBJECT
 public:
  Harlond(RDMatrix *matrix,QObject *parent=0);
  ~Harlond();
  RDMatrix::Type type();
  unsigned gpiQuantity();
  unsigned gpoQuantity();
  bool primaryTtyActive();
  bool secondaryTtyActive();
  void processCommand(RDMacro *cmd);

 private slots:
  void resetTimeoutData(int line);
  void socketConnectedData();
  void socketDisconnectedData();
  void socketReadyReadData();
  void socketErrorData(int err);
  void watchdogTimeoutData();

 private:
  void ProcessResponse(const QString &str);
  bool ProcessSalvo(const QString &str);
  bool ProcessGpo(int line,bool state,int msecs);
  bool SetInputLevel(int input,int db);
  bool ProcessCrosspoint(RDMacro::Command cmd,int input,int output);
  bool TakeCrosspoint(int input,int output);
  bool AddCrosspoint(int input,int output);
  bool RemoveCrosspoint(int input,int output);
  QString GetBussCode(int output);
  QSocket *bt_socket;
  QString bt_recv_buffer;
  QTimer *bt_watchdog_timer;
  QHostAddress bt_ip_address;
  uint16_t bt_tcp_port;
  QString bt_password;
  int bt_matrix;
  int bt_inputs;
  int bt_outputs;
  unsigned bt_start_cart;
  unsigned bt_stop_cart;
  QSignalMapper *bt_reset_mapper;
  std::vector<QTimer *> bt_reset_timers;
  std::vector<bool> bt_reset_states;
};


#endif  // HARLOND_H
