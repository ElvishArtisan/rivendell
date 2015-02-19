// export.cpp
//
// Rivendell replicator portal -- Export service
//
//   (C) Copyright 2010,2014 Fred Gleason <fredg@paravelsystems.com>
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
#include <rdcart.h>
#include <rdaudioconvert.h>
#include <rdsettings.h>
#include <rdconf.h>

#include <rdrepl.h>
#include <rdrepl_conveyor.h>

void Repl::Export()
{
  int id=-1;
  FILE *f=NULL;

  //
  // Process Package
  //
  RDReplConveyor *conv=new RDReplConveyor(repl_replicator->name());
  if(conv->nextPackageReady(&id,RDReplConveyor::Outbound)) {
    if((f=fopen(RDReplConveyor::fileName(id),"r"))==NULL) {
      XmlExit("Unable to export package",500);
    }
    printf("Content-Type: octet-stream\n\n");
    RDCopy(f,stdout);
    fclose(f);
    Exit(0);
  }
  else {
    XmlExit("No package queued",202);
  }
  Exit(0);
}
