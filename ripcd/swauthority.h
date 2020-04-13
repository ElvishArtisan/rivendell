// swauthority.h
//
// A Rivendell switcher driver for systems using Software Authority Protocol
//
//   (C) Copyright 2002-2020 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef SWAUTHORITY_H
#define SWAUTHORITY_H

#include <map>
#include <vector>

#include <qhostaddress.h>
#include <qtcpsocket.h>
#include <qtimer.h>

#include <rd.h>
#include <rdmatrix.h>
#include <rdmacro.h>

#include <switcher.h>

#define SWAUTHORITY_RECONNECT_INTERVAL 10000
#define SWAUTHORITY_MAX_LENGTH 256

class SoftwareAuthority : public Switcher
{
 Q_OBJECT
 public:
  SoftwareAuthority(RDMatrix *matrix,QObject *parent=0);
  RDMatrix::Type type();
  unsigned gpiQuantity();
  unsigned gpoQuantity();
  bool primaryTtyActive();
  bool secondaryTtyActive();
  void processCommand(RDMacro *cmd);

 private slots:
  void ipConnect();
  void connectedData();
  void connectionClosedData();
  void readyReadData();
  void errorData(QAbstractSocket::SocketError err);

 private:
  void SendCommand(const char *str);
  void DispatchCommand();
  void ExecuteMacroCart(unsigned cartnum);
  QString PrettifyCommand(const char *cmd) const;
  QString BundleString(int offset,bool state);
  QTcpSocket *swa_socket;
  char swa_buffer[SWAUTHORITY_MAX_LENGTH];
  unsigned swa_ptr;
  QHostAddress swa_ipaddress;
  QString swa_username;
  QString swa_password;
  int swa_matrix;
  int swa_card;
  int swa_ipport;
  int swa_inputs;
  int swa_outputs;
  int swa_gpis;
  int swa_gpos;
  QTimer *swa_reconnect_timer;
  unsigned swa_start_cart;
  unsigned swa_stop_cart;
  int swa_istate;
  bool swa_is_gpio;
  RDMatrix::PortType swa_porttype;
  std::map<int,QString> swa_gpi_states;
  std::map<int,QString> swa_gpo_states;
};


#endif  // SWAUTHORITY_H
