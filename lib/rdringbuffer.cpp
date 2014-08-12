// rdringbuffer.cpp
//
// A lock-free ring buffer.
//
//   (C) Copyright 2000 Paul Davis
//   (C) Copyright 2003 Rohan Drape
//   (C) Copyright 2003 Fred Gleason <fredg@paravelsystems.com>
//
//    $Id: rdringbuffer.cpp,v 1.4 2010/07/29 19:32:33 cvs Exp $
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

#include <rdringbuffer.h>

RDRingBuffer::RDRingBuffer(int sz)
{
  int power_of_two;

  rb = (ringbuffer_t *)malloc(sizeof (ringbuffer_t));

  for (power_of_two = 1; 1 << power_of_two < sz; power_of_two++);

  rb->size = 1 << power_of_two;
  rb->size_mask = rb->size;
  rb->size_mask -= 1;
  rb->write_ptr = 0;
  rb->read_ptr = 0;
  rb->buf = (char *)malloc (rb->size);
  rb->mlocked = 0;
}


RDRingBuffer::~RDRingBuffer()
{
  if (rb->mlocked) {
    munlock (rb->buf, rb->size);
  }
  free (rb->buf);
  free (rb);
}


bool RDRingBuffer::mlock()
{
  if (::mlock (rb->buf, rb->size)) {
    return false;
  }
  rb->mlocked = 1;
  return true;
}


void RDRingBuffer::reset()
{
  rb->read_ptr = 0;
  rb->write_ptr = 0;
}


void RDRingBuffer::writeAdvance(size_t cnt)
{
  rb->write_ptr += cnt;
  rb->write_ptr &= rb->size_mask;
}


void RDRingBuffer::readAdvance(size_t cnt)
{
  rb->read_ptr += cnt;
  rb->read_ptr &= rb->size_mask;
}


size_t RDRingBuffer::writeSpace() const
{
  size_t w, r;

  w = rb->write_ptr;
  r = rb->read_ptr;

  if (w > r) {
    return ((r - w + rb->size) & rb->size_mask) - 1;
  } else if (w < r) {
    return (r - w) - 1;
  }
  return rb->size - 1;
}


size_t RDRingBuffer::readSpace() const
{
  size_t w, r;

  w = rb->write_ptr;
  r = rb->read_ptr;

  if (w > r) {
    return w - r;
  }
  return (w - r + rb->size) & rb->size_mask;
}


size_t RDRingBuffer::read(char *dest,size_t cnt)
{
  size_t free_cnt;
  size_t cnt2;
  size_t to_read;
  size_t n1, n2;

  if ((free_cnt = readSpace()) == 0) {
    return 0;
  }

  to_read = cnt > free_cnt ? free_cnt : cnt;

  cnt2 = rb->read_ptr + to_read;

  if (cnt2 > rb->size) {
    n1 = rb->size - rb->read_ptr;
    n2 = cnt2 & rb->size_mask;
  } else {
    n1 = to_read;
    n2 = 0;
  }

  memcpy (dest, &(rb->buf[rb->read_ptr]), n1);
  rb->read_ptr += n1;
  rb->read_ptr &= rb->size_mask;

  if (n2) {
    memcpy (dest + n1, &(rb->buf[rb->read_ptr]), n2);
    rb->read_ptr += n2;
    rb->read_ptr &= rb->size_mask;
  }

  return to_read;
}


size_t RDRingBuffer::write(char *src,size_t cnt)
{
  size_t free_cnt;
  size_t cnt2;
  size_t to_write;
  size_t n1, n2;

  if ((free_cnt = writeSpace()) == 0) {
    return 0;
  }

  to_write = cnt > free_cnt ? free_cnt : cnt;

  cnt2 = rb->write_ptr + to_write;

  if (cnt2 > rb->size) {
    n1 = rb->size - rb->write_ptr;
    n2 = cnt2 & rb->size_mask;
  } else {
    n1 = to_write;
    n2 = 0;
  }

  memcpy (&(rb->buf[rb->write_ptr]), src, n1);
  rb->write_ptr += n1;
  rb->write_ptr &= rb->size_mask;

  if (n2) {
    memcpy (&(rb->buf[rb->write_ptr]), src + n1, n2);
    rb->write_ptr += n2;
    rb->write_ptr &= rb->size_mask;
  }

  return to_write;
}


void RDRingBuffer::getReadVector(ringbuffer_data_t *vec)
{
  size_t free_cnt;
  size_t cnt2;
  size_t w, r;

  w = rb->write_ptr;
  r = rb->read_ptr;

  if (w > r) {
    free_cnt = w - r;
  } else {
    free_cnt = (w - r + rb->size) & rb->size_mask;
  }

  cnt2 = r + free_cnt;

  if (cnt2 > rb->size) {

    /* Two part vector: the rest of the buffer after the current write
       ptr, plus some from the start of the buffer. */

    vec[0].buf = &(rb->buf[r]);
    vec[0].len = rb->size - r;
    vec[1].buf = rb->buf;
    vec[1].len = cnt2 & rb->size_mask;

  } else {

    /* Single part vector: just the rest of the buffer */

    vec[0].buf = &(rb->buf[r]);
    vec[0].len = free_cnt;
    vec[1].len = 0;
  }
}


void RDRingBuffer::getWriteVector(ringbuffer_data_t *vec)
{
  size_t free_cnt;
  size_t cnt2;
  size_t w, r;

  w = rb->write_ptr;
  r = rb->read_ptr;

  if (w > r) {
    free_cnt = ((r - w + rb->size) & rb->size_mask) - 1;
  } else if (w < r) {
    free_cnt = (r - w) - 1;
  } else {
    free_cnt = rb->size - 1;
  }

  cnt2 = w + free_cnt;

  if (cnt2 > rb->size) {

    /* Two part vector: the rest of the buffer after the current write
       ptr, plus some from the start of the buffer. */

    vec[0].buf = &(rb->buf[w]);
    vec[0].len = rb->size - w;
    vec[1].buf = rb->buf;
    vec[1].len = cnt2 & rb->size_mask;
  } else {
    vec[0].buf = &(rb->buf[w]);
    vec[0].len = free_cnt;
    vec[1].len = 0;
  }
}
