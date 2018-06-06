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

#include <dbversion.h>
#include <rddb.h>

#include "rddbmgr.h"

bool MainObject::Modify(QString *err_msg,int set_schema,
			const QString &set_version) const
{
  *err_msg="ok";

  //
  // Determine Target Schema
  //
  if(set_schema>0) {
    if((set_schema<242)||(set_schema>RD_VERSION_DATABASE)) {
      *err_msg="unsupported schema";
      return false;
    }
  }
  else {
    if(set_version.isEmpty()) {
      set_schema=RD_VERSION_DATABASE;
    }
    else {
      if((set_schema=GetVersionSchema(set_version))==0) {
	*err_msg="invalid/unsupported Rivendell version";
	return false;
      }
    }
  }

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
  RDSqlQuery *q=new RDSqlQuery(sql);
  if(q->first()) {
    ret=q->value(0).toInt();
  }
  delete q;

  return ret;
}


int MainObject::GetVersionSchema(const QString &ver) const
{
  QString version=ver;
  bool ok=false;

  //
  // Version -> Schema Map
  //
  std::map<QString,int> version_map;
  version_map["2.10"]=242;
  version_map["2.11"]=245;
  version_map["2.12"]=254;
  version_map["2.13"]=255;
  version_map["2.14"]=258;
  version_map["2.15"]=259;
  version_map["2.16"]=263;
  version_map["2.17"]=268;
  version_map["2.18"]=272;
  version_map["2.19"]=275;
  version_map["2.20"]=286;

  //
  // Normalize String
  //
  if(version.left(1).lower()=="v") {
    version=version.right(version.length()-1);
  }
  QStringList f0=f0.split(".",version);
  if(f0.size()!=3) {
    return 0;
  }
  for(int i=0;i<3;i++) {
    f0[i].toInt(&ok);
    if(!ok) {
      return 0;
    }
  }

  //
  // Lookup Schema
  //
  if(version_map.count(f0[0]+"."+f0[1])==0) {
    return 0;
  }

  return version_map[f0[0]+"."+f0[1]];
}
