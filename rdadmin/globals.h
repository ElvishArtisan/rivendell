// globals.h
//
// Global Definitions for RDAdmin
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: globals.h,v 1.11.4.1 2012/11/26 20:19:38 cvs Exp $
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

#include <rdripc.h>
#include <rdconfig.h>
#include <rduser.h>
#include <rdstation.h>
#include <rdsystem.h>
#include <rdcart_dialog.h>

//
// Global Classes
//
extern RDRipc *rdripc;
extern RDConfig *admin_config;
extern RDUser *admin_user;
extern RDStation *admin_station;
extern RDSystem *admin_system;
extern RDCartDialog *admin_cart_dialog;
extern QString admin_svc_name;
extern QString admin_admin_username;
extern QString admin_admin_password;
extern QString admin_admin_hostname;
extern QString admin_admin_dbname;
extern bool admin_skip_backup;
extern QString admin_backup_filename;
extern void PrintError(const QString &str,bool interactive);

#endif  // GLOBALS_H
