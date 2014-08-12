
// globals.h
//
// Global Variable Declarations for RDAirPlay
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: globals.h,v 1.12.14.1 2012/11/26 20:19:40 cvs Exp $
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

#ifndef GLOBALS_H
#define GLOBALS_H

#include <rdstation.h>
#include <rdsystem.h>
#include <rdairplay_conf.h>
#include <rdaudio_port.h>
#include <rduser.h>
#include <rdripc.h>
#include <rdcae.h>
#include <rdevent_player.h>
#include <rdcart_dialog.h>
#include <rdconfig.h>

//
// Global Resources
//
extern RDStation *rdstation_conf;
extern RDSystem *rdsystem_conf;
extern RDAirPlayConf *rdairplay_conf;
extern RDAudioPort *rdaudioport_conf;
extern RDUser *rduser;
extern RDRipc *rdripc;
extern RDCae *rdcae;
extern RDEventPlayer *rdevent_player;
extern RDCartDialog *rdcart_dialog;
extern void LogLine(RDConfig::LogPriority prio,const QString &line);
extern QString logfile;
extern RDConfig *air_config;

#endif  // GLOBALS_H
