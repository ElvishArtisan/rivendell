// globals.h
//
// Global Variable Declarations for RDPanel
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: globals.h,v 1.5.14.1 2012/11/26 20:19:41 cvs Exp $
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
#include <rdairplay_conf.h>
#include <rdaudio_port.h>
#include <rduser.h>
#include <rdripc.h>
#include <rdcart_dialog.h>

//
// Global Resources
//
extern RDStation *rdstation_conf;
extern RDSystem *rdsystem_conf;
extern RDAirPlayConf *rdairplay_conf;
extern RDAudioPort *rdaudioport_conf;
extern RDUser *rduser;
extern RDRipc *rdripc;
extern RDCartDialog *panel_cart_dialog;


#endif  // GLOBALS_H
