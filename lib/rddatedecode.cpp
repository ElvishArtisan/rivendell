// rddatedecode.cpp
//
// Decode Rivendell Date Macros
//
//   (C) Copyright 2002-2021 Fred Gleason <fredg@paravelsystems.com>
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

#include "rdconf.h"
#include "rddatedecode.h"
#include "rddatetime.h"

QString __RDDateCode_TZFormat(int level)
{
  int offset=RDTimeZoneOffset();
  int hours=abs(offset/3600);
  int minutes=(abs(offset)-3600*hours)/60;
  int seconds=abs(offset)-3600*hours-60*minutes;
  QString ret;

  switch(level) {
  case 0:
    ret=QString::asprintf("%02d",hours).left(2)+
      QString::asprintf("%02d",minutes).left(2);
    break;

  case 1:
    ret=QString::asprintf("%02d",hours).left(2)+":"+
      QString::asprintf("%02d",minutes).left(2);
    break;

  case 2:
    ret=QString::asprintf("%02d",hours).left(2)+":"+
      QString::asprintf("%02d",minutes).left(2)+":"+
      QString::asprintf("%02d",seconds).left(2);
    break;
  }
  if(offset<0) {
    ret="+"+ret;
  }
  else {
    ret="-"+ret;
  }

  return ret;
}

QString RDDateDecode(QString str,const QDate &date,RDStation *station,
		     RDConfig *config,const QString &svcname)
{
  QString string;
  int yearnum;
  int dow;
  bool upper_case=false;
  bool initial_case=false;
  QString field;
  int offset=0;
  int dsecs=0;
  QString dsecs_str;

  for(int i=0;i<str.length();i++) {
    field="";
    offset=0;
    if(str.at(i)!=QChar('%')) {
      string+=str.at(i);
    }
    else {
      if(i>=(str.length()-1)) {  // Out of characters to examine!
	return str;
      }

      //
      // Delta Seconds Modifier
      //
      dsecs=0;
      dsecs_str="";
      i++;
      offset++;
      if((str.at(i)==QChar('+'))||(str.at(i)==QChar('-'))||
	 (str.at(i).category()==QChar::Number_DecimalDigit)) {
	dsecs_str=str.at(i);
	i++;
	offset++;
	while((i<str.length()-1)&&
	      (str.at(i).category()==QChar::Number_DecimalDigit)) {
	  dsecs_str+=str.at(i);
	  dsecs=dsecs_str.toInt();
	  i++;
	  offset++;
	}
	i--;
	offset--;
      }
      else {
	i--;
	offset--;
      }
      QDate dt(date.addDays(dsecs/86400));

      if(i>=(str.length()-1)) {  // Out of characters to examine!
	return str;
      }

      //
      // All Upcase Modifier
      //
      i++;
      offset++;
      if(str.at(i)==QChar('^')) {
	upper_case=true;
	i++;
	offset++;
      }
      else {
	upper_case=false;
      }

      //
      // Initial Upcase Modifier
      //
      if(str.at(i)==QChar('$')) {
	initial_case=true;
	i++;
	offset++;
      }
      else {
	initial_case=false;
      }

      //
      // Wildcard Lookup
      //
      switch(str.at(i).toLatin1()) {
      case 'a':   // Abbreviated weekday name
	field=QDate::shortDayName(dt.dayOfWeek()).toLower();
	break;

      case 'A':   // Full weekday name
	field=QDate::longDayName(dt.dayOfWeek()).toLower();
	break;

      case 'b':   // Abbreviated month name
      case 'h':
	field=QDate::shortMonthName(dt.month()).toLower();
      break;

      case 'B':   // Full month name
	field=QDate::longMonthName(dt.month()).toLower();
	break;

      case 'C':   // Century
	field=QString::asprintf("%02d",dt.year()/100);
	break;

      case 'd':   // Day (01 - 31)
	field=QString::asprintf("%02d",dt.day());
	break;

      case 'D':   // Date (mm-dd-yy)
	field=dt.toString("dd-MM-yy");
	break;

      case 'e':   // Day ( 1 - 31)
	field=QString::asprintf("%2d",dt.day());
	break;

      case 'E':   // Day (1 - 31)
	field=QString::asprintf("%d",dt.day());
	break;

      case 'F':   // Date (yyyy-mm-dd)
	field=dt.toString("yyyy-MM-dd");
	break;

      case 'g':   // Two digit year number (as per ISO 8601)
	dt.weekNumber(&yearnum);
	field=QString::asprintf("%02d",yearnum-2000);
	break;

      case 'G':   // Four digit year number (as per ISO 8601)
	dt.weekNumber(&yearnum);
	field=QString::asprintf("%04d",yearnum);
	break;

      case 'j':   // Day of year
	field=QString::asprintf("%03d",dt.dayOfYear());
	break;

      case 'l':   // Unpadded Month (1 - 12)
	field=QString::asprintf("%d",dt.month());
	break;

      case 'm':   // Month (01 - 12)
	field=QString::asprintf("%02d",dt.month());
	break;

      case 'r':   // Rivendell Host Name
	field=config->stationName();
	break;

      case 'R':   // Rivendell Host Short Name
	field=station->shortName();
	break;

      case 's':   // Service name
	if(!svcname.isEmpty()) {
	  field=svcname;
	}
	break;

      case 'u':   // Day of week (numeric, 1..7, 1=Monday)
	field=QString::asprintf("%d",dt.dayOfWeek());
	break;
	    
      case 'V':   // Week number (as per ISO 8601)
      case 'W':
	field=QString::asprintf("%d",dt.weekNumber());
      break;

      case 'w':   // Day of week (numeric, 0..6, 0=Sunday)
	dow=dt.dayOfWeek();
	if(dow==7) {
	  dow=0;
	}
	field=QString::asprintf("%d",dow);
	break;
	    
      case 'y':   // Year (yy)
	field=QString::asprintf("%02d",dt.year()-2000);
	break;

      case 'Y':   // Year (yyyy)
	field=QString::asprintf("%04d",dt.year());
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
	field=field.toUpper();
      }
      if(initial_case) {
	field=field.left(1).toUpper()+field.right(field.length()-1);
      }
      string+=field;
    }
  }
  return string;
}


QString RDDateTimeDecode(QString str,const QDateTime &datetime,
			 RDStation *station,RDConfig *config,
			 const QString &svcname)
{
  QString string;
  int yearnum;
  int dow;
  bool upper_case=false;
  bool initial_case=false;
  QString field;
  int offset=0;
  int dsecs=0;
  QString dsecs_str;

  for(int i=0;i<str.length();i++) {
    field="";
    offset=0;
    if(str.at(i)!=QChar('%')) {
      string+=str.at(i);
    }
    else {
      if(i>=(str.length()-1)) {  // Out of characters to examine!
	return str;
      }

      //
      // Delta Seconds Modifier
      //
      dsecs=0;
      dsecs_str="";
      i++;
      offset++;
      if((str.at(i)==QChar('+'))||(str.at(i)==QChar('-'))||
	 (str.at(i).category()==QChar::Number_DecimalDigit)) {
	dsecs_str=str.at(i);
	i++;
	offset++;
	while((i<str.length()-1)&&
	      (str.at(i).category()==QChar::Number_DecimalDigit)) {
	  dsecs_str+=str.at(i);
	  dsecs=dsecs_str.toInt();
	  i++;
	  offset++;
	}
	i--;
	offset--;
      }
      else {
	i--;
	offset--;
      }
      QDateTime dt(datetime.addSecs(dsecs));
      if(i>=(str.length()-1)) {  // Out of characters to examine!
	return str;
      }


      //
      // All Upcase Modifier
      //
      i++;
      offset++;
      if(str.at(i)==QChar('^')) {
	upper_case=true;
	i++;
	offset++;
      }
      else {
	upper_case=false;
      }

      //
      // Initial Upcase Modifier
      //
      if(str.at(i)==QChar('$')) {
	initial_case=true;
	i++;
	offset++;
      }
      else {
	initial_case=false;
      }

      //
      // Wildcard Lookup
      //
      switch(str.at(i).toLatin1()) {
      case 'a':   // Abbreviated weekday name
	field=QDate::shortDayName(dt.date().dayOfWeek()).toLower();
	break;

      case 'A':   // Full weekday name
	field=QDate::longDayName(dt.date().dayOfWeek()).toLower();
	break;

      case 'b':   // Abbreviated month name
      case 'h':
	field=QDate::shortMonthName(dt.date().month()).toLower();
      break;

      case 'B':   // Full month name
	field=QDate::longMonthName(dt.date().month()).toLower();
	break;

      case 'C':   // Century
	field=QString::asprintf("%02d",dt.date().year()/100);
	break;

      case 'd':   // Day (01 - 31)
	field=QString::asprintf("%02d",dt.date().day());
	break;

      case 'D':   // Date (mm-dd-yy)
	field=dt.date().toString("MM-dd-yy");
	break;

      case 'e':   // Day ( 1 - 31)
	field=QString::asprintf("%2d",dt.date().day());
	break;

      case 'E':   // Day (1 - 31)
	field=QString::asprintf("%d",dt.date().day());
	break;

      case 'F':   // Date (yyyy-mm-dd)
	field=dt.date().toString("yyyy-MM-dd");
	break;

      case 'g':   // Two digit year number (as per ISO 8601)
	dt.date().weekNumber(&yearnum);
	field=QString::asprintf("%02d",yearnum-2000);
	break;

      case 'G':   // Four digit year number (as per ISO 8601)
	dt.date().weekNumber(&yearnum);
	field=QString::asprintf("%04d",yearnum);
	break;

      case 'H':   // Hour, zero padded, 24 hour
	field=QString::asprintf("%02d",dt.time().hour());
	break;

      case 'I':   // Hour, zero padded, 12 hour
	field=QString::asprintf("%02d",dt.time().hour()%12);
	break;

      case 'i':   // Hour, space padded, 12 hour
	field=QString::asprintf("%2d",dt.time().hour()%12);
	break;

      case 'J':   // Hour, unpadded, 12 hour
	field=QString::asprintf("%d",dt.time().hour()%12);
	break;

      case 'j':   // Day of year
	field=QString::asprintf("%03d",dt.date().dayOfYear());
	break;

      case 'k':   // Hour, space padded, 24 hour
	field=QString::asprintf("%2d",dt.time().hour());
	break;

      case 'l':   // Unpadded Month (1 - 12)
	field=QString::asprintf("%d",dt.date().month());
	break;
	    
      case 'M':   // Minute, zero padded
	field=QString::asprintf("%02d",dt.time().minute());
	break;

      case 'm':   // Month (01 - 12)
	field=QString::asprintf("%02d",dt.date().month());
	break;
	    
      case 'p':   // AM/PM string
	field=dt.time().toString("ap");
	break;
	    
      case 'r':   // Rivendell Host Name
	field=config->stationName();
	break;

      case 'R':   // Rivendell Host Short Name
	field=station->shortName();
	break;

      case 'S':   // Second (SS)
	field=QString::asprintf("%02d",dt.time().second());
	break;

      case 's':   // Service name
	if(!svcname.isEmpty()) {
	  field=svcname;
	}
	break;

      case 'u':   // Day of week (numeric, 1..7, 1=Monday)
	field=QString::asprintf("%d",dt.date().dayOfWeek());
	break;
    
      case 'V':   // Week number (as per ISO 8601)
      case 'W':
	field=QString::asprintf("%d",dt.date().weekNumber());
      break;

      case 'w':   // Day of week (numeric, 0..6, 0=Sunday)
	dow=dt.date().dayOfWeek();
	if(dow==7) {
	  dow=0;
	}
	field=QString::asprintf("%d",dow);
	break;

      case 'y':   // Year (yy)
	field=QString::asprintf("%02d",dt.date().year()-2000);
	break;

      case 'Y':   // Year (yyyy)
	field=QString::asprintf("%04d",dt.date().year());
	break;

      case 'z':   // +hhmm numeric time zone (e.g., -0400)
	field=__RDDateCode_TZFormat(0);
	break;

      case ':':   // Extended numeric timezones
	if((str.length()>(i+1))&&(str.at(i+1)==QChar('z'))) {
	  field=__RDDateCode_TZFormat(1);
	  i++;
	}
	else {
	  if((str.length()>(i+2))&&(str.at(i+1)==QChar(':'))&&
	     (str.at(i+2)==QChar('z'))) {
	    field=__RDDateCode_TZFormat(2);
	    i+=2;
	  }
	}
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
	field=field.toUpper();
      }
      if(initial_case) {
	field=field.left(1).toUpper()+field.right(field.length()-1);
      }
      string+=field;
    }
  }

  return string;
}
