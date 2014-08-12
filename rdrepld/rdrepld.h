// rdrepld.h
//
// The Rivendell Replicator Daemon
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdrepld.h,v 1.2 2010/07/29 19:32:37 cvs Exp $
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

#ifndef RDREPLD_H
#define RDREPLD_H

#include <vector>

#include <qobject.h>
#include <qtimer.h>

#include <rdcheck_daemons.h>
#include <rdsystem.h>
#include <rdstation.h>
#include <rdconfig.h>

#include <replfactory.h>

#define RDREPLD_USAGE "[-d][--event-id=<id>]\n\nOptions:\n\n-d\n     Set 'debug' mode, causing rdrepld(8) to stay in the foreground\n     and print debugging info on standard output.\n\n" 
#define RD_RDREPLD_PID "rdrepl.pid"
#define RD_RDREPL_SCAN_INTERVAL 10000

class MainObject : public QObject
{
  Q_OBJECT
 public:
  MainObject(QObject *parent=0,const char *name=0);

 private slots:
  void mainLoop();
  void log(RDConfig::LogPriority prio,const QString &line);

 private:
  void ProcessCarts();
  void LoadReplicators();
  void FreeReplicators();
  QTimer *repl_loop_timer;
  QString repl_temp_dir;
  RDStation *repl_station;
  QSqlDatabase *repl_db;
  std::vector<ReplFactory *> repl_replicators;
  bool debug;
};


#endif  // RDREPLD_H
