// playsession.cpp
//
// Playout session class for caed(8)
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

#include "playsession.h"

PlaySession::PlaySession(int sock,unsigned serial,unsigned cardnum,
			 unsigned portnum,unsigned streamnum)
{
  d_socket_descriptor=sock;
  d_serial_number=serial;
  d_card_number=cardnum;
  d_port_number=portnum;
  d_stream_number=streamnum;
  d_length=0;
  d_speed=100000;
}


PlaySession::PlaySession(uint64_t phandle,unsigned cardnum,unsigned portnum,
			 unsigned streamnum)
{
  d_socket_descriptor=PlaySession::socketDescriptor(phandle);
  d_serial_number=PlaySession::serialNumber(phandle);
  d_card_number=cardnum;
  d_port_number=portnum;
  d_stream_number=streamnum;
  d_length=0;
  d_speed=100000;
}


uint64_t PlaySession::handle() const
{
  return PlaySession::makeHandle(d_socket_descriptor,d_serial_number);
}


int PlaySession::socketDescriptor() const
{
  return d_socket_descriptor;
}


unsigned PlaySession::serialNumber() const
{
  return d_serial_number;
}


unsigned PlaySession::cardNumber() const
{
  return d_card_number;
}


unsigned PlaySession::portNumber() const
{
  return d_port_number;
}


unsigned PlaySession::streamNumber() const
{
  return d_stream_number;
}


unsigned PlaySession::length() const
{
  return d_length;
}


void PlaySession::setLength(unsigned msec)
{
  d_length=msec;
}


unsigned PlaySession::speed() const
{
  return d_speed;
}


void PlaySession::setSpeed(unsigned ratio)
{
  d_speed=ratio;
}


uint64_t PlaySession::makeHandle(int fd,unsigned serial)
{
  return (((uint64_t)fd)<<32)|(0xffffffff&serial);
}


int PlaySession::socketDescriptor(uint64_t phandle)
{
  return phandle>>32;
}


unsigned PlaySession::serialNumber(uint64_t phandle)
{
  return 0xffffffff&phandle;
}
