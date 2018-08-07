// rdmulticaster.h
//
// Multi-interface multicast transciever
//
//   (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
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

#ifndef RDMULTICASTER_H
#define RDMULTICASTER_H

#include <stdint.h>

#include <vector>

#include <qhostaddress.h>
#include <qobject.h>
#include <q3socketdevice.h>
#include <qsocketnotifier.h>

class RDMulticaster : public QObject
{
  Q_OBJECT
 public:
  RDMulticaster(QObject *parent=0);
  bool bind(uint16_t port);
  void enableLoopback(bool state);
  void subscribe(const QHostAddress &addr);
  void unsubscribe(const QHostAddress &addr);

 signals:
  void received(const QString &msg,const QHostAddress &src_addr);

 public slots:
  void send(const QString &msg,const QHostAddress &m_addr,uint16_t port);

 private slots:
  void activatedData(int sock);

 private:
  void GetInterfaces();
  Q3SocketDevice *multi_socket;
  QSocketNotifier *multi_notifier;
  std::vector<QHostAddress> multi_iface_addresses;
};


#endif  // RDMULTICASTER_H
