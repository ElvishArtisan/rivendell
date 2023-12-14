// playsession.h
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

#ifndef PLAYSESSION_H
#define PLAYSESSION_H

#include <stdint.h>

class PlaySession
{
 public:
  PlaySession(int sock,unsigned serial,unsigned cardnum,unsigned portnum,
	      unsigned streamnum);
  PlaySession(uint64_t phandle,unsigned cardnum,unsigned portnum,
	      unsigned streamnum);
  uint64_t handle() const;
  int socketDescriptor() const;
  unsigned serialNumber() const;
  unsigned cardNumber() const;
  unsigned portNumber() const;
  unsigned streamNumber() const;
  unsigned length() const;
  void setLength(unsigned msec);
  unsigned speed() const;
  void setSpeed(unsigned ratio);
  static uint64_t makeHandle(int fd,unsigned serial);
  static int socketDescriptor(uint64_t phandle);
  static unsigned serialNumber(uint64_t phandle);

 private:
  int d_socket_descriptor;
  unsigned d_serial_number;
  unsigned d_card_number;
  unsigned d_port_number;
  unsigned d_stream_number;
  unsigned d_length;
  unsigned d_speed;
};


#endif  // PLAYSESSION_H
