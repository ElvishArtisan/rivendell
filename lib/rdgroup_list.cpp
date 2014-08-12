// rdgroup_list.cpp
//
// A container class for Rivendell Groups
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdgroup_list.cpp,v 1.9 2010/07/29 19:32:33 cvs Exp $
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


bool RDGroupList::isGroupValid(QString group)
{
  for(unsigned i=0;i<list_groups.size();i++) {
      if(list_groups[i].upper()==group.upper()) {
      return true;
    }
  }
  return false;
}
