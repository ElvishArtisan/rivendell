// sas_shim.h
//
// An RDCatch event import shim for the SAS64000
//
//   (C) Copyright 2002-2004,2016-2018 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef SAS_SHIM_H
#define SAS_SHIM_H

#include <qapplication.h>
#include <qhostaddress.h>
#include <qobject.h>

#include <rdttydevice.h>

#define SAS_SHIM_USAGE "\n"
#define POLL_INTERVAL 100

class MainObject : public QObject
{
  Q_OBJECT
 public:
  MainObject(QObject *parent=0);

 private slots:
  void readTtyData();

 private:
  void DispatchRml(int input,int output);
  QString shim_station;
  QHostAddress shim_address;
  RDTTYDevice *shim_tty;
};


#endif  // SAS_SHIM_H
