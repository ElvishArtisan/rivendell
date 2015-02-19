// import.cpp
//
// Rivendell replicator portal -- Import service
//
//   (C) Copyright 2015 Fred Gleason <fredg@paravelsystems.com>
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <rdformpost.h>
#include <rdweb.h>
#include <rdconf.h>
#include <rdrepl_conveyor.h>

#include "rdrepl.h"

void Repl::Import()
{
  //
  // Verify Post
  //
  QString filename;
  if(!repl_post->getValue("FILENAME",&filename)) {
    XmlExit("Missing FILENAME",400);
  }
  if(!repl_post->isFile("FILENAME")) {
    XmlExit("Missing file data",400);
  }

  //
  // Process Package
  //
  RDReplConveyor *conv=new RDReplConveyor(repl_replicator->name());
  if(!conv->pushPackage(RDReplConveyor::Inbound,filename)) {
    XmlExit("Package import failed",500);
  }
  XmlExit("OK",200);
}
