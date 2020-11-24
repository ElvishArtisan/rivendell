// schemamap.cpp
//
// DB schema version <==> Rivendell version map
//
//   (C) Copyright 2018-2020 Fred Gleason <fredg@paravelsystems.com>
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

#include <qmap.h>
#include <qstringlist.h>

#include "rddbmgr.h"

class VersionString : public QString
{
 public:
  VersionString();
  VersionString(const char *str);
  int major() const;
  int minor() const;
  int point() const;
  bool operator<(const VersionString &rhs) const;
  bool operator==(const VersionString &rhs) const;

private:
  int ver_major;
  int ver_minor;
  int ver_point;
};


VersionString::VersionString()
  : QString()
{
  ver_major=0;
  ver_minor=0;
  ver_point=0;
}


VersionString::VersionString(const char *str)
  : QString(str)
{
  QStringList f0=f0.split(".",str);
  ver_major=f0[0].toInt();
  if(f0.size()>=2) {
    ver_minor=f0[1].toInt();
    if(f0.size()>=3) {
      ver_point=f0[2].toInt();
    }
  }
}


int VersionString::major() const
{
  return ver_major;
}


int VersionString::minor() const
{
  return ver_minor;
}


int VersionString::point() const
{
  return ver_point;
}


bool VersionString::operator<(const VersionString &rhs) const
{
  if(major()<rhs.major()) {
    return true;
  }
  if(major()>rhs.major()) {
    return false;
  }
  if(minor()<rhs.minor()) {
    return true;
  }
  if(minor()>rhs.minor()) {
    return false;
  }
  return false;
}


bool VersionString::operator==(const VersionString &rhs) const
{
  return (major()==rhs.major())&&(minor()==rhs.minor());
}


//
// Version -> Schema Map
//
QMap<VersionString,int> global_version_map;

void MainObject::InitializeSchemaMap() {
  //
  // Maintainer's Note
  //
  // With the (possible) exception of the very last entry, the items in
  // this mapping should be considered immutable. The value of the last
  // entry should be either that of the current production minor release
  // (if no subsequent schema updates have been made), or that of the *next*
  // (upcoming) release. If this is the first schema change made after a
  // public release, a line referencing the next (upcoming) release should
  // be added and its value set to that of the new schema version. Otherwise,
  // it should be incremented every time a schema update is committed.
  //
  // When the first production release in the minor version referenced
  // on the last line occurs, that line too is declared immutable, and
  // the cycle begins again.
  //
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
  global_version_map["3.0"]=308;
  global_version_map["3.1"]=310;
  global_version_map["3.2"]=311;
  global_version_map["3.3"]=314;
  global_version_map["3.4"]=317;
  global_version_map["3.5"]=346;
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
  if(global_version_map.count(VersionString(f0[0]+"."+f0[1]))==0) {
    return 0;
  }

  return global_version_map.value(VersionString(f0[0]+"."+f0[1]));
}


QString MainObject::GetSchemaVersion(int schema) const
{
  QString prev_version="_preproduction";

  for(QMap<VersionString,int>::const_iterator it=global_version_map.begin();
      it!=global_version_map.end();it++) {
    if(it.data()==schema) {
      return "v"+it.key()+".x";
    }
    if(it.data()>schema) {
      return tr("between")+" v"+prev_version+".x "+tr("and")+
	" v"+it.key()+".x";
    } 
    prev_version=it.key();
  }
  return QString("after")+" v"+prev_version+".x";
}
