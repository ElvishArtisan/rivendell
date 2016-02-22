// audiostore.cpp
//
// Rivendell web service portal -- AudioStore service
//
//   (C) Copyright 2014 Fred Gleason <fredg@paravelsystems.com>
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
#include <sys/statvfs.h>
#include <sys/vfs.h>
#include <fcntl.h>

#include <rdformpost.h>
#include <rdweb.h>
#include <rdcart.h>
#include <rdwavefile.h>
#include <rdconf.h>

#include <rdxport.h>

void Xport::AudioStore()
{
  struct statvfs stat;

  memset(&stat,0,sizeof(stat));
  if(statvfs(xport_config.audioRoot(),&stat)<0) {
    XmlExit("Internal Error",500);
  }
  printf("Content-type: application/xml\n");
  printf("Status: 200\n\n");
  printf("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
  printf("<audioStore>\n");
  printf("  <freeBytes>%lu</freeBytes>\n",stat.f_bfree*stat.f_bsize);
  printf("  <totalBytes>%lu</totalBytes>\n",stat.f_blocks*stat.f_bsize);
  printf("</audioStore>\n");

  Exit(0);
}
