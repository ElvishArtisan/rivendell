// rdunixserver.h
//
// UNIX Socket Server
//
//   (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
//
//   This class works much the same way as QTcpServer, only it supports
//   SOCK_STREAM connections via UNIX sockets rather than TCP/IP.
//
//   See the unix(7) man page for a description of the difference between
//   'pathname' and 'abstract' socket addresses.
//
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

#ifndef RDUNIXSERVER_H
#define RDUNIXSERVER_H

#include <qobject.h>
#include <qsocketnotifier.h>
#include <qtcpsocket.h>

class RDUnixServer : public QObject
{
  Q_OBJECT
 public:
  RDUnixServer(QObject *parent=0);
  ~RDUnixServer();
  void close();
  QString errorString() const;
  bool hasPendingConnections() const;
  bool isListening() const;
  bool listenToPathname(const QString &pathname);
  bool listenToAbstract(const QString &addr);
  QTcpSocket *nextPendingConnection();
  int maxPendingConnections() const;
  void setMaxPendingConnections(int num);
  int socketDescriptor() const;
  void setSocketDescriptor(int sock);

 signals:
  void newConnection();

 private slots:
  void newConnectionData(int fd);

 private:
  int unix_socket;
  bool unix_is_listening;
  int unix_max_pending_connections;
  QSocketNotifier *unix_notifier;
  QString unix_error_string;
};


#endif  // RDUNIXSERVER_H
