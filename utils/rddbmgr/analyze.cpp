// analyze.cpp
//
// Routines for --analyze for rddbmgr(8)
//
//   (C) Copyright 2025 Fred Gleason <fredg@paravelsystems.com>
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

#include <netdb.h>
#include <unistd.h>
#include <sys/stat.h>

#include <rd.h>
#include <rdconf.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdpaths.h>

#include "rddbmgr.h"

bool MainObject::Analyze(int schema,QString *err_msg) const
{
  printf("  This command runs the 'MainObject::Analyze()' method,\n");
  printf("  located in 'utils/rddbmgr/analyze.cpp'.\n");

  return true;
}


