// schedcodes.h
//
// Rivendell web service portal
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

#include "rddb.h"
#include "rdschedcode.h"
#include "rdxport.h"

void Xport::ListSchedCodes()
{
  QString sql;
  RDSqlQuery *q;
  RDSchedCode *schedcode;

  //
  // Generate Scheduler Code List
  //
  sql=QString("select CODE from SCHED_CODES order by CODE");
  q=new RDSqlQuery(sql);

  //
  // Process Request
  //
  printf("Content-type: application/xml\n");
  printf("Status: 200\n\n");
  printf("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
  printf("<schedCodeList>\n");
  while(q->next()) {
    schedcode=new RDSchedCode(q->value(0).toString());
    printf("%s",(const char *)schedcode->xml().utf8());
    delete schedcode;
  }
  printf("</schedCodeList>\n");

  delete q;
  Exit(0);
}
