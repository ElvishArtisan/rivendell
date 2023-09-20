// session.h
//
// Persistent context for CAE protocol commands
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

#ifndef SESSION_H
#define SESSION_H

#include <stdint.h>

#include <QHostAddress>

class SessionId
{
 public:
  SessionId(const QHostAddress &src_addr,uint16_t src_port,int serial=0);
  SessionId();
  QHostAddress address() const;
  uint16_t port() const;
  int serialNumber() const;
  void setSerialNumber(int serial);
  QString dump() const;
  bool belongsTo(const SessionId &other) const;
  SessionId normalized() const;
  bool operator!=(const SessionId &other) const;
  bool operator<(const SessionId &other) const;

 private:
  QHostAddress d_address;
  uint16_t d_port;
  int d_serial_number;
};




class Session
{
 public:
  Session(const QHostAddress &addr,uint16_t port,int serial=0);
  Session(const SessionId &sid);
  SessionId sessionId() const;
  int cardNumber() const;
  void setCardNumber(int cardnum);
  int portNumber() const;
  void setPortNumber(int portnum);
  int streamNumber() const;
  void setStreamNumber(int streamnum);
  //  uint16_t meterPort() const;
  //  void setMeterPort(uint16_t port);
  int startPosition() const;
  void setStartPosition(int pos);
  int endPosition() const;
  void setEndPosition(int pos);
  int speed() const;
  void setSpeed(int speed);
  //  bool metersEnabled();
  //  void setMetersEnabled(bool state);

 private:
  SessionId d_session_id;
  int d_card_number;
  int d_port_number;
  int d_stream_number;
  int d_start_position;
  int d_end_position;
  int d_speed;
  //  uint16_t d_meter_port;
  //  bool d_meters_enabled;
};


#endif  // SESSION_H
