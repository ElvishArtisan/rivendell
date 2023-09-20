// connection.cpp
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

#include <rdapplication.h>

#include "connection.h"

Connection::Connection(const SessionId &sid,QObject *parent)
  : QObject(parent)
{
  d_session_id=sid;
  d_interval=0;
  d_meter_port=0;

  d_timer=new QTimer(this);
  d_timer->setSingleShot(true);
  connect(d_timer,SIGNAL(timeout()),this,SLOT(timerData()));
}


Connection::~Connection()
{
  delete d_timer;
}


SessionId Connection::sessionId() const
{
  return d_session_id;
}


uint16_t Connection::meterPort() const
{
  return d_meter_port;
}


void Connection::setMeterPort(uint16_t udp_port)
{
  printf("setMeterPort(%s::setMeterPort(%u)\n",
	 dump().toUtf8().constData(),0xffff&udp_port);
  d_meter_port=udp_port;
}


QString Connection::dump() const
{
  return d_session_id.dump();
}


bool Connection::operator!=(const Connection &other) const
{
  return (other.d_session_id.address()!=d_session_id.address())||
    (other.d_session_id.port()!=d_session_id.port());
}


bool Connection::operator<(const Connection &other) const
{
  return other.d_session_id<d_session_id;
}


void Connection::setTimeout(int msecs)
{
  d_interval=msecs;
  touch();
}


void Connection::touch()
{
  d_timer->stop();
  d_timer->start(d_interval);
}


void Connection::timerData()
{
  rda->syslog(LOG_WARNING,"connection %s timed out",
	      d_session_id.dump().toUtf8().constData());
  emit connectionExpired(d_session_id);
}
