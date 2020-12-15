// rdgroup_list.cpp
//
// A list container for Rivendell Groups
//
//   (C) Copyright 2002-2020 Fred Gleason <fredg@paravelsystems.com>
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
#include "rdescape_string.h"
#include "rdgroup_list.h"

RDGroupList::RDGroupList()
{
  clear();
}


QString RDGroupList::serviceName() const
{
  return d_service_name;
}


void RDGroupList::setServiceName(const QString &str)
{
  if(d_service_name!=str) {
    QString sql;
    RDSqlQuery *q;

    clear();
    sql=QString("select ")+
      "GROUP_NAME "+  // 00
      "from AUDIO_PERMS where "+
      "SERVICE_NAME=\""+RDEscapeString(str)+"\"";
    q=new RDSqlQuery(sql);
    while(q->next()) {
      d_groups.push_back(QString(q->value(0).toString()));
    }
    delete q;
    d_service_name=str;
  }
}


void RDGroupList::clear()
{
  d_groups.clear();
}


int RDGroupList::size() const
{
  return d_groups.size();
}


QString RDGroupList::group(int n) const
{
  if(n<d_groups.size()) {
    return d_groups.at(n);
  }
  return QString();
}


bool RDGroupList::groupIsValid(QString group)
{
  for(int i=0;i<d_groups.size();i++) {
    if(d_groups.at(i).toUpper()==group.toUpper()) {
      return true;
    }
  }
  return false;
}
