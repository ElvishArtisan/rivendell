// rddbheartbeat.h
//
// Abstract a Rivendell Cart
//
//   (C) Copyright 2002-2006,2016 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDDBHEARTBEAT_H
#define RDDBHEARTBEAT_H

#include <qobject.h>
#include <qsqldatabase.h>
#include <qtimer.h>

class RDDbHeartbeat : public QObject
{
  Q_OBJECT;
 public:
  RDDbHeartbeat(int interval,QObject *parent=0);

 private slots:
  void intervalTimeoutData();
};


#endif  // RDDBHEARTBEAT_H
