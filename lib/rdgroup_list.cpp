// rdgroup_list.cpp
//
// A container class for Rivendell Groups
//
//   (C) Copyright 2019 Fred Gleason <fredg@paravelsystems.com>
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

#include <rddb.h>
#include <rdgroup_list.h>

RDGroupList::RDGroupList()
{
  clear();
}


//
// Load list_groups with groups for specified service
//
void RDGroupList::loadSvc(QString svcname)
{
  QString sql;
  RDSqlQuery *q;

  clear();
  sql=QString().sprintf("select GROUP_NAME from AUDIO_PERMS where\
                         SERVICE_NAME=\"%s\"",
			(const char *)svcname);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    list_groups.push_back(QString(q->value(0).toString()));
  }
  delete q;
}

//
// Load list_groups with services for specified group
//
void RDGroupList::loadGroup(QString groupname)
{
  QString sql;
  RDSqlQuery *q;

  clear();
  sql=QString().sprintf("select SERVICE_NAME from AUDIO_PERMS where\
                         GROUP_NAME=\"%s\"",
			(const char *)groupname);
  q=new RDSqlQuery(sql);
  while(q->next()) {
    list_groups.push_back(QString(q->value(0).toString()));
  }
  delete q;
}


void RDGroupList::clear()
{
  list_groups.clear();
}


int RDGroupList::size() const
{
  return list_groups.size();
}


QString RDGroupList::group(unsigned n) const
{
  if(n<list_groups.size()) {
    return list_groups[n];
  }
  return QString();
}


bool RDGroupList::isGroupValid(QString name)
{
  for(unsigned i=0;i<list_groups.size();i++) {
      if(list_groups[i].upper()==name.upper()) {
      return true;
    }
  }
  return false;
}


//
// Return service name for index n
//
QString RDGroupList::service(unsigned n) const
{
  return group(n);
}

//
// Return true to service name exists
//
bool RDGroupList::isServiceValid(QString name)
{
  return isGroupValid(name);
}
