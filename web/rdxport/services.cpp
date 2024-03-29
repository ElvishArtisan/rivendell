// services.cpp
//
// Rivendell web service portal -- Service services
//
//   (C) Copyright 2010-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include <rdapplication.h>
#include <rdconf.h>
#include <rddb.h>
#include <rdescape_string.h>
#include <rdformpost.h>
#include <rdsvc.h>
#include <rduser.h>
#include <rdweb.h>

#include "rdxport.h"

void Xport::ListServices()
{
  QString sql;
  QString sql2;
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
  sql=QString("select `NAME` from `SERVICES` where ");
  sql2=QString("select `SERVICE_NAME` from `USER_SERVICE_PERMS` where ")+
    "`USER_NAME`='"+RDEscapeString(rda->user()->name())+"'";
  q=new RDSqlQuery(sql2);
  sql+="(";
  while(q->next()) {
    sql+="(`NAME`='"+RDEscapeString(q->value(0).toString())+"')||";
  }
  sql=sql.left(sql.length()-2);
  sql+=")";
  delete q;

  if(trackable=="1") {
    sql+="&&(`TRACK_GROUP`!='')&&(`TRACK_GROUP` is not null)";
  }
  sql+=" order by `NAME`";
  q=new RDSqlQuery(sql);

  //
  // Process Request
  //
  printf("Content-type: application/xml; charset=utf-8\n");
  printf("Status: 200\n\n");
  printf("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
  printf("<serviceList>\n");
  while(q->next()) {
    svc=new RDSvc(q->value(0).toString(),rda->station(),rda->config());
    printf("%s",svc->xml().toUtf8().constData());
    delete svc;
  }
  printf("</serviceList>\n");

  delete q;
  Exit(0);
}
