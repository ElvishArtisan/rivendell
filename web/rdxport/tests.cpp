// savestring.cpp
//
// Rivendell web service portal -- save string test
//
//   (C) Copyright 2018-2019 Fred Gleason <fredg@paravelsystems.com>
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

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <rdapplication.h>
#include <rdconf.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdformpost.h>
#include <rdsvc.h>
#include <rduser.h>
#include <rdweb.h>

#include "rdxport.h"

void Xport::SaveString()
{
  QString string;

  //
  // Get Options
  //
  if(!xport_post->getValue("STRING",&string)) {
    XmlExit("Missing STRING",400,"tests.cpp",LINE_NUMBER);
  }

  //
  // Process Request
  //
  rda->syslog(LOG_INFO,"SAVESTRING: |%s|",(const char *)string.utf8());
  printf("Content-type: application/xml; charset: UTF-8\n");
  printf("Status: 200\n\n");
  printf("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
  printf("%s\n",(const char *)RDXmlField("string",string).utf8());

  Exit(0);
}


void Xport::SaveFile()
{
  QString filename;
  QString msg="OK";

  if(!xport_post->getValue("FILENAME",&filename)) {
    XmlExit("Missing FILENAME",400,"tests.cpp",LINE_NUMBER);
  }
  if(!xport_post->isFile("FILENAME")) {
    XmlExit("Missing file data",400,"tests.cpp",LINE_NUMBER);
  }

  if(RDCopy(filename,"/var/snd/"+RDGetBasePart(filename))) {
    msg=QString("Saved file to \"/var/snd/")+RDGetBasePart(filename)+"\"";
  }
  else {
    msg="File copy failed!";
  }

  printf("Content-type: text/html; charset: UTF-8\n");
  printf("Status: 200\n\n");
  printf("%s\n",(const char *)msg.utf8());

  Exit(0);
}
