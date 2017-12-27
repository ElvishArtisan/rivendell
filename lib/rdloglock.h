// rdloglock.h
//
// Log locking routines for Rivendell
//
//   (C) Copyright 2017 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDLOGLOCK_H
#define RDLOGLOCK_H

#include <qhostaddress.h>
#include <qobject.h>
#include <qtimer.h>

#include <rd.h>
#include <rduser.h>
#include <rdstation.h>

class RDLogLock : public QObject
{
  Q_OBJECT;
 public:
  RDLogLock(const QString &log_name,RDUser *user,RDStation *station,
	    QObject *parent=0);
  ~RDLogLock();
  bool isLocked() const;
  bool tryLock(QString *username,QString *stationname,QHostAddress *addr);
  void clearLock();
  static bool tryLock(QString *username,QString *stationname,QHostAddress *addr,
		      const QString &log_name,const QString &guid);
  static void updateLock(const QString &log_name,const QString &guid);
  static void clearLock(const QString &guid);
  static bool validateLock(const QString &log_name,const QString &guid);
  static QString makeGuid(const QString &stationname);

 private slots:
  void updateLock();

 private:
  QString lock_log_name;
  RDUser *lock_user;
  RDStation *lock_station;
  QTimer *lock_timer;
  bool lock_locked;
  QString lock_guid;
};


#endif  // RDLOGLOCK_H
