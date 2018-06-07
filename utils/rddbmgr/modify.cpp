// modify.cpp
//
// Routines for --modify for rddbmgr(8)
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

#include <dbversion.h>
#include <rddb.h>

#include "rddbmgr.h"

bool MainObject::Modify(QString *err_msg,int set_schema) const
{
  *err_msg="ok";

  //
  // Update/Revert
  //
  int current_schema=GetCurrentSchema();
  if(current_schema==0) {
    *err_msg="unable to determine DB schema, aborting";
    return false;
  }
  if(current_schema>RD_VERSION_DATABASE) {
    *err_msg="unable to modify, unknown current schema";
    return false;
  }
  if(set_schema>current_schema) {
    return UpdateSchema(current_schema,set_schema,err_msg);
  }
  if(set_schema<current_schema) {
    return RevertSchema(current_schema,set_schema,err_msg);
  }
  return true;
}


int MainObject::GetCurrentSchema() const
{
  int ret=0;

  QString sql=QString("select DB from VERSION");
  QSqlQuery *q=new QSqlQuery(sql);
  if(q->first()) {
    ret=q->value(0).toInt();
  }
  delete q;

  return ret;
}


