// rdvairplayd.h
//
// Headless log player
//
//   (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDVAIRPLAYD_H
#define RDVAIRPLAYD_H

#include <qobject.h>

#include <rdevent_player.h>
#include <rd.h>
#include <rdlogplay.h>
#include <rdmacro.h>

#define RDVAIRPLAYD_USAGE "[options]\n"

class MainObject : public QObject
{
  Q_OBJECT;
 public:
  MainObject(QObject *parent=0);

 private slots:
  void ripcConnectedData(bool state);
  void userData();
  void rmlReceivedData(RDMacro*);
  void logReloadedData(int log);
  void exitData();

 private:
  void SetAutoMode(int index);
  void SetLiveAssistMode(int index);
  void SetManualMode(int index);
  int LogMachineIndex(int log_mach,bool *all=NULL) const;
  RDLogPlay *air_logs[RD_RDVAIRPLAY_LOG_QUAN];
  QString air_start_lognames[RD_RDVAIRPLAY_LOG_QUAN];
  int air_start_lines[RD_RDVAIRPLAY_LOG_QUAN];
  bool air_start_starts[RD_RDVAIRPLAY_LOG_QUAN];
  RDEventPlayer *air_event_player;
  QDateTime air_startup_datetime;
  RDAirPlayConf::ExitCode air_previous_exit_code;
  QTimer *air_exit_timer;
};


#endif  // RDVAIRPLAYD_H
