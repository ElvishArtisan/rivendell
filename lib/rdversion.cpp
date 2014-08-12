// rdversion.cpp
//
// Get / Set Version Numbers of Rivendell Components
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdversion.cpp,v 1.8 2010/07/29 19:32:34 cvs Exp $
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

#include <rd.h>
#include <rddb.h>
#include <rdversion.h>


//
// Global Classes
//
RDVersion::RDVersion()
{
}


int RDVersion::database()
{
  int ver;

  RDSqlQuery *q=new RDSqlQuery("select DB from VERSION");
  if(!q->first()) {
    delete q;
    return 0;
  }
  ver=q->value(0).toInt();
  delete q;
  return ver;
}


void RDVersion::setDatabase(int ver)
{
  RDSqlQuery *q;
  QString sql;

  sql=QString().sprintf("update VERSION set DB=%d",ver);
  q=new RDSqlQuery(sql);
  delete q;
}
