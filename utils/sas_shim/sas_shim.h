// sas_shim.h
//
// An RDCatch event import shim for the SAS64000
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: sas_shim.h,v 1.6 2010/07/29 19:32:40 cvs Exp $
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

#include <qobject.h>
#include <qapplication.h>
#include <qsqldatabase.h>

#include <rdttydevice.h>

#include <rdstation.h>
#include <rdripc.h>
#include <rdconfig.h>

#define POLL_INTERVAL 100

class MainObject : public QObject
{
  Q_OBJECT
 public:
  MainObject(QObject *parent=0,const char *name=0);

 private slots:
  void readTtyData();

 private:
  void DispatchRml(int input,int output);
  QString shim_station;
  RDStation *shim_rdstation;
  QHostAddress shim_address;
  RDRipc *shim_ripc;
  QSqlDatabase *shim_db;
  RDTTYDevice *shim_tty;
  RDConfig *rd_config;
};


#endif 
