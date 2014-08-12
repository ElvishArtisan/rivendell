// event.h
//
// Abstract a Dial Global Spot Event
//
//   (C) Copyright 2012 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: event.h,v 1.1.2.1 2013/01/04 18:41:14 cvs Exp $
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

#ifndef EVENT_H
#define EVENT_H

#include <qstring.h>
#include <qdatetime.h>

class Event
{
 public:
  Event();
  QTime time() const;
  void setTime(const QTime &time);
  int length() const;
  void setLength(int msec);
  QString isci() const;
  void setIsci(const QString &str);
  QString title() const;
  void setTitle(const QString &str);
  QString client() const;
  void setClient(const QString &str);

 private:
  QTime evt_time;
  int evt_length;
  QString evt_isci;
  QString evt_title;
  QString evt_client;
};


#endif  // EVENT_H
