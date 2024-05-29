//   rdjsonframer.h
//
//   Frame an unsynchronized stream of JSON messages
//
//   (C) Copyright 2024 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDJSONFRAMER_H
#define RDJSONFRAMER_H

#include <QByteArray>
#include <QObject>
#include <QTcpSocket>

class RDJsonFramer : public QObject
{
  Q_OBJECT
 public:
  RDJsonFramer(QTcpSocket *in_sock,QObject *parent);
  RDJsonFramer(QObject *parent);
  ~RDJsonFramer();
  QByteArray currentDocument() const;
  
 public slots:
  void write(const QByteArray &data);
  void reset();

 signals:
  void documentReceived(const QByteArray &jdoc);
  void documentReset();

 private slots:
  void readyReadData();

 private:
  QByteArray d_current_document;
  QTcpSocket *d_socket;
};


#endif  // RDJSONFRAMER_H
