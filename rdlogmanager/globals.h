// globals.h
//
// Global Variable Declarations for RDLogManager
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: globals.h,v 1.7.4.1 2013/11/13 23:36:37 cvs Exp $
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
#include <rduser.h>
#include <rdripc.h>
#include <rdcae.h>
#include <rdconfig.h>

//
// Global Resources
//
extern RDStation *rdstation_conf;
extern RDUser *rduser;
extern RDRipc *rdripc;
extern RDCae *rdcae;
extern RDConfig *log_config;
extern QString *event_filter;
extern QString *clock_filter;
extern bool skip_db_check;

#endif  // GLOBALS_H
