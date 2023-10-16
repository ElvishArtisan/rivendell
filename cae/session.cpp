// session.cpp
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

#include <rd.h>

#include "session.h"

SessionId::SessionId(const QHostAddress &src_addr,uint16_t src_port,int serial)
{
  d_address=src_addr;
  d_port=src_port;
  d_serial_number=serial;
}


SessionId::SessionId()
{
  d_serial_number=0;
}


QHostAddress SessionId::address() const
{
  return d_address;
}


uint16_t SessionId::port() const
{
  return d_port;
}


int SessionId::serialNumber() const
{
  return d_serial_number;
}


void SessionId::setSerialNumber(int serial)
{
  d_serial_number=serial;
}


QString SessionId::dump() const
{
  return QString::asprintf("%s:%d:%d",d_address.toString().toUtf8().constData(),
			   0xFFFF&d_port,d_serial_number);
}


bool SessionId::belongsTo(const SessionId &other) const
{
  return ((other.d_address==d_address)&&(other.d_port==d_port));
}


SessionId SessionId::normalized() const
{
  return SessionId(d_address,d_port);
}


bool SessionId::operator!=(const SessionId &other) const
{
  return (other.d_address!=d_address)||(other.d_port!=d_port)||
    (other.d_serial_number!=d_serial_number);
}


bool SessionId::operator<(const SessionId &other) const
{
  if(other.d_address.toIPv4Address()!=d_address.toIPv4Address()) {
    return other.d_address.toIPv4Address()<d_address.toIPv4Address();
  }
  if(other.d_port!=d_port) {
    return other.d_port<d_port;
  }
  return other.d_serial_number<d_serial_number;
}




Session::Session(const QHostAddress &addr,uint16_t port,int serial)
{
  d_session_id=SessionId(addr,port,serial);

  d_card_number=-1;
  d_port_number=-1;
  d_stream_number=-1;
  d_start_position=-1;
  d_speed=RD_TIMESCALE_DIVISOR;
}


Session::Session(const SessionId &sid)
{
  d_session_id=sid;

  d_card_number=-1;
  d_port_number=-1;
  d_stream_number=-1;
  d_start_position=-1;
  d_speed=RD_TIMESCALE_DIVISOR;
}


SessionId Session::sessionId() const
{
  return d_session_id;
}


int Session::cardNumber() const
{
  return d_card_number;
}


void Session::setCardNumber(int cardnum)
{
  d_card_number=cardnum;
}


int Session::portNumber() const
{
  return d_port_number;
}


void Session::setPortNumber(int portnum)
{
  d_port_number=portnum;
}


int Session::streamNumber() const
{
  return d_stream_number;
}


void Session::setStreamNumber(int streamnum)
{
  d_stream_number=streamnum;
}


int Session::startPosition() const
{
  return d_start_position;
}


void Session::setStartPosition(int pos)
{
  d_start_position=pos;
}


int Session::speed() const
{
  return d_speed;
}


void Session::setSpeed(int speed)
{
  d_speed=speed;
}
