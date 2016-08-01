// rdapplication.h
//
// Base class for Rivendell modules
//
//   (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <qapplication.h>

#include <rdairplay_conf.h>
#include <rdcae.h>
#include <rdcmd_switch.h>
#include <rdconfig.h>
#include <rdlibrary_conf.h>
#include <rdstation.h>
#include <rdsystem.h>
#include <rdripc.h>
#include <rduser.h>

class RDApplication : public QApplication
{
 public:
  RDApplication(int argc,char **argv,const char *modname,const char *usage,
		bool gui=true);
  RDAirPlayConf *airplayConf() const;
  RDLibraryConf *libraryConf() const;
  RDCae *cae() const;
  RDRipc *ripc() const;
  RDStation *station() const;
  RDSystem *system() const;
  RDUser *user() const;
  void setUser(const QString &username);
  RDConfig *config() const;
  RDCmdSwitch *cmdSwitch() const;

 private:
  RDAirPlayConf *app_airplay_conf;
  RDLibraryConf *app_library_conf;
  RDCae *app_cae;
  RDRipc *app_ripc;
  RDStation *app_station;
  RDSystem *app_system;
  RDUser *app_user;
  RDConfig *app_config;
  RDCmdSwitch *app_cmd_switch;
};

extern RDApplication *rda;


#endif  // RDAPPLICATION_H
