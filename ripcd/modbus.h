// modbus.h
//
// A Rivendell switcher driver for Modbus TCP
//
//   (C) Copyright 2017 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef MODBUS_H
#define MODBUS_H

#include <vector>

#include <qsocket.h>
#include <qtimer.h>

#include <rd.h>
#include <rdmatrix.h>
#include <rdmacro.h>

#include <switcher.h>

#define MODBUS_WATCHDOG_INTERVAL 1000

class Modbus : public Switcher
{
 Q_OBJECT
 public:
  Modbus(RDMatrix *matrix,QObject *parent=0);
  ~Modbus();
  RDMatrix::Type type();
  unsigned gpiQuantity();
  unsigned gpoQuantity();
  bool primaryTtyActive();
  bool secondaryTtyActive();
  void processCommand(RDMacro *cmd);

 private slots:
  void connectedData();
  void readyReadData();
  void errorData(int err);
  void watchdogData();

 private:
  void ProcessInputByte(char byte,int base);
  void PollInputs();
  int modbus_istate;
  int modbus_input_bytes;
  std::vector<char> modbus_input_states;
  QSocket *modbus_socket;
  QTimer *modbus_watchdog_timer;
  bool modbus_watchdog_active;
  QHostAddress modbus_ip_address;
  uint16_t modbus_ip_port;
  int modbus_gpis;
  int modbus_gpos;
};


#endif  // MODBUS_H
