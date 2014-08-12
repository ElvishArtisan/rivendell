// rdrdcreateauxfieldstable.cpp
//
// Create a new, empty Rivendell log table.
//
//   (C) Copyright 2002-2003 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rdcreateauxfieldstable.cpp,v 1.4 2010/07/29 19:32:33 cvs Exp $
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

void RDCreateAuxFieldsTable(QString keyname)
{
  keyname.replace(" ","_");
  QString sql=QString().sprintf("create table if not exists %s_FIELDS (\
                                 CAST_ID int unsigned not null primary key)",
				(const char *)keyname);
  RDSqlQuery *q=new RDSqlQuery(sql);
  delete q;
}
