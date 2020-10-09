// rdmaint.h
//
// A Utility for running periodic system maintenance.
//
//   (C) Copyright 2008,2016-2018 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDMAINT_H
#define RDMAINT_H

#include <qobject.h>

#define RDMAINT_USAGE "[--help] [--verbose] [--system]\n\nRun Rivendell maintenance routines.\n"

class MainObject : public QObject
{
  Q_OBJECT;
 public:
  MainObject(QObject *parent=0);

 private slots:
  void userData();

 private:
  void RunSystemMaintenance();
  void RunLocalMaintenance();
  void PurgeCuts();
  void PurgeLogs();
  void PurgeElr();
  void PurgeDropboxes();
  void PurgeGpioEvents();
  void PurgeWebapiAuths();
  void PurgeStacks();
  void RehashCuts();
  void PrintMessage(const QString &msg) const;
  bool maint_verbose;
  bool maint_system;
};


#endif  // RDMAINT_H
