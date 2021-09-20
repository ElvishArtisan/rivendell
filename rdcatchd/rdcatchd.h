// rdcatchd.h
//
// The Rivendell Netcatcher.
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDCATCHD_H
#define RDCATCHD_H

#define XLOAD_UPDATE_INTERVAL 1000
#define RDCATCHD_USAGE ""

#include <QMap>

#include <rd.h>
#include <rdcart.h>
#include <rdcatch_conf.h>
#include <rdcatch_connect.h>
#include <rdcmd_switch.h>
#include <rddeck.h>
#include <rdmacro_event.h>
#include <rdsocket.h>
#include <rdsettings.h>
#include <rdtimeengine.h>
#include <rdtty.h>

#include "catchevent.h"

//
// Global RDCATCHD Definitions
//
#define RDCATCHD_GPO_INTERVAL 333
#define RDCATCHD_MAX_MACROS 64
#define RDCATCHD_FREE_EVENTS_INTERVAL 1000
#define RDCATCHD_HEARTBEAT_INTERVAL 10000
#define RDCATCHD_ERROR_ID_OFFSET 1000000

class MainObject : public QObject
{
  Q_OBJECT
 public:
  MainObject(QObject *parent=0);

 private slots:
  void rmlReceivedData(RDMacro *rml);
  void gpiStateChangedData(int matrix,int line,bool state);
  void engineData(int);
  void sysHeartbeatData();
  void startupCartData();
  void notificationReceivedData(RDNotification *notify);
  void userChangedData();

 private:
  void LoadEngine();
  void LoadHeartbeat();
  void SendNotification(RDNotification::Type type,RDNotification::Action,
			const QVariant &id);
  RDTimeEngine *d_engine;
  QTimer *d_heartbeat_timer;
  unsigned d_heartbeat_cartnum;
  QMap<unsigned,CatchEvent *> d_events;
};


#endif  // RDCATCHD_H
