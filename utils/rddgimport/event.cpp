// event.cpp
//
// Abstract a Dial Global Spot Event
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: event.cpp,v 1.1.2.1 2013/01/04 18:41:14 cvs Exp $
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

#include <event.h>

Event::Event()
{
  evt_length=0;
}


QTime Event::time() const
{
  return evt_time;
}


void Event::setTime(const QTime &time)
{
  evt_time=time;
}


int Event::length() const
{
  return evt_length;
}


void Event::setLength(int msec)
{
  evt_length=msec;
}


QString Event::isci() const
{
  return evt_isci;
}


void Event::setIsci(const QString &str)
{
  evt_isci=str;
}


QString Event::title() const
{
  return evt_title;
}


void Event::setTitle(const QString &str)
{
  evt_title=str;
}


QString Event::client() const
{
  return evt_client;
}


void Event::setClient(const QString &str)
{
  evt_client=str;
}
