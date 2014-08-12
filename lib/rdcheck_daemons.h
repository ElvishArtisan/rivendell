// rdcheck_daemons.h
//
// Get the status of the Rivendell system daemons.
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcheck_daemons.h,v 1.7 2010/07/29 19:32:33 cvs Exp $
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

#ifndef RDCHECK_DAEMONS_H
#define RDCHECK_DAEMONS_H

#include <qstring.h>


void RDInitializeDaemons();
bool RDCheckDaemon(QString name);
bool RDCheckDaemons();
bool RDStartDaemons();
bool RDKillDaemons();


#endif

