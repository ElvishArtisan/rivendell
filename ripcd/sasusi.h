// sasusi.h
//
// A Rivendell switcher driver for the SAS User Serial Interface Protocol
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: sasusi.h,v 1.16 2011/05/26 21:20:37 cvs Exp $
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

#ifndef SASUSI_H
#define SASUSI_H

#include <vector>

#include <qsocket.h>
#include <qhostaddress.h>
#include <qtimer.h>

#include <rd.h>
#include <rdmatrix.h>
#include <rdmacro.h>
#include <rdtty.h>

#include <switcher.h>

#define SASUSI_RECONNECT_INTERVAL 10000
#define SASUSI_MAX_LENGTH 256

class SasUsi : public Switcher
{
 Q_OBJECT
 public:
  SasUsi(RDMatrix *matrix,QObject *parent=0,const char *name=0);
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
  void errorData(int err);

 private:
  void SendCommand(char *str);
  void DispatchCommand();
  void ExecuteMacroCart(unsigned cartnum);
  QString PrettifyCommand(const char *cmd) const;
  RDTTYDevice *sas_device;
  QSocket *sas_socket;
  char sas_buffer[SASUSI_MAX_LENGTH];
  unsigned sas_ptr;
  QHostAddress sas_ipaddress;
  int sas_matrix;
  int sas_ipport;
  int sas_inputs;
  int sas_outputs;
  int sas_gpis;
  int sas_gpos;
  QTimer *sas_reconnect_timer;
  unsigned sas_start_cart;
  unsigned sas_stop_cart;
  RDMatrix::PortType sas_porttype;
  std::vector<int> sas_console_numbers;
  std::vector<int> sas_source_numbers;
  std::vector<int> sas_relay_numbers;
};


#endif  // SASUSI_H
