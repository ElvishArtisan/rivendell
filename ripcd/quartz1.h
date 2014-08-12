// quartz1.h
//
// A Rivendell switcher driver for the Quartz Type 1 Switcher Protocol
//
//   (C) Copyright 2002-2004,2008 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: quartz1.h,v 1.8 2010/08/03 23:39:26 cvs Exp $
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

#ifndef QUARTZ1_H
#define QUARTZ1_H

#include <qsocket.h>
#include <qhostaddress.h>
#include <qtimer.h>

#include <rd.h>
#include <rdmatrix.h>
#include <rdmacro.h>
#include <rdtty.h>

#include <switcher.h>

#define QUARTZ1_RECONNECT_INTERVAL 10000
#define QUARTZ1_MAX_LENGTH 256

class Quartz1 : public Switcher
{
 Q_OBJECT
 public:
  Quartz1(RDMatrix *matrix,QObject *parent=0,const char *name=0);
  RDMatrix::Type type();
  unsigned gpiQuantity();
  unsigned gpoQuantity();
  bool primaryTtyActive();
  bool secondaryTtyActive();
  void processCommand(RDMacro *cmd);

 private slots:
  void ipConnect(int conn);
  void connectedData(int conn);
  void connectionClosedData(int conn);
  void errorData(int conn,int err);
  void error0Data(int err);
  void error1Data(int err);

 private:
  void SendCommand(const char *str);
  RDTTYDevice *sas_device[2];
  QSocket *sas_socket[2];
  char sas_buffer[2][QUARTZ1_MAX_LENGTH];
  unsigned sas_ptr[2];
  QHostAddress sas_ipaddress[2];
  int sas_matrix;
  char sas_layer;
  int sas_port[2];
  int sas_ipport[2];
  int sas_inputs;
  int sas_outputs;
  QTimer *sas_reconnect_timer[2];
  RDMatrix::PortType sas_porttype[2];
  int sas_input_line[2];
  int sas_output_line[2];
};


#endif  // QUARTZ1_H
