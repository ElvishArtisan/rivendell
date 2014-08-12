// rddelete.h
//
// A Batch Deleter for Rivendell.
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rddelete.h,v 1.1.2.1 2013/04/29 22:03:23 cvs Exp $
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


#ifndef RDDELETE_H
#define RDDELETE_H

#include <vector>

#include <qobject.h>
#include <qsqldatabase.h>
#include <qfileinfo.h>
#include <qdatetime.h>

#include <rdwavedata.h>
#include <rdwavefile.h>
#include <rdconfig.h>
#include <rdcmd_switch.h>
#include <rdgroup.h>
#include <rdcart.h>
#include <rdcut.h>
#include <rdripc.h>
#include <rduser.h>
#include <rdsystem.h>
#include <rdstation.h>

#define RDDELETE_STDIN_BUFFER_LENGTH 1024
#define RDDELETE_USAGE "[options] <obj-type> [<obj-id> ...]\n\nDelete objects from a Rivendell system.  Each type of object to be deleted\nis specified by an appropriate <obj-type> switch, followed optionally by\na list of objects.  If no list is specified, then rddelete(1) will read\nthe list of objects from standard input.\n\nThe following object types can be deleted:\n--carts\n     Delete carts.  The <obj-id> values should be cart numbers.\n\n--logs\n     Delete logs.  The <obj-id> values should be log names.\n\nThe following options are available:\n\n--verbose\n     Print progress messages during processing.\n\n--continue-after-error\n     Continue processing list of objects even in the face of errors in\n     that list.\n\n--dry-run\n     Process list of objects, but don't actually delete anything.\n"

class MainObject : public QObject
{
  Q_OBJECT;
 public:
  MainObject(QObject *parent=0,const char *name=0);

 private slots:
  void userData();
  void log(RDConfig::LogPriority prio,const QString &line);

 private:
  void DeleteCarts();
  void DeleteLogs();
  bool GetNextObject(unsigned *cartnum);
  bool GetNextObject(QString *logname);
  bool GetNextStdinObject(QString *logname);
  QSqlDatabase *del_db;
  RDSystem *del_system;
  RDStation *del_station;
  RDCmdSwitch *del_cmd;
  RDRipc *del_ripc;
  RDUser *del_user;
  RDConfig *del_config;
  bool del_carts;
  bool del_logs;
  bool del_verbose;
  bool del_continue_after_error;
  bool del_dry_run;
  std::vector<QString> del_obj_ids;
  unsigned del_obj_ptr;
};


#endif  // RDDELETE_H
