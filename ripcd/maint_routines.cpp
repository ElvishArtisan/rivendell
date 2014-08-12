// ripcd.cpp
//
// Rivendell Maintenance Routines
//
//   (C) Copyright 2008 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: maint_routines.cpp,v 1.4 2010/07/29 19:32:38 cvs Exp $
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

#include <stdlib.h>

#include <rd.h>

#include <ripcd.h>

void MainObject::RunSystemMaintRoutine()
{
  if(fork()==0) {
    system("rdmaint --system");
    system("rdpurgecasts");
    exit(0);
  }

  LogLine(RDConfig::LogInfo,"ran system-wide maintenance routines");
}


void MainObject::RunLocalMaintRoutine()
{
  if(fork()==0) {
    system("rdmaint");
    exit(0);
  }
  LogLine(RDConfig::LogInfo,"ran local maintenance routines");
}


int MainObject::GetMaintInterval() const
{
  return (int)(RD_MAINT_MIN_INTERVAL+
	       (RD_MAINT_MAX_INTERVAL-RD_MAINT_MIN_INTERVAL)*
	       (double)random()/(double)RAND_MAX);
}
