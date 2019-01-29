// rddatetime.h
//
// Parse and write dates/times in various standard formats. 
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

#ifndef RDDATETIME_H
#define RDDATETIME_H

#include <qstring.h>
#include <qdatetime.h>

//
// Function Prototypes
//

//
// Auto-detect the format (XML xs:dateTime or RFC822)
//
QDateTime RDParseDateTime(const QString &str,bool *ok);

//
// XML xs:date format
//
QDate RDParseXmlDate(const QString &str,bool *ok);
QString RDWriteXmlDate(const QDate &date);

//
// XML xs:time format
//
QTime RDParseXmlTime(const QString &str,bool *ok,int *day_offset=NULL);
QString RDWriteXmlTime(const QTime &time);

//
// XML xs:dateTime format
//
QDateTime RDParseXmlDateTime(const QString &str,bool *ok);
QString RDWriteXmlDateTime(const QDateTime &dt);

//
// RFC822 date/time format
//
QDateTime RDParseRfc822DateTime(const QString &str,bool *ok);
QString RDWriteRfc822DateTime(const QDateTime &dt);

//
// Returns the UTC offset of the curently configured timezone (seconds)
//
int RDTimeZoneOffset();


#endif  // RDDATETIME_H
