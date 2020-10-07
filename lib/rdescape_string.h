// rdescape_string.h
//
// Escape non-valid characters in a string.
//
//   (C) Copyright 2002-2005,2016-2017 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDESCAPE_STRING_H
#define RDESCAPE_STRING_H

#include <qbytearray.h>
#include <qdatetime.h>
#include <qstring.h>

QString RDCheckDateTime(const QTime &time, const QString &format);
QString RDCheckDateTime(const QDateTime &datetime, const QString &format);
QString RDCheckDateTime(const QDate &date, const QString &format);
QString RDEscapeString(const QString &str);
QString RDEscapeShellString(QString str);
QString RDEscapeBlob(const QByteArray &data);
QString RDEscapeBlob(const char *data,size_t len);


#endif  // RDESCAPE_STRING_H
