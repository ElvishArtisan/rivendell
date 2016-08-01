// globals.h
//
// Global Variable Declarations for RDAirPlay
//
//   (C) Copyright 2002-2004,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdaudio_port.h>
#include <rdevent_player.h>
#include <rdcart_dialog.h>

//
// Global Resources
//
extern RDAudioPort *rdaudioport_conf;
extern RDEventPlayer *rdevent_player;
extern RDCartDialog *rdcart_dialog;
extern void LogLine(RDConfig::LogPriority prio,const QString &line);
extern QString logfile;


#endif  // GLOBALS_H
