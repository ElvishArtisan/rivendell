// printstatus.cpp
//
// Print the status of a database.
//
//   (C) Copyright 2018-2022 Fred Gleason <fredg@paravelsystems.com>
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

#include "rddbmgr.h"

bool MainObject::PrintStatus(QString *err_msg) const
{
  QString sql;
  RDSqlQuery *q;
  int schema;

  sql=QString("show tables");
  q=new RDSqlQuery(sql,false);
  if(!q->first()) {
    delete q;
    printf("empty database\n");
    return true;
  }
  delete q;

  if((schema=GetCurrentSchema())<=0) {
    printf("not a Rivendell database!\n");
    return true;
  }
  if(schema>=353) {
    sql=QString("select ")+
      "`REALM_NAME` "+  // 00
      "from `SYSTEM`";
    q=new RDSqlQuery(sql);
    if(q->first()) {
      printf("Rivendell database, schema %d [%s] (%s)\n",schema,
	     GetSchemaVersion(schema).toUtf8().constData(),
	     q->value(0).toString().toUtf8().constData());
    }
    else {
      fprintf(stderr,"rddbmgr: WARNING - unable to read Realm value.\n");
      printf("Rivendell database, schema %d [%s]\n",schema,
	     GetSchemaVersion(schema).toUtf8().constData());
    }
    delete q;
  }
  else {
    printf("Rivendell database, schema %d [%s]\n",schema,
	   GetSchemaVersion(schema).toUtf8().constData());
  }

  *err_msg="ok";
  return true;
}
