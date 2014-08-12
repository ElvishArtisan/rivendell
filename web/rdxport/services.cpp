// services.cpp
//
// Rivendell web service portal -- Service services
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: services.cpp,v 1.1.2.1 2013/10/11 22:00:53 cvs Exp $
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
#include <errno.h>

#include <rddb.h>
#include <rdformpost.h>
#include <rdweb.h>
#include <rduser.h>
#include <rdsvc.h>
#include <rdconf.h>
#include <rdescape_string.h>

#include <rdxport.h>

void Xport::ListServices()
{
  QString sql;
  RDSqlQuery *q;
  RDSvc *svc;
  QString trackable;

  //
  // Get Options
  //
  xport_post->getValue("TRACKABLE",&trackable);

  //
  // Generate Service List
  //
  sql="select NAME from SERVICES";
  if(trackable=="1") {
    sql+=" where (TRACK_GROUP!=\"\")&&(TRACK_GROUP is not null)";
  }
  sql+=" order by NAME";
  q=new RDSqlQuery(sql);

  //
  // Process Request
  //
  printf("Content-type: application/xml\n");
  printf("Status: 200\n\n");
  printf("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
  printf("<serviceList>\n");
  while(q->next()) {
    svc=new RDSvc(q->value(0).toString());
    printf("%s",(const char *)svc->xml());
    delete svc;
  }
  printf("</serviceList>\n");

  delete q;
  Exit(0);
}
