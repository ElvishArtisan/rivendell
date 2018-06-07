// schemamap.cpp
//
// DB schema version <==> Rivendell version map
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

#include <qstringlist.h>

#include "rddbmgr.h"

//
// Version -> Schema Map
//
std::map<QString,int> global_version_map;

void MainObject::InitializeSchemaMap() {
  global_version_map["1.0"]=159;
  global_version_map["1.1"]=162;
  global_version_map["1.2"]=169;
  global_version_map["1.3"]=173;
  global_version_map["1.4"]=177;
  global_version_map["1.5"]=179;
  global_version_map["1.6"]=182;
  global_version_map["1.7"]=186;
  global_version_map["2.0"]=202;
  global_version_map["2.1"]=205;
  global_version_map["2.2"]=207;
  global_version_map["2.3"]=213;
  global_version_map["2.4"]=216;
  global_version_map["2.5"]=220;
  global_version_map["2.6"]=224;
  global_version_map["2.7"]=231;
  global_version_map["2.8"]=234;
  global_version_map["2.9"]=239;
  global_version_map["2.10"]=242;
  global_version_map["2.11"]=245;
  global_version_map["2.12"]=254;
  global_version_map["2.13"]=255;
  global_version_map["2.14"]=258;
  global_version_map["2.15"]=259;
  global_version_map["2.16"]=263;
  global_version_map["2.17"]=268;
  global_version_map["2.18"]=272;
  global_version_map["2.19"]=275;
  global_version_map["2.20"]=286;
}


int MainObject::GetVersionSchema(const QString &ver) const
{
  QString version=ver;
  bool ok=false;

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
  if(global_version_map.count(f0[0]+"."+f0[1])==0) {
    return 0;
  }

  return global_version_map[f0[0]+"."+f0[1]];
}


QString MainObject::GetSchemaVersion(int schema) const
{
  QString prev_version="_preproduction";

  for(std::map<QString,int>::const_iterator it=global_version_map.begin();
      it!=global_version_map.end();it++) {
    if(it->second==schema) {
      return "v"+it->first+".x";
    }
    if(it->second>schema) {
      return tr("between")+" v"+prev_version+".x "+tr("and")+
	" v"+it->first+".x";
    } 
    prev_version=it->first;
  }
  return QString("after")+" v"+prev_version+".x";
}
