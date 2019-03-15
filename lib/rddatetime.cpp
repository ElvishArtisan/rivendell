// rddatetime.cpp
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

#include <stdio.h>
#include <time.h>

#include <qmap.h>
#include <qstringlist.h>

#include "rddatetime.h"

QString __rddatetime_month_names[]=
  {"Jan","Feb","Mar","Apr","Mar","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
QString __rddatetime_dow_names[]=
  {"Mod","Tue","Wed","Thu","Fri","Sat","Sun"};

//
// Auto-detect the format (XML xs:dateTime or RFC822)
//
QDateTime RDParseDateTime(const QString &str,bool *ok)
{
  if(str.trimmed().contains(" ")) {
    return RDParseRfc822DateTime(str,ok);
  }
  return RDParseXmlDateTime(str,ok);
}


//
// XML xs:date format
//
QDate RDParseXmlDate(const QString &str,bool *ok)
{
  QDate ret=QDate::fromString(str,"yyyy-MM-dd");
  if(ok!=NULL) {
    *ok=ret.isValid();
  }
  return ret;
}


QString RDWriteXmlDate(const QDate &date)
{
  return date.toString("yyyy-MM-dd");
}


//
// XML xs:time format
//
QTime RDParseXmlTime(const QString &str,bool *ok,int *day_offset)
{
  QTime ret;
  QStringList f0;
  QStringList f1;
  QStringList f2;
  int tz=0;
  QTime time;
  QTime tztime;

  if(ok!=NULL) {
    *ok=false;
  }
  if(day_offset!=NULL) {
    *day_offset=0;
  }
  f0=str.trimmed().split(" ");
  if(f0.size()!=1) {
    if(ok!=NULL) {
      *ok=false;
    }
    return ret;
  }

  if(f0[0].right(1).lower()=="z") {  // GMT
    tz=-RDTimeZoneOffset();
    f0[0]=f0[0].left(f0[0].length()-1);
    f2=f0[0].split(":");
  }
  else {
    f1=f0[0].split("+");
    if(f1.size()==2) {   // GMT+
      f2=f1[1].split(":");
      if(f2.size()==2) {
	tztime=QTime(f2[0].toInt(),f2[1].toInt(),0);
	if(tztime.isValid()) {
	  tz=-RDTimeZoneOffset()-QTime(0,0,0).secsTo(tztime);
	}
      }
      else {
	if(ok!=NULL) {
	  *ok=false;
	}
	return QTime();
      }
    }
    else {
      f1=f0[0].split("-");
      if(f1.size()==2) {   // GMT-
	f2=f1[1].split(":");
	if(f2.size()==2) {
	  tztime=QTime(f2[0].toInt(),f2[1].toInt(),0);
	  if(tztime.isValid()) {
	    tz=-RDTimeZoneOffset()+QTime(0,0,0).secsTo(tztime);
	  }
	}
	else {
	  if(ok!=NULL) {
	    *ok=false;
	  }
	  return QTime();
	}
      }
    }
    f2=f1[0].split(":");
  }
  if(f2.size()==3) {
    QStringList f3=f2[2].split(".");
    time=QTime(f2[0].toInt(),f2[1].toInt(),f2[2].toInt());
    if(time.isValid()) {
      ret=time.addSecs(tz);
      if(day_offset!=NULL) {
	if((tz<0)&&((3600*time.hour()+60*time.minute()+time.second())<(-tz))) {
	  *day_offset=-1;
	}
	if((tz>0)&&(86400-((3600*time.hour()+60*time.minute()+time.second()))<tz)) {
	  *day_offset=1;
	}
      }
      if(ok!=NULL) {
	*ok=true;
      }
    }
  }
  return ret;
}


QString RDWriteXmlTime(const QTime &time)
{
  int utc_off=RDTimeZoneOffset();
  QString tz_str="-";
  if(utc_off<0) {
    tz_str="+";
  }
  tz_str+=QString().
    sprintf("%02d:%02d",utc_off/3600,(utc_off-3600*(utc_off/3600))/60);

  return time.toString("hh:mm:ss")+tz_str;
}


//
// XML xs:dateTime format
//
QDateTime RDParseXmlDateTime(const QString &str,bool *ok)
{
  QDateTime ret;
  QStringList list;
  QStringList f0;
  QStringList f1;
  QStringList f2;
  int day;
  int month;
  int year;
  QTime time;
  bool lok=false;
  int day_offset=0;

  if(ok!=NULL) {
    *ok=false;
  }

  f0=str.trimmed().split(" ");
  if(f0.size()!=1) {
    if(ok!=NULL) {
      *ok=false;
    }
    return ret;
  }
  f1=f0[0].split("T");
  if(f1.size()<=2) {
    f2=f1[0].split("-");
    if(f2.size()==3) {
      year=f2[0].toInt(&lok);
      if(lok&&(year>0)) {
	month=f2[1].toInt(&lok);
	if(lok&&(month>=1)&&(month<=12)) {
	  day=f2[2].toInt(&lok);
	  if(lok&&(day>=1)&&(day<=31)) {
	    if(f1.size()==2) {
	      time=RDParseXmlTime(f1[1],&lok,&day_offset);
	      if(lok) {
		ret=QDateTime(QDate(year,month,day),time).addDays(day_offset);
		if(ok!=NULL) {
		  *ok=true;
		}
	      }
	    }
	  }
	}
      }
    }
  }
  return ret;
}


QString RDWriteXmlDateTime(const QDateTime &dt)
{
  return RDWriteXmlDate(dt.date())+"T"+RDWriteXmlTime(dt.time());
}


//
// RFC822 date/time format
//
QDateTime RDParseRfc822DateTime(const QString &str,bool *ok)
{
  QStringList f0=str.trimmed().split(" ",QString::SkipEmptyParts);

  //
  // Remove useless day-of-the-week tag
  //
  if(f0.size()==6) {
    f0.removeFirst();
  }
  if(f0.size()!=5) {
    if(ok!=NULL) {
      *ok=false;
    }
    return QDateTime();
  }

  //
  // Read Date
  //
  int month=-1;
  for(int i=0;i<7;i++) {
    if(f0.at(1).toLower()==__rddatetime_month_names[i].toLower()) {
      month=i;
    }
  }
  if(month<0) {
    if(ok!=NULL) {
      *ok=false;
    }
    return QDateTime();
  }
  if(f0.at(2).length()==2) {
    f0[2]="19"+f0.at(2);
  }
  QDate date(f0.at(2).toInt(),month+1,f0.at(0).toInt());
  if(!date.isValid()) {
    if(ok!=NULL) {
      *ok=false;
    }
    return QDateTime();
  }

  //
  // Read Time
  //
  QTime time=QTime::fromString(f0.at(3),"hh:mm:ss");
  if(!time.isValid()) {
    if(ok!=NULL) {
      *ok=false;
    }
    return QDateTime();
  }
  
  //
  // Read TZ Offset
  //
  bool ok1=false;
  bool ok2=false;
  int tz_offset=3600*f0.at(4).mid(1,2).toInt(&ok1)+
    60*f0.at(4).right(2).toInt(&ok2);
  if((f0.at(4).length()==5)&&(ok1)&&(ok2)) {
    if(f0.at(4).left(1)=="+") {
      tz_offset=-tz_offset;
    }
    else {
      if(f0.at(4).left(1)!="-") {
	if(ok!=NULL) {
	  *ok=false;
	}
	return QDateTime();
      }
    }
  }
  else {
    //
    // Try legacy timezone names (see RFC822 section 5.1)
    //
    QMap<QString,int> zones;
    zones["ut"]=0;
    zones["gmt"]=0;
    zones["z"]=0;
    zones["est"]=-5;
    zones["edt"]=-4;
    zones["cst"]=-6;
    zones["cdt"]=-5;
    zones["mst"]=-7;
    zones["mdt"]=-6;
    zones["pst"]=-8;
    zones["pdt"]=-7;
    // "Military" zone IDs are not implemented (see RFC1123 section 5.2.14)
    if(zones.value(f0.at(4).toLower(),1)>0) {
      if(ok!=NULL) {
	*ok=false;
      }
      return QDateTime();
    }
    tz_offset=-3600*zones.value(f0.at(4).toLower());
  }
  
  if(ok!=NULL) {
    *ok=true;
  }

  return QDateTime(date,time).addSecs(tz_offset-RDTimeZoneOffset());
}


QString RDWriteRfc822DateTime(const QDateTime &dt)
{
  int utc_off=RDTimeZoneOffset();
  QString tz_str="-";
  if(utc_off<0) {
    tz_str="+";
  }
  tz_str+=QString().
    sprintf("%02d%02d",utc_off/3600,(utc_off-3600*(utc_off/3600))/60);

  return __rddatetime_dow_names[dt.date().dayOfWeek()-1]+", "+
    QString().sprintf("%d ",dt.date().day())+
    __rddatetime_month_names[dt.date().month()-1]+" "+
    QString().sprintf("%04d ",dt.date().year())+
    dt.toString("hh:mm:ss")+" "+
    tz_str;
}


//
// Returns the UTC offset of the curently configured timezone (seconds)
//
int RDTimeZoneOffset()
{
  time_t t=time(&t);
  struct tm *tm=localtime(&t);
  time_t local_time=3600*tm->tm_hour+60*tm->tm_min+tm->tm_sec;
  tm=gmtime(&t);
  time_t gmt_time=3600*tm->tm_hour+60*tm->tm_min+tm->tm_sec;

  int offset=gmt_time-local_time;
  if(offset>43200) {
    offset=offset-86400;
  }
  if(offset<-43200) {
    offset=offset+86400;
  }

  return offset;
}


