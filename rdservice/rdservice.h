// rdservice.h
//
// Rivendell Services Manager
//
//   (C) Copyright 2018-2020 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDSERVICE_H
#define RDSERVICE_H

#include <qmap.h>
#include <qobject.h>
#include <qtimer.h>

#include <rdprocess.h>

#define RDSERVICE_CAED_ID 0
#define RDSERVICE_RIPCD_ID 1
#define RDSERVICE_RDCATCHD_ID 2
#define RDSERVICE_RDPADD_ID 3
#define RDSERVICE_RDPADENGINED_ID 4
#define RDSERVICE_RDVAIRPLAYD_ID 5
#define RDSERVICE_RDREPLD_ID 6
#define RDSERVICE_RDRSSD_ID 7
#define RDSERVICE_LOCALMAINT_ID 8
#define RDSERVICE_SYSTEMMAINT_ID 9
#define RDSERVICE_LAST_ID 10
#define RDSERVICE_FIRST_DROPBOX_ID 100

class MainObject : public QObject
{
  Q_OBJECT;
 public:
  enum StartupTarget {TargetCaed=0,TargetRipcd=1,TargetRdcatchd=2,
		      TargetRdpadd=3,TargetRdpadengined=4,
		      TargetRdvairplayd=5,TargetRdrepld=6,
		      TargetRdrssd=7,TargetAll=8};
  MainObject(QObject *parent=0);

 private slots:
  void processFinishedData(int id);
  void checkMaintData();
  void exitData();

 private:
  bool Startup(QString *err_msg);
  bool StartDropboxes(QString *err_msg);
  void KillProgram(const QString &program);
  void Shutdown();
  void ShutdownDropboxes();
  void RunSystemMaintRoutine();
  void RunLocalMaintRoutine();
  int GetMaintInterval() const;
  void RunEphemeralProcess(int id,const QString &program,
			   const QStringList &args);
  QString TargetCommandString(StartupTarget target) const;
  QMap<int,RDProcess *> svc_processes;
  QTimer *svc_maint_timer;
  QTimer *svc_exit_timer;
  StartupTarget svc_startup_target;
  bool svc_force_system_maintenance;
};


#endif  // RDSERVICE_H
