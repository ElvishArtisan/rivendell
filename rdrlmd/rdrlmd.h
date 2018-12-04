// rdrlmd.h
//
// Rivendell RLM Consolidation Server
//
//   (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDRLMD_H
#define RDRLMD_H

#include <qmap.h>
#include <qobject.h>
#include <qsignalmapper.h>
#include <qtcpserver.h>
#include <qtcpsocket.h>

#include <rdunixserver.h>

#define RDRLMD_USAGE "\n\n"

class MetadataSource
{
 public:
  MetadataSource(QTcpSocket *sock);
  QByteArray buffer() const;
  bool appendBuffer(const QByteArray &data);
  bool isCommitted() const;
  QTcpSocket *socket() const;

 private:
  QByteArray meta_buffer;
  bool meta_committed;
  QTcpSocket *meta_socket;
};




class MainObject : public QObject
{
  Q_OBJECT
 public:
  MainObject(QObject *parent=0);

 private slots:
  void newClientConnectionData();
  void clientDisconnected(int id);
  void newSourceConnectionData();
  void sourceReadyReadData(int id);
  void sourceDisconnected(int id);

 private:
  void SendState(int id);
  QSignalMapper *rlm_client_disconnect_mapper;
  QTcpServer *rlm_client_server;
  QMap<int,QTcpSocket *> rlm_client_sockets;

  QSignalMapper *rlm_source_ready_mapper;
  QSignalMapper *rlm_source_disconnect_mapper;
  RDUnixServer *rlm_source_server;
  QMap<int,MetadataSource *> rlm_sources;
};


#endif  // RDRLMD_H
