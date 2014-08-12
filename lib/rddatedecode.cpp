// rddatedecode.cpp
//
// Decode Rivendell Date Macros
//
//   (C) Copyright 2002-2004 Fred Gleason <fredg@paravelsystems.com>
//
//      $Id: rddatedecode.cpp,v 1.10.8.4 2012/05/10 23:40:16 cvs Exp $
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

#include <stdio.h>

#include <rddatedecode.h>

QString RDDateDecode(QString str,QDate date)
{
  QString string;
  int yearnum;
  int dow;
  bool upper_case=false;
  bool initial_case=false;
  QString field;
  int offset=0;

  for(unsigned i=0;i<str.length();i++) {
    field="";
    offset=0;
    if(str.at(i)!='%') {
      string+=str.at(i);
    }
    else {
      i++;
      offset++;
      if(((const char *)str)[i]=='^') {
	upper_case=true;
	i++;
	offset++;
      }
      else {
	upper_case=false;
      }
      if(((const char *)str)[i]=='$') {
	initial_case=true;
	i++;
	offset++;
      }
      else {
	initial_case=false;
      }
      switch(((const char *)str)[i]) {
      case 'a':   // Abbreviated weekday name
	field=QDate::shortDayName(date.dayOfWeek()).lower();
	break;

      case 'A':   // Full weekday name
	field=QDate::longDayName(date.dayOfWeek()).lower();
	break;

      case 'b':   // Abbreviated month name
      case 'h':
	field=QDate::shortMonthName(date.month()).lower();
      break;

      case 'B':   // Full month name
	field=QDate::longMonthName(date.month()).lower();
	break;

      case 'C':   // Century
	field=QString().sprintf("%02d",date.year()/100);
	break;

      case 'd':   // Day (01 - 31)
	field=QString().sprintf("%02d",date.day());
	break;

      case 'D':   // Date (mm-dd-yy)
	field=date.toString("dd-MM-yy");
	break;

      case 'e':   // Day ( 1 - 31)
	field=QString().sprintf("%2d",date.day());
	break;

      case 'E':   // Day (1 - 31)
	field=QString().sprintf("%d",date.day());
	break;

      case 'F':   // Date (yyyy-mm-dd)
	field=date.toString("yyyy-MM-dd");
	break;

      case 'g':   // Two digit year number (as per ISO 8601)
	date.weekNumber(&yearnum);
	field=QString().sprintf("%02d",yearnum-2000);
	break;

      case 'G':   // Four digit year number (as per ISO 8601)
	date.weekNumber(&yearnum);
	field=QString().sprintf("%04d",yearnum);
	break;

      case 'j':   // Day of year
	field=QString().sprintf("%03d",date.dayOfYear());
	break;

      case 'm':   // Month (01 - 12)
	field=QString().sprintf("%02d",date.month());
	break;

      case 'u':   // Day of week (numeric, 1..7, 1=Monday)
	field=QString().sprintf("%d",date.dayOfWeek());
	break;
	    
      case 'V':   // Week number (as per ISO 8601)
      case 'W':
	field=QString().sprintf("%d",date.weekNumber());
      break;

      case 'w':   // Day of week (numeric, 0..6, 0=Sunday)
	dow=date.dayOfWeek();
	if(dow==7) {
	  dow=0;
	}
	field=QString().sprintf("%d",dow);
	break;
	    
      case 'y':   // Year (yy)
	field=QString().sprintf("%02d",date.year()-2000);
	break;

      case 'Y':   // Year (yyyy)
	field=QString().sprintf("%04d",date.year());
	break;

      case '%':   // Literal '%'
	field=QString("%");
	break;

      default:   // No recognized wildcard, rollback!
	i-=offset;
	field=str.at(i);
	break;
      }
      if(upper_case) {
	field=field.upper();
      }
      if(initial_case) {
	field=field.left(1).upper()+field.right(field.length()-1);
      }
      string+=field;
    }
  }
  return string;
}


QString RDDateTimeDecode(QString str,QDateTime datetime)
{
  QString string;
  int yearnum;
  int dow;
  bool upper_case=false;
  bool initial_case=false;
  QString field;
  int offset=0;

  for(unsigned i=0;i<str.length();i++) {
    field="";
    offset=0;
    if(str.at(i)!='%') {
      string+=str.at(i);
    }
    else {
      i++;
      offset++;
      if(((const char *)str)[i]=='^') {
	upper_case=true;
	i++;
	offset++;
      }
      else {
	upper_case=false;
      }
      if(((const char *)str)[i]=='$') {
	initial_case=true;
	i++;
	offset++;
      }
      else {
	initial_case=false;
      }
      switch(((const char *)str)[i]) {
      case 'a':   // Abbreviated weekday name
	field=QDate::shortDayName(datetime.date().dayOfWeek()).lower();
	break;

      case 'A':   // Full weekday name
	field=QDate::longDayName(datetime.date().dayOfWeek()).lower();
	break;

      case 'b':   // Abbreviated month name
      case 'h':
	field=QDate::shortMonthName(datetime.date().month()).lower();
      break;

      case 'B':   // Full month name
	field=QDate::longMonthName(datetime.date().month()).lower();
	break;

      case 'C':   // Century
	field=QString().sprintf("%02d",datetime.date().year()/100);
	break;

      case 'd':   // Day (01 - 31)
	field=QString().sprintf("%02d",datetime.date().day());
	break;

      case 'D':   // Date (mm-dd-yy)
	field=datetime.date().toString("MM-dd-yy");
	break;

      case 'e':   // Day ( 1 - 31)
	field=QString().sprintf("%2d",datetime.date().day());
	break;

      case 'E':   // Day (1 - 31)
	field=QString().sprintf("%d",datetime.date().day());
	break;

      case 'F':   // Date (yyyy-mm-dd)
	field=datetime.date().toString("yyyy-MM-dd");
	break;

      case 'g':   // Two digit year number (as per ISO 8601)
	datetime.date().weekNumber(&yearnum);
	field=QString().sprintf("%02d",yearnum-2000);
	break;

      case 'G':   // Four digit year number (as per ISO 8601)
	datetime.date().weekNumber(&yearnum);
	field=QString().sprintf("%04d",yearnum);
	break;

      case 'H':   // Hour, zero padded, 24 hour
	field=QString().sprintf("%02d",datetime.time().hour());
	break;

      case 'I':   // Hour, zero padded, 12 hour
	field=QString().sprintf("%02d",datetime.time().hour()%12);
	break;

      case 'i':   // Hour, space padded, 12 hour
	field=QString().sprintf("%2d",datetime.time().hour()%12);
	break;

      case 'J':   // Hour, unpadded, 12 hour
	field=QString().sprintf("%d",datetime.time().hour()%12);
	break;

      case 'j':   // Day of year
	field=QString().sprintf("%03d",datetime.date().dayOfYear());
	break;

      case 'k':   // Hour, space padded, 24 hour
	field=QString().sprintf("%2d",datetime.time().hour());
	break;

      case 'M':   // Minute, zero padded
	field=QString().sprintf("%02d",datetime.time().minute());
	break;

      case 'm':   // Month (01 - 12)
	field=QString().sprintf("%02d",datetime.date().month());
	break;
	    
      case 'p':   // AM/PM string
	field=datetime.time().toString("ap");
	break;
	    
      case 'S':   // Second (SS)
	field=QString().sprintf("%02d",datetime.time().second());
	break;

      case 'u':   // Day of week (numeric, 1..7, 1=Monday)
	field=QString().sprintf("%d",datetime.date().dayOfWeek());
	break;
    
      case 'V':   // Week number (as per ISO 8601)
      case 'W':
	field=QString().sprintf("%d",datetime.date().weekNumber());
      break;

      case 'w':   // Day of week (numeric, 0..6, 0=Sunday)
	dow=datetime.date().dayOfWeek();
	if(dow==7) {
	  dow=0;
	}
	field=QString().sprintf("%d",dow);
	break;

      case 'y':   // Year (yy)
	field=QString().sprintf("%02d",datetime.date().year()-2000);
	break;

      case 'Y':   // Year (yyyy)
	field=QString().sprintf("%04d",datetime.date().year());
	break;

      case '%':   // Literal '%'
	field=QString("%");
	break;

      default:   // No recognized wildcard, rollback!
	i-=offset;
	field=str.at(i);
	break;
      }
      if(upper_case) {
	field=field.upper();
      }
      if(initial_case) {
	field=field.left(1).upper()+field.right(field.length()-1);
      }
      string+=field;
    }
  }
  return string;
}
