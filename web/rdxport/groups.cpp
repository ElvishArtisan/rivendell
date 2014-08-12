// groups.cpp
//
// Rivendell web service portal -- Group services
//
//   (C) Copyright 2010 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: groups.cpp,v 1.5 2012/02/13 23:01:50 cvs Exp $
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

#include <rdformpost.h>
#include <rdweb.h>
#include <rduser.h>
#include <rdgroup.h>
#include <rdconf.h>
#include <rdescape_string.h>

#include <rdxport.h>

void Xport::ListGroups()
{
  QString sql;
  RDSqlQuery *q;
  RDGroup *group;

  //
  // Generate Group List
  //
  sql=QString().sprintf("select GROUP_NAME from USER_PERMS \
                         where USER_NAME=\"%s\" order by GROUP_NAME",
			(const char *)RDEscapeString(xport_user->name()));
  q=new RDSqlQuery(sql);

  //
  // Process Request
  //
  printf("Content-type: application/xml\n");
  printf("Status: 200\n\n");
  printf("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
  printf("<groupList>\n");
  while(q->next()) {
    group=new RDGroup(q->value(0).toString());
    printf("%s",(const char *)group->xml());
    delete group;
  }
  printf("</groupList>\n");

  delete q;
  Exit(0);
}


void Xport::ListGroup()
{
  QString sql;
  RDSqlQuery *q;
  RDGroup *group;

  //
  // Verify Post
  //
  QString group_name;
  if(!xport_post->getValue("GROUP_NAME",&group_name)) {
    XmlExit("Missing GROUP_NAME",400);
  }

  //
  // Check Group Accessibility
  //
  sql=QString().sprintf("select GROUP_NAME from USER_PERMS \
                         where (USER_NAME=\"%s\")&&(GROUP_NAME=\"%s\")",
			(const char *)RDEscapeString(xport_user->name()),
			(const char *)RDEscapeString(group_name));
  q=new RDSqlQuery(sql);
  if(!q->first()) {
    delete q;
    XmlExit("No such group",404);
  }

  //
  // Process Request
  //
  printf("Content-type: application/xml\n");
  printf("Status: 200\n\n");
  printf("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
  group=new RDGroup(q->value(0).toString());
  printf("%s",(const char *)group->xml());
  delete group;

  delete q;
  Exit(0);
}
