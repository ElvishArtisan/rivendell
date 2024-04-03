// rdweb.h
//
// Functions for interfacing with web components using the
// Common Gateway Interface (CGI) Standard 
//
//   (C) Copyright 1996-2024 Fred Gleason <fredg@paravelsystems.com>
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

#ifndef RDWEB_H
#define RDWEB_H

#include <QDateTime>
#include <QString>

#include <rdaudioconvert.h>

//
// Function Prototypes
//
extern void RDXMLResult(const char *str,int resp_code,
			RDAudioConvert::ErrorCode err=RDAudioConvert::ErrorOk);
extern QString RDXmlField(const QString &tag,const QString &value,
			  const QString &attrs="");
extern QString RDXmlField(const QString &tag,const char *value,
			  const QString &attrs="");
extern QString RDXmlField(const QString &tag,const int value,
			  const QString &attrs="");
extern QString RDXmlField(const QString &tag,const int64_t value,
			  const QString &attrs="");
extern QString RDXmlField(const QString &tag,const unsigned value,
			  const QString &attrs="");
extern QString RDXmlField(const QString &tag,const bool value,
			  const QString &attrs="");
extern QString RDXmlField(const QString &tag,const QDateTime &value,
			  const QString &attrs="");
extern QString RDXmlField(const QString &tag,const QDate &value,
			  const QString &attrs="");
extern QString RDXmlField(const QString &tag,const QTime &value,
			  const QString &attrs="");
extern QString RDXmlField(const QString &tag);
extern QString RDXmlEscape(const QString &str);
extern QString RDXmlUnescape(const QString &str);
extern QString RDUrlEscape(const QString &str);
extern QString RDUrlUnescape(const QString &str);


#endif  // RDWEB_H
