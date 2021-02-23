// listsystemsettings.cpp
//
// Rivendell web service portal -- ListSystemSettings service
//
//   (C) Copyright 2011,2016 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <rdformpost.h>
#include <rdweb.h>
#include <rdsystem.h>
#include <rdconf.h>

#include <rdxport.h>

void Xport::ListSystemSettings()
{
  RDSystem *sys=new RDSystem();

  //
  // Send Data
  //
  printf("Content-type: application/xml\n\n");
  printf("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
  printf("%s\n",(const char *)sys->xml().toUtf8());
  Exit(0);
}
