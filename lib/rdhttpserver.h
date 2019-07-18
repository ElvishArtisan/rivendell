// rdhttpserver.h
//
// A container class for simple HTTP server
//
//   Patrick Linstruth <patrick@deltecent.com>
//   (C) Copyright 2019 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDHTTPSERVER_H
#define RDHTTPSERVER_H

#include <QObject>
#include <QApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>

class RDHttpRequest
{
  public:
    QString method;
    QString resource;
    QMap <QString,QString> headers;
    QByteArray body;

};

class RDHttpResponse
{
  public:
    int status;
};

class RDHttpServer : public QObject
{
  Q_OBJECT

  public:
    RDHttpServer();
    ~RDHttpServer();
    bool listen(QHostAddress address,int port);
    void sendResponse(RDHttpResponse &response);

  signals:
    void requestReceived(RDHttpRequest request);

  private slots:
    void newConnectionSlot();
    void connectedSlot();
    void disconnectedSlot();
    void bytesWrittenSlot(qint64 bytes);
    void readyReadSlot();
    void stateChangedSlot(QAbstractSocket::SocketState state);
    void errorSlot(QAbstractSocket::SocketError error);

  private:
    QTcpServer *tcp_server;
    QTcpSocket *tcp_socket;
    bool connected;
    bool have_headers;
    RDHttpRequest request;
    RDHttpResponse response;
};

#endif  // RDHTTPSERVER_H
