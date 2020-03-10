// modbus.h
//
// A Rivendell switcher driver for Modbus TCP
//
//   (C) Copyright 2017-2020 Fred Gleason <fredg@paravelsystems.com>
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

#include <queue>
#include <vector>

#include <qsignalmapper.h>
#include <qtcpsocket.h>
#include <qtimer.h>

#include <rd.h>
#include <rdmatrix.h>
#include <rdmacro.h>

#include <switcher.h>

#define MODBUS_POLL_INTERVAL 100
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
  void errorData(QAbstractSocket::SocketError err);
  void pollInputs();
  void resetStateData(int line);
  void watchdogData();

 private:
  void ProcessInputByte(char byte,int base);
  void SetCoil(int line,bool state);
  bool InputState(int line) const;
  int modbus_istate;
  int modbus_input_bytes;
  std::vector<char> modbus_input_states;
  QTcpSocket *modbus_socket;
  QTimer *modbus_poll_timer;
  QTimer *modbus_watchdog_timer;
  bool modbus_watchdog_active;
  QHostAddress modbus_ip_address;
  uint16_t modbus_ip_port;
  int modbus_gpis;
  int modbus_gpos;
  std::queue<int> modbus_coil_lines;
  std::queue<bool> modbus_coil_states;
  bool modbus_busy;
  QSignalMapper *modbus_reset_mapper;
  std::vector<QTimer *> modbus_reset_timers;
  std::vector<bool> modbus_reset_states;
};


#endif  // MODBUS_H
