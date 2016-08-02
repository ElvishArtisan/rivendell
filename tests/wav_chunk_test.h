// wav_chunk_test.h
//
// Check consistency of chunk layout in a WAV file
//
//   (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef WAV_CHUNK_TEST_H
#define WAV_CHUNK_TEST_H

#include <stdint.h>

#include <qobject.h>

#define WAV_CHUNK_TEST_USAGE "--filename=<filename>\n\n"

class MainObject : public QObject
{
 public:
  MainObject(QObject *parent=0);

 private:
  uint32_t NextChunk(QString &name,uint32_t *len);
  int file_fd;
  uint32_t file_length;
  uint32_t payload_length;
  uint32_t chunk_length;
};


#endif  // WAV_CHUNK_TEST_H
