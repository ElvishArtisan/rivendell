// createdb.h
//
// Create a Rivendell Database
//
//   (C) Copyright 2002-2017 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef CREATEDB_H
#define CREATEDB_H

#include <qstring.h>

#include <rdconfig.h>

//
// UpdateDB return codes
//
#define UPDATEDB_SUCCESS 0
#define UPDATEDB_BACKUP_FAILED -1
#define UPDATEDB_QUERY_FAILED -2

bool CreateDb(QString name,QString pwd,RDConfig *config);
bool InitDb(QString name,QString pwd,QString stationname,RDConfig *config);
int UpdateDb(int ver,RDConfig *config);


#endif  // CREATEDB_H

