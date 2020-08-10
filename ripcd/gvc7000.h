// gvc7000.h
//
// A Rivendell switcher driver for Grass Valley Series 7000 Protocol
//
//   (C) Copyright 2002-2019 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef GVC7000_H
#define GVC7000_H

#include <qhostaddress.h>
#include <qtcpsocket.h>
#include <qtimer.h>

#include <rd.h>
#include <rddatapacer.h>
#include <rdmatrix.h>
#include <rdmacro.h>

#include <switcher.h>

#define GVC7000_RECONNECT_INTERVAL 10000
#define GVC7000_KEEPALIVE_INTERVAL 5000

class Gvc7000 : public Switcher
{
 Q_OBJECT
 public:
  Gvc7000(RDMatrix *matrix,QObject *parent=0);
  RDMatrix::Type type();
  unsigned gpiQuantity();
  unsigned gpoQuantity();
  bool primaryTtyActive();
  bool secondaryTtyActive();
  void processCommand(RDMacro *cmd);

 private slots:
  void ipConnect();
  void keepaliveData();
  void connectedData();
  void disconnectedData();
  void errorData(QAbstractSocket::SocketError err);
  void sendCommandData(const QByteArray &data);

 private:
  QString ToSeries7000Native(const QString &str) const;
  QTcpSocket *gvc_socket;
  RDDataPacer *gvc_pacer;
  QHostAddress gvc_ipaddress;
  int gvc_matrix;
  int gvc_ipport;
  int gvc_inputs;
  int gvc_outputs;
  QTimer *gvc_keepalive_timer;
  QTimer *gvc_reconnect_timer;
  int gvc_input_line;
  int gvc_output_line;
};


#endif  // GVC7000_H
