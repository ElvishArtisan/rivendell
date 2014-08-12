// globals.h
//
// Global Variable Declarations for RDCatch
//
//   (C) Copyright 2002-2005 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: globals.h,v 1.8 2010/07/29 19:32:36 cvs Exp $
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

#include <rdconfig.h>
#include <rdstation.h>
#include <rdaudio_port.h>
#include <rduser.h>
#include <rdlibrary_conf.h>
#include <rdcae.h>
#include <rdripc.h>
#include <rdcart_dialog.h>
#include <rdsystem.h>

//
// Global Resources
//
extern RDConfig *catch_config;
extern RDStation *rdstation_conf;
extern RDAudioPort *rdaudioport_conf;
extern RDUser *catch_user;
extern RDLibraryConf *rdlibrary_conf;
extern RDRipc *catch_ripc;
extern RDCae *catch_cae;
extern RDCartDialog *catch_cart_dialog;
extern int catch_audition_card;
extern int catch_audition_port;
extern RDSystem *catch_system;

#endif  // GLOBALS_H
