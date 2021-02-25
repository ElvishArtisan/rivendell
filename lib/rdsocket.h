//   rdsocket.h
//
//   A QSocket object with connection-ID.
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDSOCKET_H
#define RDSOCKET_H

#include <QTcpSocket>

class RDSocket : public QTcpSocket
{
  Q_OBJECT
 public:
  RDSocket(int id,QObject *parent=0);

 signals:
  void hostFoundID(int id);
  void connectedID(int id);
  void connectionClosedID(int id);
  void readyReadID(int id);
  void errorID(QAbstractSocket::SocketError error,int id);
  
 private slots:
  void hostFoundData();
  void connectedData();
  void connectionClosedData();
  void readyReadData();
  void errorData(QAbstractSocket::SocketError error);
  
 private:
  int id_num;
};


#endif  // RDSOCKET_H
