// rdescape_string.cpp
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

#include <vector>

#include <rdescape_string.h>
#include <qdatetime.h>

/**
 * RDCheckDateTime - Checks for QTime.isValid 
 * @param time - QTime object
 * @param format - QString representing time format e.g. HH:MM
 * @return QString, "NULL" if not Valid else formatted Time String
 */
QString RDCheckDateTime(QTime const &time, QString const &format)
{
  QString checkedValue = "NULL";
  
  if(time.isValid())
    checkedValue = "\"" + time.toString(format) + "\"";
  
  return checkedValue;
  
}

/**
 * RDCheckDateTime - Checks for QDateTime.isValid 
 * @param datetime - QDateTime object
 * @param format - QString representing date time format e.g. yyyy-mm-dd HH:MM
 * @return QString, "NULL" if not Valid else formatted DateTime String
 */
QString RDCheckDateTime(QDateTime const &datetime, QString const &format)
{
  QString checkedValue = "NULL";
  
  if(datetime.isValid())
    checkedValue = "\"" + datetime.toString(format) + "\"";
  
  return checkedValue;
  
}

/**
 * RDCheckDateTime - Checks for QDate.isValid 
 * @param date - QDate object
 * @param format - QString representing date format e.g. yyyy-mm-dd
 * @return QString, "NULL" if not Valid else formatted Date String
 */
QString RDCheckDateTime(QDate const &date, QString const &format)
{
  QString checkedValue = "NULL";
  
  if(date.isValid())
    checkedValue = "\"" + date.toString(format) + "\"";
  
  return checkedValue;
  
}

QString RDEscapeString(QString const &str)
{
  QString res;

  for(unsigned i=0;i<str.length();i++) {
    switch(((const char *)str)[i]) {
	case '(':
	  res+=QString("\\\(");
	  break;

	case ')':
	  res+=QString("\\)");
	  break;

	case '{':
	  res+=QString("\\\{");
	  break;

	case '"':
	  res+=QString("\\\"");
	  break;

	case '`':
	  res+=QString("\\`");
	  break;

	case '[':
	  res+=QString("\\\[");
	  break;

	case '\'':
	  res+=QString("\\\'");
	  break;

	case '\\':
	  res+=QString("\\");
	  res+=QString("\\");
	  break;

	case '?':
	  res+=QString("\\\?");
	  break;

	case ' ':
	  res+=QString("\\ ");
	  break;

	case '&':
	  res+=QString("\\&");
	  break;

        case ';':
	  res+=QString("\\;");
	  break;

        case '<':
	  res+=QString("\\<");
	  break;

        case '>':
	  res+=QString("\\>");
	  break;

        case '|':
	  res+=QString("\\|");
	  break;

	default:
	  res+=((const char *)str)[i];
	  break;
    }
  }

  return res;
}


QString RDEscapeShellString(QString str)
{
  return "\""+str.replace("$","\\$")+"\"";
}
