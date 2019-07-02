// rdpadengined.h
//
// Rivendell PAD Consolidation Server
//
//   (C) Copyright 2018-2019 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDPADENGINED_H
#define RDPADENGINED_H

#include <qmap.h>
#include <qobject.h>
#include <qprocess.h>
#include <qtimer.h>

#include <rdnotification.h>
#include <rdprocess.h>

#define RDPADENGINED_USAGE "\n\n"


class MainObject : public QObject
{
  Q_OBJECT
 public:
  MainObject(QObject *parent=0);

 private slots:
  void ripcConnectedData(bool state);
  void notificationReceivedData(RDNotification *notify);
  void instanceStartedData(int id);
  void instanceFinishedData(int id);
  void exitData();

 private:
  bool ScriptIsActive(unsigned id) const;
  void StartScript(unsigned id);
  void KillScript(unsigned id);
  void SetRunStatus(unsigned id,bool state,int exit_code=0,
		    const QString &err_text=QString()) const;
  QMap<unsigned,RDProcess *> pad_instances;
  QTimer *pad_exit_timer;
};


#endif  // RDPADENGINED_H
