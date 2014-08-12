// logs.cpp
//
// Rivendell web service portal -- Log services
//
//   (C) Copyright 2013 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: logs.cpp,v 1.1.2.4 2013/10/23 23:32:54 cvs Exp $
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
#include <rdlog.h>
#include <rdconf.h>
#include <rdescape_string.h>

#include <rdxport.h>

void Xport::ListLogs()
{
  QString sql;
  RDSqlQuery *q;
  RDLog *log;
  QString service_name="";
  QString trackable;

  //
  // Get Options
  //
  xport_post->getValue("SERVICE_NAME",&service_name);
  xport_post->getValue("TRACKABLE",&trackable);

  //
  // Generate Log List
  //
  sql="select NAME from LOGS";
  if((!service_name.isEmpty())||(trackable=="1")) {
    sql+=" where";
    if(!service_name.isEmpty()) {
      sql+=" (SERVICE=\""+RDEscapeString(service_name)+"\")&&";
    }
    if(trackable=="1") {
      sql+=" (SCHEDULED_TRACKS>0)&&";
    }
    sql=sql.left(sql.length()-2);
  }
  sql+=" order by NAME";
  q=new RDSqlQuery(sql);

  //
  // Process Request
  //
  printf("Content-type: application/xml\n");
  printf("Status: 200\n\n");
  printf("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
  printf("<logList>\n");
  while(q->next()) {
    log=new RDLog(q->value(0).toString());
    printf("%s",(const char *)log->xml());
    delete log;
  }
  printf("</logList>\n");

  delete q;
  Exit(0);
}


void Xport::ListLog()
{
  RDLog *log;
  QString name="";

  //
  // Get Options
  //
  xport_post->getValue("NAME",&name);

  //
  // Verify that log exists
  //
  log=new RDLog(name);
  if(!log->exists()) {
    delete log;
    XmlExit("No such log",404);
  }

  //
  // Generate Log Listing
  //
  RDLogEvent *log_event=log->createLogEvent();
  log_event->load(true);

  //
  // Process Request
  //
  printf("Content-type: application/xml\n");
  printf("Status: 200\n\n");
  printf("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
  printf("%s\n",(const char *)log_event->xml());

  Exit(0);
}
