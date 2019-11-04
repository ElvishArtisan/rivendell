// rdnexusd.h
//
// The Rivendell MusicMaster Nexus Daemon
//
//   (C) Copyright 2019 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDNEXUSD_H
#define RDNEXUSD_H

#include <vector>

#include <QObject>
#include <QDate>
#include <QTcpServer>

#include "rdconfig.h"
#include "rdnotification.h"
#include "rdcart.h"
#include "rdsvc.h"
#include "rdlog.h"
#include "rdnexus.h"

#define RDNEXUSD_USAGE "[-d]\n\nOptions:\n\n-d\n     Set 'debug' mode, causing rdrepld(8) to stay in the foreground\n     and print debugging info on standard output.\n\n" 
#define RDNEXUSD_PID "rdnexusd.pid"
#define RDNEXUSD_QUEUE_TIMER 60000

class MainObject : public QObject
{
  Q_OBJECT
  public:
    MainObject(QObject *parent=0);

  private slots:
    void queueTimerSlot();
    void publishMetaDataSlot(QString &station,RDNexusIdList &list);
    void publishScheduleSlot(QString &station,RDNexusElementList &list);
    void notificationReceivedSlot(RDNotification *notify);

  private:
    RDNexus *nexus;
    QString nexusd_app;
    QTimer *nexusd_timer;
    QString nexusd_name;       // Rivendell Scheduler Name
    QString nexusd_service;    // Rivendell Service
    QString nexusd_server;     // Nexus Server
    int nexusd_port;
    QString nexusd_station;    // Nexus Station
    QString nexusd_category;   // MusicMaster New Song Category
    QString nexusd_group;      // Rivendell New Cart Group
    QString nexusd_schedcode;  // Rivendell New Cart Scheduler Code
    bool nexusd_enable_http_server;
    int nexusd_http_port;
    bool nexusd_automerge;
    bool nexusd_debug;
    RDSvc *nexusd_svc;
    bool cartNotification(RDNotification *notify,bool queue=true);
    bool playoutNotification(RDNotification *notify,bool queue=true);
    void loadFieldMap();
    void updateCart(RDNexusSongInfo &info);
    bool generateLogFile(RDSvc *svc,QDate logdate,QString &service,RDNexusElementList &list);
    void reconcileLog(RDSvc *svc,RDLog *log,QDate &logdate,RDNexusElementList &list);
    void mergeLog(RDSvc *svc,QDate &logdate,QString &logname);
    void setSongInfo(RDCart *cart,RDNexusSongInfo &info);
    void sendCartNotification(RDNotification::Action action,unsigned cartnum);
    void sendLogNotification(RDNotification::Action action,QString &logname);
    void queueNotification(RDNotification *notify,QString error);
    void removeNotificationQueue(int id);
    void processNotificationQueue();
};


#endif  // RDNEXUSD_H
