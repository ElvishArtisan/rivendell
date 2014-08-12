// rdmaint.h
//
// A Utility for running periodic system maintenance.
//
//   (C) Copyright 2008 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdmaint.h,v 1.6 2010/09/08 20:38:01 cvs Exp $
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
#include <qsqldatabase.h>

#include <rdconfig.h>
#include <rduser.h>
#include <rdstation.h>
#include <rdcmd_switch.h>

#define RDMAINT_USAGE "[--help] [--verbose] [--system]\n\nRun Rivendell maintenance routines.\n"


class MainObject : public QObject
{
 public:
  MainObject(QObject *parent=0,const char *name=0);

 private:
  void RunSystemMaintenance();
  void RunLocalMaintenance();
  void PurgeCuts();
  void PurgeLogs();
  void PurgeElr();
  void PurgeDropboxes();
  RDConfig *maint_config;
  bool maint_verbose;
  bool maint_system;
  RDCmdSwitch *maint_cmd;
  RDStation *maint_station;
  RDUser *maint_user;
};


#endif  // RDMAINT_H
