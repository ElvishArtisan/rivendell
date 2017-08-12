// wheatnet_lio.h
//
// A Rivendell switcher driver for Wheatnet LIO devices
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

#ifndef WHEATNET_LIO_H
#define WHEATNET_LIO_H

#include <vector>

#include <qsignalmapper.h>
#include <qsocket.h>
#include <qtimer.h>

#include <rd.h>
#include <rdmatrix.h>
#include <rdmacro.h>

#include <switcher.h>

#define WHEATNET_LIO_POLL_INTERVAL 100
#define WHEATNET_LIO_WATCHDOG_INTERVAL 1000

class WheatnetLio : public Switcher
{
 Q_OBJECT
 public:
  WheatnetLio(RDMatrix *matrix,QObject *parent=0);
  ~WheatnetLio();
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
  void pollInputs();
  void resetStateData(int line);
  void watchdogData();

 private:
  void CheckLineEntry(int line);
  void ProcessSys(const QString &cmd);
  void ProcessLio(int chan,QString &cmd);
  void ProcessCommand(const QString &cmd);
  void SendCommand(const QString &cmd);
  QSocket *lio_socket;
  QTimer *lio_poll_timer;
  QTimer *lio_watchdog_timer;
  bool lio_watchdog_active;
  QHostAddress lio_ip_address;
  uint16_t lio_ip_port;
  int lio_card;
  int lio_gpios;
  QString lio_accum;
  std::vector<bool> lio_gpi_states;
  QSignalMapper *lio_reset_mapper;
  std::vector<QTimer *> lio_reset_timers;
  std::vector<bool> lio_reset_states;
};


#endif  // WHEATNET_LIO_H
