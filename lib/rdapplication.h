// rdapplication.h
//
// Base Application Class
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

#ifndef RDAPPLICATION_H
#define RDAPPLICATION_H

#include <qobject.h>

#include <rdairplay_conf.h>
#include <rdcae.h>
#include <rdcmd_switch.h>
#include <rdconfig.h>
#include <rddb.h>
#include <rddbheartbeat.h>
#include <rdlibrary_conf.h>
#include <rdlogedit_conf.h>
#include <rdripc.h>
#include <rdstation.h>
#include <rdsystem.h>
#include <rduser.h>

class RDApplication : public QObject
{
  Q_OBJECT;
 public:
  RDApplication(const QString &module_name,const QString &cmdname,
		const QString &usage,QObject *parent=0);
  ~RDApplication();
  bool open(QString *err_msg);
  RDAirPlayConf *airplayConf();
  RDCae *cae();
  RDCmdSwitch *cmdSwitch();
  RDConfig *config();
  RDLibraryConf *libraryConf();
  RDLogeditConf *logeditConf();
  RDAirPlayConf *panelConf();
  RDRipc *ripc();
  RDStation *station();
  RDSystem *system();
  RDUser *user();

 private slots:
  void userChangedData();

 signals:
  void userChanged();

 private:
  RDAirPlayConf *app_airplay_conf;
  RDAirPlayConf *app_panel_conf;
  RDCae *app_cae;
  RDCmdSwitch *app_cmd_switch;
  RDConfig  *app_config;
  RDLibraryConf *app_library_conf;
  RDLogeditConf *app_logedit_conf;
  RDRipc *app_ripc;
  RDStation *app_station;
  RDSystem *app_system;
  RDUser *app_user;
  RDDbHeartbeat *app_heartbeat;
  QString app_module_name;
  QString app_command_name;
  QString app_usage;
};

extern RDApplication *rda;

#endif  // RDAPPLICATION_H
