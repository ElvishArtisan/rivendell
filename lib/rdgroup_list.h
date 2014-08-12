// rdgroup_list.h
//
// A container class for Rivendell Groups
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdgroup_list.h,v 1.7 2010/07/29 19:32:33 cvs Exp $
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

#ifndef RDGROUP_LIST_H
#define RDGROUP_LIST_H

#include <vector>
#include <qsqldatabase.h>

using namespace std;

class RDGroupList
{
 public:
  RDGroupList();
  void loadSvc(QString svcname);
  void clear();
  int size() const;
  QString group(unsigned n) const;
  bool isGroupValid(QString group);

 private:
  vector<QString> list_groups;
};


#endif 
