// rdringbuffer.h
//
// A lock-free ring buffer.
//
//   (C) Copyright 2000 Paul Davis
//   (C) Copyright 2003 Rohan Drape
//   (C) Copyright 2002 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdringbuffer.h,v 1.5 2010/07/29 19:32:33 cvs Exp $
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
//   Adapted from code by Paul Davis and Rohan Drape in 
//   'example-clients/ringbuffer.ch' in the Jack Audio Connection Kit.
//

#ifndef RDRINGBUFFER_H
#define RDRINGBUFFER_H

#include <sys/types.h>

typedef struct  
{
  char *buf;
  size_t len;
} 
ringbuffer_data_t ;

typedef struct
{
  char *buf;
  volatile size_t write_ptr;
  volatile size_t read_ptr;
  size_t size;
  size_t size_mask;
  int mlocked;
} 
ringbuffer_t ;

class RDRingBuffer
{
 public:
  RDRingBuffer(int sz);
  ~RDRingBuffer();
  bool mlock();
  void reset();
  void writeAdvance(size_t cnt);
  void readAdvance(size_t cnt);
  size_t writeSpace() const;
  size_t readSpace() const;
  size_t read(char *dest,size_t cnt);
  size_t write(char *src,size_t cnt);
  void getReadVector(ringbuffer_data_t *vec);
  void getWriteVector(ringbuffer_data_t *vec);

 private:
  ringbuffer_t *rb;
};


#endif  // RDRINGBUFFER_H

