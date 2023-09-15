// connection.h
//
// UDP connection context for CAE protocol commands
//
//   (C) Copyright 2023 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdint.h>

#include <QHostAddress>
#include <QObject>
#include <QTimer>

#include "session.h"

class Connection : public QObject
{
  Q_OBJECT;
 public:
  Connection(const SessionId &sid,QObject *parent);
  ~Connection();
  SessionId sessionId() const;
  QString dump() const;
  bool operator!=(const Connection &other) const;
  bool operator<(const Connection &other) const;

 signals:
  void connectionExpired(const SessionId &sid);

 public slots:
  void setTimeout(int msecs);
  void touch();

 private slots:
  void timerData();

 private:
  SessionId d_session_id;
  QTimer *d_timer;
  int d_interval;
};


#endif  // SESSION_H
