// wheatnet_slio.h
//
// A Rivendell switcher driver for Wheatnet SLIO devices
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

#ifndef WHEATNET_SLIO_H
#define WHEATNET_SLIO_H

#include <vector>

#include <qsignalmapper.h>
#include <qsocket.h>
#include <qtimer.h>

#include <rd.h>
#include <rdmatrix.h>
#include <rdmacro.h>

#include <switcher.h>

#define WHEATNET_SLIO_POLL_INTERVAL 1000
#define WHEATNET_SLIO_WATCHDOG_INTERVAL 5000

class WheatnetSlio : public Switcher
{
 Q_OBJECT
 public:
  WheatnetSlio(RDMatrix *matrix,QObject *parent=0);
  ~WheatnetSlio();
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
  void resetStateData(int line);
  void pollData();
  void watchdogData();

 private:
  void CheckLineEntry(int line);
  void ProcessSys(const QString &cmd);
  void ProcessSlioevent(int chan,QString &cmd);
  void ProcessCommand(const QString &cmd);
  void SendCommand(const QString &cmd);
  QSocket *slio_socket;
  QTimer *slio_watchdog_timer;
  bool slio_watchdog_active;
  QHostAddress slio_ip_address;
  uint16_t slio_ip_port;
  int slio_gpios;
  QString slio_accum;
  std::vector<bool> slio_gpi_states;
  QSignalMapper *slio_reset_mapper;
  std::vector<QTimer *> slio_reset_timers;
  std::vector<bool> slio_reset_states;
  QTimer *slio_poll_timer;
};


#endif  // WHEATNET_SLIO_H
