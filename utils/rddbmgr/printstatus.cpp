// printstatus.cpp
//
// Print the status of a database.
//
//   (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdlib.h>

#include <rddb.h>

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

  printf("Rivendell database, schema %d [%s]\n",schema,
	 (const char *)GetSchemaVersion(schema));

  *err_msg="ok";
  return true;
}
