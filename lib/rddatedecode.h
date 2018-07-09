// rddatedecode.h
//
// Decode Rivendell Filepath Macros
//
//   (C) Copyright 2002-2004,2016-2018 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDDATEDECODE_H
#define RDDATEDECODE_H

#include <qdatetime.h>

QString RDNameDecode(QString str,const QString &station_name,
		     const QString &short_station_name,
		     const QString &svcname);
QString RDDateDecode(QString str,const QDate &date,const QString &station_name,
		     const QString &short_station_name,
		     const QString &svcname="");
QString RDDateTimeDecode(QString str,const QDateTime &datetime,
			 const QString &station_name,
			 const QString &short_station_name,
			 const QString &svcname="");


#endif  // RDDATEDECODE
