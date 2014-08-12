// globals.h
//
// Global declarations for the Rivendell Interprocess Communication Daemon
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: globals.h,v 1.11 2010/07/29 19:32:38 cvs Exp $
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

#include <rdcae.h>
#include <rdstation.h>
#include <rd.h>
#include <rdconfig.h>


//
// Global Objects
//
extern RDConfig *ripcd_config;
extern RDCae *rdcae;
extern RDStation *rdstation;
extern QString ripcd_active_locks[MAX_MATRICES];
extern void LogLine(RDConfig::LogPriority prio,const QString &line);

