// globals.h
//
// Global Resources for RDLibrary.
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: globals.h,v 1.17 2010/07/29 19:32:36 cvs Exp $
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

#include <rdlibrary_conf.h>
#include <rdstation.h>
#include <rdaudio_port.h>
#include <rdripc.h>
#include <rdcae.h>
#include <disk_gauge.h>
#include <rdcut.h>
#include <rdconfig.h>
#include <rduser.h>
#include <rdsystem.h>

extern RDLibraryConf *rdlibrary_conf;
extern RDStation *rdstation_conf;
extern RDAudioPort *rdaudioport_conf;
extern RDRipc *rdripc;
extern RDCae *rdcae;
extern DiskGauge *disk_gauge;
extern RDCut *cut_clipboard;
extern RDConfig *lib_config;
extern RDUser *lib_user;
extern RDSystem *lib_system;

extern bool import_running;
extern bool ripper_running;

#endif 
