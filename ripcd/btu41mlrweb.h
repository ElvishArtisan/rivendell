// btu41mlrweb.h
//
// Rivendell switcher driver for the BroadcastTools Universal 4.1 MLR>>Web
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

#ifndef BTU41MLRWEB_H
#define BTU41MLRWEB_H

#include <vector>

#include <qsignalmapper.h>
#include <q3socket.h>
#include <qtimer.h>

#include <rd.h>
#include <rdmatrix.h>
#include <rdmacro.h>

#include <switcher.h>

#define BTU41MLRWEB_INPUT_QUAN 4
#define BTU41MLRWEB_OUTPUT_QUAN 1
#define BTU41MLRWEB_GPI_QUAN 5
#define BTU41MLRWEB_GPO_QUAN 0
#define BTU41MLRWEB_KEEPALIVE_INTERVAL 2000
#define BTU41MLRWEB_WATCHDOG_INTERVAL 5000

class BtU41MlrWeb : public Switcher
{
 Q_OBJECT
 public:
  BtU41MlrWeb(RDMatrix *matrix,QObject *parent=0);
  ~BtU41MlrWeb();
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
  void keepaliveData();
  void watchdogData();

 private:
  void ProcessCommand(const QString &cmd);
  void SendCommand(const QString &cmd);
  Q3Socket *bt_socket;
  QTimer *bt_watchdog_timer;
  bool bt_watchdog_active;
  QHostAddress bt_ip_address;
  uint16_t bt_ip_port;
  QString bt_accum;
  bool bt_gpi_states[BTU41MLRWEB_GPI_QUAN];
  QTimer *bt_keepalive_timer;
};


#endif  // BTU41MLRWEB_H
