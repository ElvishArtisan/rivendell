// globals.h
//
// Global Variable Declarations for RDCastManager
//
//   (C) Copyright 2007 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: globals.h,v 1.5 2011/08/30 23:35:44 cvs Exp $
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

#include <qstring.h>
#include <rduser.h>
#include <rdripc.h>
#include <rdstation.h>
#include <rdconfig.h>
#include <rdsystem.h>

//
// Global Resources
//
extern QString cast_filter;
extern QString cast_group;
extern QString cast_schedcode;
extern RDUser *cast_user;
extern RDRipc *cast_ripc;
extern RDConfig *config;
extern RDStation *rdstation_conf;
extern RDSystem *cast_system;


#endif  // GLOBALS_H
